# Steady-State Scans Workaround

**Issue**: First OPTO measurement has artificially high intensity
**Impact**: Skews optimization, makes manual starting point appear better than it is
**Status**: Workaround available (Varian), in development (Bruker)
**Last Updated**: 2025-11-09

## Problem Description

### What Happens

When OPTO starts optimization:

1. **First experiment**: Has proper relaxation delay before acquisition
   - Sample has equilibrated during setup
   - Full T1 relaxation occurred
   - Results in maximum possible signal

2. **Subsequent experiments**: Run back-to-back with short d1
   - Steady-state scans (ss/ds) set to 0 or low value to save time
   - Not fully relaxed between measurements
   - Results in lower signal than first measurement

### Why This is a Problem

**Optimization Impact**:
- First point (manual starting condition) appears artificially good
- OPTO sees signal decrease from "excellent" first point
- May never recover to initial signal level
- Can conclude manual parameters were better than optimization

**User Experience**:
- Loss of confidence in OPTO
- Appears optimization makes things worse
- Unclear whether to trust final result
- Wasted spectrometer time

**Example**:
```
Measurement 1 (after full relaxation): Signal = 100 (artificially high)
Measurement 2 (steady state):          Signal = 85  (true steady-state)
Measurement 3 (optimized, steady state): Signal = 90  (improved!)

User perception: "OPTO made signal drop from 100 to 90" ❌
Reality: "OPTO improved from 85 to 90" ✅
```

---

## Solution 1: Varian Workaround (Working)

### Implementation

**Setup**: Set steady-state scans in initial parameters

```bash
# In OpenVNMRJ command line:
ss=16  # Steady-state scans (Varian notation)
nt=8   # Total scans for experiment

# This is equivalent to Bruker: ds=16 ns=8
```

**Start OPTO**:
- OPTO begins first experiment with ss=16
- First 16 scans discarded, next 8 scans acquired
- First measurement properly equilibrated

**Manual Intervention** (after first experiment completes):

```bash
# Execute in OpenVNMRJ:
wexp('opto_spectral_measure ss=0')
```

**What This Does**:
- `wexp()` sets expression to execute after experiment completes
- Changes `ss` parameter to 0 for all subsequent experiments
- Next experiments don't waste time on steady-state scans
- All subsequent measurements are in true steady-state

**Result**:
- First measurement = steady-state (correct)
- All other measurements = steady-state (correct)
- No artificial inflation of first point

### Usage Instructions

1. Set up OPTO job as normal
2. Set `ss=16` (or appropriate value for your T1) and `nt=8`
3. Start OPTO
4. **Wait for first experiment to complete**
5. Execute: `wexp('opto_spectral_measure ss=0')`
6. Continue OPTO as normal

**Timing**: Execute the `wexp()` command **after** first experiment finishes but **before** second begins.

### Limitations

- Requires manual intervention
- Easy to forget
- Must execute at right time
- Not ideal for overnight/unattended runs

---

## Solution 2: Bruker Implementation (In Development)

### Current Status

**As of 2025-11-09**:
- Not yet implemented in Topspin version
- Varian approach needs porting to Python code
- Requires careful testing for side effects

### Proposed Approach

**Similar to Varian**:
- Set DS parameter (dummy scans) initially
- After first experiment, modify DS to 0
- Use Topspin Python/AU program hooks

**Concerns** (Chad/Thiru):
> "A similar approach should be possible in Topspin but we need to look more closely at the Python code to make sure it does not have other unintended consequences."

**Development Tasks**:
1. Identify where to insert parameter modification in OPTO Python code
2. Test on non-critical experiment
3. Verify no side effects on:
   - Parameter loading
   - Experiment execution
   - Data saving
   - Multi-pass optimization

### Manual Workaround (Bruker)

**Until automatic solution available**:

**Option A - Set DS Higher Initially**:
```
# In Topspin, initial setup:
DS 16  # Dummy scans
NS 8   # Number of scans

# Start OPTO
# After first experiment: manually change parameter?
# (Exact mechanism TBD - requires testing)
```

**Option B - Accept First Measurement Artifact**:
- Let OPTO run normally
- Mentally discount first measurement
- Trust optimization based on measurements 2+
- Look at log file, verify improvement over measurements 2+

**Option C - Discard First Measurement** (Thiru to implement?):
- Modify OPTO to not use first measurement in optimization
- Record twice, use second as baseline
- Cleaner solution, no manual intervention

---

## Solution 3: "Record Twice" Approach (Proposed)

