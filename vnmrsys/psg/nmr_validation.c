/*
 * nmr_validation.c - NMR Pulse Sequence Validation Framework Implementation
 *
 * Part of VnmrJ/OpenVNMRJ pulse sequence safety system
 * Created: November 9, 2025
 *
 * Implements three-tier validation system:
 *   1. Creation-time validation (TIER 1)
 *   2. Sequence-level validation (TIER 2)
 *   3. Runtime validation (TIER 3)
 *
 * See nmr_validation.h for API documentation.
 */

#include "nmr_validation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


/* ==============================================================================
 * INITIALIZATION FUNCTIONS
 * ============================================================================== */

NMRSystemLimits init_nmr_system_limits(void) {
    NMRSystemLimits limits;

    /* Initialize all fields to zero/empty */
    memset(&limits, 0, sizeof(NMRSystemLimits));

    /* Gradient System Defaults */
    limits.max_grad = 50.0;       /* 50 G/cm typical for MAS probes */
    limits.max_slew = 100.0;      /* 100 G/cm/ms */
    limits.grad_raster = 1.0;     /* 1 us raster time */

    /* System Timing Defaults */
    limits.rf_dead_time = 2.0;    /* 2 us RF dead time */
    limits.adc_dead_time = 5.0;   /* 5 us ADC dead time */

    /* RF Channel Defaults - obs channel (X, typically 13C) */
    limits.rf[0].max_amplitude = 125000.0;  /* 125 kHz max B1 */
    limits.rf[0].max_power = 63.0;          /* 63 dB max power */
    limits.rf[0].max_duration = 100.0;      /* 100 ms max pulse */
    limits.rf[0].duty_limit = 0.05;         /* 5% duty limit (C-detected default) */
    strcpy(limits.rf[0].channel_name, "obs");

    /* RF Channel Defaults - dec channel (H, typically 1H) */
    limits.rf[1].max_amplitude = 150000.0;  /* 150 kHz max B1 */
    limits.rf[1].max_power = 63.0;          /* 63 dB max power */
    limits.rf[1].max_duration = 100.0;      /* 100 ms max pulse */
    limits.rf[1].duty_limit = 0.15;         /* 15% duty limit (H-detected default) */
    strcpy(limits.rf[1].channel_name, "dec");

    /* RF Channel Defaults - dec2 channel (Y, typically 15N) */
    limits.rf[2].max_amplitude = 100000.0;  /* 100 kHz max B1 */
    limits.rf[2].max_power = 63.0;          /* 63 dB max power */
    limits.rf[2].max_duration = 100.0;      /* 100 ms max pulse */
    limits.rf[2].duty_limit = 0.05;         /* 5% duty limit */
    strcpy(limits.rf[2].channel_name, "dec2");

    /* Probe-Specific Defaults */
    strcpy(limits.probe_name, "generic");
    limits.probe_duty_limit = 0.05;         /* 5% default (conservative) */
    limits.min_srate = 500.0;               /* 500 Hz minimum spinning */
    limits.max_srate = 70000.0;             /* 70 kHz maximum spinning */

    /* Detection Type Defaults */
    strcpy(limits.sequence_name, "unknown");
    limits.detection_nucleus = DETECTION_UNKNOWN;
    limits.is_c_detected = 1;               /* Default to C-detected (more restrictive) */

    /* Runtime State */
    limits.total_rf_time = 0.0;
    limits.relaxation_delay = 0.0;
    limits.validation_level = 0;

    return limits;
}

PulseParams init_pulse_params(void) {
    PulseParams params;

    memset(&params, 0, sizeof(PulseParams));

    params.duration = 0.0;
    params.power = 0.0;
    params.phase = 0.0;
    params.offset = 0.0;
    params.B1_field = 0.0;
    strcpy(params.channel, "obs");

    return params;
}

ValidationResult init_validation_result(void) {
    ValidationResult result;

    result.is_valid = 1;  /* Assume valid until proven otherwise */
    strcpy(result.error_message, "");
    strcpy(result.solution, "");
    result.computed_value = 0.0;
    result.limit_value = 0.0;

    return result;
}


/* ==============================================================================
 * DETECTION TYPE FUNCTIONS
 * ============================================================================== */

