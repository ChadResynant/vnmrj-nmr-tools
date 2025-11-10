# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

**PUBLIC REPOSITORY** - Open source VnmrJ/OpenVNMRJ NMR tools for solid-state and solution NMR data acquisition, processing, and analysis. Developed across multiple NMR facilities (NMRFAM, Resynant, UIUC) for 400-750 MHz spectrometers.

## Repository Structure

### Key Directories

- **vnmrsys/** - Chad's custom pulse sequences and templates (hX family - updated versions)
  - psglib/ - 39 custom pulse sequences (.c files)
  - maclib/ - 158 macros for automation and calibration
  - templates/layout/ - VnmrJ XML templates for GUI parameter panels
  - psg/ - Shared C headers and utilities

- **biosolidspack/** - Agilent BioSolids Pack reference implementation (ahX family - Agilent version)
  - psglib/ - Agilent's ahX pulse sequence family (reference style)
  - maclib/ - Setup macros (AhX, AhXX, etc.)
  - templates/layout/ - Complete template examples for ahX, ahXX, ahXYX
  - parlib/ - Parameter sets for each sequence
  - studylib/ - Automated calibration protocols
  - manual/ - Help documentation

- **biopack/** - Agilent BioPack for solution NMR
  - Complete protein NMR pulse sequence library
  - bin/ - Helper scripts for NMRPipe integration

- **python/** - Python utilities for data management
  - utilities/ - organize_nmr_data.py, supercleanup.py

- **macros/** - General acquisition and processing macros
- **pulsesequences/** - Additional custom sequences
- **docs/** - Documentation

## Development Pattern: ahX as Style Guide

### The ahX/hX Relationship

**ahX (Agilent version)** in biosolidspack/ is the **style guide** and reference implementation:
- Location: `biosolidspack/psglib/ahX.c`
- Setup macro: `biosolidspack/maclib/AhX`
- Templates: `biosolidspack/templates/layout/ahX/*.xml`

**hX (Chad's version)** in vnmrsys/ is the updated/enhanced version:
- Location: `vnmrsys/psglib/hX.c`
- Templates: `vnmrsys/templates/layout/hX/*.xml`
- Enhanced with: Power-dependent duty cycle checks, additional decoupling modes, improved safety

**Naming Convention:**
- Agilent: `ahX`, `ahXX`, `ahXYX`, `ahYXX` (lowercase 'a' prefix)
- Chad's: `hX`, `hXX`, `hXYX`, `hYXX` (no 'a' prefix)

### Template/Layout Architecture (from ahX)

Each sequence has a complete template system organized by tab:

**Template Directory Structure:**
```
biosolidspack/templates/layout/ahX/
├── PulseSequence.xml    # Main pulse sequence parameters
├── Channels.xml         # RF channel configuration
├── Acquisition.xml      # Acquisition parameters
├── Receiver.xml         # Receiver settings
├── Quick.xml           # Quick access panel
├── MorePS.xml          # Additional sequence options
└── acq.xml             # Acquisition tree integration
```

**Apply this pattern to vnmrsys sequences:**
```
vnmrsys/templates/layout/hX/
├── PulseSequence.xml
├── Channels.xml
├── Acquisition.xml
├── Calibration.xml     # Chad's addition for automated calibration
├── Receiver.xml
└── acq.xml
```

### VnmrJ Template XML Syntax

**Critical VnmrJ XML Rules:**

1. **Comparison operators MUST be escaped:**
   ```xml
   <!-- CORRECT -->
   show="if (panellevel&gt;29) then $VALUE=1 else $VALUE=0 endif"

   <!-- WRONG - will not parse -->
   show="if (panellevel>29) then $VALUE=1 else $VALUE=0 endif"
   ```

2. **String comparisons use if-then-else syntax:**
   ```xml
   <!-- CORRECT -->
   show="if (pseq='one') then $VALUE=1 else $VALUE=0 endif"

   <!-- WRONG - incorrect syntax -->
   show="$SHOW=(pseq=one)"
   ```

3. **Logical operators:**
   - AND: `and`
   - OR: `or`
   - NOT EQUAL: `&lt;&gt;` (not `!=`)
   - GREATER THAN: `&gt;`
   - LESS THAN: `&lt;`

4. **Checking parameter existence:**
   ```xml
   show="on('tECHO'):$VALUE"
   ```

5. **Conditional display based on multiple parameters:**
   ```xml
   show="on('pseq'):$pseqon if (($pseqon=1) and ((pseq='four') or (pseq='five'))) then $VALUE=1 else $VALUE=0 endif"
   ```

**Template Element Types:**

- `<entry>` - Editable text field
- `<label>` - Static text label
- `<textmessage>` - Display-only calculated value
- `<menu>` - Dropdown selector
- `<button>` - Action button (calls macro/command)
- `<group>` - Container for organizing widgets

**Common Patterns:**

```xml
<!-- Parameter entry with units -->
<entry loc="90 20" size="60 20"
  style="PlainText"
  vq="tHX"
  vc="vnmrunits('set','tHX',$VALUE)"
  set="vnmrunits('get','tHX'):$VALUE"
  show="on('tHX'):$VALUE"
/>
<menu loc="150 20" size="60 20"
  style="PlainText"
  vq="tHX"
  vc="parunits('set','tHX','$VALUE')"
  set="parunits('get','tHX'):$VALUE"
  show="on('tHX'):$VALUE"
  editable="No"
>
  <mlabel label="sec" chval="sec"/>
  <mlabel label="ms" chval="ms"/>
  <mlabel label="us" chval="us"/>
</menu>

<!-- Dynamic channel label -->
<textmessage loc="100 0" size="40 20"
  style="PlainText"
  label="H1"
  vq="dn"
  set="$VALUE = dn"
/>
```

### Pulse Sequence Architecture (from ahX)

**Standard ahX.c Structure:**

```c
/* Header comment with experiment descriptions */

