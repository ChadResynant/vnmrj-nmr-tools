/*******************************************************************************
 * nmr_system_limits.h
 *
 * NMR System Hardware Limits and Validation Framework
 *
 * PURPOSE:
 *   Defines hardware limits for NMR spectrometer systems and provides
 *   validation functions to ensure pulse sequences operate within safe
 *   parameters. This is a critical safety component inspired by PulSeq
 *   architecture.
 *
 * SAFETY CONTEXT:
 *   - C-detected sequences: 5% duty cycle maximum (high-power decoupling)
 *   - H-detected sequences: 10-15% duty cycle maximum (low-power decoupling)
 *   - Exceeding limits can cause $50,000-100,000 probe damage
 *   - See SAFETY_STANDARDS.md for detailed rationale
 *
 * ARCHITECTURE:
 *   This header is part of Phase 2 of the NMR Pulse Sequence Refactoring
 *   Implementation Plan (see REFACTORING_IMPLEMENTATION_PLAN.md, Section 2.1)
 *
 * USAGE EXAMPLE:
 *   ```c
 *   #include "nmr_system_limits.h"
 *
 *   // Initialize system limits (typically in pulsesequence())
 *   NMRSystemLimits limits;
 *   init_system_limits(&limits);
 *
 *   // Validate against limits before creating pulses
 *   if (!validate_rf_pulse(&limits, 0, pw, tpwr)) {
 *       psg_abort(1);
 *   }
 *   ```
 *
 * COMPATIBILITY:
 *   - VnmrJ/OpenVNMRJ pulse sequence library (PSG)
 *   - Future Resynant spectrometer compatibility
 *   - Standard C99 with PSG extensions
 *
 * CREATED: November 9, 2025
 * AUTHOR: NMR Pulse Sequence Refactoring Team
 * VERSION: 1.0.0
 ******************************************************************************/

#ifndef NMR_SYSTEM_LIMITS_H
#define NMR_SYSTEM_LIMITS_H

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>

/*******************************************************************************
 * CONSTANTS AND DEFINITIONS
 ******************************************************************************/

/* RF Channel Indices */
#define RF_CHANNEL_OBS   0   /* Observe channel (typically 1H) */
#define RF_CHANNEL_DEC   1   /* Decoupler channel (typically 13C) */
#define RF_CHANNEL_DEC2  2   /* Second decoupler (typically 15N) */
#define RF_CHANNEL_COUNT 3   /* Total number of RF channels */

/* Detection Type Constants */
#define DUTY_LIMIT_H_DETECTED  0.15   /* 15% for H-detected sequences */
#define DUTY_LIMIT_H_DETECTED_CONSERVATIVE 0.10  /* 10% conservative limit */
#define DUTY_LIMIT_C_DETECTED  0.05   /* 5% for C-detected sequences */

/* Default Hardware Limits (conservative values) */
#define DEFAULT_MAX_GRAD       100.0  /* G/cm - typical MAS probe limit */
#define DEFAULT_MAX_SLEW       200.0  /* G/cm/ms - typical slew rate */
#define DEFAULT_RF_DEAD_TIME   2.0    /* us - typical RF blanking time */
#define DEFAULT_ADC_DEAD_TIME  5.0    /* us - typical ADC settling time */
#define DEFAULT_GRAD_RASTER    4.0    /* us - typical gradient raster time */

/* Default RF Limits */
#define DEFAULT_MAX_AMPLITUDE  100000.0  /* Hz - typical nutation frequency */
#define DEFAULT_MAX_POWER      63.0      /* dB - typical linear amplifier limit */
#define DEFAULT_MAX_DURATION   100.0     /* ms - maximum continuous RF pulse */

/* Default Sample Rate Limits */
#define DEFAULT_MIN_SRATE      1000.0    /* Hz - minimum ADC sample rate */
#define DEFAULT_MAX_SRATE      1000000.0 /* Hz - maximum ADC sample rate */

/* String Buffer Sizes */
#define PROBE_NAME_LENGTH      64
#define ERROR_MESSAGE_LENGTH   256

/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/

/**
 * RF Channel Limits
 *
 * Defines the hardware limits for a single RF channel. Each spectrometer
 * typically has 2-3 RF channels (obs, dec, dec2).
 *
 * FIELDS:
 *   max_amplitude - Maximum RF field strength in Hz (nutation frequency)
 *                   Typical values: 50-100 kHz for high-power channels
 *   max_power     - Maximum power level in dB (linear power amplifier)
 *                   Typical range: 0-63 dB (depends on amplifier)
 *   max_duration  - Maximum continuous RF pulse duration in ms
 *                   Prevents overheating of coils and amplifiers
 *   duty_limit    - Maximum duty cycle as a fraction (0.0-1.0)
 *                   CRITICAL: 0.05 for C-detected, 0.10-0.15 for H-detected
 *
 * NOTES:
 *   - max_amplitude is frequency-dependent (calibrate for each nucleus)
 *   - duty_limit is the most critical safety parameter
 *   - Exceeding duty_limit can cause catastrophic probe damage
 */
