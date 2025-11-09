# VNMRSYS Consolidation Log

**Date:** November 9, 2025
**Consolidated by:** Claude Code with Chad Rienstra

## Consolidation Strategy

### Decision: Use Taurus ovj32user as Base

**Rationale:**
1. Contains complete July 2025 refactoring infrastructure
2. Has all 5 biosolid*.h modernization headers
3. Includes comprehensive refactoring documentation
4. Most recent bug fixes (hX.c Sept 12, 2024)
5. Provides foundation for future development

### Source Packages
- **Taurus:** `/home/chad/vnmrsys_packages/Taurus_vnmrsys_packages_2025-11-09_072528.tar.gz`
- **Ayrshire:** `/home/chad/vnmrsys_packages/Ayrshire-Ubuntu_vnmrsys_packages_2025-11-09_072957.tar.gz`

## File-by-File Decisions

### Pulse Sequences - Copied from Taurus ovj32user

**All 56 .c files from psglib** including:
- FS_REDOR.c
- H_19F_FSR_rfdr_hXH_stppm.c
- H_FSR_rfdr_hXH_prototype.c
- HhXH.c, HhXX.c, HhYXH.c
- HXhhXH_4D.c
- hhYXX_S.c
- hX-blankTOMCO.c
- hX.c (Sept 12, 2024 - with phRecsoftdp fix)
- hXH.c (June 29, 2025)
- hXhhX.c
- hX-testamps.c
- hX-withYdec.c
- hXX.c
- hXXX_S.c (Sept 12, 2024 version - will be overwritten)
- hXXX_S_nt2.c, hXXX_S_nt2_invert.c
- hXXXX_S.c (July 23, 2024 version - will be overwritten)
- hXYXX_4D_S.c
- hYXH.c (July 3, 2025)
- hYXH_S.c (July 3, 2025)
- hYXX_S.c, hYXXsoft.c
- hYyXH.c
- hYYXH.c (Aug 5, 2024 version - will be overwritten)
- onepultest.c
- ZF_TEDOR.c (July 3, 2025)
- Plus all variants with .ayrshire and .backup extensions

### Pulse Sequences - Overwritten with Newer Ayrshire Versions

| File | Source | Date | Reason |
|------|--------|------|--------|
| hYYXH.c | Ayrshire rienstra | Aug 24, 2025 | Most recent production work |
| hXXXX_S.c | Ayrshire rienstra | Aug 1, 2025 | Newer than Taurus (July 23, 2024) |
| hXXX_S.c | Ayrshire rienstra | July 9, 2025 | Newer than Taurus (Sept 12, 2024 - size check needed) |

### Pulse Sequences - Added from Ayrshire (Unique)

| File | Source | Date | Reason |
|------|--------|------|--------|
| hYXX.c | Ayrshire rienstra | Sept 17, 2024 | Not in Taurus ovj32user main psglib |
| hX-50pctduty.c | Ayrshire rienstra | Oct 24, 2024 | Special duty cycle variant |
| mtune.c | Ayrshire rienstra | Oct 18, 2024 | Tuning utility |

### Development Files - Copied to psglib/development/

**Refactored Sequences (11 files):**
- HhXH-claude.c
- HhYXH-claude.c
- HXhhXH_4D-claude.c
- hXX-claude.c
- hXX_refactored.c
- hXX_complete_refactor.c
- hXXXX_S-claude.c
- hYXX-claude.c
- hYyXH-claude.c
- hYYXH-claude.c
- TEDOR-claude.c

**Refactoring Documentation (5 .txt files):**
- refactor_guide.txt
- hXX_complete_refactor_test_plan.txt
- phase_table_plan.txt
- clean_dps.txt
- hXX_phase_tables.txt

**Phase Organization Template:**
- phase_organization_template.h

**Backup Directory:**
- backupJuly26/ (9 original sequences)

### Draft Sequences - Copied to psglib/drafts/

