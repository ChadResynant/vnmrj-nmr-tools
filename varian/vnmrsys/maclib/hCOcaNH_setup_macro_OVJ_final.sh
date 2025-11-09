/*
 * hCOcaNH - Setup macro for 4D CO-CA-NH correlation experiments
 * 
 * This macro sets up optimized parameters for 4D hCOcaNH experiments:
 * H → CO → CO/CA mixing → CA → NH → H detection
 * 
 * Experiment design:
 * - F4 (direct): 1H detection
 * - F3 (first indirect, ni,sw1,d2): 15N evolution (NH)
 * - F2 (second indirect, ni2,sw2,d3): 13CA evolution
 * - F1 (third indirect, ni3,sw3,d4): 13CO evolution
 * 
 * Key optimizations:
 * - Reverse connectivity compared to hCAcoNH
 * - CO-CA mixing for inter-residue correlations
 * - Optimized for sequential backbone assignment
 * - Conservative 4D matrix with aggressive NUS
 * 
 * Usage: hCOcaNH
 * 
 * Author: Biosolids application-specific macros
 * Date: July 2025
 */

seqfil = 'hYYXH'  // Use 4D hYYXH sequence
layout = seqfil
// OVJ-Compatible empirical defaults for hCOcaNH (July 2025)
// Pulse widths (µs)
pwH90=1.8
pwX90=2.7
pwY90=1.9
pw=4.9
pwXwaltz=56
pwHspinal=85
pwHpxy=10000

// Cross-polarization contact times (µs)
tHY=4000
tYX=10000
tXH=800

// Power levels
tpwr=63
dpwr=63
dpwr2=63

// Decoupling sequences
Hseq='waltz'
Xseq='waltz'
Yseq='waltz'

// Spectral widths (Hz) and delays (µs)
sw=100000
sw2=8928.5
sw3=6793.5
d2=0
d3=0
d4=0

// Matrix sizes (to be overridden later)
ni=0
ni2=0
ni3=0


// Load base parameters
hYXH

write('line3','')
write('line3','==========================================')
write('line3','  hCOcaNH Setup (4D CO-CA-NH Sequential)')
write('line3','==========================================')
write('line3','')
write('line3','Experiment: H → CO → [CO-CA mix] → CA → NH → H')
write('line3','F4 (direct): 1H detection')
write('line3','F3 (ni,sw1,d2): 15N (NH) evolution')
write('line3','F2 (ni2,sw2,d3): 13CA evolution')
write('line3','F1 (ni3,sw3,d4): 13CO evolution')
write('line3','')

//=============================================================================
// 4D CO-CA-NH SPECIFIC SPECTRAL WIDTHS
//=============================================================================

write('line3','Setting 4D CO-CA-NH spectral widths...')

// F3: 15N dimension (NH region)
sw1 = 2500    // 2.5 kHz covers NH region
setlimit('sw1',5000,1000,1)

// F2: 13CA dimension
sw2 = 12000   // 12 kHz covers CA region (40-80 ppm)
setlimit('sw2',20000,5000,1)

// F1: 13CO dimension  
sw3 = 6000    // 6 kHz covers CO region (165-185 ppm)
setlimit('sw3',10000,3000,1)

// Direct 1H dimension
sw = 12000    // 12 kHz for amide protons
setlimit('sw',20000,5000,1)

//=============================================================================
// 4D OPTIMIZED ACQUISITION PARAMETERS
//=============================================================================

write('line3','Setting 4D CO-CA-NH acquisition parameters...')

// Conservative 4D matrix sizes
ni = 20       // 15N points (NH) - reduced for 4D
ni2 = 24      // 13CA points - more resolution for CA
ni3 = 16      // 13CO points - reduced for 4D (narrow region)
setlimit('ni',64,8,1)
setlimit('ni2',64,8,1)
setlimit('ni3',64,8,1)

// Evolution time limits (optimized for CO-CA pathway)
d2max = 12e-3    // 12 ms for 15N
d3max = 10e-3    // 10 ms for 13CA (better T2 than CO)
d4max = 6e-3     // 6 ms for 13CO (shortest T2)

//=============================================================================
// 4D CO-CA-NH CROSS-POLARIZATION OPTIMIZATION
//=============================================================================

write('line3','Optimizing 4D CO-CA-NH cross-polarization...')

// H→CO Cross Polarization (first transfer, longer than H→CA)
tHY = 1500e-6    // 1.5 ms H→CO contact time
bHY = 62000      // 62 kHz Hartmann-Hahn match
aHhy = 2600      // Medium-high power for CO transfer
aYhy = 2200      // Optimized for 13CO