typedef struct {
    double max_amplitude;  /* Hz - maximum RF field strength */
    double max_power;      /* dB - maximum power level */
    double max_duration;   /* ms - maximum continuous pulse duration */
    double duty_limit;     /* fraction - maximum duty cycle (0.0-1.0) */
} RFChannelLimits;

/**
 * NMR System Limits
 *
 * Complete hardware limits for an NMR spectrometer system. This structure
 * encapsulates all timing, RF, gradient, and probe-specific constraints.
 *
 * GRADIENT LIMITS:
 *   max_grad      - Maximum gradient strength in G/cm
 *                   Typical: 50-100 G/cm for MAS probes
 *   max_slew      - Maximum gradient slew rate in G/cm/ms
 *                   Limits how fast gradients can be switched
 *   grad_raster   - Gradient time raster in microseconds
 *                   All gradient events must align to this grid
 *
 * TIMING LIMITS:
 *   rf_dead_time  - RF blanking/recovery time in microseconds
 *                   Delay needed after RF pulse before acquisition
 *   adc_dead_time - ADC settling time in microseconds
 *                   Time for receiver to stabilize after switching
 *
 * RF CHANNEL LIMITS:
 *   rf[3]         - Array of RF limits for each channel
 *                   [0] = obs (typically 1H)
 *                   [1] = dec (typically 13C)
 *                   [2] = dec2 (typically 15N)
 *
 * PROBE-SPECIFIC LIMITS:
 *   probe_name        - String identifier for the probe
 *   probe_duty_limit  - Overall probe duty cycle limit
 *                       May be stricter than individual channel limits
 *   min_srate         - Minimum ADC sample rate in Hz
 *   max_srate         - Maximum ADC sample rate in Hz
 *
 * USAGE:
 *   This structure should be initialized once per sequence using
 *   init_system_limits() and then used to validate all pulse elements.
 *
 * THREAD SAFETY:
 *   Not thread-safe. Should be initialized in main sequence thread.
 */
typedef struct {
    /* Gradient Limits */
    double max_grad;       /* G/cm - maximum gradient strength */
    double max_slew;       /* G/cm/ms - maximum gradient slew rate */
    double grad_raster;    /* us - gradient time raster period */

    /* Timing Limits */
    double rf_dead_time;   /* us - RF blanking/recovery time */
    double adc_dead_time;  /* us - ADC settling time */

    /* Channel-Specific RF Limits (3 channels: obs, dec, dec2) */
    RFChannelLimits rf[RF_CHANNEL_COUNT];

    /* Probe-Specific Limits */
    char probe_name[PROBE_NAME_LENGTH];  /* Probe identifier string */
    double probe_duty_limit;              /* Overall probe duty cycle limit */
    double min_srate;                     /* Hz - minimum sample rate */
    double max_srate;                     /* Hz - maximum sample rate */
} NMRSystemLimits;

/**
 * Validation Result
 *
 * Return structure for validation functions providing detailed error
 * information when validation fails.
 *
 * FIELDS:
 *   is_valid      - Boolean: 1 if valid, 0 if validation failed
 *   error_code    - Numeric error code for programmatic handling
 *   error_message - Human-readable error description
 *
 * ERROR CODES:
 *   0 = No error (validation passed)
 *   1 = RF amplitude exceeds limit
 *   2 = RF power exceeds limit
 *   3 = RF duration exceeds limit
 *   4 = Duty cycle exceeds limit
 *   5 = Gradient strength exceeds limit
 *   6 = Gradient slew rate exceeds limit
 *   7 = Sample rate out of range
 *   8 = Timing resolution violation
 */
typedef struct {
    int is_valid;
    int error_code;
    char error_message[ERROR_MESSAGE_LENGTH];
} ValidationResult;

/*******************************************************************************
 * FUNCTION DECLARATIONS
 ******************************************************************************/

