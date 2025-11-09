# Schanda Group Beta Testing - Issues and Solutions

**Beta Tester**: Paul Schanda & Petra Rovó
**Location**: Grenoble, Austria
**System**: Bruker NEO, Topspin 4.5, Alma Linux 9
**OPTO Version**: 1.2.34 (stable)
**MAS Rates**: 38 kHz, 55 kHz, 100 kHz
**Last Updated**: 2025-11-09

## Overview

The Schanda group is testing OPTO for solid-state NMR CP optimization on high-field Bruker systems. This document tracks issues encountered and their resolutions.

## Critical Issues

### 1. Steady State Handling

**Problem**: First measurement has much higher intensity due to proper relaxation before starting OPTO optimization loop.

**Impact**:
- Skews optimization results
- Makes starting point appear artificially good
- Affects convergence behavior

**Status**: Workaround available (see below)

#### Varian Workaround (Working)

```bash
# In OpenVNMRJ:
# Set steady-state scans and total scans
ss=16 nt=8  # equivalent to ds=16 ns=8 on Bruker

# Start OPTO

# Execute manually after OPTO starts:
wexp('opto_spectral_measure ss=0')
```

This changes `ss` to 0 after the first experiment completes, ensuring subsequent experiments don't have the relaxation advantage.

#### Bruker Implementation (In Development)

**Proposed Solution**:
- Similar approach needed in Topspin Python code
- Requires careful review to avoid unintended consequences
- Short-term: Manual workaround possible
- Long-term: Automated handling in OPTO core

