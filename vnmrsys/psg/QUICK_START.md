# NMR Validation Framework - Quick Start Guide

## File Locations

```
/home/user/vnmrj-nmr-tools/vnmrsys/psg/
├── nmr_validation.h                    ← Header file (include this)
├── nmr_validation.c                    ← Implementation (compile with)
├── NMR_VALIDATION_USAGE_EXAMPLES.md    ← 10 detailed examples
├── VALIDATION_FRAMEWORK_SUMMARY.md     ← Complete documentation
└── QUICK_START.md                      ← This file
```

## 30-Second Integration

### Add to your sequence:

```c
#include "nmr_validation.h"

void pulsesequence() {
    // 1. Get parameters
    double d1 = getval("d1");
    char seqfil[MAXSTR];
    getstr("seqfil", seqfil);
    
    // 2. Calculate total RF time
    double tRF = pwH90 + pwX90 + tHX + d2 + tXmix + at;
    
    // 3. Validate (2 lines!)
    NMRSystemLimits limits = init_nmr_system_limits();
    validate_full_sequence(seqfil, tRF * 1e-6, d1, getval("dpwr"));
    
    // 4. Execute sequence
    status(A);
    // ... your sequence code ...
}
```

## Compilation

```bash
cc -o hXX hXX.c nmr_validation.c -lm
```

## What You Get

- Automatic C-detected vs H-detected detection
- 5% duty cycle for C-detected sequences
- 10-15% power-dependent duty for H-detected
- Clear error messages with solutions
- B1 field calculations from power levels
- Three-tier validation system

## Key Functions

| Function | Purpose |
|----------|---------|
| `init_nmr_system_limits()` | Initialize limits structure |
| `validate_sequence()` | Tier 2: Sequence-level validation |
| `validate_runtime()` | Tier 3: Runtime validation |
| `validate_full_sequence()` | All tiers in one call |
| `get_safe_duty_limit()` | Get duty limit for sequence type |
| `calculate_B1_from_power()` | Convert dB to Hz |

## Detection Type Auto-Identification

- hXX, hYXX, hXYXX → **C-detected** → 5% duty
- hXH, HhXH, hYXH → **H-detected** → 10-15% duty

## Example Error Message

```
ERROR: Duty cycle 6.5% exceeds 5% limit for hXX

SOLUTION: Solutions (in order of preference):
  1. Increase relaxation delay (d1) - most effective
  2. Reduce mixing times (tXmix, tYmix, etc.)
  3. Reduce evolution times (d2, d3) - impacts resolution
  4. Use Non-Uniform Sampling (NUS) to allow longer d1
Example: Increasing d1 from 1.0 to 2.6 seconds would achieve 5% duty cycle
```

## More Examples

See `NMR_VALIDATION_USAGE_EXAMPLES.md` for 10 detailed examples covering:
- All three validation tiers
- Power-dependent duty cycles
- B1 field calculations
- Integration with make functions
- Error handling patterns

## Full Documentation

See `VALIDATION_FRAMEWORK_SUMMARY.md` for complete documentation including:
- Structure definitions
- Function reference
- Standards compliance
- Testing recommendations
- Performance characteristics

## Support

Questions? Check:
1. SAFETY_STANDARDS.md for duty cycle rationale
2. REFACTORING_IMPLEMENTATION_PLAN.md for architecture
3. NMR_VALIDATION_USAGE_EXAMPLES.md for examples