/**
 * Initialize NMR System Limits
 *
 * Initializes the NMRSystemLimits structure with default or probe-specific
 * hardware limits. This function should be called once at the beginning of
 * each pulse sequence.
 *
 * BEHAVIOR:
 *   1. Queries VnmrJ parameters to determine probe type
 *   2. Sets conservative default limits
 *   3. Overrides with probe-specific limits if available
 *   4. Sets detection-type-appropriate duty cycle limits
 *
 * PARAMETERS:
 *   limits - Pointer to NMRSystemLimits structure to initialize
 *
 * RETURNS:
 *   0 on success, -1 on error
 *
 * VNMRJ PARAMETERS QUERIED:
 *   - probe        : Probe name/identifier
 *   - tn, dn, dn2  : Nucleus names for each RF channel
 *   - rftype       : RF amplifier type
 *   - gradtype     : Gradient system type
 *
 * EXAMPLE:
 *   ```c
 *   NMRSystemLimits limits;
 *   if (init_system_limits(&limits) != 0) {
 *       text_error("Failed to initialize system limits");
 *       psg_abort(1);
 *   }
 *   ```
 *
 * NOTES:
 *   - Uses conservative defaults if probe-specific data unavailable
 *   - Future versions will read from system configuration files
 *   - Can be extended to support Resynant spectrometer formats
 */
int init_system_limits(NMRSystemLimits* limits);

/**
 * Validate RF Pulse Against Limits
 *
 * Validates a single RF pulse against channel-specific hardware limits.
 * Checks amplitude, power level, and duration constraints.
 *
 * PARAMETERS:
 *   limits  - Pointer to initialized NMRSystemLimits structure
 *   channel - RF channel index (0=obs, 1=dec, 2=dec2)
 *   pw      - Pulse width in microseconds
 *   power   - Power level in dB
 *
 * RETURNS:
 *   ValidationResult structure with validation status and error details
 *
 * CHECKS PERFORMED:
 *   1. Channel index in valid range (0-2)
 *   2. Pulse width > 0 and < max_duration
 *   3. Power level <= max_power
 *   4. Implied RF amplitude within max_amplitude
 *
 * EXAMPLE:
 *   ```c
 *   ValidationResult result = validate_rf_pulse(&limits, RF_CHANNEL_OBS,
 *                                                pwH90, tpwr);
 *   if (!result.is_valid) {
 *       printf("RF pulse validation failed: %s\n", result.error_message);
 *       psg_abort(1);
 *   }
 *   ```
 */
ValidationResult validate_rf_pulse(const NMRSystemLimits* limits,
                                   int channel,
                                   double pw,
                                   double power);

/**
 * Validate Duty Cycle Against Limits
 *
 * Validates total duty cycle for a pulse sequence against both channel-
 * specific and probe-level duty cycle limits.
 *
 * CRITICAL SAFETY FUNCTION:
 *   This is the most important validation function. Duty cycle violations
 *   are the #1 cause of probe damage in solid-state NMR.
 *
 * PARAMETERS:
 *   limits       - Pointer to initialized NMRSystemLimits structure
 *   channel      - RF channel index (0=obs, 1=dec, 2=dec2)
 *   total_rf_time - Total RF time in microseconds (sum of all pulses)
 *   d1           - Relaxation delay in seconds
 *
 * RETURNS:
 *   ValidationResult structure with validation status and error details
 *
 * CALCULATION:
 *   duty_cycle = total_rf_time / (total_rf_time + d1 + dead_time)
 *
 * EXAMPLE:
 *   ```c
 *   double total_rf = pwH90 + tHX + pwX90 + tXmix;  // microseconds
 *   ValidationResult result = validate_duty_cycle(&limits, RF_CHANNEL_DEC,
 *                                                  total_rf, d1);
 *   if (!result.is_valid) {
 *       printf("ERROR: %s\n", result.error_message);
 *       printf("Increase d1 or reduce mixing times\n");
 *       psg_abort(1);
 *   }
 *   ```
 *
 * NOTES:
 *   - For C-detected sequences, check against dec channel (index 1)
 *   - For H-detected sequences, check against obs channel (index 0)
 *   - total_rf_time should include ALL RF: pulses, CP, decoupling, mixing
 */
ValidationResult validate_duty_cycle(const NMRSystemLimits* limits,
                                     int channel,
                                     double total_rf_time,
                                     double d1);

