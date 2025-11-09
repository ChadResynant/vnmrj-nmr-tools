/*
 * nmr_make_functions.h - PulSeq-Inspired Make Functions for NMR Sequences
 *
 * Implements validated pulse element generators following the "make functions"
 * pattern from PulSeq, adapted for VnmrJ/OpenVNMRJ solid-state NMR.
 *
 * Design Philosophy:
 * - Validate parameters at creation time (fail early)
 * - Check against hardware limits (NMRSystemLimits structure)
 * - Return clear error codes or abort with descriptive messages
 * - Provide safe defaults while allowing customization
 *
 * Key Features:
 * - Hardware-aware pulse generation
 * - Automatic duty cycle tracking
 * - Channel assignment validation
 * - Rotor synchronization support
 * - Integration with existing biosolid*.h framework
 *
 * Usage Pattern:
 *   NMRSystemLimits limits = init_system_limits();
 *   NMRPulse pulse = makePulse(90.0, "H", "obs", &limits);
 *   NMRCP cp = makeCP("HX", 2000.0, 50000.0, 50000.0, &limits);
 *   execute_pulse(&pulse);
 *   execute_cp(&cp);
 *
 * Safety Standards:
 * - C-detected sequences: 5% duty cycle maximum
 * - H-detected sequences: 10-15% duty cycle maximum
 * - Power limits enforced per channel
 * - Continuous RF time limits
 *
 * Created: November 9, 2025
 * Compatible with: VnmrJ/OpenVNMRJ biosolid pulse sequence framework
 * Reference: REFACTORING_IMPLEMENTATION_PLAN.md Section 2.2
 */

#ifndef NMR_MAKE_FUNCTIONS_H
#define NMR_MAKE_FUNCTIONS_H

#include "standard.h"
#include "biosolidstandard.h"

/*==============================================================================
 * System Limits Structure (Section 2.1 of Refactoring Plan)
 *============================================================================*/

typedef struct {
    // Gradient limits (for future MRI/microimaging integration)
    double max_grad;          // G/cm
    double max_slew;          // G/cm/ms
    double grad_raster;       // us

    // Timing limits
    double rf_dead_time;      // us - dead time between pulses
    double adc_dead_time;     // us - ADC settling time

    // Channel-specific RF limits
    struct {
        char channel_name[8];      // "obs", "dec", "dec2", "dec3"
        char nucleus_name[8];      // "H", "C", "N", etc.
        double max_amplitude;      // Hz (RF field strength)
        double max_power;          // dB (transmitter power)
        double max_duration;       // ms (longest single pulse)
        double duty_limit;         // fraction (0.05 = 5%)
        double max_continuous_rf;  // ms (longest continuous RF period)
    } rf[4];  // obs, dec, dec2, dec3

    // Probe-specific limits
    char probe_name[64];
    double probe_duty_limit;   // Overall probe duty cycle limit
    double min_srate;          // Minimum spinning rate (Hz)
    double max_srate;          // Maximum spinning rate (Hz)

    // Validation flags
    int enforce_limits;        // 1 = abort on violation, 0 = warn only
    int verbose_mode;          // 1 = print detailed validation info

} NMRSystemLimits;

/*==============================================================================
 * Pulse Element Structures
 *============================================================================*/

/* Basic hard pulse */
typedef struct {
    double flip_angle;         // degrees
    double pulse_width;        // seconds
    double rf_amplitude;       // Hz
    double rf_power;           // dB
    char nucleus[8];           // "H", "C", "N", etc.
    char channel[8];           // "obs", "dec", "dec2", "dec3"
    char phase_table[32];      // Phase table name (e.g., "phH90")
    int is_valid;              // 1 if validated, 0 otherwise
    char error_msg[256];       // Error message if invalid
} NMRPulse;

/* Shaped/adiabatic pulse */
typedef struct {
    char shape_name[64];       // Shape file name
    double pulse_width;        // seconds
    double rf_amplitude;       // Hz
    double rf_power;           // dB
    char nucleus[8];
    char channel[8];
    char phase_table[32];
    int is_adiabatic;          // 1 for adiabatic, 0 for amplitude-modulated
    double bandwidth;          // Hz (for adiabatic pulses)
    int is_valid;
    char error_msg[256];
} NMRShapedPulse;

