# Duty Cycle Error Message Enhancement

**Purpose:** Provide specific, actionable advice when duty cycle limits are exceeded

**Date:** November 9, 2025
**Status:** Proposed enhancement based on Taurus testing feedback

---

## Problem

Current error messages don't tell users:
1. What the actual duty cycle is (hX.c)
2. What minimum d1 value would fix the problem (both sequences)

**Current hX.c:**
```
Duty cycle >5%. Abort!
```
❌ Not helpful - no numbers, no solution

**Current hXX.c:**
```
Duty cycle 18.5% >5%. Check d1, d2, tRF, at, tXmix. Abort!
```
⚠️ Better (shows duty %), but still no specific advice

---

## Proposed Solution

### For hX.c (Simple Case)

**Enhanced code (lines 102-130):**

```c
// Dutycycle Protection

   double rf_time;  // Save RF time for minimum d1 calculation

   if (!strcmp(cp, "n"))  {
   duty = 4.0e-6 + getval("pwH90") + getval("tHX") + getval("ad") + getval("rd") + at;
   }
   else if (!strcmp(cp, "y"))  {
   duty = 4.0e-6 + getval("pwH90") + getval("tHX") + getval("ad") + getval("rd") + at;
   }
   else if (!strcmp(echo, "y")) {
   duty = 4.0e-6 + getval("pwH90") + getval("tHX") + tECHO + pwXecho + tECHOmax + getval("ad") + getval("rd") + at;
   }

   rf_time = duty;  // Save before division
   duty = duty/(duty + d1 + 4.0e-6);

   // 5% duty cycle limit for C-detected sequences (high-power decoupling on X channel)
   if (duty > 0.05) {
      double min_d1 = (rf_time / 0.05) - rf_time - 4.0e-6;
      abort_message("Duty cycle %.1f%% exceeds 5%% limit. Increase d1 to at least %.3f s. Abort!\n",
                    duty*100, min_d1);
   }
   else {
      printf("Duty cycle %.1f%% < 5%%. Safe to proceed.\n", duty*100);
   }
```

**Example output:**
```
Duty cycle 18.5% exceeds 5% limit. Increase d1 to at least 0.418 s. Abort!
```
✅ Clear, specific, actionable

---

### For hXX.c (Complex - Multiple Mixing Modes)

The challenge: Different mixing modes use different overhead calculations.

**Strategy:** Calculate rf_time and overhead for each mixing mode, then use common formula.

**Formula:**
```
min_d1 = (rf_time / 0.05) - rf_time - overhead
       = 19 * rf_time - overhead
```

**Enhanced code (replace lines 240-301):**

```c
    duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 + getval("ad") + getval("rd") + at + tRF;

    // Add echo time if enabled
    if (strcmp(echo,"n") != 0) {
        duty += getval("tECHO");
    }

    double rf_time;    // Save for minimum d1 calculation
    double overhead;   // Overhead time added to denominator

    // Add mixing-specific contributions
    if ((strcmp(mMix, "rad") == 0) || (strcmp(mMix, "paris") == 0)) {
        duty += 2.0*getval("pwX90") + (getval("aHmix")/4095) * (getval("aHmix")/4095) * getval("tXmix");
        rf_time = duty;
        overhead = getval("tXmix") + 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "c7") == 0) {
        duty += 2.0*getval("pwX90");
        if (strcmp(dqf_flag,"2") == 0) {
            duty += c7ref.t;
        }
        else {
            duty += c7.t;
        }
        rf_time = duty;
        overhead = 4.0e-6 + 2.0*getval("tZF");
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "c6") == 0) {
        duty += 2.0*getval("pwX90");
        if (strcmp(dqf_flag,"2") == 0) {
            duty += c6ref.t;
        }
        else {
            duty += c6.t;
        }
        rf_time = duty;
        overhead = 4.0e-6 + 2.0*getval("tZF");
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "spc5") == 0) {
        duty += 2.0*getval("pwX90");
        if (!strcmp(dqf_flag,"2")) {
            duty += spc5ref.t;
        }
        else {
            duty += spc5.t;
        }
        rf_time = duty;
        overhead = 4.0e-6 + 2.0*getval("tZF");
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "par") == 0) {
        duty += 2.0*getval("pwX90") + getval("tPAR");
        rf_time = duty;
        overhead = 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "rfdr") == 0) {
        duty += 2.0*taur*getval("qXrfdr") + 2.0*getval("pwX90");
        rf_time = duty;
        overhead = 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "r2t") == 0 || strcmp(mMix, "dream") == 0) {
        duty += 2*getval("tZF") + getval("tXr2t_in") + getval("tXr2t_mix") + getval("tXr2t_out") + 2.0*getval("pwX90");
        rf_time = duty;
        overhead = 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }
    else {
        // Default/unknown mixing mode
        rf_time = duty;
        overhead = 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }

    // Enforce 5% duty cycle limit with specific advice
    if (duty > 0.05) {
        double min_d1 = (rf_time / 0.05) - rf_time - overhead;
        abort_message("Duty cycle %.1f%% exceeds 5%% limit. Increase d1 to at least %.3f s. Abort!\n",
                      duty*100, min_d1);
    }
    else {
        printf("Duty cycle %.1f%% < 5%%. Safe to proceed. Good luck!\n", duty*100);
    }
```

**Example output:**
```
Duty cycle 6.7% exceeds 5% limit. Increase d1 to at least 1.368 s. Abort!
```

---

## Benefits

1. ✅ **Clear feedback:** Shows actual duty cycle percentage
2. ✅ **Actionable advice:** Tells user exactly what d1 to use
3. ✅ **Saves time:** No trial-and-error to find safe parameters
4. ✅ **Educational:** Helps users understand duty cycle constraints
5. ✅ **Confirmation:** Shows duty cycle when safe (builds confidence)

---

## Testing Plan

1. Apply enhancements to hX.c and hXX.c
2. Recompile on Taurus
3. Test with known violations (d1=0.1s)
4. Verify min_d1 calculation is accurate
5. Test suggested d1 value actually works
6. Document in test log

---

## Next Steps

- [ ] Implement enhancement in hX.c
- [ ] Implement enhancement in hXX.c
- [ ] Apply to other sequences (hXH, hYXX, etc.)
- [ ] Test on Taurus
- [ ] Commit to repository
- [ ] Update documentation

---

**Requested by:** Chad Rienstra (during Taurus testing, Nov 9, 2025)
**Rationale:** Current messages don't provide specific guidance on how to fix violations
