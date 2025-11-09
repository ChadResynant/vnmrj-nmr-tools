# NMR Pulse Sequence Safety Standards

**Created:** November 9, 2025
**Purpose:** Define duty cycle limits and safety standards for solid-state NMR sequences

## Critical Safety Issue Addressed

**Date:** November 9, 2025
**Issue:** Five C-detected sequences were using dangerous duty cycles (10-20%) that could damage probe hardware
**Action:** Emergency safety fixes reducing all C-detected sequences to 5% duty cycle limit

## Duty Cycle Limits by Detection Type

### C-Detected Sequences: 5% Maximum

**Rationale:**
- C-detected experiments use **high-power decoupling** on the X (¹³C) channel during acquisition
- Continuous high-power RF on sensitive carbon coils creates extreme heat
- Carbon coils are smaller and more susceptible to damage than proton coils
- **5% duty cycle is the maximum safe limit** for C-detected sequences

**Sequences requiring 5% limit:**
- hX, hXX family (basic C-detected)
- hYXX family (3D C-detected with N evolution)
- hXYXX family (4D C-detected)
- All sequences where acquisition is on X (¹³C) channel
- Any sequence with continuous decoupling during acquisition

**Fixed sequences (November 9, 2025):**
1. hYXX.c - Reduced from 15% to 5% (also fixed bug: code checked 15% but message said 10%)
2. hYXX_S.c - Reduced from 10% to 5%
3. hYXXsoft.c - Reduced from 10% to 5%
4. hhYXX_S.c - Reduced from 10% to 5%
5. hXYXX_4D_S.c - Reduced from 20% to 5% (CRITICAL - was 4x over safe limit!)

### H-Detected Sequences: 10-15% Maximum

**Rationale:**
- H-detected experiments acquire on the H (¹H) channel
- Decoupling during acquisition is typically **low-power** or absent
- Proton coils are more robust and can handle higher duty cycles
- **10-15% duty cycle is acceptable** for H-detected sequences

**Sequences using 10-15% limit:**
- hXH family (H-detected, low-power X decoupling)
- HhXH family (H-detected)
- HhYXH family (H-detected)
- Sequences where acquisition is on H (¹H) channel

**Examples in codebase:**
- hXH.c: 15% limit (line 112) - SAFE for H-detected
- HhYXH-claude.c: 15% limit (line 204) - SAFE for H-detected
- hXX_refactored.c: 10% limit - needs review (should be 5% for C-detected)

## Detection Type Identification

### How to Determine Detection Type

Look at the sequence name convention:
- **Last letter = detection nucleus**
- hYX**X** → C-detected (X = ¹³C) → **5% limit**
- hX**H** → H-detected (H = ¹H) → **10-15% limit**

**Examples:**
| Sequence | Detection | Duty Limit | Reason |
|----------|-----------|------------|---------|
| hX | C-detected | 5% | Ends in X (¹³C) |
| hXX | C-detected | 5% | Ends in X (¹³C) |
| hYXX | C-detected | 5% | Ends in X (¹³C) |
| hXYXX_4D_S | C-detected | 5% | Ends in X (¹³C) |
| hXH | H-detected | 10-15% | Ends in H (¹H) |
| HhXH | H-detected | 10-15% | Ends in H (¹H) |
| hYXH | H-detected | 10-15% | Ends in H (¹H) |

## Probe Damage Risk

### High-Power Decoupling Danger

**Why C-detected sequences are dangerous:**

1. **Continuous High-Power RF**
   - Decoupling during acquisition = continuous RF
   - Acquisition times: 10-50 ms typical
   - High power needed for effective decoupling (50-100 kHz field)

2. **Heat Accumulation**
   - Carbon coil inductance → resistive heating
   - Small coil geometry → poor heat dissipation
   - Repeated scans → cumulative heating

3. **Failure Modes**
   - Coil detuning from thermal expansion
   - Solder joint failure
   - Capacitor failure from heat
   - Complete coil burnout (catastrophic)

### Consequence of Exceeding Limits

| Duty Cycle | Risk Level | Consequence |
|------------|------------|-------------|
| < 5% | **SAFE** | Normal operation for C-detected |
| 5-10% | **WARNING** | Acceptable for H-detected only |
| 10-15% | **DANGER** | C-detected: probable probe damage |
| 15-20% | **CRITICAL** | C-detected: severe probe damage likely |
| > 20% | **CATASTROPHIC** | Probe destruction imminent |

**Real Risk:**
- A $50,000-100,000 MAS probe can be destroyed in minutes
- Repair/replacement requires weeks of downtime
- No experimental data collection during repair

## Implementation in Code

### Standard Duty Cycle Check Pattern

For **C-detected sequences:**

```c
// Dutycycle Protection
// 5% duty cycle limit for C-detected sequences (high-power decoupling on X channel)
duty = <calculate total RF time>;
duty = duty/(duty + d1 + 4.0e-6);

if (duty > 0.05) {
    printf("Duty cycle %.1f%% >5%%. Abort!\n", duty*100);
    psg_abort(1);
}
```

