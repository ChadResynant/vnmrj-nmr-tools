# Pulse Sequence Refactoring Strategy and Roadmap

**Created:** November 9, 2025
**Purpose:** Strategic plan for refactoring remaining pulse sequences
**Based on:** 59,550 experiments analyzed (2004-2025, 21 years)

## Executive Summary

The July 2025 refactoring established excellent infrastructure (biosolid*.h headers) but focused on moderate-use sequences. This roadmap prioritizes refactoring the **highest-impact sequences** based on 21 years of experimental data.

### Critical Insight

**Top 5 sequences = 63% of all experiments, but only #2 (hXX) has refactored versions!**

| Rank | Sequence | 21-Year Uses | % of Total | Refactored? |
|------|----------|--------------|------------|-------------|
| 1 | hX | 16,915 | 28.4% | ❌ **NO** |
| 2 | hXX | 6,800 | 11.4% | ✅ YES (3 versions) |
| 3 | hXH | 5,808 | 9.8% | ❌ **NO** |
| 4 | hYXX | 5,185 | 8.7% | ⚠️ hYXX-claude.c exists |
| 5 | hYXH | 3,138 | 5.3% | ❌ **NO** |

## July 2025 Refactoring Infrastructure

### Excellent Foundation Established

Five header files provide comprehensive refactoring infrastructure:

1. **biosolidvalidation.h** (11.9 KB)
   - Unified parameter validation
   - Duty cycle calculations
   - Evolution time validation
   - Gradient power protection
   - Array configuration checks
   - Consistent error reporting

2. **biosolidevolution.h** (15.3 KB)
   - Evolution period management
   - Automatic CT/regular switching
   - Composite pulse decoupling
   - Unified evolution timing

3. **biosolidcp.h** (5.1 KB)
   - Cross-polarization setup simplification
   - Automatic channel assignments
   - One-line CP initialization

4. **biosolidmixing.h** / **biosolidmixing_simple.h** (13.7 KB / 11.8 KB)
   - Mixing sequence system
   - Multiple mixing schemes (DARR, RFDR, SPC5, etc.)
   - Unified validation

5. **biosolidnus.h** (16.6 KB)
   - Non-uniform sampling support
   - Already established

### What Was Refactored (July 2025)

| Sequence | Usage (21 years) | Status |
|----------|------------------|---------|
| hXX (3 versions) | 6,800 | ✓ Complete (refactored, complete_refactor, claude) |
| hYYXH-claude.c | 107 | ✓ Refactored (surging in 2025) |
| hYyXH-claude.c | 471 | ✓ Refactored |
| HhXH-claude.c | 155 | ✓ Refactored |
| HhYXH-claude.c | ~74 | ✓ Refactored |
| HXhhXH_4D-claude.c | 90 | ✓ Refactored |
| hXXXX_S-claude.c | 310 | ✓ Refactored |
| hYXX-claude.c | 5,185 | ✓ Refactored |
| TEDOR-claude.c | Part of 197 | ✓ Refactored |

### Refactoring Benefits Demonstrated

**Original hXX.c:** 663 lines
**Refactored hXX_refactored.c:** 338 lines (49% reduction)
**Refactored hXX-claude.c:** 367 lines (45% reduction)

**Key Improvements:**
- CP setup: 20+ lines → 1 line (`setup_hx_cp()`)
- Parameter validation: Scattered checks → Unified validation system
- Evolution periods: Repetitive code → `biosolidevolution.h` functions
- Mixing sequences: Already unified in `biosolidmixing.h`
- Error messages: Vague → Specific, actionable
- Duty cycle: Manual calculations → Automatic with component tracking

## What's Missing: Critical Gap Analysis

### High-Impact Sequences Without Refactored Versions

#### Tier 1: Critical (>5,000 uses each)

**1. hX (16,915 uses - 28% of all data!)**
- **Priority: CRITICAL**
- Most used sequence across 21 years
- Basic CP sequence, foundation for many experiments
- Refactoring would benefit:
  - Better error messages for new users
  - Standardized duty cycle protection
  - Consistent parameter validation
