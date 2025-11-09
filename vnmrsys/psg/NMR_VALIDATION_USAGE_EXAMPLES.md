# NMR Validation Framework - Usage Examples

**Created:** November 9, 2025
**Files:** `nmr_validation.h`, `nmr_validation.c`

## Overview

The NMR validation framework provides three-tier validation for pulse sequences:

1. **TIER 1: Creation-Time Validation** - Validates pulse parameters when creating pulse elements
2. **TIER 2: Sequence-Level Validation** - Validates sequence configuration after parameter setup
3. **TIER 3: Runtime Validation** - Validates computed parameters before pulse execution

## Three-Tier Validation Explained

### TIER 1: Creation-Time Validation

Called by `make` functions during pulse element creation. Validates individual pulse parameters before they enter the sequence.

**When to use:**
- In `makePulse()`, `makeShapedPulse()`, `makeCP()` functions
- Before adding pulse elements to sequence
- To catch invalid parameters early

**What it checks:**
- Pulse duration within hardware limits
- Power level within channel limits
- Phase value is valid (0-360 degrees)
- B1 field strength calculation
- Channel identifier is valid

### TIER 2: Sequence-Level Validation

Called after parameter setup in `pulsesequence()`. Validates entire sequence configuration before pulse execution.

**When to use:**
- After all `getval()` calls
- Before pulse sequence execution begins
- To verify sequence-level consistency

**What it checks:**
- Detection type correctly identified
- Duty cycle limits set appropriately
- Spinning rate within limits (if MAS)
- No parameter conflicts
- Sequence configuration is valid

### TIER 3: Runtime Validation

Called immediately before pulse execution with actual computed values. Final safety check.

**When to use:**
- After computing total RF time
- Before `status(A)` and pulse execution
- As final safety gate

**What it checks:**
- Duty cycle with actual computed RF times
- Total sequence time is reasonable
- No numerical errors
- All accumulated times are positive

---

## Example 1: Complete Sequence with All Three Tiers

```c
/*
 * hXX_example.c - Example using full three-tier validation
 */

#include "standard.h"
#include "nmr_validation.h"

void pulsesequence() {
    /* ==================================================================
     * PARAMETER RETRIEVAL
     * ================================================================== */

    double pwH90 = getval("pwH90");   // 1H 90-degree pulse
    double pwX90 = getval("pwX90");   // 13C 90-degree pulse
    double tHX = getval("tHX");       // CP contact time
    double d2 = getval("d2");         // Evolution time
    double tXmix = getval("tXmix");   // Mixing time
    double d1 = getval("d1");         // Relaxation delay
    double at = getval("at");         // Acquisition time
    double tpwr = getval("tpwr");     // H power
    double dpwr = getval("dpwr");     // X power

    char seqfil[MAXSTR];
    getstr("seqfil", seqfil);         // Get sequence name

    /* ==================================================================
     * TIER 2: SEQUENCE-LEVEL VALIDATION
     * ================================================================== */

    NMRSystemLimits limits = init_nmr_system_limits();
    ValidationResult result;

    printf("\n=== Validation for %s ===\n", seqfil);

    // Validate sequence configuration
    result = validate_sequence(&limits, seqfil);
    if (!result.is_valid) {
        printf("ERROR: %s\n", result.error_message);
        printf("SOLUTION: %s\n", result.solution);
        psg_abort(1);
    }
    printf("TIER 2 PASS: %s\n", result.error_message);

    /* ==================================================================
     * TIER 1: CREATION-TIME VALIDATION (Example for H pulse)
     * ================================================================== */

    PulseParams pp_H90 = init_pulse_params();
    pp_H90.duration = pwH90 * 1e-6;   // Convert us to s
    pp_H90.power = tpwr;
    pp_H90.phase = 0.0;
    strcpy(pp_H90.channel, "dec");

    result = validate_pulse_parameters(&limits, &pp_H90, "dec");
    if (!result.is_valid) {
        printf("ERROR: H90 pulse - %s\n", result.error_message);
        printf("SOLUTION: %s\n", result.solution);
        psg_abort(1);
    }
    printf("TIER 1 PASS: H90 pulse validated (%.1f us, %.1f dB, %.0f kHz B1)\n",
           pp_H90.duration * 1e6, pp_H90.power, pp_H90.B1_field / 1000.0);

    /* ==================================================================
     * CALCULATE TOTAL RF TIME
     * ================================================================== */

    double tRF = 0.0;
    tRF += pwH90 * 1e-6;    // H 90-degree pulse
    tRF += pwX90 * 1e-6;    // X 90-degree pulse
    tRF += tHX * 1e-3;      // CP contact time
    tRF += d2;              // Evolution time
    tRF += tXmix * 1e-3;    // Mixing time
    tRF += at;              // Acquisition time (with decoupling)

    /* ==================================================================
     * TIER 3: RUNTIME VALIDATION
     * ================================================================== */

    result = validate_runtime(&limits, tRF, d1, seqfil);
    if (!result.is_valid) {
        printf("ERROR: %s\n", result.error_message);
        printf("SOLUTION: %s\n", result.solution);
        print_duty_cycle_breakdown(tRF, d1, limits.probe_duty_limit);
        psg_abort(1);
    }
    printf("TIER 3 PASS: %s\n", result.error_message);

    /* Print final duty cycle breakdown */
    print_duty_cycle_breakdown(tRF, d1, limits.probe_duty_limit);

    /* ==================================================================
     * PULSE SEQUENCE EXECUTION
     * ================================================================== */

    status(A);

    // Relaxation delay
    delay(d1);

    // H 90-degree pulse
    rgpulse(pwH90, zero, 0.0, 0.0);

    // H->X cross-polarization
    // ... CP code here ...

    // Evolution
    delay(d2);

    // Mixing
    // ... mixing code here ...

    // Acquisition with decoupling
    startacq(at);
    acquire(np, 1.0/sw);
    endacq();
}
```

