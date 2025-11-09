# Variant File Analysis (.ayrshire and .backup)

**Generated:** November 9, 2025
**Purpose:** Document differences in experimental variant files

## Overview

Several pulse sequences have `.ayrshire` and `.backup` extensions indicating they were modified for debugging or testing purposes. These variants contain experimental changes that may be important for understanding issues or optimizing experiments.

## hXH.c.ayrshire vs hXH.c

**Main File:** hXH.c (June 29, 2025 - Taurus)
**Variant:** hXH.c.ayrshire (Sept 12, 2024 - synced from Ayrshire)

### Key Differences

#### 1. Array Formatting
**Change:** Whitespace removed from phase table arrays

**Ayrshire (.ayrshire):**
```c
static int table9[16]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int table8[16]  = {1,1,2,2,1,1,2,2,1,1,2,2,1,1,2,2};
```

**Taurus (main):**
```c
static int table9[16]  = {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};
static int table8[16]  = {1,1,2,2,1,1,2,2, 1,1,2,2,1,1,2,2};
```

**Impact:** None (cosmetic only), but Taurus version groups by 8 for readability.

#### 2. Duty Cycle Calculation
**Change:** Duty cycle calculation differs

**Ayrshire (.ayrshire):**
```c
duty = 4.0e-6 + 3*getval("pwH90") + getval("tHX") + getval("tconst") + getval("tXH") + t1Hecho + t2Hecho + getval("ad") + getval("rd") + at;
```

**Taurus (main):**
```c
duty = 4.0e-6 + 3*getval("pwH90") + getval("tHX") + getval("tconst") + getval("d2") + getval("tXH") + t1Hecho + t2Hecho + getval("ad") + getval("rd") + at;
```

**Impact:** Taurus includes `d2` in duty cycle, Ayrshire doesn't. This could affect experiments with significant t1 evolution periods.

**Question for Chad:** Was d2 excluded from duty cycle calculation for a reason on Ayrshire?

#### 3. Duty Cycle Threshold
**Change:** Maximum allowed duty cycle differs

**Ayrshire (.ayrshire):**
```c
if (duty > 0.2) {
    printf("Duty cycle %.1f%% >20%%. Abort!\n", duty*100);
```

**Taurus (main):**
```c
if (duty > 0.15) {
    printf("Duty cycle %.1f%% >15%%. Abort!\n", duty*100);
```

**Impact:** Ayrshire allows 20% duty cycle vs Taurus 15%. This is a significant difference!

**Question for Chad:** Was 20% threshold needed for specific experiments? Or was 15% too conservative?

#### 4. Homospoil Pulse
**Change:** PFG homospoil code commented out

**Ayrshire (.ayrshire):**
```c
/* Acode errors on pfg board 12/4/23 CMR
    if(!strcmp(HS,"hs")) {
        status(B);
```

**Taurus (main):**
```c
    if(!strcmp(HS,"hs")) {
        status(B);
```

**Impact:** Ayrshire has PFG gradient disabled due to hardware errors. Taurus version has working PFG.

**Note:** Comment indicates PFG board on Ayrshire had Acode errors as of Dec 4, 2023.

### Recommendation

**Use Taurus main version** because:
1. Includes d2 in duty cycle (more accurate)
2. Has working PFG homospoil code
3. More conservative 15% duty cycle threshold (safer)
4. Dated June 29, 2025 (8 months newer)

**Keep .ayrshire for reference** if:
- Debugging Ayrshire-specific experiments
- Need to understand why d2 was excluded
- Need to allow >15% duty cycle

---

## hXYXX_4D_S.c.backup.JUL1 vs hXYXX_4D_S.c

**Main File:** hXYXX_4D_S.c (Sept 12, 2024 - Taurus)
**Variant:** hXYXX_4D_S.c.backup.JUL1 (Sept 12, 2024 - pre-July 2025 changes)

### Key Differences