// CA→NH Cross Polarization (after mixing, CA to NH)
tYX = 8000e-6    // 8 ms CA→NH contact time
bYX = 35000      // 35 kHz for CA-NH transfer
aYyx = 2000      // Medium power for long contact
aXyx = 2200      // Optimized for 15N

// NH→H Cross Polarization (final transfer)
tXH = 300e-6     // 300 us NH→H contact time
bXH = 70000      // 70 kHz for fast NH-H transfer
aXxh = 2600      // High power for short contact
aHxh = 2800      // High power for 1H

//=============================================================================
// 4D CO-CA MIXING OPTIMIZATION
//=============================================================================

write('line3','Setting up CO-CA mixing...')

// First mixing period (CO evolution and mixing)
mMix1 = 'spcn'   // Use SPCN for CO-CA transfer
qYspcn1 = 6      // 6 rotor periods (slightly longer for CO-CA)
NYspcn1 = 5      // N=5 for SPCN
aHmixspcn1 = 100 // Lower power during mixing
dqf_flag1 = '1'  // Standard mixing

// Second mixing period (typically off for this experiment)
mMix2 = 'n'      // No second mixing
qYspcn2 = 0
dqf_flag2 = '1'

// Y-Y mixing delay
tYmix = 120e-6   // 120 us delay between mixing periods

//=============================================================================
// 4D DECOUPLING OPTIMIZATION
//=============================================================================

write('line3','Setting up 4D decoupling...')

// X decoupling during 1H acquisition (15N decoupling)
Xseq = 'waltz'
aXwaltz = 1800
pwXwaltz = 50e-6

// H decoupling during evolution periods
Hseq = 'spinal'
aHspinal = 2200  // Slightly higher for CO experiment
pwHspinal = 8e-6
phHspinal = 15

//=============================================================================
// 4D SPECIFIC TIMING PARAMETERS
//=============================================================================

write('line3','Setting 4D timing parameters...')

// Relaxation delay (CO experiments may need longer)
d1 = 2.0         // 2.0 seconds (CO T1 consideration)

// Homospoil and constant delays
hst = 800e-6     // 800 us homospoil
hstconst = 150e-6 // 150 us constant delay

// Acquisition parameters
at = 20e-3       // 20 ms acquisition
np = 1536        // 1.5K points direct dimension

//=============================================================================
// 4D NUS PARAMETERS (essential for 4D)
//=============================================================================

$seqname = seqfil
if (strstr($seqname,'_S') > 0) then
    write('line3','Setting 4D NUS parameters (ESSENTIAL for practical 4D)...')
    
    // 4D NUS sampling - optimized for CO-CA-NH
    nimax = 40      // Max 15N increments
    ni2max = 48     // Max 13CA increments (more resolution)
    ni3max = 32     // Max 13CO increments (narrow region)
    
    // Aggressive NUS sampling for 4D (6-8%)
    $full_sampling = nimax * ni2max * ni3max
    nrows = trunc($full_sampling * 0.07)  // 7% sampling
    if (nrows < 600) then
        nrows = 600  // Minimum 600 points for CO-CA connectivity
    endif
    
    // Extended safety parameters
    tRF = 100e-3    // 100 ms relaxation delay
    
    // Schedule file
    sparse_file = userdir + '/nus_schedules/hCOcaNH_' + format(nrows,0,0) + '.sched'
    
    write('line3','4D NUS parameters set:')
    write('line3','  Full sampling: %d x %d x %d = %d points',nimax,ni2max,ni3max,$full_sampling)
    write('line3','  NUS sampling: %d points (%.1f%%)',nrows,100.0*nrows/$full_sampling)
    write('line3','  ⚠ NUS is ESSENTIAL - full 4D would take weeks!')
    
else
    write('error','4D hCOcaNH requires NUS-enabled sequence!')
    write('line3','Load hYYXH_S or similar NUS-enabled 4D sequence')
    write('line3','Full 4D sampling is impractical for routine use')
endif

//=============================================================================
// 4D SOFT PULSE PARAMETERS
//=============================================================================

write('line3','Setting 4D soft pulse parameters...')

// Typically no soft pulses for 4D backbone experiments
softpul = 'n'   // First evolution period
softpul2 = 'n'  // Second evolution period

//=============================================================================
// 4D PULSE SEQUENCE PARAMETERS
//=============================================================================

