# CP Optimization Parameters - Schanda Group Reference

**Source**: Paul Schanda's group (Grenoble)
**System**: Bruker NEO, TopSpin 4.5
**Context**: High-speed MAS optimized CP conditions for solid-state NMR of proteins

These parameters represent successful CP optimization results at various MAS rates. They serve as excellent starting points for OPTO optimization and validation of theoretical match conditions.

---

## 55 kHz MAS

At this spinning rate, low-power CP conditions work well.

### H-N CP (Proton to Nitrogen Cross-Polarization)

**Optimized Parameters:**
- **¹H RF field**: 90-95 kHz (`cnst42`)
- **¹H ramp**: 90-100%
- **¹⁵N RF field**: 35-40 kHz (`cnst52`)
- **Contact time**: 800-1000 μs

**Match condition**: Approximately n=1 sideband match with low power on both channels

---

### H-C CP (Proton to Carbon Cross-Polarization)

Two successful conditions have been identified:

#### Option 1: Low-power match (similar to H-N)
- **¹H RF field**: 90-95 kHz (`cnst42`)
- **¹H ramp**: 90-100%
- **¹³C RF field**: 35-40 kHz (`cnst52`)
- **Contact time**: 800-1000 μs

#### Option 2: Asymmetric low-power match
- **¹H RF field**: 10 kHz (`cnst41`)
- **¹H ramp**: 50-100%
- **¹³C RF field**: 45 kHz or 65 kHz (`cnst31`)
- **Contact time**: 500 μs

**Note**: Option 2 uses very low ¹H power with a wide ramp range

---

### N-CA CP (Nitrogen to Alpha-Carbon Transfer)

**Optimized Parameters:**
- **¹³Cα RF field**: 10-16 kHz (`cnst34`)
- **¹⁵N RF field**: 38-40 kHz (`cnst54`)
- **Ramp**: 70-100%
- **Contact time**: 8 ms

**Note**: Longer contact time required for heteronuclear transfer

---

## 100 kHz MAS

At ultra-high spinning rates, low-power fractional-ωr matches are optimal.

### H-N CP

**Optimized Parameters:**
- **¹H RF field**: 130 kHz (`cnst42`)
- **¹H ramp**: 90-100%
- **¹⁵N RF field**: 30 kHz (`cnst52`)
- **Contact time**: 800-1000 μs

**Match condition**: Approximately 1.3 × ωr on ¹H, 0.3 × ωr on ¹⁵N

---

### H-C CP

Two successful conditions:

#### Option 1: Low power on carbon
- **¹H RF field**: 30 kHz (`cnst41`)
- **¹H ramp**: 50-100%
- **¹³C RF field**: 70 kHz (`cnst31`)
- **Contact time**: 500 μs

**Match condition**: 0.3 × ωr on ¹H, 0.7 × ωr on ¹³C

#### Option 2: Low power on carbon (reversed ratio)
- **¹H RF field**: 130 kHz (`cnst41`)
- **¹H ramp**: 90-100%
- **¹³C RF field**: 30 kHz (`cnst31`)
- **Contact time**: 500 μs

**Match condition**: 1.3 × ωr on ¹H, 0.3 × ωr on ¹³C

---

## 38 kHz MAS

Medium-speed regime where n=±1 matches are common.

### N-H CP (Reverse CP from Nitrogen to Proton)

**Optimized Parameters:**
- **¹H RF field**: 79 kHz (`cnst40`)
- **¹⁵N RF field**: 50 kHz (`cnst56`)
- **Ramp**: 100-90% (ramp down)
- **Contact time**: 1.1 ms

**Match condition**: Approximately n=2 on ¹H, n=1 on ¹⁵N

---

### H-N CP (Forward CP from Proton to Nitrogen)

**Optimized Parameters:**
- **¹H RF field**: 80 kHz (`cnst42`)
- **¹⁵N RF field**: 50 kHz (`cnst52`)
- **Ramp**: 90-100% (ramp up)
- **Contact time**: 1 ms

**Match condition**: Similar to reverse, approximately n=2 on ¹H

---

### CA-N CP (Alpha-Carbon to Nitrogen)

**Optimized Parameters:**
- **¹³Cα RF field**: 13.2 kHz (`cnst34`)
- **¹⁵N RF field**: 25 kHz (`cnst54`)
- **Ramp**: 70-100%
- **Contact time**: 8 ms

**Match condition**: Low-power fractional match

---

### CO-N CP (Carbonyl to Nitrogen)

**Optimized Parameters:**
- **¹³CO RF field**: 11 kHz (`cnst33`)
- **¹⁵N RF field**: 25 kHz (`cnst53`)
- **Ramp**: 70-100%
- **Contact time**: 8 ms

**Match condition**: Low-power fractional match, similar to CA-N

---

### H-CA CP (Proton to Alpha-Carbon)

**Optimized Parameters:**
- **¹H RF field**: 82 kHz (`cnst41`)
- **¹³Cα RF field**: 59 kHz (`cnst31`)
- **Ramp**: 90-100%
- **Contact time**: 3 ms

**Match condition**: Approximately n=2 on ¹H, n=1.5 on ¹³C

---

## Key Observations

### Spinning Rate Dependencies

1. **Ultra-high speed (100 kHz)**:
   - Fractional-ωr matches (0.3, 0.7, 1.3 × ωr) work well
   - Low absolute RF power requirements
   - Shorter contact times sufficient (500-1000 μs)

2. **High speed (55 kHz)**:
   - Low-power conditions effective
   - Can use very low ¹H power (10 kHz) with wide ramps
   - Medium contact times (0.5-1 ms)

3. **Medium speed (38 kHz)**:
   - Higher RF powers needed (2-3 × ωr)
   - Longer contact times for heteronuclear (8 ms)
   - Both ramp-up and ramp-down successful

### Ramp Strategies

- **90-100% (ramp up)**: Start below match, sweep through to above
- **100-90% (ramp down)**: Start above match, sweep through to below
- **70-100% (wide ramp)**: Broader sweep for more robustness
- **50-100% (very wide)**: Maximum coverage of match manifold

### Heteronuclear Transfers (N-C, CA-N, CO-N)

- Require much longer contact times (8 ms vs 0.5-1 ms for H-X)
- Use low RF powers (10-40 kHz)
- Wide ramps (70-100%) provide robustness

---

## Using These Parameters

### As OPTO Starting Points

These parameters provide excellent initial guesses for OPTO optimization:

1. **Select parameters for your spinning rate**
2. **Use as center of optimization search range**
3. **Allow ±20% variation for OPTO to refine**
4. **Compare OPTO results to these values for validation**

### Cross-System Applicability

While optimized on Bruker NEO:
- Match conditions are universal (depend on physics, not hardware)
- Absolute RF fields may need adjustment for different probe/amplifier
- Contact times should transfer well
- Ramp strategies are generally applicable

---

## Related Documentation

- See `docs/opto_frontend_design.md` for comprehensive workflow automation
- See `CLAUDE.md` for expert knowledge on CP pathologies
- See main repository for OPTO tools and implementation

---

**Last Updated**: November 2025
**Contributed by**: Paul Schanda group, IBS Grenoble