- **Effort:** LOW (simpler than hXX)
- **Impact:** MAXIMUM

**2. hXH (5,808 uses - 10% of all data)**
- **Priority: CRITICAL**
- #3 most used sequence
- 2D CH/NH correlations
- Heavy use across all years (2004-2025)
- Includes variants: hXH_stppm (888 uses), hXH_FSR (903 uses)
- **Effort:** MODERATE
- **Impact:** VERY HIGH

**3. hYXX (5,185 uses - 9% of all data)**
- **Priority: CRITICAL**
- #4 most used sequence
- Note: hYXX-claude.c exists, but needs validation
- Includes hYXX_S variant (1,361 uses)
- **Effort:** LOW if hYXX-claude.c is good
- **Impact:** VERY HIGH

**4. hYXH (3,138 uses - 5% of all data)**
- **Priority: HIGH**
- #5 most used sequence
- 3D experiments (hCANH, hCONH)
- Steady use 2011-2025
- **Effort:** MODERATE-HIGH (3D complexity)
- **Impact:** HIGH

#### Tier 2: Important (1,000-3,000 uses)

**5. hXYXX_4D_S (2,137 uses)**
- **Priority: HIGH**
- Critical 4D workhorse
- Major use on Ayrshire (1,310 uses)
- Active 2015-2025
- **Effort:** HIGH (4D complexity)
- **Impact:** HIGH (specialized but critical)

**6. FS_REDOR (2,471 uses)**
- **Priority: MEDIUM**
- Historical importance (2016 campaign: 2,285 uses)
- Minimal current use (0 in 2021-2025)
- Already in repository
- **Effort:** MODERATE
- **Impact:** LOW (not currently used)
- **Decision:** Archive quality, not urgent

**7. hYXX_S (1,361 uses)**
- **Priority:** See hYXX
- May be covered by hYXX-claude.c
- Need to verify _S vs non-_S differences

## Refactoring Roadmap

### Phase 1: Critical Foundation Sequences (Q1 2026)

**Goal:** Refactor the three most-used sequences

#### 1.1 hX Refactoring (Target: January 2026)
**Usage:** 16,915 experiments (28% of all data)
**Complexity:** LOW
**Timeline:** 1-2 weeks

**Tasks:**
- [ ] Create hX-refactored.c using biosolid* infrastructure
- [ ] Implement validation system
- [ ] Add comprehensive duty cycle protection
- [ ] Create test plan document
- [ ] Low-power testing at spectrometers
- [ ] Production validation (run 10-20 experiments)
- [ ] Deploy to all spectrometers

**Expected Benefits:**
- Better error messages for most common sequence
- Standardized parameter validation
- Foundation template for future simple sequences
- Training example for new users

#### 1.2 hXH Refactoring (Target: February 2026)
**Usage:** 5,808 experiments (10% of all data)
**Complexity:** MODERATE
**Timeline:** 2-3 weeks

**Tasks:**
- [ ] Create hXH-refactored.c
- [ ] Handle variants (hXH_stppm, hXH_FSR considerations)
- [ ] Implement 2D evolution system
- [ ] Add NH/CH-specific validation
- [ ] Test with typical hNH and hCH experiments
- [ ] Low-power testing
- [ ] Production validation
- [ ] Deploy to all spectrometers

**Expected Benefits:**
- Unified 2D CH/NH correlation approach
- Consistent timing across variants
- Better evolution period handling

#### 1.3 hYXX Validation & Enhancement (Target: March 2026)
**Usage:** 5,185 experiments (9% of all data)
**Complexity:** LOW (if hYXX-claude.c is good)
**Timeline:** 1-2 weeks

**Tasks:**
- [ ] Review existing hYXX-claude.c
- [ ] Test against hYXX_S requirements
- [ ] Validate with historical parameters
- [ ] Low-power testing
- [ ] Production validation
- [ ] Document _S vs non-_S differences
- [ ] Deploy if validated

**Expected Benefits:**
- Consolidate hYXX and hYXX_S variants
- Modern infrastructure for 3D experiments

### Phase 2: 3D/4D Workhorse Sequences (Q2 2026)