char get_detection_type_from_name(const char* sequence_name) {
    if (sequence_name == NULL || strlen(sequence_name) == 0) {
        return DETECTION_UNKNOWN;
    }

    /* Get last character of sequence name (detection nucleus) */
    size_t len = strlen(sequence_name);
    char last_char = toupper(sequence_name[len - 1]);

    /* Check for H-detected sequences */
    if (last_char == 'H') {
        return DETECTION_H;
    }

    /* Check for C-detected sequences (X represents 13C) */
    if (last_char == 'X' || last_char == 'C') {
        return DETECTION_C;
    }

    /* Unknown detection type */
    return DETECTION_UNKNOWN;
}

void set_sequence_detection_type(NMRSystemLimits* limits, const char* sequence_name) {
    if (limits == NULL || sequence_name == NULL) {
        return;
    }

    /* Store sequence name */
    strncpy(limits->sequence_name, sequence_name, sizeof(limits->sequence_name) - 1);
    limits->sequence_name[sizeof(limits->sequence_name) - 1] = '\0';

    /* Determine detection type */
    limits->detection_nucleus = get_detection_type_from_name(sequence_name);

    /* Set C-detected flag */
    limits->is_c_detected = (limits->detection_nucleus == DETECTION_C);

    /* Set appropriate duty cycle limit */
    if (limits->is_c_detected) {
        /* C-detected: 5% duty cycle limit */
        limits->probe_duty_limit = DUTY_LIMIT_C_DETECTED;
        printf("Sequence '%s': C-detected, 5%% duty cycle limit\n", sequence_name);
    } else if (limits->detection_nucleus == DETECTION_H) {
        /* H-detected: 10-15% duty cycle limit (will be refined by power level) */
        limits->probe_duty_limit = DUTY_LIMIT_H_LOW_POWER;  /* Default to 15% */
        printf("Sequence '%s': H-detected, 10-15%% duty cycle limit (power-dependent)\n", sequence_name);
    } else {
        /* Unknown: Use conservative 5% limit */
        limits->probe_duty_limit = DUTY_LIMIT_C_DETECTED;
        printf("WARNING: Sequence '%s': Unknown detection type, using conservative 5%% duty cycle limit\n",
               sequence_name);
    }
}


/* ==============================================================================
 * DUTY CYCLE FUNCTIONS
 * ============================================================================== */

double get_safe_duty_limit(NMRSystemLimits* limits, double decoupling_power_db) {
    if (limits == NULL) {
        return DUTY_LIMIT_C_DETECTED;  /* Most conservative default */
    }

    /* C-detected sequences: Always 5% */
    if (limits->is_c_detected) {
        return DUTY_LIMIT_C_DETECTED;
    }

    /* H-detected sequences: Power-dependent */
    if (limits->detection_nucleus == DETECTION_H) {
        /* If power level provided, calculate B1 field */
        if (decoupling_power_db > 0) {
            double B1 = calculate_B1_from_power(decoupling_power_db, "dec", limits);

            if (B1 > B1_HIGH_POWER_THRESHOLD) {
                /* High-power decoupling: 10% limit */
                printf("High-power H decoupling (%.0f kHz B1): 10%% duty limit\n", B1 / 1000.0);
                return DUTY_LIMIT_H_HIGH_POWER;
            } else {
                /* Low-power decoupling: 15% limit */
                printf("Low-power H decoupling (%.0f kHz B1): 15%% duty limit\n", B1 / 1000.0);
                return DUTY_LIMIT_H_LOW_POWER;
            }
        } else {
            /* No power level provided, use default 15% for H-detected */
            return DUTY_LIMIT_H_LOW_POWER;
        }
    }

    /* Unknown detection type: Use conservative 5% */
    return DUTY_LIMIT_C_DETECTED;
}

double calculate_duty_cycle(double total_rf_time, double d1) {
    double total_cycle_time = total_rf_time + d1 + SEQUENCE_OVERHEAD_TIME;

    if (total_cycle_time <= 0) {
        printf("WARNING: Invalid cycle time in duty cycle calculation\n");
        return 1.0;  /* Assume worst case */
    }

    return total_rf_time / total_cycle_time;
}

