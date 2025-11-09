/*
 * hCAcoNH - Setup macro for 4D CA-CO-NH correlation experiments
 * 
 * This macro sets up optimized parameters for 4D hCAcoNH experiments:
 * H → CA → CA/CO mixing → CO → NH → H detection
 * 
 * Experiment design:
 * - F4 (direct): 1H detection
 * - F3 (first indirect, ni,sw1,d2): 15N evolution (NH)
 * - F2 (second indirect, ni2,sw2,d3): 13CO evolution  
 * - F1 (third indirect, ni3,sw3,d4): 13CA evolution
 * 
 * Key optimizations:
 * - 4D matrix sizes optimized for sequential assignment
 * - CA-CO mixing for inter-residue connectivity
 * - Spectral widths for CA, CO, and NH regions
 * - Aggressive NUS sampling for practical experiment times
 * 
 * Usage: hCAcoNH
 * 
 * Author: Biosolids application-specific macros  
 * Date: July 2025
 */

seqfil = 'hYYXH'  // Use 4D hYYXH sequence
layout = seqfil

// Load base parameters
hYXH

write('line3','')
write('line3','==========================================')
write('line3','  hCAcoNH Setup (4D CA-CO-NH Sequential)')
write('line3','==========================================')
write('line3','')
write('line3','Experiment: H → CA → [CA-CO mix] → CO → NH → H')
write('line3','F4 (direct): 1H detection')
write('line3','F3 (ni,sw1,d2): 15N (NH) evolution')
write('line3','F2 (ni2,sw2,d3): 13CO evolution')
write('line3','F1 (ni3,sw3,d4): 13CA evolution')
write('line3','')

//=============================================================================
// 4D CA-CO-NH SPECIFIC SPECTRAL WIDTHS
//=============================================================================

write('line3','Setting 4D CA-CO-NH spectral widths...')

// F3: 15N dimension (NH region)
sw1 = 2500    // 2.5 kHz covers NH region
setlimit('sw1',5000,1000,1)

// F2: 13CO dimension  
sw2 = 6000    // 6 kHz covers CO region (165-185 ppm)
setlimit('sw2',10000,3000,1)

// F1: 13CA dimension
sw3 = 12000   // 12 kHz covers CA region (40-80 ppm)
setlimit('sw3',20000,5000,1)

// Direct 1H dimension
sw = 12000    // 12 kHz for amide protons
setlimit('sw',20000,5000,1)

//=============================================================================
// 4D OPTIMIZED ACQUISITION PARAMETERS
//=============================================================================

write('line3','Setting 4D CA-CO-NH acquisition parameters...')

// Conservative 4D matrix sizes
ni = 20       // 15N points (NH) - reduced for 4D
ni2 = 16      // 13CO points - reduced for 4D
ni3 = 24      // 13CA points - slightly more for resolution
setlimit('ni',64,8,1)
setlimit('ni2',64,8,1) 
setlimit('ni3',64,8,1)

// Evolution time limits (shorter for 4D to manage experiment time)
d2max = 12e-3    // 12 ms for 15N
d3max = 6e-3     // 6 ms for 13CO (shorter T2)
d4max = 10e-3    // 10 ms for 13CA

//=============================================================================
// 4D CA-CO-NH CROSS-POLARIZATION OPTIMIZATION
//=============================================================================

write('line3','Optimizing 4D cross-polarization...')

// H→CA Cross Polarization (first transfer)
tHY = 800000000
bHY = 65000      // 65 kHz Hartmann-Hahn match
aHhy = 2800      // High power for efficient CA transfer
aYhy = 2400      // Optimized for 13CA

// CO→NH Cross Polarization (after mixing)
tYX = 10000000000
bYX = 33000      // 33 kHz for CO-NH transfer
aYyx = 1900      // Medium-low power for long contact
aXyx = 2100      // Optimized for 15N

// NH→H Cross Polarization (final transfer)
tXH = 300000000
bXH = 70000      // 70 kHz for fast NH-H transfer
aXxh = 2600      // High power for short contact
aHxh = 2800      // High power for 1H

//=============================================================================
// 4D CA-CO MIXING OPTIMIZATION
//=============================================================================

write('line3','Setting up CA-CO mixing...')

// First mixing period (CA evolution and mixing)
mMix1 = 'spcn'   // Use SPCN for CA-CO transfer
qYspcn1 = 5      // 5 rotor periods
NYspcn1 = 5      // N=5 for SPCN
aHmixspcn1 = 120 // Low power during mixing
dqf_flag1 = '1'  // Standard mixing

// Second mixing period (typically off for this experiment)
mMix2 = 'n'      // No second mixing
qYspcn2 = 0
dqf_flag2 = '1'

// Y-Y mixing delay
tYmix = 100000000

//=============================================================================
// 4D DECOUPLING OPTIMIZATION
//=============================================================================

write('line3','Setting up 4D decoupling...')

// X decoupling during 1H acquisition (15N decoupling)
Xseq = 'waltz'
aXwaltz = 1800
pwXwaltz = 50000000

