# NMR Pulse Sequence Refactoring Implementation Plan

**Date:** November 9, 2025
**Model Strategy:** Use Sonnet for implementation tasks (faster, more efficient)
**Coordinator:** Opus for planning and critical decisions

## Executive Summary

Complete refactoring of VnmrJ/OpenVNMRJ pulse sequences incorporating PulSeq-inspired architecture, fixing critical safety issues from July 2025, and preparing for Resynant spectrometer compatibility.

## Phase 1: Critical Safety Fixes [URGENT]

### 1.1 Fix Duty Cycle Limits in Existing Refactored Sequences
**Subagent Task:** Review and correct all *-claude.c sequences
- **Files to fix:**
  - hXX-claude.c (change 10% → 5% or power-dependent)
  - hYYXH-claude.c (check if H-detected low-power, else 5%)
  - hYXX-claude.c (check power levels)
  - All other *-claude.c sequences
- **Implementation:**
  - Check if H-detected (ends in 'H')
  - If H-detected, implement power-dependent limits
  - If C-detected, enforce 5% limit
  - Add clear documentation for any exceptions

### 1.2 Consolidate Multiple Versions
**Subagent Task:** Merge competing implementations
- **Primary:** Consolidate 3 hXX versions → single hXX.c
- **Strategy:**
  - Use hXX_refactored.c as base (best structure)
  - Apply correct duty cycle limits
  - Remove redundant versions
  - Document consolidation decisions

## Phase 2: PulSeq-Inspired Architecture [Week 2-3]

### 2.1 Create NMRSystemLimits Structure
**Subagent Task:** Implement hardware limits object
```c
typedef struct {
    double max_grad;      // G/cm
    double max_slew;      // G/cm/ms
    double rf_dead_time;  // us
    double adc_dead_time; // us
    double grad_raster;   // us

    // Channel-specific RF limits
    struct {
        double max_amplitude;  // Hz
        double max_power;      // dB
        double max_duration;   // ms
        double duty_limit;     // fraction
    } rf[3];  // obs, dec, dec2

    // Probe-specific
    char probe_name[64];
    double probe_duty_limit;
    double min_srate;
    double max_srate;
} NMRSystemLimits;
```

### 2.2 Implement Make Functions Pattern
**Subagent Task:** Create validated pulse element generators
```c
// Core make functions needed:
makePulse()      // Hard pulses with validation
makeShapedPulse() // Shaped/adiabatic pulses
makeCP()         // Cross-polarization blocks
makeMixing()     // DARR/RFDR/etc mixing
makeDecoupling() // TPPM/SPINAL/WALTZ
makeEvolution()  // t1/t2/t3 evolution periods
```

### 2.3 Multi-Level Validation Framework
**Subagent Task:** Implement three-tier validation
1. Creation-time validation (in make functions)
2. Sequence-level validation (after parameter setup)
3. Runtime validation (before pulse execution)

## Phase 3: High-Impact Sequence Refactoring [Week 4-8]

### 3.1 Refactor Top Sequences (Cover 50% of Usage)
**Subagent Tasks:** One agent per sequence family

#### Agent 1: hX Family (28% of all usage)
- hX.c - Basic CP-MAS (16,915 uses)
- hX-50pctduty.c - Special variant
- Create unified hX with duty cycle modes

#### Agent 2: hXX Family (11% of all usage)
- hXX.c - DARR/RFDR mixing (6,800 uses)
- Already partially refactored, needs safety fixes
- Consolidate all variants

#### Agent 3: hXH Family (10% of all usage)
- hXH.c - H-detected CP (5,808 uses)
- Implement power-dependent duty cycles
- Optimize for low-power decoupling

#### Agent 4: Calibration Sequences
- mtune.c (3,052 uses)
- Htune.c (1,835 uses)
- Critical for daily operations

### 3.2 REDOR/TEDOR Implementation
**Subagent Task:** Modern distance measurement sequences
- REDOR.c with proper phase cycling
- TEDOR.c with XY-8 decoupling
- Unified distance measurement framework

## Phase 4: Infrastructure Decisions [Week 9-10]

### 4.1 Header File Assessment
**Subagent Task:** Evaluate each biosolid*.h header

| Header | Keep/Remove | Rationale |
|--------|-------------|-----------|
| biosolidcp.h | KEEP | Useful CP abstractions |
| biosolidmixing.h | KEEP | Complex mixing simplified |
| biosolidvalidation.h | REFACTOR | Convert to functions, not structs |
| biosolidevolution.h | REMOVE | Over-engineered, use inline |
| biosolidnus.h | KEEP | NUS is complex enough to warrant |
| biosolidmixing_simple.h | REMOVE | Redundant with main mixing |

### 4.2 Create Minimal Core Library
**Subagent Task:** Build essential functions only
```c
// nmr_core.h - Essential functions only
validate_duty_cycle()
validate_power_levels()
setup_cp_transfer()
setup_mixing_period()
calculate_evolution_time()
```

## Phase 5: Testing and Validation [Week 11-12]

### 5.1 Automated Testing Framework
**Subagent Task:** Build comprehensive test suite
- Parameter range testing
- Duty cycle validation
- Power limit checks
- Phase cycling verification
- Timing accuracy tests

### 5.2 Production Testing Protocol
**Subagent Task:** Real-world validation
- Test on standard samples
- Compare S/N with original sequences
- Verify probe safety
- Document performance metrics

## Subagent Deployment Strategy

### Parallel Execution Groups

**Group 1: Safety Fixes (Immediate)**
- Agent 1.1: Fix duty cycles in *-claude.c sequences
- Agent 1.2: Consolidate hXX versions
- Agent 1.3: Document all safety exceptions

**Group 2: Architecture (After Group 1)**
- Agent 2.1: Create NMRSystemLimits
- Agent 2.2: Implement make functions
- Agent 2.3: Build validation framework

**Group 3: Sequence Refactoring (After Group 2)**
- Agent 3.1: Refactor hX family
- Agent 3.2: Refactor hXH family
- Agent 3.3: Refactor calibration sequences
- Agent 3.4: Implement REDOR/TEDOR

**Group 4: Cleanup (After Group 3)**
- Agent 4.1: Evaluate headers
- Agent 4.2: Build core library
- Agent 4.3: Create test framework

## Success Metrics

1. **Safety:** All sequences comply with documented duty cycle limits
2. **Coverage:** Top 10 sequences refactored (>50% of usage)
3. **Quality:** No regression in S/N or performance
4. **Maintainability:** 50% code reduction through reuse
5. **Future-Proof:** Resynant spectrometer compatible

## Risk Mitigation

1. **Backup Strategy:** Keep original sequences with .backup extension
2. **Rollback Plan:** Git tags for each phase completion
3. **Testing:** Never deploy without full validation
4. **Documentation:** Every change documented in comments
5. **User Communication:** Clear migration guide for users

## Timeline

- **Week 1:** Safety fixes (Group 1 agents)
- **Week 2-3:** Architecture implementation (Group 2 agents)
- **Week 4-8:** Sequence refactoring (Group 3 agents)
- **Week 9-10:** Infrastructure cleanup (Group 4 agents)
- **Week 11-12:** Testing and validation

## Next Steps

1. Launch Group 1 agents immediately for safety fixes
2. Review Group 1 results before launching Group 2
3. Iterative deployment with validation gates between groups
4. Final production testing before deployment

---

**Ready to launch Sonnet subagents for Group 1 implementation**