#### 2.1 hYXH Refactoring (Target: April 2026)
**Usage:** 3,138 experiments (5% of all data)
**Complexity:** MODERATE-HIGH
**Timeline:** 3-4 weeks

**Tasks:**
- [ ] Create hYXH-refactored.c
- [ ] Implement 3D evolution system
- [ ] Handle triple-resonance complexity
- [ ] Validate hCANH, hCONH experiments
- [ ] Test with NUS (common for this sequence)
- [ ] Low-power testing
- [ ] Production validation
- [ ] Deploy

**Expected Benefits:**
- Modern 3D infrastructure
- Better NUS integration
- Cleaner triple-resonance code

#### 2.2 hXYXX_4D_S Refactoring (Target: May-June 2026)
**Usage:** 2,137 experiments
**Complexity:** HIGH
**Timeline:** 4-6 weeks

**Tasks:**
- [ ] Create hXYXX_4D_S-refactored.c
- [ ] Implement 4D evolution system
- [ ] Handle phase cycling complexity
- [ ] Extensive soft pulse management
- [ ] NUS integration (essential for 4D)
- [ ] Test with representative 4D datasets
- [ ] Low-power testing
- [ ] Production validation
- [ ] Deploy

**Expected Benefits:**
- Modern 4D infrastructure
- Template for future 4D sequences
- Better handling of complex phase cycling

### Phase 3: Validate Existing Refactored Sequences (Ongoing)

**Timeline:** Parallel to Phases 1-2

#### 3.1 Test hXX Refactored Versions (Q1 2026)
**Status:** Already refactored, needs production testing

**Tasks:**
- [ ] Compare all three hXX versions (refactored, complete_refactor, claude)
- [ ] Low-power testing
- [ ] Production validation (50+ experiments)
- [ ] Choose best version or merge features
- [ ] Deprecate other versions
- [ ] Document decision

#### 3.2 Test Surging hYYXH-claude.c (Q1 2026)
**Usage:** 107 total, but 92 in 2025 alone (surging!)
**Status:** Refactored, needs validation

**Tasks:**
- [ ] Low-power testing
- [ ] Production validation (compare to original)
- [ ] Deploy if validated
- [ ] Monitor usage in 2026

#### 3.3 Test Other Claude Sequences (Q2 2026)

**Low Priority (specialized or lower use):**
- [ ] HhXH-claude.c (155 uses)
- [ ] HhYXH-claude.c (74 uses)
- [ ] HXhhXH_4D-claude.c (90 uses)
- [ ] hYyXH-claude.c (471 uses)
- [ ] hXXXX_S-claude.c (310 uses)
- [ ] TEDOR-claude.c

### Phase 4: Historical/Specialized Sequences (Q3-Q4 2026)

**Lower Priority - As Needed**

#### 4.1 Consider Recovering Missing Illinois Sequences

**If scientifically relevant:**
- [ ] hXX_dream (1,148 uses, DREAM mixing) - Last used 2018
- [ ] hX_inept (939 uses, INEPT enhancement) - Last used 2014
- [ ] hXH_FSR (903 uses, Finite-pulse RFDR) - Last used 2017
- [ ] HhXH_redor (172 uses) - Last used 2015
- [ ] hXH_TEDOR, hYhXH_TEDOR (37+30 uses) - Last used 2017

**Decision criteria:**
- Scientific relevance today
- User demand
- Availability of old source code
- Effort to recover vs develop new

#### 4.2 REDOR/TEDOR Family

**Status:** Major sequences already in repository
- ✓ FS_REDOR (2,471 historical uses) - In repository
- ✓ ZF_TEDOR (241 uses) - In repository
- ✓ H_FSR_rfdr variants (210 uses) - In repository
- ⚠️ TEDOR-claude.c exists - needs validation
- ❌ hXH_TEDOR (37 uses) - Not in repository
- ❌ hYhXH_TEDOR (30 uses) - Not in repository
- ❌ HhXH_redor (172 uses) - Not in repository

**Recommendation:**
- REDOR/TEDOR usage down 93% since 2019
- Existing sequences adequate for current use
- Only refactor if specific scientific need arises