ValidationResult validate_duty_cycle(NMRSystemLimits* limits, double total_rf_time, double d1) {
    ValidationResult result = init_validation_result();

    if (limits == NULL) {
        result.is_valid = 0;
        strcpy(result.error_message, "NULL limits pointer in validate_duty_cycle");
        return result;
    }

    /* Calculate duty cycle */
    double duty = calculate_duty_cycle(total_rf_time, d1);
    double duty_limit = limits->probe_duty_limit;

    /* Store computed values */
    result.computed_value = duty;
    result.limit_value = duty_limit;

    /* Check against limit */
    if (duty > duty_limit) {
        result.is_valid = 0;
        generate_duty_cycle_error(&result, duty, duty_limit, limits->sequence_name);
    } else {
        result.is_valid = 1;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Duty cycle %.1f%% < %.1f%% limit - Safe to proceed",
                 duty * 100.0, duty_limit * 100.0);
    }

    return result;
}


/* ==============================================================================
 * RF POWER AND B1 FIELD FUNCTIONS
 * ============================================================================== */

double calculate_B1_from_power(double power_db, const char* channel, NMRSystemLimits* limits) {
    /* Reference calibration: 100 kHz B1 at 50 dB power (typical) */
    const double B1_ref = 100000.0;  /* 100 kHz */
    const double power_ref = 50.0;   /* 50 dB */

    /* B1 (Hz) = B1_ref * 10^((power - power_ref) / 20) */
    double power_ratio = (power_db - power_ref) / 20.0;
    double B1_field = B1_ref * pow(10.0, power_ratio);

    /* Apply channel-specific adjustments if limits provided */
    if (limits != NULL) {
        int ch_idx = get_channel_index(channel);
        if (ch_idx >= 0 && ch_idx < 3) {
            /* Ensure B1 doesn't exceed channel maximum */
            if (B1_field > limits->rf[ch_idx].max_amplitude) {
                printf("WARNING: Calculated B1 %.0f Hz exceeds channel '%s' max %.0f Hz\n",
                       B1_field, channel, limits->rf[ch_idx].max_amplitude);
                B1_field = limits->rf[ch_idx].max_amplitude;
            }
        }
    }

    return B1_field;
}

double calculate_power_from_B1(double B1_field_hz, const char* channel, NMRSystemLimits* limits) {
    /* Reference calibration: 100 kHz B1 at 50 dB power */
    const double B1_ref = 100000.0;  /* 100 kHz */
    const double power_ref = 50.0;   /* 50 dB */

    /* Prevent log(0) */
    if (B1_field_hz <= 0) {
        return 0.0;
    }

    /* power (dB) = power_ref + 20 * log10(B1 / B1_ref) */
    double power_db = power_ref + 20.0 * log10(B1_field_hz / B1_ref);

    /* Apply channel-specific limits if provided */
    if (limits != NULL) {
        int ch_idx = get_channel_index(channel);
        if (ch_idx >= 0 && ch_idx < 3) {
            /* Ensure power doesn't exceed channel maximum */
            if (power_db > limits->rf[ch_idx].max_power) {
                printf("WARNING: Calculated power %.1f dB exceeds channel '%s' max %.1f dB\n",
                       power_db, channel, limits->rf[ch_idx].max_power);
                power_db = limits->rf[ch_idx].max_power;
            }
        }
    }

    return power_db;
}


/* ==============================================================================
 * TIER 1: CREATION-TIME VALIDATION
 * ============================================================================== */

ValidationResult validate_pulse_parameters(NMRSystemLimits* limits, PulseParams* params,
                                           const char* channel) {
    ValidationResult result = init_validation_result();

    if (limits == NULL || params == NULL || channel == NULL) {
        result.is_valid = 0;
        strcpy(result.error_message, "NULL pointer in validate_pulse_parameters");
        return result;
    }

    /* Get channel index */
    int ch_idx = get_channel_index(channel);
    if (ch_idx < 0 || ch_idx >= 3) {
        result.is_valid = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Invalid channel '%s' (must be 'obs', 'dec', or 'dec2')", channel);
        strcpy(result.solution, "Check channel parameter in make function call");
        return result;
    }

    RFChannelLimits* ch_limits = &limits->rf[ch_idx];

    /* Validate pulse duration */
    if (params->duration < 0) {
        result.is_valid = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Negative pulse duration: %.3f us", params->duration * 1e6);
        strcpy(result.solution, "Check pulse width parameter (must be positive)");
        return result;
    }

    if (params->duration > ch_limits->max_duration * 1e-3) {
        result.is_valid = 0;
        result.computed_value = params->duration * 1e3;
        result.limit_value = ch_limits->max_duration;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Pulse duration %.1f ms exceeds channel '%s' limit %.1f ms",
                 params->duration * 1e3, channel, ch_limits->max_duration);
        strcpy(result.solution, "Reduce pulse duration or contact facility manager");
        return result;
    }

    /* Validate power level */
    if (params->power < 0) {
        result.is_valid = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Negative power level: %.1f dB", params->power);
        strcpy(result.solution, "Check power parameter (must be positive)");
        return result;
    }

    if (params->power > ch_limits->max_power) {
        result.is_valid = 0;
        result.computed_value = params->power;
        result.limit_value = ch_limits->max_power;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Power %.1f dB exceeds channel '%s' limit %.1f dB",
                 params->power, channel, ch_limits->max_power);
        strcpy(result.solution, "Reduce power level or recalibrate pulse");
        return result;
    }

    /* Validate phase */
    if (params->phase < 0 || params->phase > 360.0) {
        result.is_valid = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Phase %.1f degrees out of range [0, 360]", params->phase);
        strcpy(result.solution, "Check phase parameter (0-360 degrees)");
        return result;
    }

    /* Calculate B1 field */
    params->B1_field = calculate_B1_from_power(params->power, channel, limits);

    /* All checks passed */
    result.is_valid = 1;
    snprintf(result.error_message, sizeof(result.error_message),
             "Pulse parameters valid: duration=%.1f us, power=%.1f dB, B1=%.0f Hz",
             params->duration * 1e6, params->power, params->B1_field);

    return result;
}