---

## Example 2: Simple Two-Tier Validation

For simpler sequences, you can combine Tiers 2 and 3:

```c
void pulsesequence() {
    /* Get parameters */
    double pwH90 = getval("pwH90");
    double d1 = getval("d1");
    char seqfil[MAXSTR];
    getstr("seqfil", seqfil);

    /* Calculate total RF time */
    double tRF = pwH90 * 1e-6;

    /* Initialize and validate */
    NMRSystemLimits limits = init_nmr_system_limits();
    set_sequence_detection_type(&limits, seqfil);

    ValidationResult result = validate_runtime(&limits, tRF, d1, seqfil);
    if (!result.is_valid) {
        printf("ABORT: %s\n", result.error_message);
        printf("SOLUTION: %s\n", result.solution);
        psg_abort(1);
    }

    /* Execute sequence */
    status(A);
    delay(d1);
    rgpulse(pwH90, zero, 0.0, 0.0);
    // ... rest of sequence ...
}
```

---

## Example 3: Using Convenience Function

The framework provides `validate_full_sequence()` for quick validation:

```c
void pulsesequence() {
    /* Get parameters */
    double d1 = getval("d1");
    double dpwr = getval("dpwr");  // Decoupling power
    char seqfil[MAXSTR];
    getstr("seqfil", seqfil);

    /* Calculate total RF time */
    double tRF = calculate_total_rf_time();  // Your function

    /* Validate (all three tiers) */
    validate_full_sequence(seqfil, tRF, d1, dpwr);

    /* Execute sequence */
    status(A);
    // ... sequence code ...
}
```

---

## Example 4: Power-Dependent Duty Cycle

For H-detected sequences with power-dependent duty cycle:

```c
void pulsesequence() {
    char seqfil[MAXSTR];
    getstr("seqfil", seqfil);

    double d1 = getval("d1");
    double dpwr = getval("dpwr");  // H decoupling power

    /* Initialize limits */
    NMRSystemLimits limits = init_nmr_system_limits();
    set_sequence_detection_type(&limits, seqfil);

    /* Get power-dependent duty limit */
    double duty_limit = get_safe_duty_limit(&limits, dpwr);
    printf("Duty cycle limit: %.0f%% (based on decoupling power %.1f dB)\n",
           duty_limit * 100.0, dpwr);

    /* Calculate RF time */
    double tRF = /* ... calculate ... */;

    /* Validate with adjusted limit */
    ValidationResult result = validate_runtime(&limits, tRF, d1, seqfil);
    if (!result.is_valid) {
        printf("ERROR: %s\n", result.error_message);
        psg_abort(1);
    }

    /* Execute sequence */
    status(A);
    // ... sequence code ...
}
```

---

## Example 5: In Make Functions (TIER 1)

Example `makePulse()` function with creation-time validation:

```c
Pulse makePulse(double duration_us, double power_db, double phase_deg,
                const char* channel) {
    Pulse pulse;

    /* Initialize system limits */
    static NMRSystemLimits limits;
    static int limits_initialized = 0;
    if (!limits_initialized) {
        limits = init_nmr_system_limits();
        limits_initialized = 1;
    }

    /* Create pulse parameters structure */
    PulseParams params = init_pulse_params();
    params.duration = duration_us * 1e-6;  // Convert us to s
    params.power = power_db;
    params.phase = phase_deg;
    strcpy(params.channel, channel);

    /* TIER 1: Validate pulse parameters */
    ValidationResult result = validate_pulse_parameters(&limits, &params, channel);
    if (!result.is_valid) {
        printf("ERROR in makePulse: %s\n", result.error_message);
        printf("SOLUTION: %s\n", result.solution);
        psg_abort(1);
    }

    /* Create pulse with validated parameters */
    pulse.duration = params.duration;
    pulse.power = params.power;
    pulse.phase = params.phase;
    pulse.B1_field = params.B1_field;
    strcpy(pulse.channel, params.channel);

    printf("Pulse created: %.1f us, %.1f dB, %.0f Hz B1 on %s\n",
           duration_us, power_db, pulse.B1_field, channel);

    return pulse;
}
```

---

## Example 6: Detection Type Inference

```c
void pulsesequence() {
    char seqfil[MAXSTR];
    getstr("seqfil", seqfil);

    /* Automatic detection type inference */
    char det_type = get_detection_type_from_name(seqfil);

    printf("Sequence: %s\n", seqfil);

    if (det_type == DETECTION_C) {
        printf("Detection: C-detected (13C/15N)\n");
        printf("Duty limit: 5%% (high-power decoupling on X channel)\n");
    } else if (det_type == DETECTION_H) {
        printf("Detection: H-detected (1H)\n");
        printf("Duty limit: 10-15%% (power-dependent)\n");
    } else {
        printf("WARNING: Unknown detection type, using conservative 5%% limit\n");
    }

    /* ... rest of sequence ... */
}
```

---

## Example 7: B1 Field Calculations

```c
void pulsesequence() {
    double tpwr = getval("tpwr");
    NMRSystemLimits limits = init_nmr_system_limits();

    /* Calculate B1 field from power */
    double B1_hz = calculate_B1_from_power(tpwr, "dec", &limits);
    printf("Power %.1f dB -> B1 field %.0f kHz\n", tpwr, B1_hz / 1000.0);

    /* Calculate power from desired B1 field */
    double desired_B1 = 100000.0;  // 100 kHz
    double power_db = calculate_power_from_B1(desired_B1, "dec", &limits);
    printf("Desired B1 %.0f kHz -> Power %.1f dB\n",
           desired_B1 / 1000.0, power_db);

    /* Check if high-power or low-power */
    if (B1_hz > B1_HIGH_POWER_THRESHOLD) {
        printf("High-power decoupling: 10%% duty limit\n");
    } else {
        printf("Low-power decoupling: 15%% duty limit\n");
    }
}
```

---

## Example 8: Manual Duty Cycle Check

```c
void pulsesequence() {
    double tRF = 0.010;  // 10 ms total RF
    double d1 = 2.0;     // 2 second relaxation

    /* Calculate duty cycle */
    double duty = calculate_duty_cycle(tRF, d1);
    printf("Duty cycle: %.2f%%\n", duty * 100.0);

    /* Check against limit */
    double duty_limit = DUTY_LIMIT_C_DETECTED;  // 5%

    if (duty > duty_limit) {
        printf("ABORT: Duty cycle %.2f%% exceeds %.0f%% limit\n",
               duty * 100.0, duty_limit * 100.0);
        printf("SOLUTION: Increase d1 to %.1f seconds or reduce RF time\n",
               (tRF / duty_limit) - tRF);
        psg_abort(1);
    } else {
        printf("Duty cycle OK: %.2f%% < %.0f%%\n",
               duty * 100.0, duty_limit * 100.0);
    }
}
```

---

## Example 9: Validation Result Handling

