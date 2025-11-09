/*
 * nmr_validation.h - Comprehensive NMR Pulse Sequence Validation Framework
 *
 * Part of VnmrJ/OpenVNMRJ pulse sequence safety system
 * Created: November 9, 2025
 *
 * Purpose:
 *   Three-tier validation system for NMR pulse sequences ensuring hardware
 *   safety through creation-time, sequence-level, and runtime validation.
 *
 * Validation Tiers:
 *   1. Creation-time: Called by make functions during pulse element creation
 *   2. Sequence-level: Called after parameter setup in pulsesequence()
 *   3. Runtime: Called before pulse execution to validate computed parameters
 *
 * Key Safety Features:
 *   - Power-dependent duty cycle limits (5% for C-detected, 15% for H-detected)
 *   - B1 field strength calculation from power levels
 *   - Detection type identification (C-detected vs H-detected)
 *   - Hardware-specific limits enforcement
 *   - Clear error messages with actionable solutions
 *
 * Standards Compliance:
 *   - SAFETY_STANDARDS.md Section 1: Duty Cycle Limits by Detection Type
 *   - SAFETY_STANDARDS.md Section 2: Detection Type Identification
 *   - REFACTORING_IMPLEMENTATION_PLAN.md Phase 2: PulSeq-Inspired Architecture
 *
 * Usage Example:
 *   // Sequence-level validation
 *   NMRSystemLimits limits = init_nmr_system_limits();
 *   validate_sequence(&limits, "hXX");
 *
 *   // Creation-time validation (in make functions)
 *   PulseParams pp = {.duration=0.01, .power=50.0, .phase=0.0};
 *   validate_pulse_parameters(&limits, &pp, "obs");
 *
 *   // Runtime validation
 *   validate_runtime(&limits, total_rf_time, d1, "hXX");
 *
 * Author: VnmrJ Development Team
 * License: OpenVNMRJ License
 */

#ifndef NMR_VALIDATION_H
#define NMR_VALIDATION_H

#include "standard.h"
#include <math.h>
#include <string.h>

/* ==============================================================================
 * STRUCTURES
 * ============================================================================== */

/**
 * RF Channel Limits
 *
 * Defines hardware and safety limits for a single RF channel.
 * Used for obs (X), dec (H), and dec2 (Y) channels.
 */
typedef struct {
    double max_amplitude;    /* Maximum B1 field strength (Hz) */
    double max_power;        /* Maximum power level (dB) */
    double max_duration;     /* Maximum single pulse duration (ms) */
    double duty_limit;       /* Maximum duty cycle (0.05 = 5%) */
    char channel_name[16];   /* Channel identifier ("obs", "dec", "dec2") */
} RFChannelLimits;

/**
 * NMR System Limits
 *
 * Complete hardware and safety limits for NMR spectrometer.
 * Based on REFACTORING_IMPLEMENTATION_PLAN.md Phase 2.1 NMRSystemLimits structure.
 *
 * This structure encapsulates all hardware-specific and safety-related limits
 * for an NMR system, enabling portable sequence code that adapts to different
 * hardware configurations.
 */
typedef struct {
    /* Gradient System Limits */
    double max_grad;         /* Maximum gradient strength (G/cm) */
    double max_slew;         /* Maximum slew rate (G/cm/ms) */
    double grad_raster;      /* Gradient raster time (us) */

    /* System Timing Limits */
    double rf_dead_time;     /* RF dead time (us) */
    double adc_dead_time;    /* ADC dead time (us) */

    /* RF Channel Limits */
    RFChannelLimits rf[3];   /* [0]=obs (X), [1]=dec (H), [2]=dec2 (Y) */

    /* Probe-Specific Limits */
    char probe_name[64];     /* Probe identifier */
    double probe_duty_limit; /* Probe-specific duty cycle limit */
    double min_srate;        /* Minimum spinning rate (Hz) */
    double max_srate;        /* Maximum spinning rate (Hz) */

    /* Detection Type Identification */
    char sequence_name[32];  /* Sequence name for detection type inference */
    char detection_nucleus;  /* 'H' or 'X' (C/N) */
    int is_c_detected;       /* 1 if C-detected, 0 if H-detected */

    /* Runtime State */
    double total_rf_time;    /* Accumulated RF time for duty cycle (s) */
    double relaxation_delay; /* d1 relaxation delay (s) */
    int validation_level;    /* Current validation level (1-3) */

} NMRSystemLimits;