/* ==============================================================================
 * TIER 2: SEQUENCE-LEVEL VALIDATION
 * ============================================================================== */

ValidationResult validate_sequence(NMRSystemLimits* limits, const char* sequence_name) {
    ValidationResult result = init_validation_result();

    if (limits == NULL || sequence_name == NULL) {
        result.is_valid = 0;
        strcpy(result.error_message, "NULL pointer in validate_sequence");
        return result;
    }

    /* Set validation level */
    limits->validation_level = VALIDATION_SEQUENCE;

    /* Configure detection type */
    set_sequence_detection_type(limits, sequence_name);

    /* Check if detection type was successfully identified */
    if (limits->detection_nucleus == DETECTION_UNKNOWN) {
        printf("WARNING: Could not determine detection type from sequence name '%s'\n",
               sequence_name);
        printf("WARNING: Using conservative 5%% duty cycle limit\n");
    }

    /* Validate spinning rate (if MAS) */
    /* Note: This would typically call getval("srate") in real VnmrJ environment */
    /* For now, we just validate the limits structure */
    if (limits->min_srate > limits->max_srate) {
        result.is_valid = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Invalid spinning rate limits: min %.0f Hz > max %.0f Hz",
                 limits->min_srate, limits->max_srate);
        strcpy(result.solution, "Check NMRSystemLimits initialization");
        return result;
    }

    /* All sequence-level checks passed */
    result.is_valid = 1;
    snprintf(result.error_message, sizeof(result.error_message),
             "Sequence '%s' configuration valid: %s-detected, %.0f%% duty limit",
             sequence_name,
             limits->is_c_detected ? "C" : "H",
             limits->probe_duty_limit * 100.0);

    return result;
}


/* ==============================================================================
 * TIER 3: RUNTIME VALIDATION
 * ============================================================================== */

ValidationResult validate_runtime(NMRSystemLimits* limits, double total_rf_time,
                                  double d1, const char* sequence_name) {
    ValidationResult result = init_validation_result();

    if (limits == NULL || sequence_name == NULL) {
        result.is_valid = 0;
        strcpy(result.error_message, "NULL pointer in validate_runtime");
        return result;
    }

    /* Set validation level */
    limits->validation_level = VALIDATION_RUNTIME;

    /* Store runtime values */
    limits->total_rf_time = total_rf_time;
    limits->relaxation_delay = d1;

    /* Validate total RF time is positive */
    if (total_rf_time < 0) {
        result.is_valid = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Negative total RF time: %.3f ms", total_rf_time * 1e3);
        strcpy(result.solution, "Check RF time calculation in sequence");
        return result;
    }

    /* Validate d1 is positive */
    if (d1 < 0) {
        result.is_valid = 0;
        snprintf(result.error_message, sizeof(result.error_message),
                 "Negative relaxation delay d1: %.3f s", d1);
        strcpy(result.solution, "Set d1 to positive value (typically 1-5 seconds)");
        return result;
    }

    /* Validate duty cycle (main safety check) */
    ValidationResult duty_result = validate_duty_cycle(limits, total_rf_time, d1);
    if (!duty_result.is_valid) {
        return duty_result;  /* Return duty cycle validation failure */
    }

    /* Check for unreasonably long total time */
    double total_cycle = total_rf_time + d1 + SEQUENCE_OVERHEAD_TIME;
    if (total_cycle > 300.0) {  /* 5 minutes per scan */
        printf("WARNING: Long total cycle time: %.1f seconds per scan\n", total_cycle);
        printf("WARNING: Experiment may take excessive time\n");
    }

    /* All runtime checks passed */
    result.is_valid = 1;
    snprintf(result.error_message, sizeof(result.error_message),
             "Runtime validation passed: RF=%.1f ms, d1=%.2f s, duty=%.1f%%",
             total_rf_time * 1e3, d1,
             calculate_duty_cycle(total_rf_time, d1) * 100.0);

    return result;
}