#include "standard.h"
#include "solidstandard.h"
#include "pboxpulse.h"

// Define phase tables as static int arrays
static int table1[4] = {2,2,2,2};  // phH90
static int table2[4] = {0,3,0,3};  // phXhx
// ... more phase tables

// Define phase references using #define
#define phH90 t1
#define phXhx t2
// ... more phase references

void pulsesequence() {
    // 1. Get parameter values and create objects
    CP hx = getcp("HX",0.0,0.0,0,1);
    DSEQ dec = getdseq2("H");
    PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);

    // 2. Select experiment type based on pseq parameter
    char pseq[MAXSTR];
    getstr("pseq",pseq);
    if (!strcmp(pseq,"one")) {
        // CP experiment
    } else if (!strcmp(pseq,"two")) {
        // DP experiment
    }
    // ... more experiment types

    // 3. Calculate timing parameters
    double taur = roundoff((1.0/srate), 0.125e-6);
    double tECHOhalf = (tECHO - pwXecho)/2.0;

    // 4. Duty cycle protection
    // See SAFETY section below

    // 5. Set phase tables
    settable(phH90, 4, table1);
    // ... more settable calls

    // 6. Execute pulse sequence
    txphase(phH90); decphase(phHhx);
    obsstepsize(1.0);
    initval(7.0/360.0,phH90);
    xmtrphase(phH90);

    // Begin sequence
    rgpulse(pwH90,phH90,rof1,0.0);
    decunblank(); decon();
    _cp_(hx,phHhx,phXhx);
    decoff(); decblank();
    // ... rest of sequence
}
```

**Key Objects:**

- **CP** - Cross polarization transfer (constant, linear, tangent ramp)
  - Parameters: aHhx, aXhx, bHX, dHX, tHX, ofHX, shHX
  - Channels: fr (from), to (to)

- **DSEQ** - Decoupling sequences (SPINAL, TPPM, WALTZ)
  - Parameters: aHspinal, pwHspinal, phHspinal, alpHspinal
  - Modes: Hseq='tppm', 'spinal', 'waltz'

- **PBOXPULSE** - Shaped pulses via Pbox
  - Parameters: wvXshp1, aXshp1, pwXshp1, dbXshp1, ofXshp1

**Multi-Experiment Design:**

Use `pseq` parameter to select experiment within one sequence:
- `pseq='one'` → Standard CP
- `pseq='two'` → Direct Polarization
- `pseq='three'` → CP with flipback
- `pseq='four'` → CP with hard echo
- `pseq='five'` → CP with soft echo
- etc.

This reduces code duplication and simplifies maintenance.

## Safety Standards

### Duty Cycle Limits

**CRITICAL:** C-detected sequences (hX, hXX, hYXX, hXYXX) use **5% maximum duty cycle**
- Reason: High-power decoupling on ¹³C channel during acquisition
- Carbon coils are sensitive to heating
- Exceeding 5% can damage probe hardware

**H-detected sequences** (hXH, hYXH) can use **10-15% duty cycle**
- Lower power decoupling during acquisition
- Proton coils more robust

**Detection Type Identification:**
- Last letter = detection nucleus
- hYX**X** → C-detected → **5% limit**
- hYX**H** → H-detected → **10-15% limit**

**Implementation Pattern (from hX.c):**

```c
// Duty cycle protection with clear error messages
double duty = (total_rf_time) / (total_rf_time + d1 + 4.0e-6);