/**
 * Pulse Parameters
 *
 * Parameters for a single pulse element, used in creation-time validation.
 * These are validated before pulse elements are added to the sequence.
 */
typedef struct {
    double duration;         /* Pulse duration (s) */
    double power;            /* Power level (dB) */
    double phase;            /* Phase (degrees) */
    double offset;           /* Frequency offset (Hz) */
    double B1_field;         /* Calculated B1 field strength (Hz) */
    char channel[16];        /* Channel name ("obs", "dec", "dec2") */
} PulseParams;

/**
 * Validation Result
 *
 * Return structure for validation functions, allowing detailed error reporting.
 */
typedef struct {
    int is_valid;            /* 1 if validation passed, 0 if failed */
    char error_message[256]; /* Detailed error message */
    char solution[256];      /* Suggested solution */
    double computed_value;   /* Computed value that failed validation */
    double limit_value;      /* Limit that was exceeded */
} ValidationResult;


/* ==============================================================================
 * INITIALIZATION FUNCTIONS
 * ============================================================================== */

/**
 * Initialize NMR System Limits
 *
 * Creates NMRSystemLimits structure with default values suitable for typical
 * solid-state NMR spectrometers. Values should be adjusted based on actual
 * hardware specifications.
 *
 * Default Values:
 *   - Gradients: 50 G/cm max, 100 G/cm/ms slew
 *   - RF channels: Set for typical MAS probe
 *   - Probe duty: 5% for C-detected sequences
 *   - Spinning: 500 Hz - 70 kHz range
 *
 * Returns:
 *   Initialized NMRSystemLimits structure
 *
 * Example:
 *   NMRSystemLimits limits = init_nmr_system_limits();
 *   set_sequence_detection_type(&limits, "hXX");  // Auto-detect from name
 */
NMRSystemLimits init_nmr_system_limits(void);

/**
 * Initialize Pulse Parameters
 *
 * Creates PulseParams structure with safe default values.
 *
 * Returns:
 *   Initialized PulseParams structure
 */
PulseParams init_pulse_params(void);

/**
 * Initialize Validation Result
 *
 * Creates ValidationResult structure with "valid" initial state.
 *
 * Returns:
 *   Initialized ValidationResult structure
 */
ValidationResult init_validation_result(void);


/* ==============================================================================
 * DETECTION TYPE FUNCTIONS
 * ============================================================================== */

/**
 * Determine Detection Type from Sequence Name
 *
 * Identifies whether sequence is C-detected or H-detected based on naming
 * convention: last letter indicates detection nucleus.
 *
 * Convention (SAFETY_STANDARDS.md Section 2):
 *   - hXX, hYXX, hXYXX -> ends in 'X' -> C-detected -> 5% duty limit
 *   - hXH, HhXH, hYXH -> ends in 'H' -> H-detected -> 10-15% duty limit
 *
 * Parameters:
 *   sequence_name: Sequence name (e.g., "hXX", "hXH")
 *
 * Returns:
 *   'H' for H-detected, 'X' for C-detected, '?' if unknown
 *
 * Example:
 *   char det = get_detection_type_from_name("hXX");  // Returns 'X'
 *   char det = get_detection_type_from_name("hXH");  // Returns 'H'
 */
char get_detection_type_from_name(const char* sequence_name);

/**
 * Set Sequence Detection Type
 *
 * Configures NMRSystemLimits structure based on sequence name, automatically
 * setting appropriate duty cycle limits.
 *
 * Parameters:
 *   limits: Pointer to NMRSystemLimits structure
 *   sequence_name: Sequence name for detection type inference
 *
 * Side Effects:
 *   - Sets limits->detection_nucleus
 *   - Sets limits->is_c_detected
 *   - Updates limits->probe_duty_limit based on detection type
 *   - Copies sequence_name to limits->sequence_name
 *
 * Example:
 *   NMRSystemLimits limits = init_nmr_system_limits();
 *   set_sequence_detection_type(&limits, "hXX");  // Sets 5% duty for C-detected
 */