**Concept** (Petra's suggestion):
- OPTO automatically runs first condition twice
- First run: has full relaxation (discarded)
- Second run: true steady-state (used as baseline)
- All subsequent runs: steady-state (normal optimization)

### Advantages

- ✅ No manual intervention required
- ✅ Cleaner solution
- ✅ Works for unattended runs
- ✅ Clear what's happening

### Disadvantages

- ❌ Wastes time on one extra measurement
- ❌ Requires code modification
- ❌ Not yet implemented

### Implementation Considerations

**Code Changes Required**:
1. Detect first measurement
2. Run twice automatically
3. Discard first, keep second
4. Continue optimization normally

**Overhead**:
- One extra measurement (~15-30 seconds for typical CP)
- Negligible in context of full optimization (30+ minutes)
- Worth it for cleaner solution

**Priority**: Medium - good long-term solution

---

## Alternative Approaches

### Approach 4: Longer d1 for First Measurement Only

**Concept**:
- First measurement: d1 = 5×T1 (full relaxation)
- Subsequent measurements: d1 = 1.5s (steady-state)

**Issues**:
- Still different from subsequent measurements
- Harder to implement (different acquisition parameters)
- Less clean than "record twice"

### Approach 5: Always Use Steady-State Scans

**Concept**:
- Set DS=16 (or ss=16) for ALL measurements
- Never change

**Issues**:
- ❌ Wastes significant time
- ❌ 30-minute optimization becomes 1+ hour
- ❌ Not practical for high-throughput

**When Acceptable**:
- Long experiments anyway (e.g., 2D)
- T1 is very long
- Spectrometer time not limiting

---

## Current Recommendations

### For Varian Users

**Use manual workaround** (Solution 1):
1. Set `ss=16 nt=8`
2. Start OPTO
3. Execute `wexp('opto_spectral_measure ss=0')` after first experiment
4. Works reliably

### For Bruker Users

**Until automatic solution available**:

**Priority 1**: Focus on other parameters first
> "I think the steady state issue is not the largest problem from yesterday. Getting the integral ranges properly defined, the names matching, and the limits set to better values seemed to get things moving in the right direction." - Chad, 2025-11-09

**Priority 2**: Use workaround if needed
- Option B (accept artifact) is simplest for now
- Verify optimization improves from measurements 2+ onward
- Check log file for actual progression

**Priority 3**: Wait for automated solution
- In development
- Will be tested and released when ready

### For Developers

**Next Steps**:
1. Port Varian approach to Bruker (Thiru + Barry)
2. OR implement "record twice" (better long-term)
3. Test thoroughly for side effects
4. Document and release in next version

---

## Verification

### How to Check if Issue Affects Your Data

**Look at OPTO log file**:

```
# Example log (abbreviated):
Line  Param1  Param2  Score
1     85.0    40.0    1000   ← Suspiciously high?
2     85.0    40.0    850    ← More realistic steady-state
3     87.0    41.0    870    ← Improving
4     89.0    42.0    890    ← Better
...
```

**Questions to Ask**:
1. Is line 1 significantly higher than line 2 (same parameters)?
2. If yes, by how much? (>10% suggests steady-state issue)
3. Does optimization improve from line 2 onward?
4. Is the final result better than line 2 (not line 1)?

**If steady-state issue present**:
- Ignore line 1
- Evaluate optimization based on lines 2+
- Final result should beat line 2, not necessarily line 1

---

## Related Issues

### Issue: Optimization Appears to Make Things Worse

**If you see**:
- First measurement very high
- All subsequent measurements lower
- "OPTO made my signal worse!"

**Check**:
1. Is this steady-state effect? (see verification above)
2. Are integration ranges correct? (see `integration-ranges.md`)
3. Are parameter limits reasonable? (see `schanda-group-issues.md`)

**Usually**:
- Combination of steady-state + other issues
- Fix integration ranges first
- Then assess steady-state impact
- Then evaluate optimization success

---

## Examples

### Example 1: Good Optimization with Steady-State Artifact

```
Log file (Signal values):
1.  1000  (first measurement, full relaxation)
2.  850   (same params, steady-state) ← Real baseline
3.  860   (slight improvement)
4.  870   (continuing improvement)
...
25. 920   (final optimized)

Conclusion: Optimization successful!
Improved from 850 → 920 (8.2% gain)
First measurement artifact not relevant
```

### Example 2: Failed Optimization (Not Steady-State Issue)

```
Log file (Signal values):
1.  1000
2.  950   ← Small steady-state drop, but still high
3.  920   ← Decreasing
4.  880   ← Getting worse
...
25. 650   ← Much worse

Conclusion: Optimization failed
Steady-state effect is small (1000→950 = 5%)
Real problem: wrong integration range or parameter limits
```

**Action**: Check integration ranges and parameter setup first!

---

## FAQ

**Q: Should I always use steady-state scans?**
A: For CP optimization: Set initially, then drop to 0 (use workaround). For very long T1 or specific experiments, may be necessary throughout.

**Q: How many steady-state scans do I need?**
A: Depends on T1. Rule of thumb:
- Short T1 (<5s): ss=8-16
- Medium T1 (5-20s): ss=16-32
- Long T1 (>20s): ss=32-64 or longer d1

**Q: Can I just ignore the first point?**
A: Yes! That's a valid workaround. Evaluate optimization based on points 2+.

**Q: Will this be fixed automatically?**
A: Yes, in development. Use workarounds until then.

**Q: Does this affect shimming too?**
A: Can affect any OPTO optimization. But CP optimization is most sensitive due to variable relaxation paths.

---

## References

- **Email**: Chad to Schanda group, 2025-11-08
- **Beta Testing**: `opto/docs/beta-testing/schanda-group-issues.md`
- **OPTO Manual**: Section on steady-state scans (to be added)

---

**Document Owner**: Chad Rienstra
**Contributors**: Thiru, Petra Rovó (suggestion)
**Status**: Workaround documented, automation in development
**Last Updated**: 2025-11-09