/* Cross-polarization block */
typedef struct {
    char transfer_name[16];    // "HX", "HY", "YX", "XH", etc.
    double contact_time;       // seconds
    double rf_amplitude_from;  // Hz (source nucleus)
    double rf_amplitude_to;    // Hz (target nucleus)
    double rf_power_from;      // dB
    double rf_power_to;        // dB
    char nucleus_from[8];      // "H", "N", etc.
    char nucleus_to[8];        // "C", "H", etc.
    char channel_from[8];      // "dec", "dec2", etc.
    char channel_to[8];        // "obs", "dec", etc.
    int use_ramp;              // 1 = use CP ramp, 0 = constant amplitude
    char ramp_shape[64];       // Ramp shape name if use_ramp = 1
    int is_valid;
    char error_msg[256];
} NMRCP;

/* Mixing period */
typedef struct {
    char mixing_type[32];      // "DARR", "RFDR", "RAD", "DREAM", etc.
    double mixing_time;        // seconds
    double rf_amplitude;       // Hz (for DARR/RAD)
    double rf_power;           // dB
    char nucleus[8];           // Nucleus for assisted mixing
    char channel[8];
    int num_cycles;            // Number of rotor cycles (if rotor-synchronized)
    double srate;              // Spinning rate (Hz)
    int is_rotor_sync;         // 1 = synchronized to rotor, 0 = fixed time
    int is_valid;
    char error_msg[256];
} NMRMixing;

/* Decoupling period */
typedef struct {
    char decoupling_type[32];  // "TPPM", "SPINAL", "WALTZ", "CW", "MPSEQ"
    double rf_amplitude;       // Hz (average RF field)
    double rf_power;           // dB
    char nucleus[8];
    char channel[8];
    double phase_angle;        // degrees (for TPPM)
    double pulse_width;        // us (for TPPM/SPINAL)
    char shape_name[64];       // Shape for SPINAL/MPSEQ
    int is_valid;
    char error_msg[256];
} NMRDecoupling;

/* Evolution period */
typedef struct {
    char evolution_name[16];   // "t1", "t2", "t3"
    double evolution_time;     // seconds
    int is_constant_time;      // 1 = constant time, 0 = variable
    int is_rotor_sync;         // 1 = synchronized to rotor
    int num_cycles;            // Number of rotor cycles (if synchronized)
    double srate;              // Spinning rate (Hz)
    char decoupling_nucleus[8];// Nucleus for decoupling during evolution
    char decoupling_channel[8];// Channel for decoupling
    int has_decoupling;        // 1 = decoupling on, 0 = no decoupling
    int is_valid;
    char error_msg[256];
} NMREvolution;

/*==============================================================================
 * System Limits Initialization
 *============================================================================*/

/**
 * init_system_limits - Initialize NMRSystemLimits with safe defaults
 *
 * Returns NMRSystemLimits structure with conservative defaults suitable
 * for most solid-state NMR probes.
 *
 * Default Limits:
 * - C-detected (obs): 5% duty cycle, 100 kHz max RF
 * - H-detected (dec): 10% duty cycle, 150 kHz max RF
 * - N channel (dec2): 5% duty cycle, 80 kHz max RF
 *
 * Example:
 *   NMRSystemLimits limits = init_system_limits();
 *   limits.rf[0].duty_limit = 0.05;  // 5% for C-detected
 */
NMRSystemLimits init_system_limits(void);

/**
 * init_system_limits_from_probe - Initialize limits from probe parameters
 *
 * Reads probe configuration from VnmrJ parameters and sets appropriate
 * limits based on probe type.
 *
 * Parameters:
 *   probe_name - Name of probe (e.g., "1.6mm HXY MAS", "3.2mm HCN")
 *
 * Returns initialized NMRSystemLimits structure
 */
NMRSystemLimits init_system_limits_from_probe(const char* probe_name);

/**
 * set_channel_limits - Configure limits for a specific RF channel
 *
 * Parameters:
 *   limits - Pointer to NMRSystemLimits structure
 *   channel_idx - Channel index (0=obs, 1=dec, 2=dec2, 3=dec3)
 *   max_rf_hz - Maximum RF amplitude (Hz)
 *   duty_limit - Duty cycle limit (0.05 = 5%)
 */
void set_channel_limits(NMRSystemLimits* limits, int channel_idx,
                        double max_rf_hz, double duty_limit);

/*==============================================================================
 * Make Functions - Pulse Elements
 *============================================================================*/

