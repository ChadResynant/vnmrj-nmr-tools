/*
 * biosolidvalidation.h - Unified Parameter Validation System
 * 
 * Provides consolidated parameter validation for NMR pulse sequences
 * including duty cycle calculations, timing validation, and safety checks.
 * 
 * Key Features:
 * - Unified duty cycle calculation
 * - Parameter range validation
 * - Gradient power protection
 * - Array configuration checks
 * - Consistent error reporting
 * 
 * Usage:
 *   NMRValidation validator = init_nmr_validation();
 *   add_pulse_time(&validator, "H90", getval("pwH90"));
 *   validate_all_parameters(&validator);
 */

#ifndef BIOSOLIDVALIDATION_H
#define BIOSOLIDVALIDATION_H

#include "standard.h"

/* Validation structure */
typedef struct {
    // Duty cycle components
    double total_pulse_time;        // Total high-power pulse time
    double total_cycle_time;        // Total sequence cycle time
    double duty_limit;              // Maximum allowed duty cycle (e.g., 0.1 for 10%)
    
    // Timing parameters
    double evolution_times[4];      // d2, d3, d4, etc.
    double max_evolution_limit;     // Maximum evolution time (e.g., 10e-3)
    int n_evolution_periods;        // Number of evolution periods
    
    // RF safety
    double continuous_rf_time;      // Longest continuous RF period
    double max_continuous_rf;       // Maximum allowed continuous RF
    
    // Gradient safety
    double gradient_power;          // Total gradient power
    double max_gradient_power;      // Maximum allowed gradient power
    
    // Sequence configuration
    int ndim;                       // Number of dimensions
    int ni_values[3];              // ni, ni2, ni3
    char array_string[MAXSTR];     // Array parameter
    
    // Component tracking
    char pulse_names[20][16];      // Names of pulses for reporting
    double pulse_times[20];        // Individual pulse times
    int n_pulses;                  // Number of pulses tracked
    
} NMRValidation;

/* Initialize validation structure */
NMRValidation init_nmr_validation(void) {
    NMRValidation validator = {0};
    
    // Set default limits
    validator.duty_limit = 0.1;              // 10% duty cycle
    validator.max_evolution_limit = 10e-3;   // 10 ms evolution
    validator.max_continuous_rf = 50e-3;     // 50 ms continuous RF
    validator.max_gradient_power = 300;      // Gradient power units
    
    validator.n_evolution_periods = 0;
    validator.n_pulses = 0;
    
    return validator;
}

/* Set custom limits */
void set_duty_limit(NMRValidation* validator, double limit) {
    validator->duty_limit = limit;
}

void set_evolution_limit(NMRValidation* validator, double limit) {
    validator->max_evolution_limit = limit;
}

void set_rf_limit(NMRValidation* validator, double limit) {
    validator->max_continuous_rf = limit;
}

/* Add pulse time for duty cycle calculation */
void add_pulse_time(NMRValidation* validator, const char* name, double time) {
    if (validator->n_pulses < 20) {
        strncpy(validator->pulse_names[validator->n_pulses], name, 15);
        validator->pulse_times[validator->n_pulses] = time;
        validator->total_pulse_time += time;
        validator->n_pulses++;
    } else {
        printf("WARNING: Maximum pulse tracking exceeded\n");
        validator->total_pulse_time += time;
    }
}

/* Add multiple standard pulses at once */
void add_standard_pulses(NMRValidation* validator) {
    add_pulse_time(validator, "H90", getval("pwH90"));
    add_pulse_time(validator, "X90", getval("pwX90"));
    add_pulse_time(validator, "Y90", getval("pwY90"));
    
    // Add common timing parameters
    validator->total_pulse_time += getval("ad");    // Acquisition time
    validator->total_pulse_time += getval("rd");    // Ring-down delay
    validator->total_pulse_time += getval("at");    // Acquisition time
}

/* Add CP times */
void add_cp_times(NMRValidation* validator, const char* cp_names[], int n_cp) {
    for (int i = 0; i < n_cp; i++) {
        char param_name[32];
        snprintf(param_name, sizeof(param_name), "t%s", cp_names[i]);
        double cp_time = getval(param_name);
        add_pulse_time(validator, cp_names[i], cp_time);
    }
}

