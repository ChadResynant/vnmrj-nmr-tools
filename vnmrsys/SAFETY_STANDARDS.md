# NMR Pulse Sequence Safety Standards

**Created:** November 9, 2025
**Purpose:** Define safety limits and validation requirements for all NMR pulse sequences
**Critical:** These standards protect probe hardware from damage

## Executive Summary

This document establishes mandatory safety standards for NMR pulse sequences based on hardware specifications and operational experience. All sequences MUST comply with these limits unless explicitly documented exceptions exist.

## 1. Duty Cycle Limits

### Standard Limits

| Parameter | Default Limit | Rationale |
|-----------|--------------|-----------|
| **RF Duty Cycle** | 5% (0.05) | Industry standard for probe protection |
| **Continuous RF** | 50 ms | Maximum continuous pulse duration |
| **Minimum d1** | 100 ms | Minimum relaxation delay for duty cycle |

### Probe-Specific Exceptions

| Spectrometer | Probe | Duty Cycle Limit | Documentation |
|--------------|-------|------------------|---------------|
| Taurus | Standard | 5% | Default - most conservative |
| Taurus | High-Power | 15% | Special probe design, see PROBE_SPECS_TAURUS.md |
| Ayrshire | Standard | 20% | Modified cooling system, see PROBE_SPECS_AYRSHIRE.md |
| Ribeye | Standard | 5% | Default |

### Critical Safety Issue (November 2025)

**PROBLEM IDENTIFIED:** July 2025 refactoring relaxed duty cycle limits without documentation:
- Original sequences: 5% limit
- Refactored sequences: 10-20% limit
- **This is potentially dangerous and must be corrected**

### Duty Cycle Calculation

```c
duty_cycle = (total_rf_time + acquisition_time) / (total_rf_time + acquisition_time + d1 + overhead)

where:
- total_rf_time = sum of all pulse durations + CP time + mixing time
- acquisition_time = at
- d1 = relaxation delay
- overhead = 4.0e-6 (sequence overhead)
```

### Implementation

All sequences must:
1. Calculate duty cycle using the standard formula
2. Compare against probe-specific limit (default 5%)
3. Abort with clear error message if exceeded
4. Report duty cycle components for debugging

## 2. RF Power Limits

### Channel-Specific Limits

| Channel | Nucleus | Max Power (dB) | Max Amplitude (Hz) | Max Continuous (ms) |
|---------|---------|----------------|--------------------|-------------------|
| obs | 1H | 63 | 100 kHz | 10 |
| dec | 13C | 63 | 50 kHz | 50 |
| dec2 | 15N | 63 | 30 kHz | 50 |

### Power Calculation

```c
// Never exceed hardware limits
if (aH90 > 4095) abort("1H amplitude exceeds hardware limit");
if (aX90 > 4095) abort("13C amplitude exceeds hardware limit");
if (aY90 > 4095) abort("15N amplitude exceeds hardware limit");

// Check duty cycle with actual power levels
double rf_heating = calculate_heating_factor(amplitude, duration);
if (rf_heating > probe_limit) abort("RF heating exceeds probe limit");
```

## 3. Timing Constraints

### Minimum Pulse Widths

| Pulse Type | Minimum Duration | Reason |
|------------|------------------|--------|
| Hard pulse | 0.1 μs | Hardware limitation |
| Shaped pulse | 1.0 μs | DAC update rate |
| CP contact | 10 μs | Efficiency threshold |
| Decoupling | 1 ms | Minimum useful duration |

### Maximum Evolution Times

| Dimension | Maximum Time | Reason |
|-----------|--------------|--------|
| d2 (t1) | 50 ms | T2 relaxation, duty cycle |
| d3 (t2) | 50 ms | T2 relaxation |
| d4 (t3) | 20 ms | 3D experiments |
| d5 (t4) | 10 ms | 4D experiments |

## 4. Spinning Rate Requirements

### MAS Limits

| Sequence Type | Minimum srate | Maximum srate | Notes |
|---------------|---------------|---------------|-------|
| CP-MAS | 5 kHz | 70 kHz | Probe-dependent |
| Static | 0 | 0 | No spinning |
| Slow MAS | 500 Hz | 5 kHz | Special sequences |

### Rotor Synchronization

```c
// Minimum spinning rate for rotor-synchronized sequences
#define MIN_SRATE_ROTOR_SYNC 500.0  // Hz

if (srate < MIN_SRATE_ROTOR_SYNC) {
    printf("ABORT: Spinning rate %.0f Hz < minimum %.0f Hz for rotor sync\n",
           srate, MIN_SRATE_ROTOR_SYNC);
    printf("Solution: Increase srate or use non-synchronized version\n");
    psg_abort(1);
}
```