/**
 * makePulse - Create validated hard pulse
 *
 * Creates a hard rectangular pulse with parameter validation against
 * hardware limits. Automatically calculates pulse width from flip angle
 * and RF amplitude.
 *
 * Parameters:
 *   flip_angle - Pulse flip angle in degrees (e.g., 90.0)
 *   nucleus - Target nucleus ("H", "C", "N", etc.)
 *   channel - RF channel ("obs", "dec", "dec2", "dec3")
 *   limits - Pointer to NMRSystemLimits for validation
 *
 * Returns:
 *   NMRPulse structure (check is_valid field)
 *
 * Validation:
 *   - Flip angle: 0-360 degrees (warning if >360)
 *   - RF amplitude: within channel limits
 *   - Pulse width: within max_duration limit
 *   - Channel assignment: valid for nucleus
 *
 * Example:
 *   NMRSystemLimits limits = init_system_limits();
 *   NMRPulse h90 = makePulse(90.0, "H", "dec", &limits);
 *   if (!h90.is_valid) {
 *       printf("Error: %s\n", h90.error_msg);
 *       psg_abort(1);
 *   }
 *   execute_pulse(&h90);
 */
NMRPulse makePulse(double flip_angle, const char* nucleus,
                   const char* channel, NMRSystemLimits* limits);

/**
 * makeShapedPulse - Create validated shaped/adiabatic pulse
 *
 * Creates a shaped or adiabatic pulse with automatic bandwidth and
 * amplitude validation.
 *
 * Parameters:
 *   shape_name - Name of shape file (e.g., "tanh_adiabatic.RF")
 *   pulse_width - Pulse duration in microseconds
 *   rf_amplitude - RF amplitude in Hz
 *   nucleus - Target nucleus
 *   channel - RF channel
 *   is_adiabatic - 1 for adiabatic, 0 for amplitude-modulated
 *   limits - Pointer to NMRSystemLimits
 *
 * Returns:
 *   NMRShapedPulse structure (check is_valid field)
 *
 * Example:
 *   NMRShapedPulse adiabatic = makeShapedPulse("tanh90.RF", 2000.0,
 *                                               50000.0, "C", "obs", 1, &limits);
 */
NMRShapedPulse makeShapedPulse(const char* shape_name, double pulse_width_us,
                               double rf_amplitude, const char* nucleus,
                               const char* channel, int is_adiabatic,
                               NMRSystemLimits* limits);

/*==============================================================================
 * Make Functions - Transfer Elements
 *============================================================================*/

/**
 * makeCP - Create validated cross-polarization block
 *
 * Creates a CP transfer with Hartmann-Hahn matching condition validation.
 * Automatically checks matching condition and power levels.
 *
 * Parameters:
 *   transfer_name - Transfer type ("HX", "HY", "YX", "XH", etc.)
 *   contact_time_us - Contact time in microseconds
 *   rf_from_hz - RF amplitude on source nucleus (Hz)
 *   rf_to_hz - RF amplitude on target nucleus (Hz)
 *   limits - Pointer to NMRSystemLimits
 *
 * Returns:
 *   NMRCP structure (check is_valid field)
 *
 * Validation:
 *   - Contact time: 100 us - 20 ms typical range
 *   - Hartmann-Hahn matching: |rf_from - rf_to| < srate (within ±1 rotor period)
 *   - RF amplitudes: within channel limits
 *   - Channel assignments: correct for transfer type
 *
 * Example:
 *   // H→C CP at 10 kHz MAS with 50 kHz on both channels
 *   NMRCP hx = makeCP("HX", 2000.0, 50000.0, 50000.0, &limits);
 *   if (!hx.is_valid) {
 *       printf("Error: %s\n", hx.error_msg);
 *       psg_abort(1);
 *   }
 */
NMRCP makeCP(const char* transfer_name, double contact_time_us,
             double rf_from_hz, double rf_to_hz, NMRSystemLimits* limits);

/**
 * makeCP_with_ramp - Create CP with amplitude ramp
 *
 * Creates a ramped CP transfer (typically on the target nucleus) for
 * improved polarization transfer across broader matching conditions.
 *
 * Parameters:
 *   transfer_name - Transfer type
 *   contact_time_us - Contact time in microseconds
 *   rf_from_hz - RF on source (constant)
 *   rf_to_center_hz - Center RF on target (ramped)
 *   ramp_width_hz - Ramp width (±Hz from center)
 *   ramp_shape - Ramp shape ("linear", "tangent", "sine")
 *   limits - Pointer to NMRSystemLimits
 *
 * Returns:
 *   NMRCP structure with ramping enabled
 *
 * Example:
 *   // H→C CP with ±5 kHz tangent ramp on C channel
 *   NMRCP hx = makeCP_with_ramp("HX", 2000.0, 50000.0, 50000.0,
 *                                5000.0, "tangent", &limits);
 */