/* Add evolution time */
void add_evolution_time(NMRValidation* validator, double d_time) {
    if (validator->n_evolution_periods < 4) {
        validator->evolution_times[validator->n_evolution_periods] = d_time;
        validator->total_pulse_time += d_time;  // Count as pulse time for duty cycle
        validator->n_evolution_periods++;
    }
}

/* Add cycle time components */
void add_cycle_time(NMRValidation* validator, double time) {
    validator->total_cycle_time += time;
}

/* Calculate duty cycle */
double calculate_duty_cycle(NMRValidation* validator) {
    // Extract values to avoid struct references in dps
    double total_pulse = validator->total_pulse_time;
    double total_cycle = validator->total_cycle_time;
    
    // Add d1 and overhead to cycle time
    total_cycle += getval("d1") + 4.0e-6;
    
    if (total_cycle <= 0) {
        printf("WARNING: Invalid cycle time for duty calculation\n");
        return 1.0;  // Assume worst case
    }
    
    return total_pulse / total_cycle;
}

/* Validate duty cycle */
void validate_duty_cycle(NMRValidation* validator) {
    // Extract values to local variables for clean dps display
    double total_pulse = validator->total_pulse_time;
    double total_cycle = validator->total_cycle_time + getval("d1") + 4.0e-6;
    double duty_limit = validator->duty_limit;
    int n_pulses = validator->n_pulses;
    
    double duty = total_pulse / total_cycle;
    
    printf("Duty cycle calculation:\n");
    printf("  Total pulse time: %.1f ms\n", total_pulse * 1e3);
    printf("  Total cycle time: %.1f ms\n", total_cycle * 1e3);
    printf("  Duty cycle: %.1f%%\n", duty * 100);
    
    if (duty > duty_limit) {
        printf("ABORT: Duty cycle %.1f%% exceeds limit %.1f%%\n",
               duty * 100, duty_limit * 100);
        printf("Pulse breakdown:\n");
        for (int i = 0; i < n_pulses; i++) {
            printf("  %s: %.1f us\n", validator->pulse_names[i], 
                   validator->pulse_times[i] * 1e6);
        }
        psg_abort(1);
    }
}

/* Validate evolution times */
void validate_evolution_times(NMRValidation* validator) {
    // Extract values for clean dps display
    int n_periods = validator->n_evolution_periods;
    double max_limit = validator->max_evolution_limit;
    
    for (int i = 0; i < n_periods; i++) {
        double evo_time = validator->evolution_times[i];
        
        if (evo_time > max_limit) {
            printf("ABORT: Evolution time %d (%.1f ms) exceeds limit (%.1f ms)\n",
                   i+1, evo_time * 1e3, max_limit * 1e3);
            psg_abort(1);
        }
        
        if (evo_time < 0) {
            printf("ABORT: Negative evolution time %d: %.1f us\n",
                   i+1, evo_time * 1e6);
            psg_abort(1);
        }
    }
    
    if (n_periods > 0) {
        printf("Evolution times validated: ");
        for (int i = 0; i < n_periods; i++) {
            printf("d%d=%.1f ms ", i+2, validator->evolution_times[i] * 1e3);
        }
        printf("\n");
    }
}

/* Validate gradient power */
void validate_gradient_power(NMRValidation* validator) {
    // Extract values for clean dps display
    double grad_power = validator->gradient_power;
    double max_power = validator->max_gradient_power;
    
    if (grad_power > max_power) {
        printf("ABORT: Gradient power %.1f exceeds limit %.1f\n",
               grad_power, max_power);
        psg_abort(1);
    } else if (grad_power > 0) {
        printf("Gradient power: %.1f%% of maximum\n", 
               (100 * grad_power) / max_power);
    }
}

/* Add gradient power calculation */
void add_gradient_power(NMRValidation* validator, double q_pulses, double pw_pulse, double grad_level) {
    // Extract values to avoid struct references in dps
    double power_contrib = q_pulses * pw_pulse * grad_level;
    validator->gradient_power += power_contrib;
}