// H decoupling during evolution periods
Hseq = 'spinal'
aHspinal = 2100
pwHspinal = 8000000
phHspinal = 15

//=============================================================================
// 4D SPECIFIC TIMING PARAMETERS
//=============================================================================

write('line3','Setting 4D timing parameters...')

// Relaxation delay (critical for 4D experiment time)
d1 = 1.8         // 1.8 seconds (compromise between S/N and time)

// Homospoil and constant delays
hst = 800e-6     // 800 us homospoil
hstconst = 150e-6 // 150 us constant delay

// Acquisition parameters
at = 20e-3       // 20 ms acquisition (shorter for 4D)
np = 1536        // 1.5K points direct dimension

//=============================================================================
// 4D NUS PARAMETERS (strongly recommended for 4D)
//=============================================================================

$seqname = seqfil
if (strstr($seqname,'_S') > 0) then
    write('line3','Setting 4D NUS parameters (ESSENTIAL for practical 4D)...')
    
    // 4D NUS sampling - aggressive but necessary
    nimax = 40      // Max 15N increments
    ni2max = 32     // Max 13CO increments  
    ni3max = 48     // Max 13CA increments
    
    // Very aggressive NUS sampling for 4D (5-8%)
    $full_sampling = nimax * ni2max * ni3max
    nrows = trunc($full_sampling * 0.06)  // 6% sampling
    if (nrows < 500) then
        nrows = 500  // Minimum 500 points
    endif
    
    // Extended safety parameters for 4D
    tRF = 120e-3    // 120 ms relaxation delay
    
    // Schedule file
    sparse_file = userdir + '/nus_schedules/hCAcoNH_' + format(nrows,0,0) + '.sched'
    
    write('line3','4D NUS parameters set:')
    write('line3','  Full sampling: %d x %d x %d = %d points',nimax,ni2max,ni3max,$full_sampling)
    write('line3','  NUS sampling: %d points (%.1f%%)',nrows,100.0*nrows/$full_sampling)
    write('line3','  ⚠ NUS is ESSENTIAL - full 4D would take weeks!')
    
else
    write('error','4D hCAcoNH requires NUS-enabled sequence!')
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
pwHpxy = 50000000

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

// Calculate duty cycle (higher for 4D)
$duty_pulse_time = tHY + tYX + tXH + d2max + d3max + d4max + 4*pwH90 + tYmix
$duty_cycle_time = $duty_pulse_time + d1 + at + 4e-6
$duty_cycle = $duty_pulse_time / $duty_cycle_time

if ($duty_cycle > 0.20) then
    write('line3','⚠ WARNING: Duty cycle %.1f%% > 20%% for 4D experiment',$duty_cycle*100)
    write('line3','  Consider reducing contact times or evolution limits')
endif

// Check spinning rate requirement (critical for 4D)
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
write('line3','✓ 4D hCAcoNH setup complete!')
write('line3','')
write('line3','Key parameters set:')
write('line3','  Pathway: H(800μs)→CA → [CA-CO mix] → CO(10ms)→NH(300μs)→H')
write('line3','  Spectral widths: 1H(12kHz), 15N(2.5kHz), 13CO(6kHz), 13CA(12kHz)')
if (strstr($seqname,'_S') > 0) then
    write('line3','  NUS matrix: %d x %d x %d → %d points (%.1f%%)',nimax,ni2max,ni3max,nrows,100.0*nrows/(nimax*ni2max*ni3max))
else
    write('line3','  Matrix: %d x %d x %d points',ni,ni2,ni3)
endif
write('line3','  Evolution limits: 15N(%.0fms), 13CO(%.0fms), 13CA(%.0fms)',d2max*1000,d3max*1000,d4max*1000)
write('line3','  Estimated duty cycle: %.1f%%',$duty_cycle*100)
write('line3','  Estimated time: %.1f hours',$expt_time)
write('line3','')
write('line3','4D Optimization checklist:')
write('line3','□ Calibrate all pulse widths precisely')
write('line3','□ Optimize H→CA and CO→NH cross-polarizations')
write('line3','□ Test CA-CO mixing efficiency (SPCN parameters)')
write('line3','□ Verify 13C labeling pattern (CA and CO)') 
write('line3','□ Ensure stable high-speed spinning (≥12 kHz)')
write('line3','□ Set conservative d1 for quantitative transfers')
if (strstr($seqname,'_S') > 0) then
    write('line3','□ Generate optimized 4D NUS schedule')
    write('line3','□ Enable NUS: SPARSE=\'y\'')
    write('line3','□ Validate NUS parameters: nus_utils(\'validate\')')
else
    write('line3','⚠ CRITICAL: Load NUS-enabled sequence for practical 4D!')
endif
write('line3','□ Test with 2D/3D analogs first')
write('line3','□ Plan for multi-day acquisition')
write('line3','')

// Set parameter focus for 4D
if (strstr($seqname,'_S') > 0) then
    dg('4D_MIX')
else
    dg('cpHY')
endif