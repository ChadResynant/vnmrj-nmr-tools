# Core Pulse Sequence Version Analysis

**Generated:** November 9, 2025
**Purpose:** Identify absolute most recent versions of core sequences

## Summary of Findings

### ⚠️ CRITICAL: Sequences Where Ayrshire Has Newer Versions

| Sequence | Current (GitHub) | Ayrshire Latest | Date Difference | Action Needed |
|----------|------------------|-----------------|-----------------|---------------|
| **hXX.c** | Sept 12, 2024 (18,830 bytes) | **July 9, 2025** (19,074 bytes) | 10 months newer | **UPDATE** |
| **hXXXX_S.c** | Aug 1, 2025 (24,531 bytes) | Aug 1, 2025 (24,531 bytes) | Same | ✓ Current |
| **hXXX_S.c** | July 9, 2025 (22,681 bytes) | July 9, 2025 (22,681 bytes) | Same | ✓ Current |
| **hYYXH.c** | Aug 24, 2025 (16,986 bytes) | Aug 24, 2025 (16,986 bytes) | Same | ✓ Current |
| **hXH.c** | June 29, 2025 (9,658 bytes) | **Oct 25, 2024** (9,605 bytes) | Taurus newer | ✓ Current |
| **hX.c** | Sept 12, 2025 (8,104 bytes) | Sept 17, 2024 (7,775 bytes) | Taurus newer | ✓ Current |

### Additional Sequences Found on Ayrshire Only

| Sequence | Date | Size | Notes |
|----------|------|------|-------|
| **hXXXX_S_623am.c** | July 9, 2025 | 26,816 bytes | Working version before cleanup? |
| **hXXXX_S_bad.c** | July 8, 2025 | 35,370 bytes | Marked as "bad" - likely buggy |
| **hX-50pctduty.c** | Oct 24, 2024 | 7,860 bytes | Already in GitHub ✓ |
| **mtune.c** | Oct 18, 2024 | 1,549 bytes | Already in GitHub ✓ |

## Detailed Core Sequence Analysis

### hX.c - CURRENT VERSION IS NEWEST ✓
```
Taurus ovj32user:  Sept 12, 2025    8,104 bytes  [CURRENT - has phRecsoftdp fix]
Ayrshire rienstra: Sept 17, 2024    7,775 bytes  [Older]
GitHub vnmrsys:    Sept 12, 2025    8,104 bytes  ✓
```
**Status:** GitHub has the newest version with the Sept 2025 bug fix

### hXX.c - ⚠️ AYRSHIRE IS NEWER
```
Taurus ovj32user:  Sept 12, 2024   18,830 bytes  [CURRENT IN GITHUB]
Ayrshire rienstra:  July 9, 2025   19,074 bytes  [NEWER - 10 months!]
GitHub vnmrsys:    Sept 12, 2024   18,830 bytes  [NEEDS UPDATE]
```
**Status:** ⚠️ **NEEDS UPDATE** - Ayrshire has version 10 months newer!
**Action:** Copy hXX.c from Ayrshire rienstra (July 9, 2025)

### hYXX.c - Only on Ayrshire ✓
```
Ayrshire rienstra: Sept 17, 2024   15,398 bytes
GitHub vnmrsys:    Sept 17, 2024   15,398 bytes  ✓
```
**Status:** Current (only exists on Ayrshire, already copied)

### hXYX.c / hXYXX.c - NOT FOUND
**Status:** These sequences don't exist. Possibly you meant **hXYXX_4D_S.c**?

### hXYXX_4D_S.c - FOUND ✓
```
All locations:     Sept 12/17, 2024  22,490 bytes
GitHub vnmrsys:    Sept 12, 2024     22,490 bytes  ✓
```
**Status:** Current (same across all locations)

### hXXX_S.c - CURRENT ✓
```
Taurus ovj32user:  Sept 12, 2024   21,337 bytes  [Older]
Ayrshire rienstra:  July 9, 2025   22,681 bytes  [NEWEST]
GitHub vnmrsys:     July 9, 2025   22,681 bytes  ✓
```
**Status:** Current (already using Ayrshire version)

### hXH.c - CURRENT (Taurus is newer) ✓
```
Taurus ovj32user:  June 29, 2025    9,658 bytes  [NEWEST]
Ayrshire rienstra: Oct 25, 2024     9,605 bytes  [Older]
GitHub vnmrsys:    June 29, 2025    9,658 bytes  ✓
```
**Status:** Current (using newer Taurus version)

### hXYH.c - NOT FOUND
**Status:** This sequence doesn't exist in either location

### hXYYH.c - NOT FOUND
**Status:** This sequence doesn't exist. Possibly you meant **hYYXH.c**?

### hYYXH.c - CURRENT ✓
```
Taurus ovj32user:  Aug 5, 2025     15,976 bytes  [Older]
Ayrshire rienstra: Aug 24, 2025    16,986 bytes  [NEWEST]
GitHub vnmrsys:    Aug 24, 2025    16,986 bytes  ✓
```
**Status:** Current (already using newest Ayrshire version from Aug 24)

### hYXH.c - CURRENT ✓
```
Taurus ovj32user:  July 3, 2025    13,507 bytes  [NEWEST]
Ayrshire rienstra: Sept 17, 2024    9,779 bytes  [Older]
GitHub vnmrsys:    July 3, 2025    13,507 bytes  ✓
```
**Status:** Current (using newer Taurus version)