write('line3','Setting 4D pulse sequence parameters...')

// No H-H mixing for backbone experiments
qHrfdr = 0

// Minimal presaturation
qHpxy = 1
aHpxy = 150
pwHpxy = 50e-6

//=============================================================================
// 4D PHASE CYCLING
//=============================================================================

write('line3','Setting 4D phase cycling...')

// 4D phase cycling (standard order)
array = 'phase3,phase2,phase'
phase = 1,2
phase2 = 1,2
phase3 = 1,2

//=============================================================================
// 4D SAFETY AND VALIDATION
//=============================================================================

write('line3','Performing 4D safety checks...')

// Calculate duty cycle
$duty_pulse_time = tHY + tYX + tXH + d2max + d3max + d4max + 4*pwH90 + tYmix
$duty_cycle_time = $duty_pulse_time + d1 + at + 4e-6
$duty_cycle = $duty_pulse_time / $duty_cycle_time

if ($duty_cycle > 0.20) then
    write('line3','⚠ WARNING: Duty cycle %.1f%% > 20%% for 4D experiment',$duty_cycle*100)
    write('line3','  Consider reducing contact times or evolution limits')
endif

// Check spinning rate requirement
if (srate < 12000) then
    write('line3','⚠ WARNING: Spinning rate %.0f Hz may be too low for 4D',srate)
    write('line3','  Recommend srate ≥ 12 kHz for 4D stability')
endif

// Estimate experiment time
if (strstr($seqname,'_S') > 0) then
    $expt_time = nrows * (d1 + at + tHY + tYX + tXH + d2max + d3max + d4max) / 3600
else
    $expt_time = ni * ni2 * ni3 * (d1 + at + tHY + tYX + tXH + d2max + d3max + d4max) / 3600
endif

write('line3','ℹ Estimated experiment time: %.1f hours',$expt_time)
if ($expt_time > 48) then
    write('line3','⚠ WARNING: Experiment time > 48 hours!')
    write('line3','  Consider more aggressive NUS or smaller matrix')
endif

//=============================================================================
// 4D COMPLETION MESSAGE
//=============================================================================

write('line3','')
write('line3','✓ 4D hCOcaNH setup complete!')
write('line3','')
write('line3','Key parameters set:')
write('line3','  Pathway: H(1.5ms)→CO → [CO-CA mix] → CA(8ms)→NH(300μs)→H')
write('line3','  Spectral widths: 1H(12kHz), 15N(2.5kHz), 13CA(12kHz), 13CO(6kHz)')
if (strstr($seqname,'_S') > 0) then
    write('line3','  NUS matrix: %d x %d x %d → %d points (%.1f%%)',nimax,ni2max,ni3max,nrows,100.0*nrows/(nimax*ni2max*ni3max))
else
    write('line3','  Matrix: %d x %d x %d points',ni,ni2,ni3)
endif
write('line3','  Evolution limits: 15N(%.0fms), 13CA(%.0fms), 13CO(%.0fms)',d2max*1000,d3max*1000,d4max*1000)
write('line3','  Estimated duty cycle: %.1f%%',$duty_cycle*100)
write('line3','  Estimated time: %.1f hours',$expt_time)
write('line3','')
write('line3','4D Optimization checklist:')
write('line3','□ Calibrate all pulse widths precisely')
write('line3','□ Optimize H→CO and CA→NH cross-polarizations')
write('line3','□ Test CO-CA mixing efficiency (SPCN parameters)')
write('line3','□ Verify 13C labeling pattern (CO and CA)')
write('line3','□ Compare with hCAcoNH for complementary connectivity')
write('line3','□ Ensure stable high-speed spinning (≥12 kHz)')
write('line3','□ Set conservative d1 for CO T1 recovery')
if (strstr($seqname,'_S') > 0) then
    write('line3','□ Generate optimized 4D NUS schedule')
    write('line3','□ Enable NUS: SPARSE=\'y\'')
    write('line3','□ Validate NUS parameters: nus_utils(\'validate\')')
else
    write('line3','⚠ CRITICAL: Load NUS-enabled sequence for practical 4D!')
endif
write('line3','□ Test with 2D/3D analogs first')
write('line3','□ Plan for multi-day acquisition')
write('line3','□ Consider running both hCAcoNH and hCOcaNH for complete assignment')
write('line3','')

// Set parameter focus for 4D
if (strstr($seqname,'_S') > 0) then
    dg('4D_MIX')
else
    dg('cpHY')
endif