/**
 * Validate Gradient Against Limits
 *
 * Validates gradient parameters against hardware limits including maximum
 * gradient strength and slew rate.
 *
 * PARAMETERS:
 *   limits    - Pointer to initialized NMRSystemLimits structure
 *   gamp      - Gradient amplitude in G/cm
 *   gtime     - Gradient duration in microseconds
 *   prev_gamp - Previous gradient amplitude for slew rate calculation
 *   ramp_time - Ramp time between gradients in microseconds
 *
 * RETURNS:
 *   ValidationResult structure with validation status and error details
 *
 * CHECKS PERFORMED:
 *   1. Gradient amplitude <= max_grad
 *   2. Gradient time aligned to grad_raster
 *   3. Slew rate = |gamp - prev_gamp| / ramp_time <= max_slew
 *
 * EXAMPLE:
 *   ```c
 *   ValidationResult result = validate_gradient(&limits, gzlvl1, gt1,
 *                                               0.0, grise);
 *   if (!result.is_valid) {
 *       printf("Gradient validation failed: %s\n", result.error_message);
 *       psg_abort(1);
 *   }
 *   ```
 */
ValidationResult validate_gradient(const NMRSystemLimits* limits,
                                   double gamp,
                                   double gtime,
                                   double prev_gamp,
                                   double ramp_time);

/**
 * Validate Sample Rate Against Limits
 *
 * Validates ADC sample rate (spectral width) against hardware limits.
 *
 * PARAMETERS:
 *   limits - Pointer to initialized NMRSystemLimits structure
 *   sw     - Spectral width in Hz (equals sample rate for quadrature detection)
 *
 * RETURNS:
 *   ValidationResult structure with validation status and error details
 *
 * CHECKS PERFORMED:
 *   1. sw >= min_srate
 *   2. sw <= max_srate
 *
 * EXAMPLE:
 *   ```c
 *   ValidationResult result = validate_sample_rate(&limits, sw);
 *   if (!result.is_valid) {
 *       printf("Sample rate validation failed: %s\n", result.error_message);
 *       psg_abort(1);
 *   }
 *   ```
 */
ValidationResult validate_sample_rate(const NMRSystemLimits* limits,
                                      double sw);

/**
 * Validate All Parameters Against Limits
 *
 * Convenience function that performs comprehensive validation of all
 * sequence parameters against hardware limits.
 *
 * PARAMETERS:
 *   limits - Pointer to initialized NMRSystemLimits structure
 *
 * RETURNS:
 *   ValidationResult structure with validation status and error details
 *
 * CHECKS PERFORMED:
 *   1. All RF channel parameters
 *   2. Duty cycle for all channels
 *   3. Gradient parameters
 *   4. Sample rate
 *   5. Timing resolution
 *
 * EXAMPLE:
 *   ```c
 *   NMRSystemLimits limits;
 *   init_system_limits(&limits);
 *
 *   // ... set up all pulse sequence parameters ...
 *
 *   ValidationResult result = validate_all_parameters(&limits);
 *   if (!result.is_valid) {
 *       printf("Parameter validation failed: %s\n", result.error_message);
 *       psg_abort(1);
 *   }
 *   ```
 *
 * NOTES:
 *   - This function queries VnmrJ parameters directly
 *   - Use for simple validation; complex sequences may need custom checks
 *   - Returns on first validation failure (does not check all parameters)
 */
ValidationResult validate_all_parameters(const NMRSystemLimits* limits);

/**
 * Print System Limits
 *
 * Diagnostic function to print all system limits to stdout. Useful for
 * debugging and verifying correct initialization.
 *
 * PARAMETERS:
 *   limits - Pointer to initialized NMRSystemLimits structure
 *
 * OUTPUT FORMAT:
 *   Formatted table showing all limits by category:
 *   - Gradient limits
 *   - Timing limits
 *   - RF limits by channel
 *   - Probe-specific limits
 *
 * EXAMPLE:
 *   ```c
 *   NMRSystemLimits limits;
 *   init_system_limits(&limits);
 *   print_system_limits(&limits);  // Print to verify initialization
 *   ```
 */
void print_system_limits(const NMRSystemLimits* limits);

/**
 * Set Detection Type Duty Limits
 *
 * Helper function to set appropriate duty cycle limits based on detection
 * type (H-detected vs C-detected).
 *
 * PARAMETERS:
 *   limits        - Pointer to NMRSystemLimits structure to modify
 *   is_h_detected - 1 if H-detected sequence, 0 if C-detected
 *   conservative  - 1 to use conservative limits, 0 for standard
 *
 * BEHAVIOR:
 *   - H-detected: Sets 15% limit (or 10% if conservative)
 *   - C-detected: Sets 5% limit (always conservative)
 *
 * EXAMPLE:
 *   ```c
 *   NMRSystemLimits limits;
 *   init_system_limits(&limits);
 *
 *   // For C-detected sequence (e.g., hXX, hYXX)
 *   set_detection_type_duty_limits(&limits, 0, 0);
 *
 *   // For H-detected sequence (e.g., hXH, HhXH)
 *   set_detection_type_duty_limits(&limits, 1, 0);
 *   ```
 *
 * NOTES:
 *   - C-detected sequences ALWAYS use 5% limit (conservative flag ignored)
 *   - See SAFETY_STANDARDS.md for detailed rationale
 *   - Detection type usually inferred from sequence name (last letter)
 */