NMRCP makeCP_with_ramp(const char* transfer_name, double contact_time_us,
                       double rf_from_hz, double rf_to_center_hz,
                       double ramp_width_hz, const char* ramp_shape,
                       NMRSystemLimits* limits);

/*==============================================================================
 * Make Functions - Mixing and Decoupling
 *============================================================================*/

/**
 * makeMixing - Create validated mixing period
 *
 * Creates a homonuclear mixing period (DARR, RFDR, RAD, DREAM, etc.)
 * with optional rotor synchronization.
 *
 * Parameters:
 *   mixing_type - Type of mixing ("DARR", "RFDR", "RAD", "DREAM")
 *   mixing_time_ms - Mixing time in milliseconds
 *   rf_amplitude - RF amplitude in Hz (0 for RFDR, ~10-20 kHz for DARR)
 *   nucleus - Nucleus for mixing (typically "C")
 *   rotor_sync - 1 for rotor-synchronized, 0 for fixed time
 *   limits - Pointer to NMRSystemLimits
 *
 * Returns:
 *   NMRMixing structure (check is_valid field)
 *
 * Validation:
 *   - Mixing time: 1-200 ms typical range
 *   - DARR RF: typically 5-20 kHz (should match srate)
 *   - RFDR: RF amplitude must be 0 (field-free)
 *   - Rotor sync: srate must be non-zero if enabled
 *
 * Example:
 *   // 50 ms DARR mixing at 12.5 kHz MAS with 12.5 kHz RF
 *   NMRMixing darr = makeMixing("DARR", 50.0, 12500.0, "C", 1, &limits);
 */
NMRMixing makeMixing(const char* mixing_type, double mixing_time_ms,
                     double rf_amplitude, const char* nucleus,
                     int rotor_sync, NMRSystemLimits* limits);

/**
 * makeDecoupling - Create validated decoupling sequence
 *
 * Creates a heteronuclear decoupling sequence with appropriate phase
 * cycling and pulse widths.
 *
 * Parameters:
 *   decoupling_type - Type ("TPPM", "SPINAL", "WALTZ", "CW", "MPSEQ")
 *   rf_amplitude - RF amplitude in Hz
 *   nucleus - Decoupled nucleus (typically "H")
 *   channel - RF channel
 *   limits - Pointer to NMRSystemLimits
 *
 * Returns:
 *   NMRDecoupling structure (check is_valid field)
 *
 * Validation:
 *   - RF amplitude: within channel limits
 *   - TPPM phase: typically 10-20 degrees
 *   - TPPM pulse width: typically 5-10 us
 *   - Decoupling strength: adequate for resolution
 *
 * Example:
 *   // 100 kHz TPPM decoupling on H channel
 *   NMRDecoupling tppm = makeDecoupling("TPPM", 100000.0, "H", "dec", &limits);
 */
NMRDecoupling makeDecoupling(const char* decoupling_type, double rf_amplitude,
                             const char* nucleus, const char* channel,
                             NMRSystemLimits* limits);

/*==============================================================================
 * Make Functions - Evolution Periods
 *============================================================================*/

/**
 * makeEvolution - Create validated evolution period
 *
 * Creates an indirect evolution period (t1, t2, t3) with optional
 * rotor synchronization and decoupling.
 *
 * Parameters:
 *   evolution_name - Evolution period name ("t1", "t2", "t3")
 *   evolution_time_us - Evolution time in microseconds
 *   rotor_sync - 1 for rotor-synchronized, 0 for fixed time
 *   has_decoupling - 1 to enable decoupling during evolution
 *   decoupling_nucleus - Nucleus to decouple (if has_decoupling = 1)
 *   limits - Pointer to NMRSystemLimits
 *
 * Returns:
 *   NMREvolution structure (check is_valid field)
 *
 * Validation:
 *   - Evolution time: positive value, typically < 10 ms
 *   - Rotor sync: srate must be non-zero if enabled
 *   - Number of rotor cycles: calculated from time and srate
 *   - Decoupling: channel assignment validated
 *
 * Example:
 *   // t1 evolution with rotor synchronization and H decoupling
 *   NMREvolution t1 = makeEvolution("t1", 5000.0, 1, 1, "H", &limits);
 *
 *   // Rotor-synchronized t2 evolution (no decoupling)
 *   NMREvolution t2 = makeEvolution("t2", 10000.0, 1, 0, "", &limits);
 */
