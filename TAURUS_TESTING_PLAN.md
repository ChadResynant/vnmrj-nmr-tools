# Taurus Spectrometer Testing Plan - Morning Session

**Date:** November 10, 2025 (tomorrow morning)
**Location:** NMRFAM
**Spectrometer:** Taurus
**Estimated Time:** 2-3 hours
**Tester:** Chad Rienstra

---

## Pre-Testing Setup (15 minutes)

### 1. Update Repository on Taurus
```bash
# SSH to Taurus or work directly at console
cd /path/to/vnmrj-nmr-tools  # Adjust path as needed

# Fetch latest changes
git fetch origin

# Verify you have both branches
git branch -a
# Should see:
#   main
#   remotes/origin/development
#   remotes/origin/main

# For today's testing: use main branch (tested safety fixes)
git checkout main
git pull origin main

# Current version should be v1.1-stage1-complete
git describe --tags
```

### 2. Load Standard Sample
- **Sample:** Glycine or other standard CP sample
- **Purpose:** Reproducible baseline for comparisons
- **Preparation:** Standard packing, room temperature
- **Spinning:** 10-12 kHz (standard MAS rate)

### 3. Set Up Logging
```bash
# Create test log file
cd ~/vnmrj-nmr-tools/vnmrsys
echo "Taurus Testing Session - $(date)" > taurus_test_log.txt
echo "Tester: Chad Rienstra" >> taurus_test_log.txt
echo "Sample: [describe sample here]" >> taurus_test_log.txt
echo "Spinning rate: [rate] kHz" >> taurus_test_log.txt
echo "---" >> taurus_test_log.txt
```

---

## Testing Session (2-3 hours)

### Test 1: Verify Stage 1 Safety Fixes (30 minutes)

**Purpose:** Confirm that duty cycle fixes don't affect performance

#### Test 1A: hX.c - Most Critical Sequence (15 min)
```bash
# In VnmrJ/OpenVNMRJ
cd /vnmr/fidlib  # or your experiment directory

# Run standard hX experiment
hX  # Load sequence
# Set parameters:
d1=2.0         # Should be safe with new 5% limit
tHX=2.0m       # Standard CP time
pwH90=2.5u     # Standard proton 90
at=0.020       # Standard acquisition
srate=12000    # 12 kHz spinning

# Check that it doesn't abort
go  # Should run without errors

# Verify FID
dscale  # Check signal intensity
ft      # Process
aph     # Phase
```

**Expected Results:**
✅ Sequence runs without abort
✅ Signal intensity normal (compare to previous hX data)
✅ No warnings about duty cycle
✅ Spectrum looks normal

**Log Results:**
```bash
echo "Test 1A - hX.c: [PASS/FAIL]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  S/N: [value]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  Notes: [any observations]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
```

#### Test 1B: hXX.c - 2D DARR (15 min)
```bash
# Load hXX sequence
hXX
# Set parameters:
d1=2.0         # Safe with 5% limit
tHX=2.0m       # CP time
tXmix=20.0m    # DARR mixing (20 ms)
ni=8           # Small 2D for quick test (not full resolution)
phase=1,2      # Standard States phase cycling

# Estimate duty cycle manually:
# RF time = pwH90 + tHX + tXmix + at ≈ 2.5μs + 2ms + 20ms + 20ms = 42ms
# Cycle time = 42ms + d1(2s) = 2.042s
# Duty = 42/2042 = 2.1% < 5% ✓

# Should run without issue
go  # Start acquisition
```

**Expected Results:**
✅ 2D acquisition completes
✅ No duty cycle warnings
✅ t1 increments properly
✅ Data quality normal

**Log Results:**
```bash
echo "Test 1B - hXX.c: [PASS/FAIL]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  Duty cycle calculated: ~2.1%" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  Notes: [observations]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
```

---

### Test 2: Duty Cycle Protection Validation (45 minutes)

**Purpose:** Verify that the 5% limit actually protects the probe

#### Test 2A: Intentionally Violate Duty Cycle (15 min)

**⚠️ CRITICAL SAFETY TEST - Should ABORT before acquisition**

```bash
# Load hX sequence
hX

# Set parameters that SHOULD cause abort:
d1=0.1         # Very short relaxation (100 ms)
tHX=2.0m       # Standard CP
at=0.020       # Standard acquisition

# Calculate expected duty:
# RF time ≈ 2.5μs + 2ms + 20ms = 22ms
# Cycle time = 22ms + 100ms = 122ms
# Duty = 22/122 = 18% >> 5% limit
# This SHOULD ABORT

go  # Attempt to run
```

**Expected Results:**
✅ Sequence ABORTS before acquisition starts
✅ Clear error message: "Duty cycle 18% exceeds 5% limit"
✅ Message suggests increasing d1
✅ NO acquisition occurs (probe protected)

**If it DOESN'T abort:**
🚨 **STOP IMMEDIATELY** - Safety fix may not be working
🚨 **DO NOT run sequence** - probe damage risk
🚨 **Document in log** and investigate