```c
void pulsesequence() {
    NMRSystemLimits limits = init_nmr_system_limits();
    ValidationResult result;

    /* Perform validation */
    result = validate_sequence(&limits, "hXX");

    /* Check result */
    if (!result.is_valid) {
        /* Validation failed */
        printf("=== VALIDATION FAILED ===\n");
        printf("Error: %s\n", result.error_message);
        printf("Computed: %.3f\n", result.computed_value);
        printf("Limit: %.3f\n", result.limit_value);

        if (strlen(result.solution) > 0) {
            printf("\nSuggested solution:\n%s\n", result.solution);
        }

        psg_abort(1);
    } else {
        /* Validation passed */
        printf("=== VALIDATION PASSED ===\n");
        printf("%s\n", result.error_message);
    }
}
```

---

## Example 10: Printing Validation Summary

```c
void pulsesequence() {
    char seqfil[MAXSTR];
    getstr("seqfil", seqfil);

    /* Initialize and configure */
    NMRSystemLimits limits = init_nmr_system_limits();
    set_sequence_detection_type(&limits, seqfil);

    /* Print detailed summary */
    print_validation_summary(&limits, seqfil);

    /* Output:
     * === NMR Validation Summary ===
     * Sequence: hXX
     * Detection: X-detected (Carbon/Hetero)
     * Duty Limit: 5%
     * Validation Level: 2 (Sequence-level)
     *
     * RF Channel Limits:
     *   obs: max_B1=125 kHz, max_power=63 dB, duty=5%
     *   dec: max_B1=150 kHz, max_power=63 dB, duty=15%
     *   dec2: max_B1=100 kHz, max_power=63 dB, duty=5%
     *
     * Probe: generic
     * Spinning: 500 - 70000 Hz
     * =========================
     */
}
```

---

## Common Patterns

### Pattern 1: Minimal Validation (Quick Check)

```c
NMRSystemLimits limits = init_nmr_system_limits();
set_sequence_detection_type(&limits, seqfil);
validate_runtime(&limits, tRF, d1, seqfil);
```

### Pattern 2: Standard Validation (Recommended)

```c
NMRSystemLimits limits = init_nmr_system_limits();
ValidationResult result;

result = validate_sequence(&limits, seqfil);
if (!result.is_valid) { /* handle error */ }

result = validate_runtime(&limits, tRF, d1, seqfil);
if (!result.is_valid) { /* handle error */ }
```

### Pattern 3: Comprehensive Validation (Full Safety)

```c
NMRSystemLimits limits = init_nmr_system_limits();
ValidationResult result;

// Sequence-level
result = validate_sequence(&limits, seqfil);
if (!result.is_valid) { abort_with_message(&result); }

// Creation-time (for each pulse)
for (each pulse) {
    result = validate_pulse_parameters(&limits, &params, channel);
    if (!result.is_valid) { abort_with_message(&result); }
}

// Runtime
result = validate_runtime(&limits, tRF, d1, seqfil);
if (!result.is_valid) { abort_with_message(&result); }
```

---

## Integration with Existing Code

### Replacing Old Duty Cycle Checks

**Old code:**
```c
duty = tRF / (tRF + d1 + 4.0e-6);
if (duty > 0.05) {
    printf("Duty cycle %.1f%% >5%%. Abort!\n", duty*100);
    psg_abort(1);
}
```

**New code:**
```c
NMRSystemLimits limits = init_nmr_system_limits();
set_sequence_detection_type(&limits, seqfil);
ValidationResult result = validate_runtime(&limits, tRF, d1, seqfil);
if (!result.is_valid) {
    printf("%s\n", result.error_message);
    printf("%s\n", result.solution);
    psg_abort(1);
}
```

### Benefits:
- Automatic detection type identification
- Power-dependent duty cycle limits
- Clear error messages with solutions
- Consistent validation across sequences

---

## Compilation

To use the validation framework, compile with:

```bash
cc -o sequence_name sequence_name.c nmr_validation.c -lm -I/path/to/psg
```

Or add to your sequence Makefile:

```makefile
LDFLAGS += -L../psg
LDLIBS += -lnmr_validation -lm
```

---

## Summary

The three-tier validation framework provides:

1. **Safety**: Catches errors at creation, sequence, and runtime levels
2. **Clarity**: Clear error messages with actionable solutions
3. **Flexibility**: Use one, two, or all three tiers based on needs
4. **Consistency**: Standardized validation across all sequences
5. **Power-Aware**: Automatic power-dependent duty cycle limits

Choose validation level based on sequence complexity:
- Simple sequences: TIER 3 only (runtime)
- Standard sequences: TIER 2 + TIER 3 (recommended)
- Complex sequences: All three tiers (comprehensive)
