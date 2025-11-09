# OPTO Integration Ranges - Setup and Troubleshooting

**Platform**: Bruker Topspin (primary), Varian/OpenVNMRJ (secondary)
**OPTO Version**: 1.2.34+
**Last Updated**: 2025-11-09

## Overview

OPTO uses integration ranges to calculate the score for optimization. Properly defining these ranges is **critical** for successful optimization. This guide provides step-by-step instructions and troubleshooting for both platforms.

## Why Integration Ranges Matter

**What OPTO Does**:
1. Acquires a spectrum with current parameters
2. Integrates signal in defined region(s)
3. Uses integral value as optimization score
4. Adjusts parameters to maximize (or minimize) score

**Common Problems**:
- ❌ Wrong peak integrated → optimizes wrong transfer
- ❌ No range defined → uses full spectral width (includes noise)
- ❌ Multiple ranges → only first range used
- ❌ Range doesn't match current experiment → zero signal

---

## Bruker/Topspin Setup

### Step-by-Step Process

#### 1. Acquire Initial Spectrum

```bash
# In Topspin:
# Set up your pulse sequence with starting parameters
# Acquire a test spectrum to identify peaks of interest
zg
```

**Requirements**:
- Sufficient SNR to identify target peak
- Proper phase correction
- Baseline correction applied

#### 2. Open Integration Module

```bash
# Topspin command line:
.int
```

This opens the integration window.

#### 3. Clear Existing Regions

**Important**: Remove any previous integration ranges.

In integration module:
- Click "Clear All Integrals" or similar
- Verify no regions remain highlighted

#### 4. Define Region of Interest

**Methods**:

**Method A - Mouse Selection**:
1. Click and drag on spectrum to define region
2. Ensure region covers target peak with some margin
3. Avoid including baseline or neighboring peaks

**Method B - Manual Entry**:
1. Note chemical shift range (e.g., 118.5 to 122.3 ppm)
2. Enter values in integration dialog
3. Verify region appears correctly

**Best Practices**:
- Include peak with ~10-20% margin on each side
- Don't include baseline regions
- For multiple peaks: use single range covering all (if optimizing total signal)
- For specific transfer: use narrow range around target peak only

#### 5. Export Integration Regions

1. Click "Export integration regions" button
2. Integration file save dialog appears

#### 6. Save to opto-intrng File

**Critical**: File must be named exactly `opto-intrng` and saved in correct location.

**File Location**:
```
/opt/topspin4.x.x/exp/stan/nmr/lists/intrng/opto-intrng
```

**Steps**:
1. In save dialog, navigate to: `/opt/topspin<version>/exp/stan/nmr/lists/intrng/`
2. **If first time**: Click "Write New", enter filename: `opto-intrng`
3. **If updating**: Double-click existing `opto-intrng` to overwrite
4. Verify "File location is selected as default topspin folder"

**Verification**:
```bash
# Check file exists:
ls -la /opt/topspin4.5.0/exp/stan/nmr/lists/intrng/opto-intrng

# View contents:
cat /opt/topspin4.5.0/exp/stan/nmr/lists/intrng/opto-intrng
```

Expected format:
```
# Integration regions file
# ppm_start ppm_end
118.5 122.3
```

#### 7. Verify in OPTO

When starting OPTO, check Topspin terminal for:
```
Loading integration ranges from opto-intrng
Region 1: 118.5 to 122.3 ppm
```

**Warning Message** (if file missing):
```
No opto-intrng file found!!!!!
using the full spectra width for integration!!!!!!
save the opto-intrng file to select the region for integration
```

If you see this warning → **Stop and fix integration range setup before continuing!**

---

## Varian/OpenVNMRJ Setup

### Step-by-Step Process

#### 1. Acquire Initial Spectrum

```bash
# In OpenVNMRJ:
go  # or appropriate acquisition command
```

#### 2. Process and Phase

```bash
wft
aph  # or manual phasing
```

#### 3. Define Integration Region

