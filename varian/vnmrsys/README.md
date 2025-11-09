# Consolidated VNMRSYS Repository

**Created:** November 9, 2025
**Purpose:** Unified pulse sequence and macro library combining best code from Taurus and Ayrshire spectrometers
**Refactoring Project:** July 2025 modernization effort preserved and integrated

## Repository Structure

```
vnmrsys_consolidated/
├── psglib/              # Production pulse sequences (consolidated)
├── psglib/development/  # Refactoring work and documentation
│   ├── *-claude.c       # AI-assisted refactored sequences
│   ├── *_refactored.c   # Manually refactored sequences
│   ├── backupJuly26/    # Original versions before refactoring
│   ├── *.txt            # Refactoring documentation and test plans
│   └── *.h              # Refactoring header templates
├── psglib/drafts/       # Experimental sequences from junk directory
├── psg/                 # Header files and libraries
│   ├── biosolid*.h      # Refactoring infrastructure (July 2025)
│   ├── solid*.h         # Standard solid-state NMR headers
│   └── backupJuly26/    # Header backups
├── maclib/              # Macro library (Taurus ovj32user base)
└── templates/           # Parameter templates (Taurus ovj32user)
```

## Source Information

### Base System
**Taurus `/mnt/data_4tbSSD/ovj32user/vnmrsys`** - Development/refactoring hub
- Contains July 2025 refactoring infrastructure
- 71 pulse sequences
- 158 macros (688 with subdirectories)
- Most comprehensive header file collection
- Bug fixes and modernization efforts

### Merged Content from Ayrshire

**From `/home/rienstra/vnmrsys`** (Production system):
- `hYYXH.c` (Aug 24, 2025) - Most recent production sequence
- `hXXXX_S.c` (Aug 1, 2025) - Updated version
- `hXXX_S.c` (July 9, 2025) - Updated version
- `hYXX.c` - Unique to Ayrshire
- `hX-50pctduty.c` - Special duty cycle variant
- `mtune.c` - Tuning utility

## July 2025 Refactoring Project

### Goals
- Better error messages and parameter validation
- Unified duty cycle calculations
- Organized phase table management
- Modular evolution period handling
- Simplified cross-polarization setup

### New Infrastructure (biosolid*.h headers)
1. **biosolidevolution.h** (15KB) - Unified evolution period system
2. **biosolidvalidation.h** (12KB) - Parameter validation framework
3. **biosolidcp.h** (5KB) - Cross-polarization simplification
4. **biosolidmixing.h** (14KB) - Mixing sequence system
5. **biosolidmixing_simple.h** (12KB) - Simplified mixing

### Documentation Files (in psglib/development/)
- `refactor_guide.txt` - Overall refactoring strategy
- `hXX_complete_refactor_test_plan.txt` - Testing protocols
- `phase_table_plan.txt` - Phase table organization
- `clean_dps.txt` - DPS cleanup documentation
- `hXX_phase_tables.txt` - Phase cycling analysis

### Refactored Sequences (11 files)
- `hXX-claude.c`, `hXX_refactored.c`, `hXX_complete_refactor.c`
- `HhXH-claude.c`, `HhYXH-claude.c`
- `HXhhXH_4D-claude.c`
- `hXXXX_S-claude.c`
- `hYXX-claude.c`, `hYyXH-claude.c`, `hYYXH-claude.c`
- `TEDOR-claude.c`

## Pulse Sequence Inventory

### Production Sequences (psglib/)
Total: 56 sequences including:
- **hX family:** hX.c, hXH.c, hXX.c, hXXX_S.c, hXXXX_S.c, etc.
- **hY family:** hYXH.c, hYXH_S.c, hYXX.c, hYXX_S.c, hYYXH.c, etc.
- **Capital H family:** HhXH.c, HhXX.c, HhYXH.c, HXhhXH_4D.c
- **Specialized:** FS_REDOR.c, ZF_TEDOR.c, H_FSR_rfdr variants
- **Utilities:** mtune.c, onepultest.c, testamp.c

### Development Sequences (psglib/development/)
- 11 refactored versions with modern infrastructure
- 9 backup versions in backupJuly26/
- Refactoring documentation and test plans

### Draft Sequences (psglib/drafts/)
From Taurus junk directory:
- `ahX.c`, `ahXX.c`, `ahXYX.c`, `ahYX.c`, `ahYXX.c` - "a" prefix variants
- `hYXH_collin.c`, `hYXX_collin.c` - Collin's versions
- `s2pul-cmr.c`, `s2pulecho-cmr.c` - Simple test sequences

## Variant Files (.ayrshire and .backup)

**IMPORTANT:** Several sequences have `.ayrshire` and `.backup` extensions that contain experimental modifications:

### hXH.c.ayrshire
- Different array formatting (spacing)
- Modified duty cycle calculation (excludes `d2`)
- Changed duty cycle threshold (20% vs 15%)
- Commented out PFG homospoil code

### hXYXX_4D_S.c.backup.JUL1
- Significantly different phase cycling
- Modified spin temperature alternation
- Different receiver phase tables
- Represents pre-July 2025 refactoring state

**Recommendation:** Keep these files for reference during debugging but use main versions for production.

## Macro Library

**Base:** Taurus ovj32user (158 macros, 688 with subdirs)
**Comparison:** See `MACRO_COMPARISON.txt` for detailed analysis

The chadnew maclib (724 macros) contains many additional macros. Review `MACRO_COMPARISON.txt` to determine which should be merged.

## Git Repository

**Intended Remote:** ChadResynant GitHub account
**Branches (planned):**
- `main` - Production-ready sequences
- `refactoring` - Ongoing modernization work

## Key Decisions Made

1. **Base System:** Taurus ovj32user (contains refactoring infrastructure)
2. **Newer Sequences:** Merged from Ayrshire rienstra
3. **Junk Directory:** Moved to psglib/drafts/ (unique sequences preserved)
4. **Maclib:** Taurus base, chadnew comparison available
5. **Templates:** Taurus ovj32user (low priority)
6. **Variant Files:** Preserved in psglib for reference

## Recent Bug Fixes (Sept 2024)

### hX.c (Sept 12, 2024)
- Added `phRecsoftdp` phase table
- Fixed receiver logic issues

## Next Steps

1. Review `MACRO_COMPARISON.txt` and merge desired chadnew macros
2. Test compile all sequences
3. Validate parameter files match sequences
4. Initialize git repository
5. Create refactoring and production branches
6. Connect to ChadResynant remote
7. Consider deploying refactored sequences to production

## Files for Review

- `MACRO_COMPARISON.txt` - Detailed macro library comparison
- `CONSOLIDATION_LOG.md` - Decision log and file provenance
- `VARIANT_FILE_ANALYSIS.md` - Analysis of .ayrshire and .backup differences

## Contact

**Project Lead:** Chad M. Rienstra
**Consolidation Date:** November 9, 2025
**Assistant:** Claude Code (Anthropic)
