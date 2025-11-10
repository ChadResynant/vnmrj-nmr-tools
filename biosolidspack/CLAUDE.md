# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is the **BioSolids Pack** - a comprehensive package of solid-state NMR pulse sequences, macros, and automated calibration protocols for VnmrJ/OpenVNMRJ spectrometers. It's designed specifically for biosolids applications including protein solid-state NMR experiments.

## Repository Structure

The codebase follows the standard VnmrJ/OpenVNMRJ application pack structure:

- **psglib/** - C pulse sequence programs (.c files) that define NMR experiments
- **seqlib/** - Compiled pulse sequences (binary files)
- **maclib/** - Macro library containing automation/calibration scripts (240 macros)
- **manual/** - Text-based help/documentation for each pulse sequence
- **parlib/** - Parameter sets organized by sequence (*.par directories)
- **studylib/** - Automated study protocols (e.g., FullCalibration, StdycpH1C13alphaALL)
- **templates/** - XML UI layout templates for each sequence's parameter interface

## Key Pulse Sequences

The main sequences are variations on the ahX.c family:

- **ahX.c** - Single-channel 1D experiments (H→X)
- **ahYX.c** - Two-channel experiments (H→Y→X)
- **ahXX.c** - 2D homonuclear experiments
- **ahYXX.c** - 3D heteronuclear experiments
- **ahXYX.c** - Triple-resonance experiments

Each sequence is multifunctional, controlled by the `pseq` parameter:
- `pseq='one'` - Cross Polarization (CP)
- `pseq='two'` - Direct Polarization (DP)
- `pseq='three'` - CP with flipback
- `pseq='four'` - CP with hard echo
- `pseq='five'` - CP with soft echo
- etc.

## Common Calibration Macros

Macros follow a naming convention based on experiment type and region:

### Setup Macros
- `cpHXmatch1` - CP match condition calibration (H→X)
- `dcpYXmatch1` - Double CP match calibration (Y→X)
- `cpHXtanramp` - Tangent ramp CP optimization
- `cpHXcntct` - Contact time optimization

### Automated Study Protocols
Located in `studylib/`, these define multi-experiment calibration sequences:
- `FullCalibration` - Complete spectrometer setup
- `StdycpH1C13alphaALL` - H→13C alpha region calibrations
- `StdycpH1C13carbonylALL` - H→13C carbonyl region calibrations
- `StdycpH1N15amideALL` - H→15N amide calibrations
- `StdydcpN15C13alphaALL` - Double CP (N→C) calibrations

## Nucleus and Region Codes

Macros use standardized codes for nucleus/region targeting:

**Carbon-13 Regions:**
- `alpha` or `sHCa` - Alpha carbons (~50-60 ppm, offset at 100 ppm for 's' prefix)
- `carbonyl` or `sHCO` - Carbonyl carbons (~170-180 ppm)
- `adam` or `HC` - Adamantane reference (37.77 ppm)

**Nitrogen-15 Regions:**
- `amide` or `HNai` - Amide nitrogen
- `amine` or `HNam` - Amine nitrogen (glycine)
- `amino` or `HN` - Amino nitrogen

## Development Commands

### No Build System
This is a package of interpreted scripts and pre-compiled sequences. There is no build, test, or lint system.

### Typical Development Workflow

1. **Editing Pulse Sequences**: Modify .c files in `psglib/`
   - Sequences must be compiled on the spectrometer using VnmrJ's `seqgen` command
   - Not compiled in this directory

2. **Editing Macros**: Modify files in `maclib/`
   - Macros are plain text, executed directly by VnmrJ
   - Test by loading into VnmrJ and executing

3. **Editing Parameter Sets**: Modify directories in `parlib/`
   - These contain binary parameter files managed by VnmrJ

4. **Editing UI Templates**: Modify XML files in `templates/layout/`
   - Define parameter panel layouts in VnmrJ GUI

### Installation
The entire directory is typically loaded into VnmrJ using:
```bash
# From VnmrJ command line:
load('biosolidspack')
```

## Code Architecture

### Pulse Sequence Structure (C files in psglib/)

All sequences follow this pattern:

1. **Include Headers**
   ```c
   #include "standard.h"
   #include "solidstandard.h"
   #include "pboxpulse.h"
   ```

2. **Define Phase Tables** - Static arrays for phase cycling
   ```c
   static int table1[4] = {2,2,2,2};
   ```

3. **Main Function: `pulsesequence()`**
   - Get parameters using `getval()`, `getstr()`
   - Initialize pulse/decoupling objects (CP, DSEQ, PBOXPULSE)
   - Calculate timing parameters
   - Set duty cycle protection
   - Define pulse sequence logic with branching based on `pseq`
   - Execute acquisition

4. **Key Objects**
   - `CP` - Cross polarization transfer
   - `DSEQ` - Decoupling sequences (SPINAL, TPPM, WALTZ)
   - `PBOXPULSE` - Shaped pulses via Pbox

### Macro Structure (files in maclib/)

Macros are written in the VnmrJ MAGICAL language:

```
"macro macroname"
// Comments
if ($# > 0) then
    if ($1 = 'setup') then
        // Setup code
    endif
endif
```

Common patterns:
- Macros check argument count `$#` and argument values `$1`, `$2`, etc.
- `rtppar()` - Recall parameter set
- `AhX` - Load sequence with default parameters
- `parammodule()` - Module operations
- `exists()` - Check if parameters/files exist
- `create()` - Create parameters
- `writeparam()` / `fread()` - Save/load parameter modules

## Important Concepts

### Rotor Synchronization
Many timing parameters should be synchronized to the rotor period `taur = 1/srate`:
- Echo delays: `tECHO = 2*n*taur`
- Recoupling periods: synchronized to rotor cycles

### Constant-Time Decoupling
Controlled by `ctd='y'` or `ctd='n'`:
- When enabled, adds decoupling RF for fixed period `tRFmax`
- Maintains constant sample heating across delay arrays

### Decoupling Schemes
- **SPINAL** (2-angle): Two phase angles `ph` and `alp`
- **TPPM**: Two-pulse phase modulation (`alp=0`, `ph≠0`)
- **WALTZ**: Composite pulse decoupling

### Module System
Parameters can be grouped into modules (saved/loaded sets):
- Study-level modules in `archivedir/sample/dirinfo/`
- User modules in `userdir/modules/`
- Macros like `reset_ahX` manage module creation/editing

## Common Issues

1. **Phase Tables**: Always defined as static arrays, indexed by named phase references (phH90, phXhx, etc.)

2. **Channel Naming Convention**:
   - H = 1H (dec channel)
   - X = observe channel (typically 13C or 15N)
   - Y = dec2 channel (third channel)
   - Z = dec3 channel (fourth channel)

3. **Duty Cycle Protection**: All sequences check duty cycle and abort if >10%

4. **Parameter Return**: Use `putCmd()` to return calculated values to VnmrJ panels
   ```c
   putCmd("tRFmax = %f\n", tRFmax*1.0e6);  // Return in microseconds
   ```