**Log Results:**
```bash
echo "Test 2A - Duty cycle violation test: [PASS/FAIL]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  Expected duty: ~18%" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  Abort occurred: [YES/NO]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  Error message: [copy message]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
```

#### Test 2B: Find Minimum Safe d1 (15 min)
```bash
# Starting from Test 2A parameters, find minimum d1 that passes

# Try gradually increasing d1:
d1=0.3   # Try 300 ms
go       # Test - may still abort

d1=0.5   # Try 500 ms
go       # Test

# Continue until it runs without abort
# Expected minimum d1 ≈ 0.44s for these parameters
# (Duty = 22ms / (22ms + 440ms) = 4.8% < 5%)
```

**Expected Results:**
✅ Find specific d1 value where sequence runs
✅ Confirm calculated duty cycle is just under 5%
✅ Demonstrates that protection is accurate

**Log Results:**
```bash
echo "Test 2B - Minimum safe d1: [value]s" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  Calculated duty at minimum: [value]%" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
```

#### Test 2C: Long Experiment Safety (15 min)
```bash
# Test that protection works for overnight experiments

# Load hXX for 2D
hXX
d1=1.0         # Moderately short d1
tHX=2.0m
tXmix=50.0m    # Long mixing time (50 ms)
ni=128         # Realistic 2D size
nt=16          # Standard scans

# Calculate duty:
# RF time ≈ 2.5μs + 2ms + 50ms + 20ms = 72ms
# Cycle time = 72ms + 1s = 1.072s
# Duty = 72/1072 = 6.7% > 5% - should ABORT

go  # Should abort
```

**Expected Results:**
✅ Aborts with duty cycle error
✅ Prevents starting overnight run that would damage probe
✅ Suggests safe parameters (d1 ≥ 1.44s for this case)

---

### Test 3: Performance Validation (30 minutes)

**Purpose:** Verify that safety fixes don't degrade performance

#### Test 3A: Signal-to-Noise Comparison (15 min)
```bash
# Run identical experiments with old vs new code
# (If you have old data for comparison, otherwise skip)

# New code (current main branch):
hX
d1=2.0, tHX=2.0m, pwH90=2.5u, at=0.020, srate=12000
nt=4  # 4 scans for averaging
go

# After completion:
dscale
ft
aph
dsn  # Measure S/N

# Record value for comparison
```

**Expected Results:**
✅ S/N comparable to previous hX data (within 5%)
✅ Lineshape unchanged
✅ Phase behavior normal

**Log Results:**
```bash
echo "Test 3A - S/N comparison:" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  New code S/N: [value]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  Previous S/N: [value if available]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
echo "  Ratio: [new/old]" >> ~/vnmrj-nmr-tools/vnmrsys/taurus_test_log.txt
```

#### Test 3B: Timing Accuracy (15 min)
```bash
# Verify that pulse timing hasn't changed

# Use oscilloscope if available, or:
# Run hX and measure FID characteristics

hX
go

# Check FID:
# - Initial amplitude (CP efficiency)
# - Decay rate (no extra delays introduced)
# - Baseline (no timing glitches)
```

**Expected Results:**
✅ CP efficiency unchanged
✅ No timing artifacts in FID
✅ T2* decay matches previous data

---

### Test 4: Multiple Sequence Types (30 minutes)

**Purpose:** Ensure fixes work across sequence families

#### Test 4A: H-detected Sequence (15 min)
```bash
# Test hXH (H-detected, should have different duty limit if power-dependent code active)
# NOTE: On main branch, this may not have power-dependent limits yet
#       That's on development branch - will test later

hXH  # If available on Taurus
# Standard parameters
d1=1.5
tHX=2.0m

go  # Should run normally
```

**Expected Results:**
✅ Runs without error
✅ Normal H-detected spectrum

#### Test 4B: Calibration Sequence (15 min)
```bash
# Test mtune or Htune if available
mtune  # Or Htune

# These may have different duty cycle requirements
# Verify they still work
go
```

**Expected Results:**
✅ Calibration sequences still functional
✅ No unexpected errors

---

### Test 5: Edge Cases (30 minutes - if time permits)

#### Test 5A: Very Long CP Time
```bash
hX
d1=3.0         # Long d1
tHX=10.0m      # Very long CP (10 ms)
go
```

**Expected:** Should run (duty ~12ms/3s = 0.4%)

#### Test 5B: Fast Repetition
```bash
hX
d1=0.5         # Short but still safe
tHX=1.0m       # Short CP
go
```

**Expected:** Should run (duty ~11ms/500ms = 2.2%)

---

## Post-Testing Summary (15 minutes)

### 1. Review Test Log
```bash
cd ~/vnmrj-nmr-tools/vnmrsys
cat taurus_test_log.txt

# Count passes/fails
grep "PASS" taurus_test_log.txt | wc -l
grep "FAIL" taurus_test_log.txt | wc -l
```

### 2. Update Testing Checklist
```bash
# Edit TESTING_CHECKLIST.md
nano TESTING_CHECKLIST.md

# Mark completed tests:
# [x] Test 1: Architecture integration
# [x] Test 6: Duty cycle protection - C-detected
# etc.
```