NMREvolution makeEvolution(const char* evolution_name, double evolution_time_us,
                          int rotor_sync, int has_decoupling,
                          const char* decoupling_nucleus,
                          NMRSystemLimits* limits);

/**
 * makeConstantTimeEvolution - Create constant-time evolution period
 *
 * Creates a constant-time evolution period where the total time remains
 * fixed while the active evolution time varies.
 *
 * Parameters:
 *   evolution_name - Evolution period name
 *   total_time_us - Total constant time (us)
 *   evolution_time_us - Active evolution time (us) - must be < total_time
 *   limits - Pointer to NMRSystemLimits
 *
 * Returns:
 *   NMREvolution structure with is_constant_time = 1
 *
 * Example:
 *   // Constant-time t1 with 10 ms total time
 *   NMREvolution ct_t1 = makeConstantTimeEvolution("t1", 10000.0, 5000.0, &limits);
 */
NMREvolution makeConstantTimeEvolution(const char* evolution_name,
                                      double total_time_us,
                                      double evolution_time_us,
                                      NMRSystemLimits* limits);

/*==============================================================================
 * Execution Functions
 *============================================================================*/

/**
 * execute_pulse - Execute a validated pulse
 *
 * Translates the NMRPulse structure into actual VnmrJ/OpenVNMRJ
 * pulse sequence code.
 *
 * Parameters:
 *   pulse - Pointer to NMRPulse structure
 */
void execute_pulse(NMRPulse* pulse);

/**
 * execute_shaped_pulse - Execute a validated shaped pulse
 */
void execute_shaped_pulse(NMRShapedPulse* pulse);

/**
 * execute_cp - Execute a validated CP transfer
 */
void execute_cp(NMRCP* cp);

/**
 * execute_mixing - Execute a validated mixing period
 */
void execute_mixing(NMRMixing* mixing);

/**
 * execute_decoupling - Start a validated decoupling sequence
 */
void execute_decoupling(NMRDecoupling* dec);

/**
 * execute_evolution - Execute a validated evolution period
 */
void execute_evolution(NMREvolution* evo);

/*==============================================================================
 * Utility Functions
 *============================================================================*/

/**
 * validate_pulse_element - Validate any pulse element against system limits
 *
 * Generic validation function that can be called on any pulse element.
 *
 * Returns: 1 if valid, 0 if invalid (error message in structure)
 */
int validate_pulse_element(void* element, const char* element_type,
                          NMRSystemLimits* limits);

/**
 * print_pulse_summary - Print summary of pulse element
 *
 * Prints a human-readable summary of pulse parameters for debugging
 * and sequence verification.
 */
void print_pulse_summary(void* element, const char* element_type);

/**
 * calculate_total_duty_cycle - Calculate total duty cycle from pulse elements
 *
 * Accumulates RF time from multiple pulse elements to calculate
 * overall duty cycle for sequence validation.
 *
 * Parameters:
 *   elements - Array of pulse element pointers
 *   element_types - Array of element type strings
 *   n_elements - Number of elements
 *   d1 - Relaxation delay (seconds)
 *
 * Returns: Duty cycle as fraction (0.05 = 5%)
 */
double calculate_total_duty_cycle(void** elements, const char** element_types,
                                 int n_elements, double d1);

/**
 * check_hartmann_hahn_match - Check HH matching condition
 *
 * Validates that two RF amplitudes satisfy the Hartmann-Hahn matching
 * condition: rf1 - rf2 = n * srate (where n is integer)
 *
 * Parameters:
 *   rf1_hz - RF amplitude 1 (Hz)
 *   rf2_hz - RF amplitude 2 (Hz)
 *   srate_hz - Spinning rate (Hz)
 *   tolerance - Matching tolerance (Hz)
 *
 * Returns: 1 if matched, 0 if not matched
 */
int check_hartmann_hahn_match(double rf1_hz, double rf2_hz,
                              double srate_hz, double tolerance);

/**
 * calculate_rotor_cycles - Calculate number of rotor cycles for given time
 *
 * Parameters:
 *   time_us - Time in microseconds
 *   srate_hz - Spinning rate in Hz
 *
 * Returns: Number of complete rotor cycles (rounded)
 */
int calculate_rotor_cycles(double time_us, double srate_hz);

/**
 * get_channel_index - Convert channel name to index
 *
 * Parameters:
 *   channel - Channel name ("obs", "dec", "dec2", "dec3")
 *
 * Returns: Channel index (0-3) or -1 if invalid
 */
int get_channel_index(const char* channel);

#endif /* NMR_MAKE_FUNCTIONS_H */
