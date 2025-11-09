# NMR Refactoring Testing Checklist

**Status:** Architecture untested at spectrometers
**Branch:** development
**Safe for Production:** NO - requires testing first

## Stage 2 Architecture Testing Requirements

### Pre-Testing Validation (Can do now)
- [ ] Code compiles without errors
- [ ] No syntax errors in headers
- [ ] Proper header guards present
- [ ] No obvious memory leaks (static analysis)
- [ ] Documentation matches implementation

### Spectrometer Testing (Required before production)

#### Test 1: Architecture Integration Test
**Sequence:** Simple test sequence using make functions
**Duration:** 10 minutes
**Sample:** Standard sample (glycine, etc.)
**Success Criteria:**
- [ ] Sequence compiles with new headers
- [ ] No runtime errors
- [ ] Parameters validated correctly
- [ ] Duty cycle calculations accurate

#### Test 2: Validation Framework Test
**Sequence:** Test with intentionally bad parameters
**Duration:** 5 minutes
**Sample:** No sample needed (abort before acquisition)
**Success Criteria:**
- [ ] Catches duty cycle violations (try 20% duty)
- [ ] Catches power limit violations
- [ ] Error messages are clear and helpful
- [ ] Suggested solutions are correct

#### Test 3: Make Functions Test - Basic Pulses
**Sequence:** Simple hX using makePulse() and makeCP()
**Duration:** 15 minutes
**Sample:** Standard sample
**Success Criteria:**
- [ ] Spectrum acquired successfully
- [ ] S/N matches original hX.c
- [ ] No artifacts from new code
- [ ] Timing is correct (compare FIDs)

#### Test 4: Make Functions Test - Complex
**Sequence:** hXX using makeMixing() with DARR
**Duration:** 30 minutes
**Sample:** Standard 2D sample
**Success Criteria:**
- [ ] 2D spectrum acquired
- [ ] Mixing time accurate
- [ ] Cross peaks present
- [ ] Performance matches original

#### Test 5: Power-Dependent Duty Cycle
**Sequence:** H-detected sequence (hXH)
**Duration:** 20 minutes
**Sample:** Standard sample
**Success Criteria:**
- [ ] Detects H-detected correctly
- [ ] Uses 15% duty limit (not 5%)
- [ ] Power calculation correct
- [ ] No false positives on duty cycle

### Safety Validation (Critical)

#### Test 6: Duty Cycle Protection - C-detected
**Sequence:** hX with very short d1 (should abort)
**Expected:** ABORT before acquisition
**Success Criteria:**
- [ ] Aborts when duty > 5%
- [ ] Error message shows actual duty cycle
- [ ] Suggests correct d1 value
- [ ] Does NOT damage probe

#### Test 7: Duty Cycle Protection - H-detected
**Sequence:** hXH with short d1 (should abort if >15%)
**Expected:** ABORT before acquisition
**Success Criteria:**
- [ ] Allows up to 15% for low-power H
- [ ] Aborts when duty > 15%
- [ ] Calculates power correctly
- [ ] Distinguishes from C-detected

### Production Readiness Tests

#### Test 8: Backward Compatibility
**Sequences:** All original sequences (hX, hXX, hXH)
**Duration:** 1 hour
**Success Criteria:**
- [ ] Original sequences still compile
- [ ] Original sequences still run
- [ ] No performance regression
- [ ] Users see no difference

#### Test 9: Multiple Spectrometers
**Spectrometers:** Test on 2-3 different systems
**Duration:** 2 hours total
**Success Criteria:**
- [ ] Works on Taurus
- [ ] Works on Ayrshire
- [ ] Works on Ribeye
- [ ] Probe-specific limits respected

#### Test 10: Long-term Stability
**Duration:** Run overnight experiments
**Success Criteria:**
- [ ] No memory leaks
- [ ] No crashes during long runs
- [ ] Temperature stable (probe cooling adequate)
- [ ] Reproducible results

## Testing Timeline

**Minimum testing:** Tests 1-7 (2 hours)
**Recommended testing:** Tests 1-9 (4 hours)
**Full validation:** All tests (1 day)

## Sign-off

After testing complete:
- [ ] All critical tests passed (Tests 1-7)
- [ ] No probe damage observed
- [ ] Performance acceptable
- [ ] Ready for production merge to main

**Tested by:** _________________
**Date:** _________________
**Spectrometer:** _________________
**Notes:**

---

## Quick Reference: What's Safe Now

### ✅ SAFE TO USE (tested, on main branch):
- Original hX.c, hXX.c, hXH.c, etc. (with Stage 1 safety fixes)
- All sequences that don't use new architecture
- Duty cycle limits are now correct (5% for C-detected)

### ⚠️ EXPERIMENTAL (untested, on development branch):
- nmr_system_limits.h
- nmr_make_functions.h/c
- nmr_validation.h/c
- Any sequences refactored to use new architecture

### 🚫 NOT READY:
- Production use of make functions
- Automated validation in production sequences
- Stage 3 refactored sequences (not yet created)

---

**Branch Strategy:**
- `main` = production-tested code only
- `development` = integration branch for testing
- Merge `development` → `main` only after this checklist passes