### 3. Create Summary Report
```bash
cat > taurus_test_summary.txt << 'EOF'
TAURUS TESTING SUMMARY
Date: [date]
Tester: Chad Rienstra
Duration: [time]

CRITICAL TESTS (Must Pass):
- Duty cycle protection: [PASS/FAIL]
- Safety abort function: [PASS/FAIL]
- No probe damage: [PASS/FAIL]

PERFORMANCE TESTS:
- S/N maintained: [PASS/FAIL]
- Timing accurate: [PASS/FAIL]

OVERALL ASSESSMENT:
- Stage 1 safety fixes: [PRODUCTION READY / NEEDS WORK]

ISSUES FOUND:
[List any problems encountered]

RECOMMENDATIONS:
[Next steps based on results]
EOF
```

### 4. Commit Test Results
```bash
git add taurus_test_log.txt taurus_test_summary.txt TESTING_CHECKLIST.md
git commit -m "Taurus testing results - Stage 1 safety fixes

Completed tests:
- Duty cycle protection validation
- Performance comparison
- Edge case testing

Results: [summary]"

git push origin main
```

---

## Quick Reference Card (Print This!)

```
╔══════════════════════════════════════════════════════════════════════╗
║              TAURUS TESTING - QUICK REFERENCE                       ║
╠══════════════════════════════════════════════════════════════════════╣
║                                                                      ║
║  1. Setup (15 min)                                                   ║
║     - Load glycine sample, spin at 10-12 kHz                        ║
║     - git checkout main && git pull                                 ║
║     - Create test log: taurus_test_log.txt                          ║
║                                                                      ║
║  2. Safety Test - CRITICAL (30 min)                                 ║
║     - hX with d1=0.1s → SHOULD ABORT (duty ~18%)                   ║
║     - If it doesn't abort: STOP and investigate!                    ║
║     - Find minimum safe d1 (expect ~0.44s)                          ║
║                                                                      ║
║  3. Performance Test (30 min)                                       ║
║     - hX with normal parameters → should run fine                   ║
║     - Measure S/N, compare to previous                              ║
║     - Check FID for timing artifacts                                ║
║                                                                      ║
║  4. Normal Operation (30 min)                                       ║
║     - hX: Basic CP                                                  ║
║     - hXX: 2D DARR                                                  ║
║     - Both should work normally                                     ║
║                                                                      ║
║  EMERGENCY:                                                          ║
║    If duty cycle protection fails:                                  ║
║    - DO NOT run sequences with d1 < 1.0s                           ║
║    - Document the failure                                           ║
║    - Check for compilation errors                                   ║
║                                                                      ║
║  SUCCESS CRITERIA:                                                   ║
║    ✓ Short d1 causes abort (protection works)                      ║
║    ✓ Normal d1 runs fine (no false positives)                      ║
║    ✓ S/N unchanged (no performance loss)                           ║
║    ✓ No probe heating issues                                        ║
║                                                                      ║
╚══════════════════════════════════════════════════════════════════════╝
```

---

## Troubleshooting Guide

### Problem: Sequence aborts unexpectedly with normal parameters
**Cause:** Duty cycle calculation may be too conservative
**Solution:**
1. Calculate duty manually: (RF_time) / (RF_time + d1)
2. If calculated duty < 5% but still aborts, check code
3. Document exact parameters and error message
4. May need to adjust calculation in sequence

### Problem: Sequence doesn't abort with d1=0.1s
**Cause:** Safety fix may not be compiled/active
**Solution:**
1. Check which hX.c is being used: `which hX`
2. Check compilation date: `ls -l $(which hX)`
3. May need to recompile: `makeseqlib hX`
4. DO NOT use sequences until fixed!

### Problem: S/N is significantly lower
**Cause:** Possible timing change or parameter issue
**Solution:**
1. Compare exact parameters with old experiments
2. Check if d1 was inadvertently changed
3. Verify probe tuning hasn't drifted
4. Run old sequence version for comparison if possible

### Problem: Probe temperature rising
**Cause:** Duty cycle protection may not be working
**Solution:**
1. STOP acquisition immediately
2. Let probe cool (15-30 min)
3. Check duty cycle calculation
4. Use longer d1 (≥2.0s) until resolved

---

## Notes Section (Fill in during testing)

**Sample Details:**
- Sample ID: _______________
- Packing date: _______________
- Rotor: _______________
- Actual spinning rate: _______________ kHz

**System State:**
- Probe tuning: _______________
- Decoupling power (dpwr): _______________
- Temperature: _______________

**Observations:**
```
[Use this space for notes during testing]








```

**Follow-up Required:**
- [ ] Re-test specific sequence: _______________
- [ ] Investigate issue: _______________
- [ ] Update documentation: _______________
- [ ] Report to team: _______________

---

**END OF TESTING PLAN**

*Estimated total time: 2-3 hours*
*Bring: Notebook, this printout, coffee*
*Goal: Validate Stage 1 safety fixes on production spectrometer*
