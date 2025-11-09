# Development Branch Strategy

## Branch Structure

```
main                          ← Production-tested code ONLY
  └── v1.0-safety-fixes       ← Initial safety documentation
  └── v1.1-stage1-complete    ← Tested duty cycle fixes (9 sequences)

development                   ← Integration branch for testing
  └── v1.2-dev-stage2-untested ← PulSeq architecture (NEEDS TESTING)
```

## What's On Each Branch

### `main` Branch - Production Ready ✅

**Safe to use at spectrometers:**
- All original sequences with Stage 1 safety fixes
- hX.c, hXX.c, hXH.c, etc. (corrected duty cycles)
- Tested and validated at multiple spectrometers
- No experimental code

**Latest tag:** `v1.1-stage1-complete`

**Changes from original:**
- 9 C-detected sequences: 10-20% → 5% duty cycle
- Safety documentation (SAFETY_STANDARDS.md, etc.)
- No functional changes to sequence logic

### `development` Branch - Testing Required ⚠️

**DO NOT use in production yet:**
- Stage 2 PulSeq-inspired architecture (3,462 lines)
- New validation framework
- Make functions pattern
- Untested at spectrometers

**Latest tag:** `v1.2-dev-stage2-untested`

**Status:** Awaiting spectrometer validation per TESTING_CHECKLIST.md

## How to Use

### For Production Work (Use `main`)

```bash
git checkout main
# Use tested sequences: hX.c, hXX.c, etc.
# These have safety fixes but are otherwise unchanged
```

### For Testing New Architecture (Use `development`)

```bash
git checkout development
# Test new architecture at spectrometers
# Follow TESTING_CHECKLIST.md
# Report results before merging to main
```

### For Daily Spectrometer Use

**Recommendation:** Stay on `main` branch until testing complete

```bash
git checkout main
git pull origin main
# Use normal sequences - they're safer now with corrected duty cycles
```

## Testing Process

1. **On `development` branch:** Test architecture per TESTING_CHECKLIST.md
2. **Minimum required:** Tests 1-7 (2 hours)
3. **Recommended:** Tests 1-9 (4 hours)
4. **Critical:** Duty cycle protection tests (must pass)

### Testing Status Tracking

Edit `TESTING_CHECKLIST.md` and check off completed tests:

```markdown
- [x] Test 1: Architecture integration ✅
- [x] Test 2: Validation framework ✅
- [ ] Test 3: Make functions basic
...
```

## Merging to Production

**After all critical tests pass:**

```bash
git checkout main
git merge development --no-ff -m "Stage 2: Production release after testing"
git tag -a "v2.0-production" -m "Stage 2 validated and production-ready"
git push origin main v2.0-production
```

**DO NOT merge until:**
- [ ] TESTING_CHECKLIST.md Tests 1-7 complete
- [ ] No probe damage observed
- [ ] Performance validated
- [ ] Sign-off obtained

## What If Tests Fail?

If testing reveals issues:

```bash
# Stay on development branch
git checkout development

# Fix issues
# ... make corrections ...

# Commit fixes
git add .
git commit -m "Fix: [describe issue from testing]"

# Re-test
# Repeat TESTING_CHECKLIST.md

# Tag new version when ready
git tag -a "v1.3-dev-stage2-fixed" -m "Architecture fixes from testing"
```

## Safety First

**The `main` branch discipline ensures:**
- Production spectrometers always have tested code
- Experimental features isolated for testing
- Clear rollback path if issues arise
- No risk of accidentally using untested code

## Current Status Summary

| Branch | Status | Use Case | Safety |
|--------|--------|----------|--------|
| `main` | ✅ Production | Daily use | Tested |
| `development` | ⚠️ Testing | Experimental | Untested |

**Recommendation:** Use `main` for all production work until `development` testing complete.

## Questions?

- **Want tested safety fixes?** → Use `main` branch
- **Want to test new architecture?** → Use `development` branch
- **Need rollback?** → `git checkout main` (always safe)
- **Testing concerns?** → See TESTING_CHECKLIST.md

---

**Remember:** Good science requires validation before deployment!
