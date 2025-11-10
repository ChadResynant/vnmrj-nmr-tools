hXXX VnmrJ Template Set
========================

Created: 2025-11-09
Purpose: 3D H-detected X-X-X correlation with dual mixing periods

SEQUENCE TOPOLOGY:
H → HX CP → [Y-dec X evolution d3] → [X mixing #1] → [Y-dec X evolution d2] → [X mixing #2] → XH CP → H acquisition

DIMENSIONALITY:
- F1: Direct H acquisition (sw, at)
- F2: Indirect X via d2 (sw1, ni, phase)
- F3: Indirect X via d3 (sw2, ni2, phase2)

KEY FEATURES:
1. Dual X-X mixing periods (mMix and mMix1)
2. Optional soft pulses in both indirect dimensions (softpul, softpul2)
3. C-detected style with 5% duty cycle protection
4. Multiple mixing options: SPC5, SPCN, C7, RFDR, RAD/DARR, PARIS

TAB STRUCTURE:
1. Acquisition.xml
   - F1 (H direct): sw, at, d1, nt, tRF
   - F2 (X indirect): sw1, ni, f1180, phase
   - F3 (X indirect): sw2, ni2, f2180, phase2
   - Sample: srate, temp, gain
   - Receiver timing: rd, ad

2. PulseSequence.xml
   - HX cross-polarization: tHX, aHhx, aXhx
   - Sequence control: softpul, softpul2, dqf_flag
   - First X-X mixing (mMix): method, tXmix, tZF, amplitudes
   - Second X-X mixing (mMix1): method, tXmix1, aHmix, PARIS params
   - Soft pulse shapes: shp1X and shp2X parameters

3. Decoupling.xml
   - H channel: TPPM, SPINAL, WALTZ
   - Y channel: TPPM, SPINAL, WALTZ, PIPS
   - Z channel: TPPM, SPINAL, WALTZ (conditional on dfrq3>0)

4. Calibration.xml
   - Transmitter (tn): nucleus, frequency, pwX90, tpwr, tof
   - Decoupler 1 (dn/H): pwH90, dpwr, dof
   - Decoupler 2 (dn2/Y): pwY90, dpwr2, dof2 (conditional)
   - Decoupler 3 (dn3/Z): pwZ90, dpwr3, dof3 (conditional)

DIFFERENCES FROM hXX (2D):
- Added F3 dimension (sw2, ni2, phase2, f2180)
- Added second mixing period (mMix1, tXmix1)
- Added softpul2 for F2 dimension
- Added tRF parameter for duty cycle compensation
- Expanded soft pulse section for dual dimensions

XML SYNTAX NOTES:
- HTML-escaped comparisons: &lt; &gt; &lt;&gt;
- Conditional visibility: show="if condition then $SHOW=1 else $SHOW=0 endif"
- Dynamic labels: show="$LABEL='Text ('+param+')'"
- Menu value setting: vc="parunits('set','param','$VALUE')"