void set_sequence_detection_type(NMRSystemLimits* limits, const char* sequence_name);


/* ==============================================================================
 * DUTY CYCLE FUNCTIONS
 * ============================================================================== */

/**
 * Get Safe Duty Cycle Limit
 *
 * Returns appropriate duty cycle limit based on detection type and power level.
 * Implements power-dependent duty cycle limits from SAFETY_STANDARDS.md.
 *
 * Logic:
 *   - C-detected sequences: Always 5% (high-power decoupling on carbon)
 *   - H-detected sequences: 10-15% depending on decoupling power
 *     - High-power decoupling (>50 kHz B1): 10%
 *     - Low-power decoupling (<50 kHz B1): 15%
 *
 * Parameters:
 *   limits: Pointer to NMRSystemLimits structure
 *   decoupling_power_db: Decoupling power level in dB (optional, use -1 for default)
 *
 * Returns:
 *   Safe duty cycle limit (0.05 = 5%, 0.10 = 10%, 0.15 = 15%)
 *
 * Example:
 *   double limit = get_safe_duty_limit(&limits, 50.0);  // 0.05 for C-detected
 *   double limit = get_safe_duty_limit(&limits, 35.0);  // 0.15 for low-power H-detected
 */
double get_safe_duty_limit(NMRSystemLimits* limits, double decoupling_power_db);

/**
 * Calculate Duty Cycle
 *
 * Computes duty cycle as ratio of RF time to total cycle time.
 *
 * Formula:
 *   duty = total_rf_time / (total_rf_time + d1 + overhead)
 *   overhead = 4.0e-6 s (standard VnmrJ overhead)
 *
 * Parameters:
 *   total_rf_time: Total RF time in sequence (s)
 *   d1: Relaxation delay (s)
 *
 * Returns:
 *   Duty cycle as fraction (0.05 = 5%)
 *
 * Example:
 *   double duty = calculate_duty_cycle(0.010, 2.0);  // 0.497% duty cycle
 */
double calculate_duty_cycle(double total_rf_time, double d1);

/**
 * Validate Duty Cycle
 *
 * Checks if computed duty cycle is within safe limits for the sequence type.
 * Provides detailed error messages and solutions if limit is exceeded.
 *
 * Parameters:
 *   limits: Pointer to NMRSystemLimits structure
 *   total_rf_time: Total RF time in sequence (s)
 *   d1: Relaxation delay (s)
 *
 * Returns:
 *   ValidationResult structure with detailed status
 *
 * Error Solutions (from SAFETY_STANDARDS.md Section 8):
 *   1. Increase d1 (most effective)
 *   2. Reduce mixing times
 *   3. Reduce evolution times (impacts resolution)
 *   4. Use Non-Uniform Sampling (NUS)
 *
 * Example:
 *   ValidationResult result = validate_duty_cycle(&limits, 0.010, 2.0);
 *   if (!result.is_valid) {
 *       printf("%s\n", result.error_message);
 *       printf("Solution: %s\n", result.solution);
 *       psg_abort(1);
 *   }
 */
ValidationResult validate_duty_cycle(NMRSystemLimits* limits, double total_rf_time, double d1);


/* ==============================================================================
 * RF POWER AND B1 FIELD FUNCTIONS
 * ============================================================================== */

/**
 * Calculate B1 Field from Power Level
 *
 * Converts power level (dB) to B1 field strength (Hz) using probe calibration.
 * This is an approximation based on typical MAS probe characteristics.
 *
 * Formula:
 *   B1 (Hz) = B1_ref * 10^((power - power_ref) / 20)
 *
 * Where:
 *   B1_ref = 100 kHz at power_ref = 50 dB (typical 90-degree pulse calibration)
 *
 * Parameters:
 *   power_db: Power level in dB
 *   channel: Channel identifier ("obs", "dec", "dec2")
 *   limits: Pointer to NMRSystemLimits for channel-specific calibration
 *
 * Returns:
 *   B1 field strength in Hz
 *
 * Note:
 *   For accurate values, calibrate with pw90 measurements and adjust reference.
 *   B1 = 1/(4*pw90) for a 90-degree pulse
 *
 * Example:
 *   double B1 = calculate_B1_from_power(50.0, "obs", &limits);  // Returns ~100000 Hz
 *   double B1 = calculate_B1_from_power(44.0, "dec", &limits);  // Returns ~50000 Hz
 */