/* Validate array configuration for multidimensional experiments */
void validate_array_config(NMRValidation* validator) {
    // Extract values for clean dps display
    char array_str[MAXSTR];
    getstr("array", array_str);
    strncpy(validator->array_string, array_str, sizeof(validator->array_string)-1);
    validator->array_string[sizeof(validator->array_string)-1] = '\0';  // Ensure null termination
    
    int ndim = (int)getval("ndim");
    int ni_val = (int)getval("ni");
    int ni2_val = (int)getval("ni2"); 
    int ni3_val = (int)getval("ni3");
    
    validator->ndim = ndim;
    validator->ni_values[0] = ni_val;
    validator->ni_values[1] = ni2_val;
    validator->ni_values[2] = ni3_val;
    
    printf("Array configuration: %s (ndim=%d)\n", array_str, ndim);
    
    // Validate 3D configuration
    if (ndim == 3) {
        if (!strcmp(array_str, "phase,phase2")) {
            printf("ABORT: Change array to 'phase2,phase' for 3D experiment\n");
            psg_abort(1);
        }
        if (ni_val == 0 || ni2_val == 0) {
            printf("ABORT: ni and ni2 must be non-zero for 3D experiment\n");
            psg_abort(1);
        }
    }
    
    // Validate 4D configuration  
    if (ndim == 4) {
        if (strcmp(array_str, "phase3,phase2,phase")) {
            printf("ABORT: Change array to 'phase3,phase2,phase' for 4D experiment\n");
            psg_abort(1);
        }
        if (ni_val == 0 || ni2_val == 0 || ni3_val == 0) {
            printf("ABORT: All ni values must be non-zero for 4D experiment\n");
            psg_abort(1);
        }
    }
}

/* Validate spinning rate */
void validate_spinning_rate(double min_rate) {
    double srate = getval("srate");
    if (srate < min_rate) {
        printf("ABORT: Spinning rate %.0f Hz < minimum %.0f Hz\n", srate, min_rate);
        psg_abort(1);
    }
    printf("Spinning rate: %.0f Hz\n", srate);
}

/* Comprehensive validation function */
void validate_all_parameters(NMRValidation* validator) {
    printf("\n=== Parameter Validation ===\n");
    
    // Basic parameter checks
    validate_evolution_times(validator);
    validate_duty_cycle(validator);
    validate_gradient_power(validator);
    validate_array_config(validator);
    
    // Spinning rate check (if applicable)
    if (getval("srate") > 0) {
        validate_spinning_rate(500.0);  // Default minimum 500 Hz
    }
    
    printf("=== All Parameters Valid ===\n\n");
}

/* Convenience functions for common validation patterns */

/* Standard 2D validation */
void validate_2d_sequence(double d2, const char* pulse_list[], int n_pulses) {
    NMRValidation validator = init_nmr_validation();
    add_evolution_time(&validator, d2);
    
    for (int i = 0; i < n_pulses; i++) {
        char param_name[32];
        snprintf(param_name, sizeof(param_name), "pw%s", pulse_list[i]);
        add_pulse_time(&validator, pulse_list[i], getval(param_name));
    }
    
    add_cycle_time(&validator, getval("d1") + getval("ad") + getval("rd") + getval("at"));
    validate_all_parameters(&validator);
}

/* Standard 3D validation */
void validate_3d_sequence(double d2, double d3, const char* cp_list[], int n_cp) {
    NMRValidation validator = init_nmr_validation();
    add_evolution_time(&validator, d2);
    add_evolution_time(&validator, d3);
    add_standard_pulses(&validator);
    add_cp_times(&validator, cp_list, n_cp);
    add_cycle_time(&validator, getval("d1") + getval("ad") + getval("rd") + getval("at"));
    validate_all_parameters(&validator);
}

/* Error reporting macros */
#define NMR_ABORT(msg, ...) do { \
    printf("ABORT: " msg "\n", ##__VA_ARGS__); \
    psg_abort(1); \
} while(0)

#define NMR_WARN(msg, ...) \
    printf("WARNING: " msg "\n", ##__VA_ARGS__)

#define NMR_INFO(msg, ...) \
    printf("INFO: " msg "\n", ##__VA_ARGS__)

#endif /* BIOSOLIDVALIDATION_H */