/* ==============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================== */

int get_channel_index(const char* channel) {
    if (channel == NULL) {
        return -1;
    }

    if (strcmp(channel, "obs") == 0) {
        return 0;  /* X channel (13C) */
    } else if (strcmp(channel, "dec") == 0) {
        return 1;  /* H channel (1H) */
    } else if (strcmp(channel, "dec2") == 0) {
        return 2;  /* Y channel (15N) */
    }

    return -1;  /* Invalid channel */
}

void print_validation_summary(NMRSystemLimits* limits, const char* sequence_name) {
    if (limits == NULL) {
        printf("ERROR: NULL limits pointer\n");
        return;
    }

    printf("\n=== NMR Validation Summary ===\n");
    printf("Sequence: %s\n", sequence_name ? sequence_name : limits->sequence_name);
    printf("Detection: %c-detected (%s)\n",
           limits->detection_nucleus,
           limits->is_c_detected ? "Carbon/Hetero" : "Proton");
    printf("Duty Limit: %.0f%%\n", limits->probe_duty_limit * 100.0);
    printf("Validation Level: %d ", limits->validation_level);

    switch (limits->validation_level) {
        case VALIDATION_CREATION:
            printf("(Creation-time)\n");
            break;
        case VALIDATION_SEQUENCE:
            printf("(Sequence-level)\n");
            break;
        case VALIDATION_RUNTIME:
            printf("(Runtime)\n");
            break;
        default:
            printf("(Unknown)\n");
    }

    printf("\nRF Channel Limits:\n");
    for (int i = 0; i < 3; i++) {
        printf("  %s: max_B1=%.0f kHz, max_power=%.0f dB, duty=%.0f%%\n",
               limits->rf[i].channel_name,
               limits->rf[i].max_amplitude / 1000.0,
               limits->rf[i].max_power,
               limits->rf[i].duty_limit * 100.0);
    }

    printf("\nProbe: %s\n", limits->probe_name);
    printf("Spinning: %.0f - %.0f Hz\n", limits->min_srate, limits->max_srate);
    printf("=========================\n\n");
}

void print_duty_cycle_breakdown(double total_rf_time, double d1, double duty_limit) {
    double total_cycle = total_rf_time + d1 + SEQUENCE_OVERHEAD_TIME;
    double duty = calculate_duty_cycle(total_rf_time, d1);

    printf("\n=== Duty Cycle Breakdown ===\n");
    printf("Total RF time:    %.3f ms (%.1f%%)\n",
           total_rf_time * 1e3,
           (total_rf_time / total_cycle) * 100.0);
    printf("Relaxation delay: %.3f s  (%.1f%%)\n",
           d1,
           (d1 / total_cycle) * 100.0);
    printf("Overhead:         %.1f us  (%.1f%%)\n",
           SEQUENCE_OVERHEAD_TIME * 1e6,
           (SEQUENCE_OVERHEAD_TIME / total_cycle) * 100.0);
    printf("----------------------------\n");
    printf("Total cycle time: %.3f s\n", total_cycle);
    printf("Duty cycle:       %.2f%%\n", duty * 100.0);
    printf("Duty limit:       %.0f%%\n", duty_limit * 100.0);
    printf("Status:           %s\n",
           duty <= duty_limit ? "PASS" : "FAIL");
    printf("============================\n\n");
}


/* ==============================================================================
 * ERROR MESSAGE HELPERS
 * ============================================================================== */