**From junk directory (11 files):**
- ahX.c, ahXX.c, ahXYX.c, ahYX.c, ahYXX.c (a-prefix variants)
- hYXH.c, hYyXH.c (duplicates in drafts)
- hYXH_collin.c, hYXX_collin.c (Collin's versions)
- s2pul-cmr.c, s2pulecho-cmr.c (test sequences)

### PSG Headers - Copied from Taurus ovj32user

**All header files including:**

**Biosolid Refactoring Headers (5 critical files):**
- biosolidevolution.h (15KB, July 23, 2024)
- biosolidvalidation.h (12KB, July 23, 2024)
- biosolidcp.h (5KB, July 23, 2024)
- biosolidmixing.h (14KB, July 23, 2024)
- biosolidmixing_simple.h (12KB, July 23, 2024)

**Other Biosolid Headers:**
- biosoliddecshapes.h
- biosolid_jmc.h
- biosolidmpseqs.h
- biosolidnus.h (Aug 5, 2024)
- biosolidpboxpulse.h
- biosolidstandard.h

**Solid-State Headers:**
- asolidstandard.h
- solidchoppers.h
- soliddecshapes.h, soliddecshapes.ap.h, soliddecshapes2.h
- soliddefs.h
- soliddutycycle.h
- solidelements.h
- solidhhdec.h
- solidmisc_donghua.h
- solidmpseqs.h
- solidobjects.h
- solidpulses.h
- solidshapegen.h, solidshapegen_backup.h
- solidshapes_donghua.h
- solidstandard.h, solidstandard_donghua.h
- solidstates.h
- solidwshapes.h

**Other Headers:**
- master_switch.h
- backupJuly26/ directory

### Maclib - Copied from Taurus ovj32user

**All 158 macros plus subdirectories (688 total files)**

Decision: Use Taurus as base, provide comparison with chadnew for review.

### Templates - Copied from Taurus ovj32user

**All template files (low priority)**

## Variant Files Not Copied (Preserved in Original)

### Files with .ayrshire extension (kept in main psglib for reference):
- hXH.c.ayrshire
- hXYXX_4D_S.c.ayrshire

### Files with .backup extension (kept in main psglib for reference):
- hXYXX_4D_S.c.backup.JUL1

**Note:** These files contain experimental modifications for debugging and should be reviewed before use. See VARIANT_FILE_ANALYSIS.md for details.

## Files Requiring Further Review

### Sequences with Similar Dates (Sept 12-17, 2024)
Need MD5 checksum or detailed diff:
- FS_REDOR.c
- H_19F_FSR_rfdr_hXH_stppm.c
- H_FSR_rfdr_hXH_prototype.c
- HhXH.c, HhXX.c, HhYXH.c
- hhYXX_S.c
- hX-blankTOMCO.c
- hXhhX.c
- hX-testamps.c
- hX-withYdec.c
- hXX.c (July 9, 2025 Ayrshire vs Sept 12, 2024 Taurus)
- hXYXX_4D_S.c
- hYXX_S.c, hYXXsoft.c
- hYyXH.c

## Statistics

### Pulse Sequences
- Production (psglib/): 56 sequences
- Development (psglib/development/): 11 refactored + 9 backups + 6 docs
- Drafts (psglib/drafts/): 11 sequences
- **Total: 78 pulse sequence files + 6 documentation files**

### Headers
- psg/: 35 header files + 1 backup directory

### Macros
- maclib/: 158 direct macros (688 with subdirectories)

### Templates
- templates/: Full directory from Taurus

## Excluded Items

1. **Junk directory:** Moved to drafts/, not excluded
2. **System accounts:** vnmr1, walkup, service directories not included
3. **Empty directories:** Not copied
4. **Compressed archives:** Not copied (already extracted)

## Git Repository Setup (Pending)

**Remote:** ChadResynant GitHub account
**Branches (planned):**
- `main` - Production sequences
- `refactoring` - Development work

## Validation Steps Remaining

1. Compile all sequences to verify syntax
2. Check for broken #include dependencies
3. Validate macro references in sequences
4. Test parameter file compatibility
5. Review macro comparison and merge selections
6. Initialize git with proper .gitignore
7. Create initial commit preserving provenance

## Notes

- All original packages preserved in `/home/chad/vnmrsys_packages/`
- No data loss risk - all source material backed up
- Refactoring work from July 2025 successfully preserved
- Production updates from Ayrshire integrated
- Ready for git initialization and deployment planning