void set_detection_type_duty_limits(NMRSystemLimits* limits,
                                   int is_h_detected,
                                   int conservative);

/*******************************************************************************
 * INLINE HELPER FUNCTIONS
 ******************************************************************************/

/**
 * Calculate Duty Cycle
 *
 * Inline helper to calculate duty cycle from RF time and delays.
 *
 * FORMULA:
 *   duty_cycle = total_rf_time / (total_rf_time + d1 + overhead)
 *
 * Where:
 *   total_rf_time = sum of all RF pulse widths (microseconds)
 *   d1 = relaxation delay (converted to microseconds)
 *   overhead = small constant for dead times (4.0 microseconds)
 *
 * PARAMETERS:
 *   total_rf_us - Total RF time in microseconds
 *   d1_seconds  - Relaxation delay in seconds
 *
 * RETURNS:
 *   Duty cycle as a fraction (0.0-1.0)
 */
static inline double calculate_duty_cycle(double total_rf_us, double d1_seconds)
{
    double d1_us = d1_seconds * 1.0e6;  /* Convert d1 to microseconds */
    double overhead_us = 4.0;           /* Small overhead for dead times */

    return total_rf_us / (total_rf_us + d1_us + overhead_us);
}

/**
 * Convert Power to Amplitude
 *
 * Inline helper to estimate RF amplitude from power level and pulse width.
 * This is an approximation based on typical calibrations.
 *
 * FORMULA:
 *   amplitude_Hz = 10^((power_dB - ref_power) / 20) * ref_amplitude
 *
 * PARAMETERS:
 *   power_db      - Power level in dB
 *   ref_power_db  - Reference power level in dB
 *   ref_amplitude - Reference RF amplitude in Hz
 *
 * RETURNS:
 *   Estimated RF amplitude in Hz
 *
 * NOTES:
 *   - This is an approximation; actual calibration varies by probe
 *   - Use for validation only; do not use for precise pulse calibrations
 */
static inline double convert_power_to_amplitude(double power_db,
                                                double ref_power_db,
                                                double ref_amplitude)
{
    double power_ratio = (power_db - ref_power_db) / 20.0;
    return ref_amplitude * pow(10.0, power_ratio);
}

/*******************************************************************************
 * USAGE NOTES
 ******************************************************************************/

/*
 * INTEGRATION WITH EXISTING SEQUENCES:
 *
 * To add limit checking to an existing pulse sequence:
 *
 * 1. Include this header:
 *    #include "nmr_system_limits.h"
 *
 * 2. Initialize limits at start of pulsesequence():
 *    NMRSystemLimits limits;
 *    init_system_limits(&limits);
 *    set_detection_type_duty_limits(&limits, 0, 0);  // For C-detected
 *
 * 3. Validate individual pulses:
 *    ValidationResult result = validate_rf_pulse(&limits, RF_CHANNEL_OBS,
 *                                                pwH90, tpwr);
 *    if (!result.is_valid) {
 *        text_error(result.error_message);
 *        psg_abort(1);
 *    }
 *
 * 4. Validate duty cycle before sequence execution:
 *    double total_rf = pwH90 + tHX + pwX90 + tXmix;  // Sum all RF times
 *    result = validate_duty_cycle(&limits, RF_CHANNEL_DEC, total_rf, d1);
 *    if (!result.is_valid) {
 *        printf("%s\n", result.error_message);
 *        printf("Increase d1 or reduce mixing times\n");
 *        psg_abort(1);
 *    }
 *
 * MIGRATION PATH:
 *
 * This framework is designed to gradually replace ad-hoc duty cycle checks
 * scattered throughout individual sequences. Existing sequences can adopt
 * this incrementally:
 *
 * Phase 1: Add init_system_limits() and keep existing checks
 * Phase 2: Replace manual duty cycle calculations with validate_duty_cycle()
 * Phase 3: Add validate_rf_pulse() for all RF events
 * Phase 4: Use validate_all_parameters() for comprehensive checking
 *
 * FUTURE EXTENSIONS:
 *
 * - Read limits from system configuration files
 * - Support for probe-specific limit databases
 * - Integration with Resynant spectrometer formats
 * - Real-time duty cycle monitoring during sequence execution
 * - Automatic parameter adjustment to meet limits
 */

#endif /* NMR_SYSTEM_LIMITS_H */