## 5. Validation Requirements

### Multi-Level Validation

All sequences MUST implement three levels of validation:

#### Level 1: Parameter Validation (at start)
```c
// Check all parameters are within bounds
validate_pulse_widths();
validate_power_levels();
validate_delays();
validate_evolution_times();
```

#### Level 2: Calculation Validation (after setup)
```c
// Check calculated values are safe
validate_duty_cycle();
validate_mixing_time();
validate_phase_cycling();
```

#### Level 3: Sequence Validation (before execution)
```c
// Final safety checks
validate_total_time();
validate_array_setup();
validate_probe_limits();
```

### Error Message Requirements

All error messages MUST include:
1. What failed
2. Current value
3. Required limit
4. Suggested solution

**Example:**
```c
printf("ABORT: Duty cycle %.1f%% exceeds limit %.1f%% for probe %s\n",
       duty*100, duty_limit*100, probe_name);
printf("Current settings:\n");
printf("  CP time: %.1f ms\n", tHX*1000);
printf("  Mixing time: %.1f ms\n", tXmix*1000);
printf("  Relaxation delay: %.1f s\n", d1);
printf("Solutions:\n");
printf("  1. Increase d1 to %.1f s\n", min_d1);
printf("  2. Reduce tHX to %.1f ms\n", max_tHX*1000);
printf("  3. Reduce tXmix to %.1f ms\n", max_tXmix*1000);
psg_abort(1);
```

## 6. Special Considerations

### Cross-Polarization

- Maximum CP contact time: 10 ms (probe heating)
- Minimum CP contact time: 100 μs (efficiency)
- Hartmann-Hahn matching tolerance: ±2 kHz

### Decoupling

- Maximum continuous decoupling: acquisition time
- TPPM/SPINAL duty cycle: included in total
- Decoupling power: typically 80-100 kHz for 1H (high-power)
- Low-power decoupling: 1-2 kHz for H-detected experiments

### Power-Dependent Duty Cycle Limits

**CRITICAL:** H-detected experiments (sequences ending in 'H') typically use low-power decoupling, allowing higher duty cycles safely.

| Experiment Type | Decoupling Power | Safe Duty Cycle | Validation Required |
|-----------------|------------------|-----------------|---------------------|
| C-detected | 80-100 kHz | 5% | Standard validation |
| H-detected (low power) | 1-2 kHz | 15-20% | Power level check |
| H-detected (high power) | >10 kHz | 5% | Standard validation |

#### Implementation for H-detected Experiments

```c
double get_safe_duty_limit(double dpwr_H, int is_H_detected) {
    if (!is_H_detected) {
        return 0.05;  // Standard 5% for C-detected
    }

    // Calculate decoupling field strength
    double gammaH = 42.577e6;  // Hz/T for 1H
    double B1_Hz = calculate_B1_from_power(dpwr_H);

    if (B1_Hz < 5000.0) {  // Less than 5 kHz
        printf("Low-power H decoupling detected: %.1f Hz\n", B1_Hz);
        printf("Using relaxed duty cycle limit: 15%%\n");
        return 0.15;  // 15% for low-power
    } else {
        printf("High-power H decoupling detected: %.1f Hz\n", B1_Hz);
        printf("Using standard duty cycle limit: 5%%\n");
        return 0.05;  // 5% for high-power
    }
}
```

### High-Power Sequences

Sequences using high-power pulses (REDOR, TEDOR, etc.) must:
1. Use conservative duty cycle (≤5%)
2. Implement additional cooling delays
3. Monitor probe temperature if available

## 7. Implementation Standards

### Required Parameters

All sequences MUST accept these safety parameters:

```c
// In sequence parameter file
duty_limit    0.05   // Duty cycle limit (5% default)
max_rf_time   50e-3  // Maximum continuous RF (50 ms default)
min_d1        0.1    // Minimum relaxation delay (100 ms)
probe_name    "standard"  // For documentation
```

### Standard Safety Check Function