For **H-detected sequences:**

```c
// Dutycycle Protection
// 10-15% duty cycle limit for H-detected sequences (low-power or no decoupling during acquisition)
duty = <calculate total RF time>;
duty = duty/(duty + d1 + 4.0e-6);

if (duty > 0.15) {  // or 0.10 for conservative approach
    printf("Duty cycle %.1f%% >15%%. Abort!\n", duty*100);
    psg_abort(1);
}
```

### Components Included in Duty Cycle

**Must include:**
- All pulse widths (pwH90, pwX90, pwY90, etc.)
- Cross-polarization times (tHX, tHY, tYX)
- Evolution periods (d2, d3, d4)
- Mixing times with RF (tXmix, etc.)
- Decoupling during acquisition (tRF or "at" if decoupling)
- Pre-acquisition delays (ad, rd)

**Do NOT include:**
- Relaxation delay (d1) - goes in denominator
- Pure delays without RF
- Dead time between pulses

## Refactored Sequence Standards

For sequences using the `biosolidvalidation.h` framework:

```c
#include "biosolidvalidation.h"

NMRValidation validator = init_nmr_validation();

// For C-detected sequences:
set_duty_limit(&validator, 0.05);  // 5% duty cycle

// For H-detected sequences:
set_duty_limit(&validator, 0.15);  // 15% duty cycle (or 0.10 for conservative)

// Add all RF components
add_pulse_time(&validator, "H90", pwH90);
add_pulse_time(&validator, "CP", tHX);
add_evolution_time(&validator, d2);
// ... etc ...

validate_all_parameters(&validator);  // Automatic duty cycle check
```

## Sequence Audit Status

### Critical Sequences Reviewed (November 9, 2025)

| Sequence | Usage | Old Limit | New Limit | Status |
|----------|-------|-----------|-----------|--------|
| hYXX.c | 5,185 | 15% ⚠️ | 5% ✅ | **FIXED** |
| hYXX_S.c | 1,361 | 10% ⚠️ | 5% ✅ | **FIXED** |
| hYXXsoft.c | ~1,000 | 10% ⚠️ | 5% ✅ | **FIXED** |
| hhYXX_S.c | ~500 | 10% ⚠️ | 5% ✅ | **FIXED** |
| hXYXX_4D_S.c | 2,137 | 20% 🔥 | 5% ✅ | **FIXED** |

### Sequences Needing Review

| Sequence | Current Limit | Expected Limit | Action Needed |
|----------|--------------|----------------|---------------|
| hXX.c | 5% | 5% | ✅ Correct |
| hXX_refactored.c | 10% | 5% | ⚠️ Fix needed |
| hXX_complete_refactor.c | 10% | 5% | ⚠️ Fix needed |
| hXX-claude.c | 10% | 5% | ⚠️ Fix needed |
| hX.c | 20% | 5% | 🔥 **URGENT** |
| HXhhXH_4D.c | 10% | 5% | ⚠️ Fix needed |

**Note:** hX.c currently allows 20% and is the **#1 most used sequence (16,915 experiments)**. This is a critical safety issue requiring immediate attention.

## Testing Requirements

### Before Deploying Duty Cycle Changes

1. **Parameter Range Testing**
   - Test with minimum d1 values
   - Test with maximum evolution times
   - Test with maximum mixing times
   - Verify abort happens when duty exceeds limit

2. **Real Experiment Testing**
   - Run actual experiments with typical parameters
   - Verify spectra are unchanged
   - Confirm duty cycle calculations are correct
   - Check that experiments previously "just under limit" now abort properly

3. **Documentation**
   - Update sequence documentation
   - Inform users of stricter limits
   - Provide guidance on adjusting parameters (increase d1, reduce mixing times, etc.)

## User Guidance

### If Experiment Aborts Due to Duty Cycle

**Error message:**
```
Duty cycle 6.5% >5%. Abort!
```

**Solutions (in order of preference):**

1. **Increase relaxation delay (d1)**
   - Most effective solution
   - Often improves sensitivity anyway (better T1 recovery)
   - Example: d1=1s → d1=2s can reduce duty from 6% to 3%

2. **Reduce mixing times**
   - If using long mixing (tXmix > 50ms), try reducing
   - May still give good correlations with shorter mixing

3. **Reduce evolution times** (use with caution)
   - Reduces resolution and folding limits
   - Last resort option

4. **Use Non-Uniform Sampling (NUS)**
   - Reduces total experiment time
   - Allows longer d1 without excessive total time
   - Requires processing with NUS reconstruction

## Contact and Questions

**For questions about duty cycle limits:**
- Review this document
- Check sequence-specific comments in code
- Consult with experienced NMR users in the group

**For probe damage or overheating:**
- Stop experiment immediately
- Allow probe to cool (30+ minutes)
- Report to facility manager
- Do not resume until issue is resolved

---

**Last Updated:** November 9, 2025
**Next Review:** When refactoring additional sequences
