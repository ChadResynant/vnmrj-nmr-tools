# Quick Start Guide: CP Optimization with OPTO

**For Topspin Users | Version 1.0**

This guide provides a complete workflow for optimizing cross-polarization (CP) conditions using OPTO on Bruker Topspin systems. It's designed for users familiar with OPTO who need guidance on CP-specific workflows.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Complete Workflow](#complete-workflow)
3. [Worked Examples](#worked-examples)
4. [Real-Time Monitoring](#real-time-monitoring)
5. [Troubleshooting](#troubleshooting)
6. [Reference Tables](#reference-tables)

---

## Prerequisites

### Sample Setup
- Sample loaded and spinning at target MAS rate
- Spinning rate stabilized (±2 Hz or better)
- Temperature equilibrated (allow 15-30 min after starting spin)
- Probe tuned and matched

### Initial Spectrum
- Acquire initial CP spectrum with reasonable starting parameters
- Verify you have detectable signal (SNR > 10:1 recommended)
- Parameters should be close to expected match conditions

### Software Requirements
- Topspin 3.6+ or 4.x
- OPTO installed and configured
- Python 3.x with numpy and matplotlib (for monitoring utility)

### Knowledge Requirements
- Theoretical CP match condition for your spinning rate
- Approximate RF power levels for your probe
- Safe power limits for extended experiments

---

## Complete Workflow

### Step 1: Acquire Initial Spectrum

```
1. Load experiment with starting CP parameters
2. Set appropriate:
   - Number of scans (NS) - enough for good SNR
   - Acquisition time (AQ)
   - Decoupling parameters
3. Acquire spectrum: zg
4. Process: efp
5. Phase correct if needed
```

### Step 2: Define Integration Region

**Critical:** OPTO maximizes the integrated signal in a defined region.

```
1. Display processed spectrum
2. Enter integration mode: .int
3. Clear any existing regions (if present)
4. Click and drag to define region around peak(s) of interest
   - For CP: integrate aliphatic and/or aromatic regions
   - Exclude baseline regions with only noise
5. Click "Export integration regions"
6. In dialog, type: opto-intrng
7. Select "Write New" (or double-click existing to overwrite)
8. Verify saved in: /opto/topspin4.x.x/exp/stan/nmr/lists/intrng
```

**Note:** If multiple regions exist, OPTO uses only the first one.

### Step 3: Set Up OPTO Parameter File

```
1. Open OPTO interface: open-opto
2. Click "New" to create new optimization job
3. For each parameter to optimize, click "Add Row":
```

**For typical CP optimization:**

| Parameter | Nickname | Safe Min | Safe Max | Step Size | Notes |
|-----------|----------|----------|----------|-----------|-------|
| `cnst41` | H1_power | 70000 | 150000 | 5000 | H1 RF field (Hz) |
| `cnst31` | C13_power | 20000 | 80000 | 2000 | C13 RF field (Hz) |
| `BETA41` | ramp_amp | 0 | 500 | 10 | Ramp amplitude |
| `DELTA41` | ramp_curve | -500 | 500 | 10 | Ramp curvature |

**Adjust ranges based on:**
- Your starting parameters (use ±20-30% around initial guess)
- Probe specifications (never exceed safe limits)
- Theoretical match conditions for your spinning rate

### Step 4: Configure Optimization Pass

```
1. Click "Add Pass"
2. Select which parameters to optimize together:
   - Option A: All parameters in one pass (slower but comprehensive)
   - Option B: Power levels first, then shapes (faster, sequential)
3. Set optimization criteria:
   - CoV tolerance: 0.01 (default)
   - Max time: 60-120 min (adjust based on NS and experiment time)
```

**Recommended strategy:**
- **Pass 1:** Optimize power levels (cnst41, cnst31) with fixed ramp
- **Pass 2:** Optimize ramp shape (BETA41, DELTA41) with optimized powers
- **Pass 3:** Fine-tune all parameters together

### Step 5: Save and Run

```
1. Click "Save" to save parameter file
2. Give it a descriptive name: CP_HN_55kHz_2025-11-09
3. Click "Run job"
4. Monitor progress in Topspin window
```

**Expected behavior:**
- OPTO will run multiple experiments, varying parameters
- Integration value plotted in real-time
- Terminal shows parameter values being tested (if started from terminal)

### Step 6: Monitor Optimization

**In Topspin window:**
- Real-time plot shows optimization progress
- Y-axis: integrated signal intensity
- X-axis: experiment number

**Optional external monitoring:**
See [Real-Time Monitoring](#real-time-monitoring) section below.

### Step 7: Retrieve Results

```
1. When optimization completes:
   - Final optimized parameters automatically loaded into experiment
   - Ramp shape files saved as: tan-shape-opto-41-beta_X-delta_Y
   - Shape automatically loaded into SPNAM41
2. Verify results:
   - Acquire spectrum with optimized parameters: zg
   - Compare signal intensity to initial spectrum
   - Expected improvement: 20-100% (varies by starting point)
```

---

## Worked Examples

These examples translate specific CP conditions (like those from Paul Schanda's group) into OPTO parameter files.

### Example 1: H-N CP at 55 kHz MAS

**Target condition:**
- ¹H: 90-95 kHz, ramp 90-100%
- ¹⁵N: 35-40 kHz
- Contact time: 800-1000 μs

**OPTO setup:**

| Parameter | Safe Min | Safe Max | Step | Starting Point | Notes |
|-----------|----------|----------|------|----------------|-------|
| `cnst42` | 80000 | 105000 | 2500 | 92500 | H1 power (Hz) |
| `cnst52` | 30000 | 45000 | 1000 | 37500 | N15 power (Hz) |
| `BETA42` | 0 | 500 | 10 | 250 | Ramp amplitude |
| `DELTA42` | -500 | 500 | 10 | 0 | Ramp curvature |
| `p15` | 800 | 1000 | 20 | 900 | Contact time (μs) |

**Expected match condition:** Low-power n=1 sideband match at 55 kHz

**Pass strategy:**
1. Pass 1: cnst42, cnst52 (find power match)
2. Pass 2: BETA42, DELTA42 (optimize ramp)
3. Pass 3: p15 (optimize contact time)

### Example 2: H-C CP at 100 kHz MAS (Low power on C)

**Target condition:**
- ¹H: 30 kHz, ramp 50-100%
- ¹³C: 70 kHz
- Contact time: 500 μs

**OPTO setup:**

| Parameter | Safe Min | Safe Max | Step | Starting Point | Notes |
|-----------|----------|----------|------|----------------|-------|
| `cnst41` | 20000 | 40000 | 1000 | 30000 | H1 power (Hz) |
| `cnst31` | 60000 | 80000 | 2000 | 70000 | C13 power (Hz) |
| `BETA41` | 0 | 1000 | 20 | 500 | Wide ramp amplitude |
| `DELTA41` | -500 | 500 | 10 | 0 | Ramp curvature |

**Expected match condition:** 0.3 × ωr on ¹H, 0.7 × ωr on ¹³C (fractional match)

**Note:** The wide ramp (50-100% = BETA range 0-1000) is critical for this condition.

### Example 3: Alternative H-C CP at 100 kHz (High power on H)

**Target condition:**
- ¹H: 130 kHz, ramp 90-100%
- ¹³C: 30 kHz
- Contact time: 500 μs

**OPTO setup:**

| Parameter | Safe Min | Safe Max | Step | Starting Point | Notes |
|-----------|----------|----------|------|----------------|-------|
| `cnst41` | 115000 | 145000 | 3000 | 130000 | H1 power (Hz) |
| `cnst31` | 20000 | 40000 | 1000 | 30000 | C13 power (Hz) |
| `BETA41` | 0 | 500 | 10 | 250 | Ramp amplitude |
| `DELTA41` | -500 | 500 | 10 | 0 | Ramp curvature |

**Expected match condition:** 1.3 × ωr on ¹H, 0.3 × ωr on ¹³C (reversed fractional match)

**Why two conditions?** At 100 kHz, both match conditions can work well. OPTO can explore both by widening search ranges.

### Example 4: N-CA CP at 38 kHz MAS

**Target condition:**
- ¹³Cα: 13.2 kHz
- ¹⁵N: 25 kHz, ramp 70-100%
- Contact time: 8 ms

**OPTO setup:**

| Parameter | Safe Min | Safe Max | Step | Starting Point | Notes |
|-----------|----------|----------|------|----------------|-------|
| `cnst34` | 10000 | 18000 | 500 | 13200 | CA power (Hz) |
| `cnst54` | 20000 | 30000 | 500 | 25000 | N15 power (Hz) |
| `BETA54` | 0 | 1000 | 20 | 500 | Wide ramp |
| `DELTA54` | -500 | 500 | 10 | 0 | Ramp curvature |
| `p15` | 7000 | 9000 | 200 | 8000 | Contact time (μs) |

**Expected match condition:** Low-power fractional match for heteronuclear transfer

**Note:** Heteronuclear transfers (N-C, C-N) require:
- Much longer contact times (8-15 ms vs 0.5-1 ms for H-X)
- Lower RF powers
- More patient optimization (longer experiments)

### Example 5: H-CA CP at 38 kHz MAS

**Target condition:**
- ¹H: 82 kHz
- ¹³Cα: 59 kHz, ramp 90-100%
- Contact time: 3 ms

**OPTO setup:**

| Parameter | Safe Min | Safe Max | Step | Starting Point | Notes |
|-----------|----------|----------|------|----------------|-------|
| `cnst41` | 70000 | 95000 | 2500 | 82000 | H1 power (Hz) |
| `cnst31` | 50000 | 70000 | 2000 | 59000 | CA power (Hz) |
| `BETA41` | 0 | 500 | 10 | 250 | Ramp amplitude |
| `DELTA41` | -500 | 500 | 10 | 0 | Ramp curvature |
| `p15` | 2500 | 3500 | 100 | 3000 | Contact time (μs) |

**Expected match condition:** n=2 on ¹H, n=1.5 on ¹³C at 38 kHz

---

## Real-Time Monitoring

The `opto_stripchart4.py` utility provides enhanced monitoring beyond the built-in Topspin plots.

### Setup

**Install dependencies:**
```bash
pip3 install numpy matplotlib
```

**Locate utility:**
```bash
cd /path/to/opto-docs/utilities/
```

### Usage

**Monitor specific OPTO file:**
```bash
./opto_stripchart4.py OPTO-2025-11-09-14-30-00 -p
```

**Monitor directory (auto-switches to new runs):**
```bash
./opto_stripchart4.py /opt/topspin4.4.0/data/user/nmr/OPTO-output -p --monitor-dir
```

**Command-line options:**
- `-p`: Plot parameter changes (highly recommended)
- `--monitor-dir`: Monitor directory for new OPTO files
- `-r N`: Refresh every N ms (default: 5000)
- `-i N`: Update interval N ms (default: 200)
- `-n N`: Keep last N points (default: 500)

### What You'll See

**Main plot:**
- Optimization score (integrated signal) vs iteration
- Real-time updates as OPTO runs
- Shows convergence behavior

**Parameter subplot (with `-p` flag):**
- All optimized parameters vs iteration
- Each parameter shown in different color
- Helps identify which parameters are still changing

### Interpreting Results

**Good optimization:**
- Score increases then plateaus
- Parameters converge to stable values
- No wild oscillations

**Problem indicators:**
- Score continuously decreasing → check integration region
- Parameters oscillating → may need tighter convergence criteria
- Flat score throughout → starting point may already be optimal (or terrible!)

### Saving Plots

- Plots automatically saved when:
  - Switching to new OPTO file (in monitor-dir mode)
  - Exiting with Ctrl+C
- Saved as: `OPTO-filename.png` in same directory

---

## Troubleshooting

### Problem: No Signal Improvement

**Possible causes:**
1. **Already at optimum:** Starting parameters were already excellent
   - Check: Compare initial and final parameters - very similar?
   - Solution: Widen search ranges to explore further

2. **Wrong match condition:** Optimizing around wrong CP condition
   - Check: Are power levels far from theoretical match?
   - Solution: Try alternative match conditions (see spinning rate guide below)

3. **Integration region includes noise:** Signal increase masked by baseline
   - Check: Re-examine integration region
   - Solution: Redefine region to exclude baseline, focus on peaks

4. **Hardware problem:** Probe detuning, power delivery issue
   - Check: Verify pulse widths independently
   - Solution: Re-tune probe, verify amplifier output

### Problem: OPTO Won't Start

**Error: "OPTO session already active"**
- Cause: Previous session not properly closed
- Solution:
  ```
  close-opto
  # Wait for "Closing server socket....." message
  # If no message appears: restart Topspin
  ```

**Error: "Cannot find integration region"**
- Cause: `opto-intrng` file missing or in wrong location
- Solution: Re-create integration region (see Step 2)

### Problem: Parameters Hit Limits

**Observation:** Optimized value = Safe Min or Safe Max
- Cause: Optimum lies outside search range
- Solution: Expand range in that direction, re-run
- Warning: Check probe specifications before expanding power ranges!

### Problem: High Variability in Results

**Observation:** Repeated optimizations give different results
- Causes:
  - Temperature not stabilized
  - Spinning rate fluctuating
  - Low SNR (random noise affects integration)
- Solutions:
  - Wait longer for equilibration (30-60 min)
  - Check bearing gas supply
  - Increase number of scans (NS)

### Problem: Ramp Shape Not Loading

**Error:** Shape file not found or not loaded into SPNAM
- Check: Shape file location
  ```
  ls $TOPSPIN_HOME/exp/stan/nmr/lists/wave/user/tan-shape-opto*
  ```
- Solution: Manually load shape file or re-run optimization

---

## Reference Tables

### Spinning Rate Guidelines

Use these to select appropriate match conditions and search ranges:

| MAS Rate | Regime | Typical Match Conditions | RF Powers | Contact Time |
|----------|--------|--------------------------|-----------|--------------|
| **100 kHz** | Ultra-high | Fractional ωr (0.3, 0.7, 1.3) | 30-130 kHz | 500-1000 μs |
| **55 kHz** | High | Low-power n=1 | 10-95 kHz | 500-1000 μs |
| **38 kHz** | Medium | n=1,2 sidebands | 50-120 kHz | 1-3 ms (H-X) |
| **20 kHz** | Medium | n=±1,±2 sidebands | 40-100 kHz | 1-5 ms |
| **10 kHz** | Low | Broader sideband exploration | 30-80 kHz | 2-8 ms |

**Heteronuclear (N-C, C-N) at any rate:** 10-40 kHz, 6-12 ms contact time

### Common Parameter Mappings

Based on typical Bruker pulse sequences:

| Parameter | Typical Assignment | Used In |
|-----------|-------------------|---------|
| `cnst41` | ¹H power level | H-C CP |
| `cnst42` | ¹H power level | H-N CP |
| `cnst40` | ¹H power level | N-H reverse CP |
| `cnst31` | ¹³C power level | H-C, C-N CP |
| `cnst33` | ¹³CO power level | CO-N CP |
| `cnst34` | ¹³Cα power level | CA-N CP |
| `cnst52` | ¹⁵N power level | H-N CP |
| `cnst54` | ¹⁵N power level | N-CA, CO-N CP |
| `p15` | Contact time | Most CP sequences |
| `d1` | Recycle delay | Most sequences |
| `BETA41` | Ramp amplitude (H1) | Sequences with SPNAM41 |
| `DELTA41` | Ramp curvature (H1) | Sequences with SPNAM41 |

**Note:** Actual mappings depend on your specific pulse sequence. Check pulse program code to verify.

### BETA/DELTA Ramp Translation

| Ramp Description | BETA Range | Notes |
|------------------|------------|-------|
| 90-100% (ramp up) | 0-500 | Standard narrow ramp |
| 100-90% (ramp down) | 0-500 | Reverse ramp direction in pulse program |
| 70-100% (wide) | 0-1000 | Broader coverage of match manifold |
| 50-100% (very wide) | 0-2000 | Maximum robustness, slower optimization |

**DELTA** controls curvature:
- Negative: Concave ramp profile
- Zero: Linear ramp
- Positive: Convex ramp profile

Range: -500 to +500 (typical), can extend to ±1000

### Search Range Recommendations

**Power levels:**
- Initial range: ±20% around theoretical match
- If optimization hits limit: Expand by 50% in that direction
- Maximum range: ±50% (wider suggests wrong match condition)

**Ramp parameters:**
- BETA: Start 0-500, expand to 0-1000 if needed
- DELTA: Usually -500 to +500 sufficient
- Step size: 10 (standard), use 20 for faster coarse search

**Contact time:**
- H-X transfers: ±20% around 0.5-1 ms
- Heteronuclear: ±25% around 6-10 ms
- Step size: 20-50 μs for H-X, 200-500 μs for N-C

### Optimization Time Estimates

Approximate time for one OPTO pass:

| Parameters | Search Points | Time per Expt | Total Time |
|------------|---------------|---------------|------------|
| 2 powers | ~100 | 30 sec | 50 min |
| 4 (2 powers + 2 ramp) | ~400 | 30 sec | 3.5 hr |
| 2 powers (fine) | ~50 | 60 sec | 50 min |
| 1 contact time | ~20 | 30 sec | 10 min |

**Strategies to reduce time:**
- Use fewer scans (NS) if SNR permits
- Sequential passes instead of simultaneous
- Coarse search first (larger steps), then refine
- Use theoretical starting point to narrow ranges

---

## Quick Reference: Complete Workflow Checklist

- [ ] Sample spinning and equilibrated
- [ ] Initial CP spectrum acquired
- [ ] Integration region defined and saved as `opto-intrng`
- [ ] Theoretical match condition identified for spinning rate
- [ ] Safe power limits verified for probe
- [ ] OPTO interface opened: `open-opto`
- [ ] Parameters added with appropriate ranges
- [ ] Optimization pass(es) configured
- [ ] Job saved with descriptive name
- [ ] Optional: Stripchart monitoring started
- [ ] Optimization running, monitoring progress
- [ ] Results verified: spectrum acquired with optimized parameters
- [ ] OPTO session closed: `close-opto`
- [ ] Optimized parameters documented for future reference

---

## Additional Resources

**Related Files:**
- `reference_data/schanda_cp_parameters.md` - Tested CP conditions at various MAS rates
- `OPTO_Topspin_Manual_1.2.34.docx` - Complete Topspin OPTO manual
- `docs/opto_frontend_design.md` - Comprehensive workflow automation design
- `CLAUDE.md` - Expert knowledge on CP pathologies and diagnostics

**Contact:**
- Primary: Chad Rienstra (NMRFAM / Resynant Inc.)
- Beta testers: Paul Schanda group (Grenoble)

---

**Version:** 1.0
**Last Updated:** November 2025
**Prepared by:** OPTO Workflow Automation Team