if (duty > 0.05) {
    double min_d1 = (total_rf_time / 0.05) - total_rf_time - 4.0e-6;
    abort_message("Duty cycle %.1f%% exceeds 5%% limit. "
                  "Increase d1 to at least %.3f s. Abort!\n",
                  duty*100, min_d1);
}
```

**Reference Documents:**
- vnmrsys/SAFETY_STANDARDS.md - Detailed duty cycle explanations
- vnmrsys/SAFETY_FIX_SUMMARY_REPORT.md - Recent safety fixes
- vnmrsys/DUTY_CYCLE_ENHANCEMENT.md - Future power-dependent enhancements

### Validation

**Run validation script after changes:**
```bash
cd vnmrsys
./validate_refactoring.sh
```

This checks:
- Stage 1: C-detected sequences have 5% duty cycle
- Stage 2: Architecture files present
- Stage 3: Refactored sequences
- Stage 4: Testing and documentation
- Template XML syntax validation

## Common Development Tasks

### Adding a New Pulse Sequence

1. **Create pulse sequence following ahX pattern:**
   ```bash
   # Use ahX.c as template
   cp biosolidspack/psglib/ahX.c vnmrsys/psglib/mySequence.c
   # Edit sequence, maintain structure
   ```

2. **Create template directory:**
   ```bash
   mkdir -p vnmrsys/templates/layout/mySequence
   ```

3. **Copy and adapt templates from ahX:**
   ```bash
   cp biosolidspack/templates/layout/ahX/*.xml vnmrsys/templates/layout/mySequence/
   # Edit XML files, ensure proper VnmrJ syntax
   ```

4. **Validate XML syntax:**
   ```bash
   cd vnmrsys
   ./validate_refactoring.sh  # Checks XML well-formedness
   ```

5. **Test on spectrometer:**
   - Copy to spectrometer vnmrsys/psglib/
   - Compile with `seqgen('mySequence')`
   - Test with short d1 to verify duty cycle abort
   - Test normal operation

### Updating Existing Templates

1. **Always reference ahX templates for syntax:**
   ```bash
   # Check ahX for proper conditional syntax
   cat biosolidspack/templates/layout/ahX/PulseSequence.xml
   ```

2. **Common template updates:**
   - Adding new parameters: Follow entry/label/menu pattern
   - Conditional display: Use proper if-then-else syntax
   - Channel labeling: Use textmessage with vq="dn" or vq="tn"
   - Units handling: Always provide sec/ms/us menu

3. **Validate after editing:**
   ```bash
   xmllint --noout vnmrsys/templates/layout/mySeq/*.xml
   ```

### Working with Setup Macros

**Setup macros are typically simple:**

```bash
# AhX macro (from biosolidspack):
"macro AhX"
cqexp('AhX','solidsseq1d')
```

This calls the `cqexp` framework which:
- Loads sequence-specific parameters
- Initializes templates
- Sets up parameter modules

For vnmrsys sequences, follow same pattern:
```bash
"macro HX"
cqexp('HX','solidsseq1d')
```

## Architecture Patterns

### Sequence Families

**1D Sequences (X-detected):**
- hX - Single-channel CP/DP with echo options
- Usage: Tuning, optimization, basic 1D spectra

**2D Homonuclear:**
- hXX - X-X correlation (C-C, N-N)
- Templates: Add dimension parameters (ni, sw1, etc.)

**2D Heteronuclear:**
- hYX - Y→X transfer (N→C)
- Multiple evolution/mixing periods

**3D Sequences:**
- hYXX - Y evolution, X-X mixing
- hXYX - Triple resonance
- Templates: Multiple dimension tabs

**H-Detected:**
- hXH - X→H transfer (C→H, N→H)
- hYXH - Y→X→H transfer
- Lower duty cycle requirements

### Parameter Naming Conventions

**Channel designation:**
- H = ¹H (dec channel)
- X = observe channel (¹³C or ¹⁵N)
- Y = dec2 channel (third channel)
- Z = dec3 channel (fourth channel)
- W = dec4 channel (fifth channel)

**Timing parameters:**
- `tHX` - CP contact time (H→X)
- `tECHO` - Total echo delay
- `tRFmax` - Constant-time decoupling period
- `taur` - Rotor period (calculated from srate)

**RF parameters:**
- `aHhx` - Amplitude H during HX CP
- `pwH90` - 90° pulse width on H
- `ofHX` - Offset for CP
- `shHX` - CP shape ('l'=linear, 'c'=constant, 't'=tangent)

**Decoupling parameters:**
- `aHspinal` - Amplitude for SPINAL
- `pwHspinal` - Pulse width for SPINAL
- `phHspinal` - Phase angle for SPINAL
- `alpHspinal` - Second phase angle for 2-angle SPINAL

### Rotor Synchronization

Many timing parameters should be synchronized to rotor period:
```c
double srate = getval("srate");
double taur = roundoff((1.0/srate), 0.125e-6);

// Echo delays should be multiples of rotor cycles
double tECHO = 2.0 * n * taur;  // n = integer

// Recoupling periods
double tmix = m * taur;  // m = integer
```

### Constant-Time Decoupling

Controlled by `ctd` parameter:
- `ctd='y'` - Add decoupling for fixed period `tRFmax`
- `ctd='n'` - No constant-time decoupling
- Maintains constant sample heating across arrays

```c
if (!strcmp(ctd,"y")) {
    // Calculate tRF based on tECHOmax
    tRF = tRFmax - 2.0*tECHOhalf;
    if (tRF < 0.0) tRF = 0.0;

    // Apply decoupling
    _dseqon(dec);
    delay(tRF);
    _dseqoff(dec);
}
```

## Testing and Validation

### Pre-Deployment Testing

**1. Compile sequence:**
```bash
# On spectrometer, in VnmrJ:
seqgen('mySequence')
```

**2. Safety test (CRITICAL):**
```bash
# Load sequence
mySeq
# Set very short d1 (should abort)
d1=0.1
go
# Expected: "Duty cycle XX% exceeds 5% limit. Abort!"
```

**3. Normal operation test:**
```bash
d1=2.0
tHX=2.0m
at=0.020
go
# Expected: "Duty cycle XX% < 5%. Safe to proceed."
```

**4. Template testing:**
- Verify all tabs appear correctly
- Check conditional display logic
- Test parameter entry and unit conversion
- Verify channel labels update dynamically

### Validation Checklist

After making changes:
- [ ] Pulse sequence compiles without errors
- [ ] Safety abort works (short d1 test)
- [ ] Normal operation succeeds (appropriate d1)
- [ ] Templates display correctly in VnmrJ
- [ ] XML validates with xmllint
- [ ] Duty cycle calculation is correct
- [ ] Phase cycling is appropriate (4, 8, or 16 steps)
- [ ] Documentation updated

## Common Issues and Solutions

### Template Display Issues

**Problem:** Parameters not showing in VnmrJ
**Solution:** Check `show=` attribute syntax, ensure proper escaping

**Problem:** Dropdown menus not working
**Solution:** Verify `<mlabel>` syntax and `chval` attributes

**Problem:** Conditional display not working
**Solution:** Use proper if-then-else syntax, not simple boolean expressions

### Pulse Sequence Compilation Errors

**Problem:** "undefined reference" errors
**Solution:** Check #include statements, ensure solidstandard.h is included

**Problem:** Phase table errors
**Solution:** Verify static int array declarations and #define references match

**Problem:** Parameter not found
**Solution:** Check getval() and getstr() calls match parameter names

### Duty Cycle Issues

**Problem:** Sequence aborts when it shouldn't
**Solution:** Recalculate RF time, ensure d1 is adequate, check 5% vs 10% limit

**Problem:** Sequence doesn't abort when it should
**Solution:** Verify duty cycle calculation includes all RF periods

## File Organization

### Production Files (for spectrometer deployment)

```
vnmrsys/
├── psglib/           # Copy to spectrometer vnmrsys/psglib/
├── maclib/           # Copy to spectrometer vnmrsys/maclib/
├── templates/        # Copy to spectrometer vnmrsys/templates/
└── parlib/           # Copy to spectrometer vnmrsys/parlib/
```

### Development/Documentation Files (keep in repo)

```
vnmrsys/
├── SAFETY_STANDARDS.md
├── REFACTORING_STRATEGY_AND_ROADMAP.md
├── COMPREHENSIVE_SEQUENCE_VALIDATION_2021-2025.md
├── TEMPLATE_SYNTAX_GUIDE.md
└── validate_refactoring.sh
```

### Reference Implementation (do not modify)

```
biosolidspack/     # Agilent BioSolids Pack - reference only
biopack/           # Agilent BioPack - reference only
```

## Git Workflow

### Committing Changes

```bash
# After making changes
cd /home/chad/github-staging/vnmrj-nmr-tools

# Check what changed
git status
git diff

# Stage changes
git add vnmrsys/psglib/mySequence.c
git add vnmrsys/templates/layout/mySequence/

# Commit with descriptive message
git commit -m "Add mySequence pulse sequence with ahX-style templates"

# Push to remote
git push origin main
```

### Recent Development History

Recent work has focused on:
1. Safety fixes for C-detected sequences (5% duty cycle)
2. Template enhancements following ahX pattern
3. XML syntax fixes (proper operator escaping)
4. Addition of biosolidspack and biopack reference libraries

See git log for detailed history:
```bash
git log --oneline --since="2024-11-01"
```

## Related Documentation

- **README.md** - Repository overview and quick start
- **ARCHITECTURE.md** - High-level system architecture
- **INSTALLATION.md** - Installation procedures
- **CONTRIBUTING.md** - Contribution guidelines
- **vnmrsys/SAFETY_STANDARDS.md** - Duty cycle safety standards
- **vnmrsys/TEMPLATE_SYNTAX_GUIDE.md** - VnmrJ template syntax reference
- **biosolidspack/CLAUDE.md** - BioSolids Pack specific documentation

## Key Principles

1. **Use ahX as the style guide** for all template and sequence development
2. **Safety first** - Always implement proper duty cycle checks
3. **Follow VnmrJ XML syntax** - Escape operators, use if-then-else conditionals
4. **Test thoroughly** - Both safety abort and normal operation
5. **Document clearly** - Include header comments and usage examples
6. **Maintain consistency** - Follow established naming conventions and patterns

---

**Version:** 1.0
**Last Updated:** 2025-11-10
**Repository:** https://github.com/ChadResynant/vnmrj-nmr-tools