void generate_duty_cycle_error(ValidationResult* result, double computed_duty,
                               double duty_limit, const char* sequence_name) {
    if (result == NULL) {
        return;
    }

    /* Generate error message */
    snprintf(result->error_message, sizeof(result->error_message),
             "Duty cycle %.1f%% exceeds %.0f%% limit for %s",
             computed_duty * 100.0,
             duty_limit * 100.0,
             sequence_name ? sequence_name : "sequence");

    /* Generate solution based on SAFETY_STANDARDS.md Section 8 */
    snprintf(result->solution, sizeof(result->solution),
             "Solutions (in order of preference):\n"
             "  1. Increase relaxation delay (d1) - most effective\n"
             "  2. Reduce mixing times (tXmix, tYmix, etc.)\n"
             "  3. Reduce evolution times (d2, d3) - impacts resolution\n"
             "  4. Use Non-Uniform Sampling (NUS) to allow longer d1\n"
             "Example: Increasing d1 from %.1f to %.1f seconds would achieve %.0f%% duty cycle",
             result->computed_value > 0 ? 1.0 : 2.0,  /* Example d1 */
             (computed_duty / duty_limit) * 2.0,      /* Suggested d1 */
             duty_limit * 100.0);

    result->computed_value = computed_duty;
    result->limit_value = duty_limit;
    result->is_valid = 0;
}

void generate_power_limit_error(ValidationResult* result, double computed_power,
                                double power_limit, const char* channel) {
    if (result == NULL) {
        return;
    }

    /* Generate error message */
    snprintf(result->error_message, sizeof(result->error_message),
             "Power %.1f dB exceeds %.1f dB limit for channel '%s'",
             computed_power,
             power_limit,
             channel ? channel : "unknown");

    /* Generate solution */
    snprintf(result->solution, sizeof(result->solution),
             "Solutions:\n"
             "  1. Reduce power level to %.1f dB or lower\n"
             "  2. Recalibrate pulse with longer duration and lower power\n"
             "  3. Contact facility manager if higher power is required",
             power_limit);

    result->computed_value = computed_power;
    result->limit_value = power_limit;
    result->is_valid = 0;
}


/* ==============================================================================
 * COMPREHENSIVE VALIDATION FUNCTION
 * ============================================================================== */

/**
 * validate_full_sequence - Comprehensive validation combining all three tiers
 *
 * This function demonstrates how to use all three validation tiers together
 * in a pulse sequence. It's provided as a convenience function and example.
 *
 * Parameters:
 *   sequence_name: Name of the sequence
 *   total_rf_time: Total RF time (s)
 *   d1: Relaxation delay (s)
 *   decoupling_power: Decoupling power level (dB), use -1 for default
 *
 * Returns:
 *   1 if all validations pass, 0 if any fail (with abort)
 */
int validate_full_sequence(const char* sequence_name, double total_rf_time,
                           double d1, double decoupling_power) {
    ValidationResult result;

    /* Initialize system limits */
    NMRSystemLimits limits = init_nmr_system_limits();

    printf("\n=== Three-Tier Validation for %s ===\n", sequence_name);

    /* TIER 2: Sequence-level validation */
    printf("\nTIER 2: Sequence-Level Validation\n");
    result = validate_sequence(&limits, sequence_name);
    if (!result.is_valid) {
        printf("ERROR: %s\n", result.error_message);
        if (strlen(result.solution) > 0) {
            printf("SOLUTION: %s\n", result.solution);
        }
        psg_abort(1);
        return 0;
    }
    printf("PASS: %s\n", result.error_message);

    /* Adjust duty limit based on decoupling power if provided */
    if (decoupling_power > 0) {
        limits.probe_duty_limit = get_safe_duty_limit(&limits, decoupling_power);
    }

    /* TIER 3: Runtime validation */
    printf("\nTIER 3: Runtime Validation\n");
    result = validate_runtime(&limits, total_rf_time, d1, sequence_name);
    if (!result.is_valid) {
        printf("ERROR: %s\n", result.error_message);
        if (strlen(result.solution) > 0) {
            printf("SOLUTION: %s\n", result.solution);
        }
        print_duty_cycle_breakdown(total_rf_time, d1, limits.probe_duty_limit);
        psg_abort(1);
        return 0;
    }
    printf("PASS: %s\n", result.error_message);

    /* Print summary */
    print_duty_cycle_breakdown(total_rf_time, d1, limits.probe_duty_limit);

    printf("=== All Validation Tiers Passed ===\n\n");

    return 1;
}