double calculate_B1_from_power(double power_db, const char* channel, NMRSystemLimits* limits);

/**
 * Calculate Power from B1 Field
 *
 * Inverse function: converts B1 field strength (Hz) to power level (dB).
 *
 * Formula:
 *   power (dB) = power_ref + 20 * log10(B1 / B1_ref)
 *
 * Parameters:
 *   B1_field_hz: B1 field strength in Hz
 *   channel: Channel identifier ("obs", "dec", "dec2")
 *   limits: Pointer to NMRSystemLimits for channel-specific calibration
 *
 * Returns:
 *   Power level in dB
 *
 * Example:
 *   double power = calculate_power_from_B1(50000.0, "dec", &limits);  // Returns ~44 dB
 */
double calculate_power_from_B1(double B1_field_hz, const char* channel, NMRSystemLimits* limits);


/* ==============================================================================
 * THREE-TIER VALIDATION FUNCTIONS
 * ============================================================================== */

/**
 * TIER 1: Creation-Time Validation
 *
 * Validates pulse parameters at the time of pulse element creation (in make functions).
 * This is the first line of defense, catching invalid parameters before they enter
 * the sequence.
 *
 * Checks:
 *   - Pulse duration within hardware limits
 *   - Power level within channel limits
 *   - Phase value is valid (0-360 degrees)
 *   - B1 field strength calculation
 *   - Channel identifier is valid
 *
 * Parameters:
 *   limits: Pointer to NMRSystemLimits structure
 *   params: Pointer to PulseParams structure to validate
 *   channel: Channel identifier ("obs", "dec", "dec2")
 *
 * Returns:
 *   ValidationResult structure with detailed status
 *
 * Example (in makePulse() function):
 *   PulseParams pp;
 *   pp.duration = getval("pwH90") * 1e-6;  // Convert us to s
 *   pp.power = getval("tpwr");
 *   pp.phase = 0.0;
 *   strcpy(pp.channel, "dec");
 *
 *   ValidationResult result = validate_pulse_parameters(&limits, &pp, "dec");
 *   if (!result.is_valid) {
 *       printf("ERROR: %s\n", result.error_message);
 *       psg_abort(1);
 *   }
 */
ValidationResult validate_pulse_parameters(NMRSystemLimits* limits, PulseParams* params,
                                           const char* channel);

/**
 * TIER 2: Sequence-Level Validation
 *
 * Validates entire sequence configuration after parameter setup in pulsesequence().
 * This checks for parameter consistency and sequence-level safety before any
 * pulses are executed.
 *
 * Checks:
 *   - Detection type correctly identified
 *   - Duty cycle limits set appropriately
 *   - Spinning rate within limits (if MAS)
 *   - Evolution times are reasonable
 *   - Array configuration is valid
 *   - No parameter conflicts
 *
 * Parameters:
 *   limits: Pointer to NMRSystemLimits structure
 *   sequence_name: Name of sequence for detection type inference
 *
 * Returns:
 *   ValidationResult structure with detailed status
 *
 * Example (in pulsesequence() after parameter setup):
 *   void pulsesequence() {
 *       // Get all parameters...
 *
 *       NMRSystemLimits limits = init_nmr_system_limits();
 *       ValidationResult result = validate_sequence(&limits, seqfil);
 *       if (!result.is_valid) {
 *           printf("ERROR: %s\n", result.error_message);
 *           printf("SOLUTION: %s\n", result.solution);
 *           psg_abort(1);
 *       }
 *
 *       // Continue with pulse sequence...
 *   }
 */
ValidationResult validate_sequence(NMRSystemLimits* limits, const char* sequence_name);

