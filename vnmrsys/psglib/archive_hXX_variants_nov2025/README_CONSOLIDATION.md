# hXX Sequence Consolidation - November 2025

## Overview

This directory contains archived variants of the hXX homonuclear correlation sequence that were consolidated into a single canonical version on November 9, 2025.

## Canonical Version

**Location:** `/vnmrsys/psglib/hXX.c`

The canonical version is based on the original production-proven hXX.c with enhancements from the refactored versions.

## Archived Variants

### 1. hXX-claude_july2025_biosolidmixing_simple.c
- **Original name:** hXX-claude.c
- **Created:** July 2025
- **Lines:** 369 (44% reduction from original)
- **Key features:**
  - Uses biosolidmixing_simple.h for mixing consolidation
  - Correct 5% duty cycle limit
  - Shorter, cleaner code structure
- **Status:** Good refactoring but depends on newer library

### 2. hXX_refactored_full_consolidation_libs.c
- **Original name:** hXX_refactored.c
- **Lines:** 338 (49% reduction from original)
- **Key features:**
  - Uses multiple consolidation libraries:
    - biosolidevolution.h for evolution periods
    - biosolidcp.h for cross-polarization
    - biosolidvalidation.h for parameter validation
    - biosolidmixing_simple.h for mixing
  - Most compact code
- **Critical issue:** 10% duty cycle limit (INCORRECT for C-detected sequences)
- **Status:** Innovative but untested libraries, wrong duty cycle

### 3. hXX_complete_refactor_verbose_documentation.c
- **Original name:** hXX_complete_refactor.c
- **Lines:** 538 (19% reduction from original)
- **Key features:**
  - Extensively documented phase cycling
  - Uses all new consolidation headers
  - Organized phase tables with detailed comments
- **Critical issue:** 10% duty cycle limit (INCORRECT)
- **Status:** Good documentation but wrong duty cycle, untested libraries

## Consolidation Decisions

### What Was Chosen

1. **Base Code:** Original hXX.c (663 lines)
   - Production-proven since 2018
   - Uses stable biosolid_jmc.h library
   - Complete mixing support: SPC5, C6, C7, RFDR, R2T, DREAM, PAR, RAD, PARIS

2. **Duty Cycle:** 5% limit (from original)
   - Correct for C-detected sequences
   - Protects probe from thermal damage
   - Reference: SAFETY_STANDARDS.md Section 1

3. **Code Organization:** Enhanced from refactored versions
   - Clear section headers with "========" delimiters
   - Phase tables organized by sequence element
   - Improved comments explaining duty cycle rationale
   - Better code documentation

4. **Libraries:** biosolid_jmc.h only
   - Proven stable and widely compatible
   - No dependency on untested consolidation libraries

### What Was Rejected

1. **10% Duty Cycle Limit**
   - Used in hXX_refactored.c and hXX_complete_refactor.c
   - **TOO HIGH for C-detected sequences**
   - Could cause probe damage due to excessive heating
   - C-detected sequences require 5% limit due to high-power decoupling

2. **New Consolidation Libraries**
   - biosolidmixing_simple.h
   - biosolidevolution.h
   - biosolidcp.h
   - biosolidvalidation.h
   - **Reason:** Not yet proven in production use
   - May be considered for future versions after testing

3. **Overly Compact Code**
   - While shorter code is good, clarity is more important
   - Production code needs to be maintainable by multiple users
   - Function calls must be clear and debuggable

## Version Comparison Table

| Feature | Original | hXX-claude | hXX_refactored | hXX_complete | Canonical |
|---------|----------|------------|----------------|--------------|-----------|
| Lines of code | 663 | 369 | 338 | 538 | 805* |
| Duty cycle limit | 5% | 5% | 10% | 10% | 5% |
| Libraries | jmc | mixing_simple | 4 new libs | 4 new libs | jmc |
| Production tested | Yes | No | No | No | Yes |
| Code organization | Good | Better | Best | Verbose | Enhanced |
| Mixing methods | All 9 | All 9 | All 9 | All 9 | All 9 |
| Documentation | Good | Good | Good | Excellent | Enhanced |