## Refactoring Standards and Best Practices

### Code Organization Template

Based on successful hXX refactoring:

```c
/*
 * <sequence_name>_refactored.c - Brief description
 *
 * Refactored using biosolid* infrastructure
 * Original: <author>, <date>
 * Refactored: <date>, using July 2025 infrastructure
 */

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"
#include "biosolidevolution.h"      // Evolution periods
#include "biosolidcp.h"             // Cross-polarization
#include "biosolidvalidation.h"     // Parameter validation
#include "biosolidmixing_simple.h"  // If needed

// Phase tables (unchanged from original)
static int table1[N] = {... };
// ... more phase tables ...

// Phase macros (unchanged from original)
#define phH90 t1
// ... more phase macros ...

void pulsesequence() {
    check_array();

    // ===============================================
    // PARAMETER SETUP WITH VALIDATION
    // ===============================================

    NMRValidation validator = init_nmr_validation();
    set_duty_limit(&validator, 0.1);  // Adjust as needed

    // Get parameters
    double pwH90 = getval("pwH90");
    // ... more parameters ...

    // ===============================================
    // CP SETUP (if applicable)
    // ===============================================

    CP hx = setup_hx_cp();  // One line!

    // ===============================================
    // MIXING SETUP (if applicable)
    // ===============================================

    MixingParams mixing = init_mixing_params(mMix, tXmix);
    setup_mixing_sequences(&mixing, "X");
    validate_mixing_parameters(&mixing);

    // ===============================================
    // EVOLUTION SETUP (if applicable)
    // ===============================================

    // Use biosolidevolution.h functions

    // ===============================================
    // COMPREHENSIVE VALIDATION
    // ===============================================

    add_pulse_time(&validator, "H90", pwH90);
    add_evolution_time(&validator, d2);
    add_cycle_time(&validator, getval("d1") + ...);
    validate_all_parameters(&validator);

    // ===============================================
    // PHASE TABLE SETUP
    // ===============================================

    settable(t1, 16, table1);
    // ... more phase table setup ...

    // ===============================================
    // PULSE SEQUENCE
    // ===============================================

    // Actual pulse sequence code here
    // Use helper functions from biosolid* headers
}
```

### Key Refactoring Principles

1. **Preserve Phase Cycling**
   - Never change phase tables without extensive testing
   - Phase cycling is scientifically validated
   - Document any changes with experimental justification

2. **Maintain Backward Compatibility**
   - Refactored sequences must produce identical spectra
   - All existing parameter files must work
   - Default behavior must be unchanged

3. **Improve Error Messages**
   - Replace `abort_message()` with specific validation
   - Provide actionable error messages
   - Include expected ranges and current values

4. **Consolidate Repetitive Code**
   - Use biosolid* helper functions
   - Eliminate copy-paste code
   - Maintain single source of truth for calculations

5. **Add Safety Checks**
   - Comprehensive duty cycle validation
   - Evolution time limits
   - Spinning rate minimums
   - Array configuration validation

6. **Document Changes**
   - Clear header comments explaining refactoring
   - Reference original sequence
   - Document any behavior changes
   - Create test plans

### Testing Requirements

**Every refactored sequence must pass:**

1. **Compilation Test**
   - Compiles without errors or warnings
   - All dependencies available

2. **Parameter Validation Test**
   - Test with known bad parameters
   - Verify error messages are clear
   - Check duty cycle limits

3. **Low-Power Testing** (in-person required)
   - Verify RF timing with oscilloscope
   - Check phase cycling
   - Validate all pulse widths

4. **Production Validation**
   - Run 10-20 real experiments
   - Compare spectra to original sequence
   - Quantitative SNR comparison
   - Check for artifacts

5. **Edge Case Testing**
   - Minimum parameter values
   - Maximum parameter values
   - Unusual combinations
   - All mixing/echo/CP variants

### Success Metrics

**For each refactored sequence:**

