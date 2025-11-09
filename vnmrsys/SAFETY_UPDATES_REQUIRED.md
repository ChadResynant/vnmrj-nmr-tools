# NMR Sequence Safety Updates Required

**Created:** November 9, 2025
**Purpose:** Action list for correcting safety exceptions found in comprehensive audit
**Related:** SAFETY_EXCEPTIONS.md, SAFETY_STANDARDS.md

---

## URGENT: Correct C-Detected Sequences Using 10% (Should be 5%)

These sequences are DANGEROUS and must be corrected immediately. They are C-detected but use 10% instead of the required 5% duty cycle limit.

### 1. hYXX.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/hYXX.c
**Current Limit:** 10% (line 189)
**Required Limit:** 5%
**Issue:** C-detected with high-power H decoupling, no justification for 10%

**Change Required:**
```c
// OLD:
if (duty > 0.1) {
    printf("Duty cycle %.1f%% >10%%. Abort!\n", duty*100);
    psg_abort(1);
}

// NEW:
if (duty > 0.05) {
    printf("Duty cycle %.1f%% >5%%. Abort!\n", duty*100);
    psg_abort(1);
}
```

**Add Documentation:**
```c
// Dutycycle Protection
// C-detected sequence: Standard 5% duty cycle limit
// See SAFETY_STANDARDS.md Section 1: Duty Cycle Limits
```

---

### 2. hYXX_S.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/hYXX_S.c
**Current Limit:** 10% (line 361-362)
**Required Limit:** 5%
**Issue:** Same as hYXX.c

**Change Required:** Same as above

---

### 3. hYXXsoft.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/hYXXsoft.c
**Current Limit:** 10% (line 201)
**Required Limit:** 5%
**Issue:** Same as hYXX.c

**Change Required:** Same as above

---

### 4. hhYXX_S.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/hhYXX_S.c
**Current Limit:** 10% (line 347-348)
**Required Limit:** 5%
**Issue:** Same as hYXX.c

**Change Required:** Same as above

---

## HIGH PRIORITY: C-Detected Sequence with 20% Limit

### 5. hXYXX_4D_S.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/hXYXX_4D_S.c
**Current Limit:** 20% (line 388)
**Required Limit:** 5% (unless probe-specific exception documented)
**Issue:** CRITICAL - 20% on C-detected is extremely dangerous

**Investigation Required:**
1. Is this sequence probe-specific (Ayrshire with enhanced cooling)?
2. Has this been validated with probe manufacturer?
3. Are there operational temperature measurements?

**If NO probe exception:** Change to 5% immediately
**If YES probe exception:** Add comprehensive documentation

---

## HIGH PRIORITY: Update H-Detected Sequences to 15%

These sequences are H-detected but use 10% instead of the documented 15% standard for low-power decoupling.

### 6. hYXH.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/hYXH.c
**Current Limit:** 10% (line 151-154)
**Required Limit:** 15%
**Issue:** H-detected sequence should use 15% with proper documentation

**Change Required:**
```c
// OLD:
if (duty > 0.1) {
    printf("Duty cycle %.1f%% >10%%. Abort!\n", duty*100);
    psg_abort(1);
}

// NEW:
// H-detected sequence: Low-power H decoupling during acquisition allows 15% duty cycle
// See SAFETY_STANDARDS.md Section 6: Power-Dependent Duty Cycle Limits
duty = duty/(duty + d1 + 4.0e-6);
if (duty > 0.15) {
    printf("Duty cycle %.1f%% >15%%. Abort!\n", duty*100);
    psg_abort(1);
}
```

---

### 7. hYXH_S.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/hYXH_S.c
**Current Limit:** 10% (line 145)
**Required Limit:** 15%
**Issue:** Same as hYXH.c

**Change Required:** Same as hYXH.c

---

### 8. HhXH.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/HhXH.c
**Current Limit:** 10% (line 123)
**Required Limit:** 15%
**Issue:** Inconsistent with HhXH-claude.c which uses 15%

**Change Required:** Update to match HhXH-claude.c (which has proper documentation)

---

### 9. HXhhXH_4D.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/HXhhXH_4D.c
**Current Limit:** 10% (line 156)
**Required Limit:** 15%
**Issue:** Inconsistent with HXhhXH_4D-claude.c which uses 15%

**Change Required:** Update to match HXhhXH_4D-claude.c

---

### 10. HhYXH.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/HhYXH.c
**Current Limit:** 10% (line 167)
**Required Limit:** 15%
**Issue:** Inconsistent with HhYXH-claude.c which uses 15%

**Change Required:** Update to match HhYXH-claude.c

---

## MEDIUM PRIORITY: Review 20% H-Detected Sequence

### 11. hYYXH.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/hYYXH.c
**Current Limit:** 20% (line 215)
**Recommended Limit:** 15% (unless special case documented)
**Issue:** Higher than standard 15% for H-detected

**Investigation Required:**
1. What is the decoupling power during acquisition?
2. Is there a probe-specific reason for 20%?
3. Has this been validated operationally?

**Likely Action:** Reduce to 15% and add documentation

---

## MEDIUM PRIORITY: Validate F19 Sequences

### 12. H_19F_FSR_rfdr_hXH_stppm.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/H_19F_FSR_rfdr_hXH_stppm.c
**Current Limit:** 10% (line 348)
**Issue:** F19 sequences may have different heating characteristics

**Investigation Required:**
1. What is typical F19 decoupling power?
2. Are there probe-specific F19 considerations?
3. Should this use 15% like other H-detected?

---