```c
void check_duty_cycle(double duty_limit_param) {
    double duty_limit = (duty_limit_param > 0) ? duty_limit_param : 0.05;

    // Calculate actual duty cycle
    double rf_time = pwH90 + pwX90 + tHX + tXmix + /* other RF */;
    double cycle_time = rf_time + at + d1;
    double duty = rf_time / cycle_time;

    // Report calculation
    printf("Duty cycle calculation:\n");
    printf("  RF time: %.1f ms\n", rf_time * 1000);
    printf("  Cycle time: %.1f ms\n", cycle_time * 1000);
    printf("  Duty cycle: %.1f%%\n", duty * 100);
    printf("  Limit: %.1f%%\n", duty_limit * 100);

    // Validate
    if (duty > duty_limit) {
        // Detailed error message with solutions
        report_duty_cycle_error(duty, duty_limit, rf_time, cycle_time);
        psg_abort(1);
    }
}
```

## 8. Testing Requirements

### Safety Testing Protocol

Before deploying any sequence:

1. **Low-Power Test** (oscilloscope)
   - Verify pulse widths
   - Check phase cycling
   - Confirm timing

2. **Duty Cycle Test**
   - Set minimum d1
   - Verify sequence aborts
   - Check error message clarity

3. **Limit Test**
   - Test at maximum parameters
   - Verify all safety checks trigger
   - Confirm no hardware limits exceeded

4. **Production Test**
   - Run with standard sample
   - Monitor probe temperature
   - Verify expected performance

## 9. Exceptions and Overrides

### Documented Exceptions Only

Exceptions to these standards require:
1. Written justification in sequence comments
2. Probe manufacturer documentation
3. Experimental validation
4. User warning in sequence

**Example:**
```c
// EXCEPTION: Ayrshire probe allows 20% duty cycle
// Justification: Enhanced cooling system (see PROBE_SPECS_AYRSHIRE.md)
// Validated: 2023-12-04 by CMR
// WARNING: Do not use on standard probes!
#ifdef AYRSHIRE_PROBE
  #define DUTY_LIMIT 0.20
#else
  #define DUTY_LIMIT 0.05
#endif
```

### Override Parameters

For special cases, allow user override with warnings:

```c
double duty_limit_override = getval("duty_limit_override");
if (duty_limit_override > 0.05) {
    printf("WARNING: Duty cycle limit override to %.1f%%\n",
           duty_limit_override * 100);
    printf("WARNING: This may damage the probe!\n");
    printf("WARNING: User accepts responsibility\n");
    // Log this override
    fprintf(logfile, "OVERRIDE: Duty limit %.1f%% by user\n",
            duty_limit_override * 100);
}
```

## 10. Compliance

### Mandatory Compliance

- All new sequences MUST comply with these standards
- Existing sequences MUST be updated by January 2026
- Refactored sequences MUST be corrected immediately

### Verification

Each sequence must include:
```c
// Safety standards compliance
// Complies with: SAFETY_STANDARDS.md v1.0
// Last safety review: 2025-11-09
// Reviewed by: [name]
```

### Violations

Safety violations must be:
1. Fixed immediately
2. Documented in SAFETY_VIOLATIONS.log
3. Reported to sequence maintainer
4. Tested before re-deployment

## Appendix A: Common Duty Cycle Values

| Sequence | Typical RF Time | Min d1 for 5% | Min d1 for 10% |
|----------|-----------------|---------------|----------------|
| hX | 3 ms | 57 ms | 27 ms |
| hXX | 5 ms + mixing | 95 ms + 20×tmix | 45 ms + 9×tmix |
| hXH | 4 ms | 76 ms | 36 ms |
| hYXH | 6 ms | 114 ms | 54 ms |
| hYYXH | 8 ms | 152 ms | 72 ms |
| hXYXX_4D | 10 ms | 190 ms | 90 ms |

## Appendix B: Temperature Coefficients

RF heating approximation:
```
ΔT ≈ k × (amplitude)² × duration × duty_cycle

where k is probe-specific (typically 0.1-0.5 °C/W)
```

## Appendix C: Historical Issues

### Known Problems from July 2025 Refactoring

1. **Duty cycle relaxation without documentation**
   - hXX-claude.c: Changed 5% → 10%
   - hYYXH.c: Changed 5% → 20%
   - No justification provided

2. **Missing validation in refactored sequences**
   - No maximum evolution time checks
   - No minimum d1 validation
   - Incomplete power level checks

3. **Inconsistent limits across spectrometers**
   - Taurus: 15% (high-power probe)
   - Ayrshire: 20% (cooling system)
   - Ribeye: 5% (standard)
   - Not documented in sequences

These issues MUST be corrected in the current refactoring effort.

---

**Document Version:** 1.0
**Last Updated:** November 9, 2025
**Next Review:** January 2026
**Owner:** NMR Sequence Development Team