- ✓ Code reduction: Target 30-50%
- ✓ Error message quality: All users understand errors
- ✓ Spectrum quality: Identical to original (±2% SNR)
- ✓ Parameter compatibility: 100% of old parameters work
- ✓ User acceptance: Positive feedback from group
- ✓ Maintenance: Easier to modify and extend

## Resource Requirements

### Time Estimates

| Sequence Complexity | Refactoring Time | Testing Time | Total |
|---------------------|------------------|--------------|-------|
| Simple (hX) | 1 week | 1 week | 2 weeks |
| Moderate (hXH, hYXX) | 2 weeks | 1-2 weeks | 3-4 weeks |
| Complex (hYXH) | 3 weeks | 1-2 weeks | 4-5 weeks |
| Very Complex (hXYXX_4D_S) | 4-5 weeks | 2-3 weeks | 6-8 weeks |

### Personnel

**Refactoring Work:**
- Can be done remotely with Claude Code assistance
- Requires NMR programming expertise
- Benefits from automated refactoring tools

**Testing Work:**
- In-person at spectrometers (low-power tests)
- Real experiments for production validation
- Graduate students can assist with testing

### Infrastructure

**Already in place:**
- ✓ biosolid* header infrastructure
- ✓ Git repository for version control
- ✓ 21 years of experimental validation data
- ✓ Three production spectrometers for testing

## Priority Matrix

### Immediate Priority (Q1 2026)

1. **hX refactoring** - CRITICAL (16,915 uses, 28% of data)
2. **hXX testing/deployment** - HIGH (already refactored, needs validation)
3. **hYYXH testing** - HIGH (surging usage in 2025)

### High Priority (Q2 2026)

4. **hXH refactoring** - CRITICAL (5,808 uses, 10% of data)
5. **hYXX validation** - CRITICAL (5,185 uses, 9% of data)
6. **hYXH refactoring** - HIGH (3,138 uses, 5% of data)

### Medium Priority (Q3 2026)

7. **hXYXX_4D_S refactoring** - HIGH (2,137 uses, specialized)
8. **Remaining claude sequences testing** - MEDIUM (various)

### Low Priority (Q4 2026 and beyond)

9. **Historical sequence recovery** - LOW (if scientifically needed)
10. **REDOR/TEDOR refactoring** - LOW (already in repository, low current use)

## Long-Term Vision

### Complete Refactored Suite (2026-2027)

**By end of 2027, have refactored versions of:**
- All sequences with >1,000 uses (top 10-15 sequences)
- All actively used 3D/4D sequences
- Complete biosolid* infrastructure documentation
- Training materials for new sequences

### Benefits

1. **Immediate:**
   - Better error messages
   - Fewer failed experiments
   - Easier troubleshooting

2. **Medium-term:**
   - Faster new sequence development
   - Consistent behavior across sequences
   - Easier maintenance

3. **Long-term:**
   - Foundation for next-generation sequences
   - Training platform for new users
   - Reduced technical debt

## Recommendations

### Start Now (November 2025)

1. **Validate existing refactored sequences**
   - Get hXX refactored versions tested
   - Confirm they work in production
   - Build confidence in approach

2. **Begin hX refactoring**
   - Simplest high-impact sequence
   - Perfect learning example
   - Immediate benefit to most users

3. **Document process**
   - Record lessons learned
   - Refine standards
   - Create templates

### Next Steps (December 2025 - January 2026)

1. Schedule low-power testing sessions
2. Recruit graduate students for testing
3. Set up testing protocols
4. Begin hX refactoring work

### Success Indicators

**After Phase 1 (Q1 2026):**
- hX, hXX, hYXX refactored and validated
- Covers ~50% of all experimental usage
- Proven refactoring process
- User acceptance confirmed

**After Phase 2 (Q2 2026):**
- hXH, hYXH refactored
- Covers ~73% of all experimental usage
- 3D infrastructure solid
- Ready for 4D work

**After Complete Roadmap (End 2026):**
- All major sequences refactored
- Covers >85% of experimental usage
- Complete, consistent infrastructure
- Modern, maintainable codebase

---

*This roadmap provides a clear path to modernizing the pulse sequence library while prioritizing sequences that will benefit the most users and experiments.*