### 13. H_FSR_rfdr_hXH_prototype.c
**File:** /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib/H_FSR_rfdr_hXH_prototype.c
**Current Limit:** 10% (line 347)
**Issue:** Prototype status - needs validation before production

---

## DOCUMENTATION ONLY: Add References to Compliant Sequences

These sequences have correct limits but should add SAFETY_STANDARDS.md references for completeness.

### Already Documented Well:
- hXX.c (5%, complete documentation)
- HhXH-claude.c (15%, references SAFETY_STANDARDS.md)
- hXH.c (15%, references SAFETY_STANDARDS.md)
- HXhhXH_4D-claude.c (15%, references SAFETY_STANDARDS.md)
- hYYXH-claude.c (15%, references SAFETY_STANDARDS.md)
- All 4D _S sequences with 5% limits

### Could Add Brief Reference:
- hYXX-claude.c (add standards reference)
- hXXXX_S.c (add standards reference)
- hXXXX_S-claude.c (already good)
- hXXX_S.c (add standards reference)

---

## Test and Special Purpose Sequences (OK as-is)

These sequences have documented exceptions and do not require changes:

### 20% Test Sequences (Supervised Use Only):
- hX.c (basic CP calibration)
- hX-blankTOMCO.c (TOMCO amp testing)
- hX-testamps.c (amplitude testing)
- hX-withYdec.c (Y-channel testing)

### 50% Shimming Sequence:
- hX-50pctduty.c (shimming only, well documented)

---

## Implementation Protocol

For each sequence requiring updates:

### Step 1: Read Current Sequence
```bash
cd /home/chad/github-staging/vnmrj-nmr-tools/vnmrsys/psglib
cat [sequence_name].c | grep -A 5 -B 5 "duty.*>"
```

### Step 2: Make Changes
- Update duty cycle threshold
- Update error message
- Add documentation comment with SAFETY_STANDARDS.md reference

### Step 3: Validate
```bash
# Compile test
make [sequence_name]

# Verify changes
grep -n "duty > 0\." [sequence_name].c
grep -n "SAFETY_STANDARDS" [sequence_name].c
```

### Step 4: Test on Hardware
- Load sequence
- Set parameters to trigger duty cycle abort
- Verify error message is clear
- Test with normal parameters

### Step 5: Document
- Update SAFETY_EXCEPTIONS.md status
- Commit changes with clear message
- Update sequence version/date in comments

---

## Git Commit Messages (Suggested)

For urgent C-detected corrections:
```
CRITICAL: Correct duty cycle limit in hYXX.c from 10% to 5%

C-detected sequence was using 10% duty cycle without justification.
Standard 5% limit protects probe from thermal damage during high-power
H decoupling. Added reference to SAFETY_STANDARDS.md.

Risk: Potential probe damage from excessive RF heating
Testing: Validated abort triggers correctly at 5% threshold
References: SAFETY_EXCEPTIONS.md Section 3.3
```

For H-detected updates to 15%:
```
Update duty cycle limit in hYXH.c from 10% to 15%

H-detected sequence with low-power decoupling can safely use 15%
duty cycle. Standardized with other H-detected sequences and added
documentation reference to SAFETY_STANDARDS.md Section 6.

Justification: Low-power decoupling (1-2 kHz) vs high-power (80-100 kHz)
reduces RF heating by factor of ~1000, allowing 3x higher duty cycle.
References: SAFETY_EXCEPTIONS.md Section 3.1
```

---

## Validation Checklist

Before marking any sequence as complete:

- [ ] Duty cycle limit matches detection type
- [ ] Error message is clear and informative
- [ ] Documentation comment added with standards reference
- [ ] Code compiles without errors
- [ ] Abort condition tested on hardware
- [ ] SAFETY_EXCEPTIONS.md updated
- [ ] Git commit with clear message
- [ ] Peer review completed

---

## Priority Timeline

**Week 1 (URGENT):**
- [ ] Correct 4 C-detected sequences to 5% (items 1-4)
- [ ] Investigate hXYXX_4D_S.c 20% limit (item 5)

**Week 2 (HIGH):**
- [ ] Update 5 H-detected sequences to 15% (items 6-10)
- [ ] Review hYYXH.c 20% limit (item 11)

**Week 3-4 (MEDIUM):**
- [ ] Validate F19 sequences (items 12-13)
- [ ] Add documentation references to compliant sequences
- [ ] Review development/draft sequences

**Month 2:**
- [ ] Complete testing of all updated sequences
- [ ] Update facility documentation
- [ ] Train users on new standards

---

## Contact for Questions

**Safety Standards:** nmr-safety@institution.edu
**Sequence Development:** chad.rienstra@institution.edu
**Technical Issues:** [facility manager contact]

---

## Progress Tracking

| Sequence | Status | Assigned | Date | Tested | Complete |
|----------|--------|----------|------|--------|----------|
| hYXX.c | PENDING | | | | |
| hYXX_S.c | PENDING | | | | |
| hYXXsoft.c | PENDING | | | | |
| hhYXX_S.c | PENDING | | | | |
| hXYXX_4D_S.c | INVESTIGATING | | | | |
| hYXH.c | PENDING | | | | |
| hYXH_S.c | PENDING | | | | |
| HhXH.c | PENDING | | | | |
| HXhhXH_4D.c | PENDING | | | | |
| HhYXH.c | PENDING | | | | |
| hYYXH.c | PENDING | | | | |
| H_19F_*.c | INVESTIGATING | | | | |

---

**Document Status:** READY FOR IMPLEMENTATION
**Created:** November 9, 2025
**Next Review:** Weekly until all URGENT items complete