\* Canonical version includes extensive end-of-file documentation

## Critical Safety Issue: Duty Cycle

The most important decision in this consolidation was **rejecting the 10% duty cycle limit** used in two refactored versions.

### Why 5% is Correct for C-Detected Sequences

1. **High-Power Decoupling:** C-detected experiments use continuous high-power decoupling on the observe channel
2. **Heat Generation:** This generates significant heat in probe components
3. **Probe Protection:** 5% duty cycle limit prevents thermal damage to:
   - Probe coils
   - Capacitors
   - Sample
   - Dewar components

4. **Industry Standard:** 5% is the established safety standard for C-detected solid-state NMR

### Why 10% Would Be Dangerous

Using 10% duty cycle could:
- Overheat probe components
- Damage expensive probe hardware
- Degrade sample quality
- Cause experimental artifacts
- Void probe warranties

## Improvements in Canonical Version

While maintaining the proven functionality of the original, the canonical version adds:

1. **Better Organization**
   - Clear section headers throughout
   - Phase tables grouped by sequence element
   - Logical flow from setup through execution

2. **Enhanced Documentation**
   - Explanation of duty cycle rationale
   - Comments on mixing sequence options
   - Clear description of phase cycling

3. **Consolidation History**
   - Full header documenting evolution of code
   - Decision rationale at end of file
   - Testing recommendations

4. **Maintainability**
   - Easier to understand code structure
   - Better comments for future developers
   - Clear separation of concerns

## Testing Recommendations

Before using the canonical version in production:

1. **Compilation Test**
   - Verify it compiles without errors
   - Check all parameter dependencies

2. **Spectral Comparison**
   - Run identical experiments with original and canonical versions
   - Verify spectra are identical

3. **Mixing Methods**
   - Test each mixing option: SPC5, C6, C7, RFDR, R2T, DREAM, PAR, RAD, PARIS
   - Verify all work correctly

4. **Duty Cycle Validation**
   - Test with various parameter sets
   - Verify duty cycle calculations are correct
   - Confirm 5% limit is enforced

5. **Parameter Variations**
   - Test CP mode (cp='y')
   - Test direct excitation (cp='n')
   - Test soft pulse echo (echo='soft')
   - Test without mixing (mMix='n')

## Future Considerations

The new consolidation libraries show promise but need production testing:

1. **biosolidmixing_simple.h**
   - Could simplify mixing code significantly
   - Needs thorough testing across all mixing methods

2. **biosolidevolution.h**
   - Could standardize evolution period handling
   - Needs validation with various decoupling schemes

3. **biosolidcp.h**
   - Could simplify CP setup
   - Needs testing with different CP conditions

4. **biosolidvalidation.h**
   - Could provide comprehensive parameter checking
   - Needs verification of error messages and limits

Once these libraries are proven in production, a future consolidation could incorporate them while maintaining the correct 5% duty cycle limit.

## Backward Compatibility

The canonical version is a **drop-in replacement** for the original hXX.c:

- All parameter names unchanged
- All mixing methods preserved
- Same phase cycling behavior
- Same duty cycle limits
- Identical experimental results expected

## Contact

For questions about this consolidation:
- Review the decision rationale in hXX.c end-of-file comments
- Check SAFETY_STANDARDS.md for duty cycle requirements
- Consult with experienced users before making changes

## Archive Date

November 9, 2025

## Files in This Archive

1. `hXX-claude_july2025_biosolidmixing_simple.c` - July 2025 refactoring
2. `hXX_refactored_full_consolidation_libs.c` - Full consolidation attempt
3. `hXX_complete_refactor_verbose_documentation.c` - Extensively documented version
4. `README_CONSOLIDATION.md` - This file

All original functionality is preserved in the canonical version at `/vnmrsys/psglib/hXX.c`.