```bash
# Method 1 - Interactive:
ds   # Display spectrum
# Use mouse to select region

# Method 2 - Command line:
# Set integration limits in ppm
intmod = 'partial'
```

#### 4. Save Integration File

```bash
# Save to opto-intrng in standard location
# (OpenVNMRJ-specific commands - to be verified with Varian users)
```

**File Location**:
```
~/vnmrsys/lists/intrng/opto-intrng
```

---

## Troubleshooting

### Problem: "No opto-intrng file found" Warning

**Symptoms**:
- Warning message in terminal
- OPTO uses full spectral width
- Optimization targeting noise instead of signal

**Diagnosis**:
```bash
# Check if file exists:
ls -la /opt/topspin*/exp/stan/nmr/lists/intrng/ | grep opto
```

**Solutions**:

1. **File doesn't exist**: Follow setup steps above

2. **Wrong location**:
   ```bash
   # Find any opto-intrng files:
   find /opt/topspin* -name "opto-intrng"

   # Move to correct location:
   cp /wrong/path/opto-intrng /opt/topspin4.5.0/exp/stan/nmr/lists/intrng/
   ```

3. **Permission issues**:
   ```bash
   # Check permissions:
   ls -la /opt/topspin4.5.0/exp/stan/nmr/lists/intrng/opto-intrng

   # Fix if needed:
   chmod 644 /opt/topspin4.5.0/exp/stan/nmr/lists/intrng/opto-intrng
   ```

### Problem: OPTO Optimizes Wrong Peak

**Symptoms**:
- Optimization converges but wrong signal improves
- Expected peak decreases while other peak increases
- Non-target resonance optimized

**Diagnosis**:
1. Display last acquired spectrum
2. Check which peak is in integration range
3. Verify against `opto-intrng` file contents

**Solutions**:

1. **Range too wide**:
   - Includes multiple peaks
   - Narrow range to target peak only

2. **Range shifted**:
   - Referenced incorrectly
   - Check if spectral reference (0 ppm) is correct
   - Redefine range on current spectrum

3. **Multiple regions defined**:
   - OPTO only uses **first** region in file
   - Remove extra regions
   - Redefine with single target region

### Problem: Optimization Score is Zero or Negative

**Symptoms**:
- OPTO reports zero integral
- Negative values in log
- No improvement possible

**Diagnosis**:
```bash
# Acquire spectrum manually
# Check integration in defined range
# Verify signal actually exists
```

**Solutions**:

1. **No signal in range**:
   - Wrong nucleus selected
   - Wrong frequency offset
   - Failed transfer (need better starting parameters)

2. **Phase correction needed**:
   - Acquire and phase spectrum manually
   - Verify positive integral
   - Check pulse sequence phase cycling

3. **Range outside spectrum**:
   - Spectral width too narrow
   - Offset incorrect
   - Redefine range within acquired spectrum

### Problem: Integration Range Changed Unexpectedly

**Symptoms**:
- Previously working range now wrong
- Different experiments have conflicting ranges
- Range from previous user's experiment

**Solutions**:

1. **Always verify before starting OPTO**:
   ```bash
   # Check current opto-intrng:
   cat /opt/topspin4.5.0/exp/stan/nmr/lists/intrng/opto-intrng
   ```

2. **Create experiment-specific ranges**:
   - Save different ranges for different experiment types
   - Copy appropriate file before starting OPTO
   - Consider versioning: `opto-intrng-hNH`, `opto-intrng-hCANH`, etc.

3. **Document your ranges**:
   - Keep notes on chemical shifts for each experiment type
   - Standard ranges for common samples (e.g., NAV, protein)

---

## Best Practices

### For CP Optimization

**1. Start with Manual Optimization**:
- Get reasonable starting point
- Verify signal is present
- Note chemical shift of target peak

**2. Define Narrow Ranges**:
- Single peak for specific transfer
- Exclude baseline and noise
- Typical range: 3-5 ppm width for 13C, 10-15 ppm for 15N

**3. Examples by Experiment Type**:

**H-N CP (15N detection)**:
```
# NAV sample, 15N amide region:
# Range: ~118-122 ppm (15N)
118.5 122.3
```