/**
 * TIER 3: Runtime Validation
 *
 * Validates computed parameters immediately before pulse execution.
 * This is the final safety check with actual runtime values.
 *
 * Checks:
 *   - Duty cycle with actual computed RF times
 *   - Total sequence time is reasonable
 *   - No divide-by-zero or numerical errors
 *   - All accumulated times are positive
 *
 * Parameters:
 *   limits: Pointer to NMRSystemLimits structure
 *   total_rf_time: Actual total RF time computed from sequence (s)
 *   d1: Relaxation delay (s)
 *   sequence_name: Sequence name for error reporting
 *
 * Returns:
 *   ValidationResult structure with detailed status
 *
 * Example (before pulse sequence execution):
 *   // Calculate total RF time
 *   double tRF = pwH90 + pwX90 + tHX + d2 + tXmix + at;
 *
 *   ValidationResult result = validate_runtime(&limits, tRF, d1, seqfil);
 *   if (!result.is_valid) {
 *       printf("ABORT: %s\n", result.error_message);
 *       printf("SOLUTION: %s\n", result.solution);
 *       psg_abort(1);
 *   }
 *
 *   // Execute pulse sequence
 */
ValidationResult validate_runtime(NMRSystemLimits* limits, double total_rf_time,
                                  double d1, const char* sequence_name);


/* ==============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================== */

/**
 * Get Channel Index
 *
 * Converts channel name to array index for limits->rf[] array.
 *
 * Mapping:
 *   "obs" -> 0 (X channel, typically 13C)
 *   "dec" -> 1 (H channel, typically 1H)
 *   "dec2" -> 2 (Y channel, typically 15N)
 *
 * Parameters:
 *   channel: Channel name string
 *
 * Returns:
 *   Channel index (0-2), or -1 if invalid
 */
int get_channel_index(const char* channel);

/**
 * Print Validation Summary
 *
 * Outputs formatted validation summary to console for debugging and logging.
 *
 * Parameters:
 *   limits: Pointer to NMRSystemLimits structure
 *   sequence_name: Sequence name
 */
void print_validation_summary(NMRSystemLimits* limits, const char* sequence_name);

/**
 * Print Duty Cycle Breakdown
 *
 * Outputs detailed duty cycle calculation showing all components.
 * Useful for debugging duty cycle failures.
 *
 * Parameters:
 *   total_rf_time: Total RF time (s)
 *   d1: Relaxation delay (s)
 *   duty_limit: Duty cycle limit (fraction)
 */
void print_duty_cycle_breakdown(double total_rf_time, double d1, double duty_limit);


/* ==============================================================================
 * ERROR MESSAGE HELPERS
 * ============================================================================== */

/**
 * Generate Duty Cycle Error Message
 *
 * Creates standardized error message for duty cycle violations with
 * actionable solutions per SAFETY_STANDARDS.md Section 8.
 *
 * Parameters:
 *   result: Pointer to ValidationResult to populate
 *   computed_duty: Computed duty cycle (fraction)
 *   duty_limit: Allowed duty cycle limit (fraction)
 *   sequence_name: Sequence name for context
 */
void generate_duty_cycle_error(ValidationResult* result, double computed_duty,
                               double duty_limit, const char* sequence_name);

/**
 * Generate Power Limit Error Message
 *
 * Creates standardized error message for power limit violations.
 *
 * Parameters:
 *   result: Pointer to ValidationResult to populate
 *   computed_power: Computed power level (dB)
 *   power_limit: Allowed power limit (dB)
 *   channel: Channel name
 */
void generate_power_limit_error(ValidationResult* result, double computed_power,
                                double power_limit, const char* channel);


/* ==============================================================================
 * CONSTANTS
 * ============================================================================== */

/* Detection types */
#define DETECTION_H 'H'
#define DETECTION_C 'X'  /* X represents 13C or other heteronuclei */
#define DETECTION_UNKNOWN '?'

/* Duty cycle limits (as fractions) */
#define DUTY_LIMIT_C_DETECTED 0.05    /* 5% for C-detected */
#define DUTY_LIMIT_H_HIGH_POWER 0.10  /* 10% for H-detected with high-power dec */
#define DUTY_LIMIT_H_LOW_POWER 0.15   /* 15% for H-detected with low-power dec */

/* B1 field threshold for power-dependent limits */
#define B1_HIGH_POWER_THRESHOLD 50000.0  /* 50 kHz B1 field */

/* Validation levels */
#define VALIDATION_CREATION 1
#define VALIDATION_SEQUENCE 2
#define VALIDATION_RUNTIME 3

/* Standard overhead time in seconds */
#define SEQUENCE_OVERHEAD_TIME 4.0e-6

#endif /* NMR_VALIDATION_H */