#### 1. Header Comment Changes
**Change:** Comments about phase cycle modifications

**Backup (JUL1):**
```c
CMR 7/26/17 6 pm:  substantially modified phase cycle optimized primarily for soft pulses on;
may not be optimal with soft pulses off (and I don't care) */
```

**Main (current):**
```c
CMR 7/26/17 6 pm:  substantially modified phase cycle optimized primarily for soft pulses on;
may not be optimal with soft pulses off (and I don't care)
CMR 7/24/24: Now I care. */
```

**Impact:** Indicates phase cycle was revisited in July 2025 to optimize for both soft and hard pulse modes.

#### 2. Phase Table: phH90 (table1)
**SIGNIFICANT CHANGE:** Spin temperature alternation added

**Backup (JUL1):**
```c
static int table1[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};  // phH90
```

**Main (current):**
```c
static int table1[32] = {0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,
                         2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0};  // phH90
```

**Impact:** Major change! Current version alternates 0-2-0-2 instead of blocks of 0s and 2s. This implements spin temperature alternation on every scan.

**Comment from code:** "7/26/24 4 am fixing DC offset issue with hard pulse version; need spin temp alternation on H for scans 1-2 and Y for scans 3-4"

#### 3. Phase Table: phXhard (table18)
**Change:** Hard X pulse phase shifted

**Backup (JUL1):**
```c
static int table18[8] = {0,0,0,0,0,0,0,0};   //*phXhard
```

**Main (current):**
```c
static int table18[8] = {1,1,1,1,1,1,1,1};   //*phXhard  12/24/22 shift from 0 to 1 CMR
```

**Impact:** Phase shifted by 90° for hard X pulses (change from Dec 24, 2022).

#### 4. Phase Table: phYxy (table7)
**Change:** Spin temperature alternation on Y channel

**Backup (JUL1):**
```c
static int table7[8]  = {0,0,0,0,0,0,0,0};   // phYxy
```

**Main (current):**
```c
static int table7[8]  = {0,0,2,2,0,0,2,2};   // phYxy
```

**Impact:** Alternation pattern added (pairs of 0s and 2s).

**Comment:** "7/26/24 CMR"

#### 5. Phase Table: phYyx (table9)
**Change:** Inversion pattern added

**Backup (JUL1):**
```c
static int table9[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};   // phYyx
```

**Main (current):**
```c
static int table9[16] = {0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2};   // phYyx
```

**Impact:** Second half (scans 9-16) inverted to 2s.

**Comment:** "invert scans 9 through 16 to setup CP condition CMR 4/14/23"

#### 6. Receiver Phase Tables
**MAJOR CHANGES:** All four receiver phase tables completely redesigned

**table20 (phRec):**

Backup (JUL1):
```c
{0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,2}
```

Current:
```c
{2,0,0,2,0,2,2,0, 0,2,2,0,2,0,0,2, 0,2,2,0,2,0,0,2, 2,0,0,2,0,2,2,0}
```

**table21 (phRecsoft1):**

Backup (JUL1):
```c
{0,0,2,2,2,2,0,0,2,2,0,0,0,0,2,2,2,2,0,0,0,0,2,2,0,0,2,2,2,2,0,0}
```

Current:
```c
{0,2,0,2,2,0,2,0, 2,0,2,0,0,2,0,2, 2,0,2,0,0,2,0,2, 0,2,0,2,2,0,2,0}
```

**table22 (phRecsoft2):**

Backup (JUL1):
```c
{0,2,0,2,2,0,2,0,2,0,2,0,0,2,0,2,2,0,2,0,0,2,0,2,0,2,0,2,2,0,2,0}
```

Current:
```c
{2,2,0,0,0,0,2,2, 2,2,0,0,0,0,2,2, 0,0,2,2,2,2,0,0, 0,0,2,2,2,2,0,0}
```

**table23 (phRecsoft12):**