**Alternative Solution (Petra's Suggestion)**:
- Record first spectrum twice
- Use second measurement as optimization baseline
- Discard first measurement from optimization

**Priority**: Medium - Integration ranges and parameter naming are more urgent

---

### 2. Integration Ranges

**Problem**: Setting and verifying integration ranges is not well documented. Users unclear how to:
- Define regions of interest
- Verify ranges are properly loaded
- Troubleshoot when ranges don't match expected peaks

**Impact**:
- Incorrect optimization targets
- Failed optimizations
- User confusion

**Status**: Documentation needed urgently

#### Current Process (Bruker/Topspin)

1. Acquire initial 1D spectrum
2. Access integration module: `.int`
3. Clear existing regions
4. Define region of interest
5. Export integration regions
6. Save to `opto-intrng` file in: `/topspin-home/exp/stan/nmr/lists/intrng`

**Issues**:
- Steps not clearly documented for users
- No verification step
- Unclear what happens if multiple regions exist
- No warning if file is missing

**Solution**:
- Create step-by-step guide with screenshots
- Add verification command
- Document default behavior (full spectral width if no file)
- See: `opto/docs/troubleshooting/integration-ranges.md`

---

### 3. Parameter Naming Conventions

**Problem**: Confusion about how to name shape parameters (BETA/DELTA) and match them to pulse sequence variables.

**Impact**:
- Shape optimization fails
- Parameters don't get optimized
- Generated shapes not loaded correctly

**Status**: Requires clear documentation

#### Naming Rules

**Critical Convention**:
```
SPNAM[X] in pulse sequence → BETA[X] and DELTA[X] in OPTO job
```

**Example**:
```
Pulse sequence uses: SPNAM41
OPTO job must define: BETA41 and DELTA41

This generates: tan-shape-opto-41
Final optimized shape: tan-shape-opto-41-beta_[value]-delta_[value]
```

**Multiple Shapes**:
- SPNAM31 → BETA31, DELTA31
- SPNAM41 → BETA41, DELTA41
- SPNAM52 → BETA52, DELTA52
- All can be optimized in single OPTO run

**Parameter Ranges**:
- BETA: typically 0-500 (can extend to 0-1000)
- DELTA: typically -500 to 500 (can extend to -1000 to 1000)
- Step size: 10 is typical

**Solution**: See `opto/docs/quick-start/parameter-naming.md`

---

### 4. Optimization Behavior Issues

**Problem**: Multiple behavioral concerns during optimization:
- Takes many hours (unexpected duration)
- Signal drops from manually optimized starting point
- Doesn't recover to initial level
- Unclear if final value is actually the best

**Specific Observations**:
- Starting from manually optimized CP condition
- OPTO decreases signal and never returns to manual optimum
- Final selected value is NOT the best value recorded
- Best value was often the initial manual setting

**Impact**:
- Loss of confidence in OPTO
- Wasted spectrometer time
- Unclear whether to trust results

**Status**: Under investigation

#### Possible Causes

1. **Integration ranges incorrect**: Optimizing wrong peak/region
2. **Parameter ranges too wide**: Exploring non-productive parameter space
3. **Limits not matching**: Safe min/max don't reflect HH conditions
4. **Pass structure suboptimal**: Not progressing from coarse to fine properly
5. **Convergence criteria**: CoV tolerance or max time inappropriate
6. **Starting point issues**: Steady-state giving false initial high value

#### Initial Troubleshooting Steps

From Chad/Thiru's feedback:
> "Getting the integral ranges properly defined, the names matching, and the limits set to better values seemed to get things moving in the right direction."

**Action Items**:
1. ✅ Verify integration ranges cover correct resonances
2. ✅ Check parameter naming matches pulse sequence
3. ✅ Set realistic parameter limits based on HH conditions
4. Review pass structure (amplitudes → shapes → fine-tune)
5. Check convergence criteria for MAS rate
6. Verify steady-state handling

**Expected Runtime**:
- Target: <30 minutes
- Assumption: ~15 seconds per spectrum (ns=8, d1=1.5s, plus overhead)
- Multiple passes with different parameter sets

---

### 5. UI Clarity - "Abort Nicely" vs "Accept Pass"

**Problem**: Button functions not clearly explained in documentation.

**Impact**:
- Users unsure when to use each button
- Potential loss of optimization results
- Unclear what happens to data

**Status**: Needs documentation

#### Button Functions

**"Abort Nicely"**:
- Stops optimization immediately
- **Saves best result found so far**
- Ends OPTO job
- Keeps all log data

**"Accept Pass"**:
- Completes current pass
- Accepts results and moves to next pass (if defined)
- Continues optimization
- Used for normal pass-to-pass progression

**When to Use**:
- Use "Abort Nicely" when:
  - Optimization is not improving
  - You've found a good result and want to stop early
  - Something is clearly wrong (bad parameters, wrong target)
  - Approaching scheduled spectrometer time limit

- Use "Accept Pass" when:
  - Current pass has converged acceptably
  - Ready to move to next optimization stage
  - Normal progression through multi-pass strategy

**Solution**: See `opto/docs/quick-start/ui-reference.md`

---

## Feature Requests

### 6. HH Condition Intelligence

**Request** (Petra): OPTO should calculate distance from Hartmann-Hahn conditions and preferentially explore parameter space near HH matching.

**Current Status**:
- ✅ Implemented on Varian
- ❌ Not yet ported to Bruker/Topspin

**Proposed Implementation**:
- Setup wizard during job creation
- Pop-up asks for:
  - Nuclei involved (1H, 13C, 15N, etc.)
  - MAS rate
  - Type of HH condition (ZQ, DQ, n=0, n=1, n=2, etc.)
- OPTO calculates theoretical HH matching conditions
- Search algorithm biases exploration near calculated conditions
- Reduces parameter space dramatically

**Example**:
```
At 55 kHz MAS:
n=1 ZQ: ωH = 3/4 × ωR, ωN = 1/4 × ωR
       ωH ≈ 41.25 kHz, ωN ≈ 13.75 kHz

n=1 DQ: ωH = 3/2 × ωR, ωN = 1/2 × ωR
       ωH ≈ 82.5 kHz, ωN ≈ 27.5 kHz
```

**Priority**: Medium - Would significantly improve optimization efficiency

---

### 7. Spectrum Storage and Display

**Request**: Automatic storage and retrieval of optimization spectra.

**Current Limitations**:
- Can save all spectra, but crashes "Network for Advanced NMR" software at NMRFAM
- No option to save only "best N" spectra
- Can't automatically display best spectrum at end

**Proposed Features**:

1. **Save Best N Spectra** (N=5 typical):
   - Keep top 5 results by score
   - Automatic cleanup of intermediate spectra
   - Reduces storage burden

2. **Auto-Display Best Result**:
   - At completion, automatically load best spectrum
   - Show in Topspin window
   - Include comparison with initial spectrum

3. **ML Data Collection**:
   - Option to save all spectra for machine learning
   - Structured metadata (parameters, scores, conditions)
   - Compatible with future ML-based optimization

**Current Workaround** (Thiru to provide):
- Instructions for saving all files (causes issues)
- Disable for general use to prevent Network software crashes

**Priority**: Medium - Nice-to-have, not blocking current testing

---

### 8. Log File Analysis

**Request**: Tools to analyze OPTO logs for best N results when optimization stalls or has low SNR.

**Use Case**:
- Optimization has stalled but hasn't converged
- Low SNR makes individual measurements noisy
- Want to average best N results for final recommendation

**Current Solutions**:

#### Manual Sorting (Tedious but Works)

```bash
# Sort OPTO log by score (column Val = n_vars + 2)
more OPTO-log-name | nl | sort -nk(Val)
```

For example, if optimizing 3 parameters:
- Column for score = 3 + 2 = 5
```bash
more OPTO-2025-11-09-14-30-00 | nl | sort -nk5
```

#### Python Scripts (In Testing - Chad)

Chad has Python scripts that:
- Parse OPTO logs
- Extract best N results
- Calculate statistics (mean, std)
- Report optimized parameters

**Issue**: Require Python libraries that may conflict with Topspin on shared systems.

**IT Concern**: Installation of additional Python packages on shared spectrometers.

**Solution**:
- Package scripts as standalone tool
- Install on separate processing workstation
- Or use manual sorting method (works, just slower)

**Scripts Location**: To be added to `opto/utilities/`

**Priority**: Low - Workaround exists

---

## Solutions Summary

### Immediate Actions (High Priority)

1. **Documentation**:
   - ✅ Integration range setup guide
   - ✅ Parameter naming conventions
   - ✅ UI reference (button functions)
   - ⏳ Expected optimization behavior
   - ⏳ Performance benchmarks

2. **Default Job Files** (Chad working on):
   - hNH (out and back CP) - 700 MHz, 55 kHz
   - hCANH - 700 MHz, 55 kHz
   - hCONH - 700 MHz, 55 kHz
   - Design: <30 min runtime, multi-pass structure
   - Will be shared via Box directory

3. **Testing** (Thiru + Chad):
   - Complete Bruker integration range documentation
   - Test on Girolando (1.1 GHz Bruker)
   - Verify with Schanda group in Austria

### Medium-Term Development

1. **Steady-State Fix** (Thiru + Barry):
   - Implement Bruker equivalent of Varian workaround
   - Or implement "record twice" approach
   - Test for side effects

2. **HH Intelligence** (Future):
   - Port Varian implementation to Bruker
   - Add setup wizard
   - Document theoretical basis

3. **Spectrum Storage** (Barry + IT):
   - Implement "save best N" option
   - Resolve Network software conflicts
   - Add auto-display feature

### Long-Term Features

1. **ML Data Collection**:
   - Structured data export for all optimizations
   - Metadata standardization
   - Integration with analysis pipelines

2. **Web Interface**:
   - Real-time monitoring (stripchart in browser)
   - Remote job submission
   - Multi-user support

---

## Communication Log

### 2025-11-09: Initial Beta Testing Session

**Participants**: Chad, Thiru, Paul, Petra

**Duration**: Extended Zoom call

**Outcomes**:
- Identified integration range and naming as primary issues
- Resolved immediate blockers
- Planned default job file creation
- Scheduled follow-up testing

**Key Quote** (Chad):
> "Getting the integral ranges properly defined, the names matching, and the limits set to better values seemed to get things moving in the right direction."

### Next Steps

1. **Chad** (by EOD 2025-11-09):
   - Create default job files for 700 MHz, 55 kHz
   - Upload to Box directory
   - Test on available Bruker system

2. **Thiru** (by EOD 2025-11-09):
   - Document integration range setup (Bruker-specific)
   - Document shape file naming conventions
   - Document recent improvements to 1.2.34

3. **Schanda Group**:
   - Send pulse sequences with parameter names
   - Provide best-guess CP conditions for:
     - n=1 ZQ: ωH=3/2ωR, ωN=1/2ωR
     - n=1 DQ: ωH=3/4ωR, ωN=1/4ωR
     - NC and CN transfers
   - Test default jobs when available

4. **All**:
   - Schedule testing on Taurus or Ribeye (Varian, post-Ubuntu 24 setup)
   - Consider testing session tomorrow or Friday on Girolando

---

## Version Compatibility

| OPTO Version | Status | Notes |
|--------------|--------|-------|
| 1.2.34 | **Stable** | Currently recommended for beta testing |
| 1.3.x | **Broken** | Shimming panel issues, do not use |
| 1.2.24 | Last fully tested Varian | Pre-Oct bug fixes |

**Recommendation**: Use 1.2.34 for all current testing (both Varian and Bruker).

---

## Platform-Specific Notes

### Bruker NEO / Topspin 4.5

**Confirmed Working**:
- ✅ Basic OPTO installation
- ✅ CP optimization (with proper setup)
- ✅ Shape generation and loading
- ✅ Alma Linux 9 compatibility
- ✅ Java dependencies (install separately)

**Known Issues**:
- ⚠️ Integration range setup needs better docs
- ⚠️ Steady-state workaround not implemented
- ⚠️ Spectrum saving causes Network software issues

### Varian Comparison

**Varian Advantages** (as of 1.2.24):
- Steady-state workaround functional
- HH condition intelligence working
- Extensive field testing

**Bruker Catching Up**:
- Recent bug fixes (Oct 2024)
- Active development focus
- Beta testing providing critical feedback

---

## References

- **Email Thread**: Chad ↔ Schanda Group (2025-11-08 to 2025-11-09)
- **OPTO Manual**: `opto/docs/manual/OPTO_Topspin_Manual_1.2.34.docx`
- **Schanda CP Conditions**: `opto/examples/parameters/schanda-requests.txt`
- **Troubleshooting**: `opto/docs/troubleshooting/`

---

**Document Owner**: Chad Rienstra
**Contributors**: Thiru, Barry Dezonia, Paul Schanda, Petra Rovó
**Status**: Living document - updated as issues are resolved