### REDOR Sequences - FOUND Multiple
```
FS_REDOR.c:        Sept 12/17, 2024  22,820 bytes  ✓ Current
ZF_TEDOR.c:        Sept 12/17, 2024  11,550 bytes  ✓ Current
```
**Status:** Current (same across all locations)

### TEDOR - FOUND (Refactored version only)
```
TEDOR-claude.c:    July 23/26, 2025  14,345 bytes  ✓ Current
```
**Status:** Only refactored version exists (in development/)
**Note:** No production TEDOR.c found - may need to check if TEDOR-claude.c should be promoted

### CTUC_COSY - NOT FOUND
**Status:** This sequence doesn't exist in any collected packages

## All Sequences Modified in Last 2 Years

### Taurus ovj32user (Most Recent Work)
**July 2025 Refactoring Session:**
- hX.c (Sept 12, 2025) - phRecsoftdp bug fix
- hYYXH.c (Aug 5, 2025)
- hYXX-claude.c (July 26, 2025) - Refactored
- hXXXX_S-claude.c (July 26, 2025) - Refactored
- HhYXH-claude.c (July 26, 2025) - Refactored
- hYYXH-claude.c (July 26, 2025) - Refactored
- HXhhXH_4D-claude.c (July 26, 2025) - Refactored
- TEDOR-claude.c (July 26, 2025) - Refactored
- hYyXH-claude.c (July 26, 2025) - Refactored
- HhXH-claude.c (July 26, 2025) - Refactored
- hXX-claude.c (July 26, 2025) - Refactored
- hXXXX_S.c (July 23, 2025) - Production version before Aug 1 update
- hXX_complete_refactor.c (July 23, 2025) - Complete rewrite
- hXX_refactored.c (July 23, 2025) - Partial refactor

**Earlier 2025:**
- hYXH.c (July 3, 2025)
- hYXH_S.c (July 3, 2025)
- hXH.c (June 29, 2025)

**2024:**
- Multiple sequences synchronized Sept 12, 2024

### Ayrshire rienstra (Production Updates)
**2025 Production Work:**
- hYYXH.c (Aug 24, 2025) - **MOST RECENT PRODUCTION**
- hXXXX_S.c (Aug 1, 2025) - Production update
- hXXXX_S_623am.c (July 9, 2025) - Working version
- hXXX_S.c (July 9, 2025) - **Production update**
- **hXX.c (July 9, 2025) - ⚠️ MISSING FROM GITHUB**
- hXXXX_S_bad.c (July 8, 2025) - Marked bad, don't use

**2024:**
- hXH.c (Oct 25, 2024)
- hX-50pctduty.c (Oct 24, 2024)
- mtune.c (Oct 18, 2024)
- Multiple sequences synchronized Sept 17, 2024

## Recommendations

### Immediate Actions Required

1. **UPDATE hXX.c** ⚠️ PRIORITY
   ```bash
   # Current: Sept 12, 2024 (18,830 bytes)
   # Update to: July 9, 2025 (19,074 bytes) from Ayrshire
   ```
   This is 10 months newer and likely has important fixes/improvements.

2. **Review hXXXX_S versions**
   - GitHub has Aug 1, 2025 version (24,531 bytes) ✓
   - Ayrshire also has July 9 "623am" version (26,816 bytes)
   - Ayrshire has July 8 "bad" version (35,370 bytes) - AVOID

   **Question:** Is the 623am version relevant or was it intermediate work?

3. **Verify TEDOR sequences**
   - Only TEDOR-claude.c exists (July 2025 refactored version)
   - No production TEDOR.c found

   **Question:** Should TEDOR-claude.c be tested and promoted to production?

4. **Check for CTUC_COSY**
   - Not found in either Taurus or Ayrshire packages

   **Question:** Is this sequence named differently or on a different machine?

### Sequences to Investigate

1. **hXXXX_S_623am.c** (Ayrshire, July 9, 2025, 26,816 bytes)
   - Larger than final version (24,531 bytes from Aug 1)
   - Possibly working version before cleanup/optimization
   - Should we review what changed between 623am and Aug 1?

2. **Variant files** (.ayrshire, .backup)
   - We documented differences
   - May contain debugging modifications worth preserving

### Next Steps

1. **Copy hXX.c from Ayrshire** (July 9, 2025 version)
2. **Investigate hXXXX_S_623am.c** - understand why it's different
3. **Locate CTUC_COSY** if it exists elsewhere
4. **Review refactored sequences** for production readiness
5. **Consider deploying tested refactored versions** to production

## Files Requiring Your Review

### High Priority
- [ ] hXX.c - Update from Ayrshire (July 9, 2025)
- [ ] hXXXX_S_623am.c - Understand relationship to Aug 1 version
- [ ] TEDOR-claude.c - Ready for production testing?

### Medium Priority
- [ ] All *-claude.c refactored versions - Testing status?
- [ ] hXXXX_S_bad.c - What went wrong? Lessons learned?

### Low Priority
- [ ] Variant files analysis - Apply any useful debugging modifications

## Source File Locations

**Ayrshire (for copying hXX.c):**
```
/home/chad/vnmrsys_packages/Ayrshire-Ubuntu_vnmrsys_packages_2025-11-09_072957/home_rienstra_vnmrsys/psglib/hXX.c
Date: 2025-07-09
Size: 19,074 bytes
```

**For investigation:**
```
/home/chad/vnmrsys_packages/Ayrshire-Ubuntu_vnmrsys_packages_2025-11-09_072957/home_rienstra_vnmrsys/psglib/hXXXX_S_623am.c
```