Backup (JUL1):
```c
{0,2,2,0,2,0,0,2,2,0,0,2,0,2,2,0,2,0,0,2,0,2,2,0,0,2,2,0,2,0,0,2}
```

Current:
```c
{0,0,0,0,2,2,2,2, 0,0,0,0,2,2,2,2, 2,2,2,2,0,0,0,0, 2,2,2,2,0,0,0,0}
```

**Impact:** Complete redesign of receiver phase cycling. Current version implements patterns compatible with spin temperature alternation.

**Multiple comments:**
- "BDZ an older declaration"
- "7/26/24 CMR now change all recr in pattern 0220 on top of previous"
- "7/24 modulate 9-16 fixed!"
- "7/26/24 CMR: note that with spin temp alternation on H and Y, soft12 phase cycle will be suboptimal in 4 scans"
- "may need to define two separate logical cases for phH90 and phHxy to enable optimal performance for both use cases"

#### 7. CP Setup Code
**Change:** Bug fix comment removed

**Backup (JUL1):**
```c
strcpy(xy.to,"dec2");  // BDZ bug fixed here on 9-13-23: only "dec" was getting copied into "to" field.
```

**Main (current):**
```c
strcpy(xy.to,"dec2");
```

**Impact:** Bug fix integrated, comment removed.

### Timeline of Changes (hXYXX_4D_S.c)

1. **Dec 24, 2022:** phXhard shifted from phase 0 to 1
2. **April 14, 2023:** phYyx scans 9-16 inverted for CP condition
3. **Sept 13, 2023:** BDZ CP bug fix
4. **July 24, 2024 (4 am):** Major phase cycle redesign for DC offset
5. **July 26, 2024:** Receiver phase tables redesigned

### Recommendation

**Use main (current) version** because:
1. Contains critical July 2025 phase cycling fixes
2. Addresses DC offset issues in hard pulse mode
3. Implements proper spin temperature alternation
4. Optimized for both soft and hard pulse modes
5. Contains 2+ years of accumulated bug fixes

**Keep backup.JUL1 for reference** to:
- Understand evolution of phase cycling
- Revert if July 2025 changes cause issues
- Compare experimental results before/after changes

---

## Additional Variant Files

### hXYXX_4D_S.c.ayrshire
**Status:** Exists in Taurus psglib
**Note:** Not analyzed in detail yet. Likely contains Ayrshire-specific modifications similar to those in hXH.c.ayrshire (duty cycle, PFG, etc.)

## General Observations

1. **Systematic Development:** Changes are well-documented with dates and initials
2. **Hardware Dependencies:** Some changes (PFG disable) are hardware-specific
3. **Performance Optimization:** Phase cycling continuously refined for better results
4. **Safety vs Performance:** Duty cycle thresholds adjusted based on experience

## Recommendations for Chad

### Questions to Consider:

1. **Duty Cycle Thresholds:**
   - Should we standardize on 15% or 20%?
   - Are different thresholds needed for different spectrometers?

2. **d2 in Duty Cycle:**
   - Should d2 always be included in duty cycle calculations?
   - Were there specific experiments where excluding d2 was beneficial?

3. **PFG Hardware:**
   - Is Ayrshire PFG still broken?
   - Should we maintain separate versions for spectrometers with/without working PFG?

4. **Phase Cycling:**
   - Are July 2025 phase cycle changes validated by experiments?
   - Should we test both old and new phase cycles side-by-side?

### Next Steps:

1. Test compile all main versions
2. Run test experiments comparing phase cycles if possible
3. Document spectrometer-specific requirements
4. Consider maintaining branches for hardware variants
5. Archive variant files but use main versions for production

## Summary

The variant files contain valuable information about:
- Hardware-specific workarounds (PFG issues)
- Performance tuning (duty cycles)
- Systematic optimization (phase cycling evolution)

**Main versions are recommended for production** with variant files preserved for debugging and historical reference.