**H-C CP (13C detection)**:
```
# Adamantane, 13C:
# Range: ~38-39 ppm
37.5 39.5
```

**NC Transfer (13C → 15N)**:
```
# Protein, 15N amide:
# Range: ~115-125 ppm
115.0 125.0
```

### For Shimming

**Full spectral width or single peak**:
- Shimming: Often use full width or tallest peak
- Can be broader than CP optimization ranges

**Verification**:
- After shimming, spectrum should show improved linewidth across all peaks
- If optimizing single peak, check others didn't degrade

---

## Advanced: Multiple Regions

**Current Limitation**: OPTO uses **only the first** region in `opto-intrng` file.

**Workaround for Multiple Peaks**:

1. **Single wide region** covering all target peaks:
   ```
   # Optimize total signal from all peaks:
   35.0 55.0
   ```

2. **Sequential optimization** with different ranges:
   - Optimize peak 1 with `opto-intrng` set to peak 1 range
   - Save best parameters
   - Change `opto-intrng` to peak 2 range
   - Optimize peak 2 starting from peak 1 optimum
   - Compromise parameters for both

3. **Future enhancement**: Multi-objective optimization with weighted regions

---

## Quick Reference

### Bruker/Topspin Checklist

- [ ] Acquire initial spectrum with good SNR
- [ ] Open integration module (`.int`)
- [ ] Clear all existing regions
- [ ] Define region around target peak
- [ ] Export integration regions
- [ ] Save as `opto-intrng` in `/opt/topspin*/exp/stan/nmr/lists/intrng/`
- [ ] Verify file exists: `ls -la /opt/.../opto-intrng`
- [ ] Start OPTO and check terminal for "Loading integration ranges" message
- [ ] Verify no warning about missing file

### Varian/OpenVNMRJ Checklist

- [ ] Acquire and process initial spectrum
- [ ] Define integration region
- [ ] Save to `~/vnmrsys/lists/intrng/opto-intrng`
- [ ] Verify file exists
- [ ] Start OPTO

---

## Examples

### Example 1: H-N CP Optimization (Bruker, 700 MHz, 55 kHz MAS)

**Sample**: Protein or NAV
**Nucleus**: 15N
**Target**: Optimize H→N CP transfer

**Setup**:
1. Acquire 1D 15N spectrum with starting CP condition
2. Identify amide peak region (e.g., 118-122 ppm)
3. Define integration range: 117.5 to 122.5 ppm (with margin)
4. Save to `opto-intrng`
5. OPTO job optimizes: `cnst42` (H power), `cnst52` (N power), `BETA42`, `DELTA42`

**Expected**:
- OPTO maximizes 15N signal in defined range
- Optimal parameters improve H→N transfer
- Other resonances may also improve (good sign!)

### Example 2: Magic Angle Setting (Bruker, KBr sample)

**Sample**: KBr standard
**Nucleus**: 79Br
**Target**: Minimize linewidth

**Setup**:
1. Acquire 79Br spectrum
2. Use **full spectral width** for integration (measures total intensity)
3. OR use narrow range around single peak
4. Save to `opto-intrng`
5. Manually adjust magic angle while OPTO monitors in real-time

**Note**: For MA setting, often use FID area rather than integral
- See OPTO manual section on Magic Angle

---

## Related Documentation

- **OPTO Manual**: `opto/docs/manual/OPTO_Topspin_Manual_1.2.34.docx`
- **Schanda Group Issues**: `opto/docs/beta-testing/schanda-group-issues.md`
- **Parameter Naming**: `opto/docs/quick-start/parameter-naming.md`
- **Troubleshooting Guide**: `opto/docs/troubleshooting/common-issues.md`

---

## Support

**Questions?** Check:
1. This guide first
2. OPTO manual
3. Beta testing issues document
4. Contact: Chad Rienstra, Thiru, or Barry Dezonia

---

**Document Owner**: Chad Rienstra
**Contributors**: Thiru, Paul Schanda feedback
**Last Updated**: 2025-11-09
