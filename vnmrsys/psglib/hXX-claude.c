/*
*  hXX.c - A sequence to provide XX homonuclear correlation with the option of using multiple XX mixing schemes
*  J. Rapp 09/17/09, CMR test 11/4/09, LJS & MT 8/18/10
*  add suppress 13C 90 pulse after PAR. MT 3/21/11 
*  CMR 11/1/2018:  
*   1.  renamed hXX_dream.c to hXX.c.  This is the version of code that has been extensively used for most of 2018.
*   2.  Fixed soft pulse decoupling naming convention to be consistent with hX and hYXX and other 3D code.    
*  [USER] 07/23/2025: Updated to use biosolidmixing.h consolidated mixing library
*/

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"
#include "biosolidmixing_simple.h"

// Define Values for Phasetables

static int table1[16] = {1,1,1,1,3,3,3,3,
                         3,3,3,3,1,1,1,1};          // phH90
static int table2[8] =  {0,0,0,0,0,0,0,0};          // phHhx
static int table3[8] =  {0,2,0,2,2,0,2,0};          // phXhx

static int table22[16] ={1,3,1,3,1,3,1,3,
                         3,1,3,1,3,1,3,1};          // phX90
static int table23[16] ={1,1,1,1,3,3,3,3,
                         3,3,3,3,1,1,1,1};          // phX90_soft

static int table21[8] = {0,0,0,0,2,2,2,2};          // phXhx_soft
static int table18[16]= {0,1,0,1,0,1,0,1,
                         0,1,0,1,0,1,0,1};          // phXshp1

static int table4[16] = {3,3,1,1,1,1,3,3,
                         3,3,1,1,1,1,3,3};          // phXmix1  CMR 4/9/18 fix for RFDR phase cycle
static int table5[32] = {1,1,3,3,1,1,3,3,
                         1,1,3,3,1,1,3,3,
                         2,2,0,0,2,2,0,0,
                         2,2,0,0,2,2,0,0};          // phXmix2; CMR 4/9/18 fix for RFDR phase cycle

static int table9[32] =  {1,3,3,1,1,3,3,1,
                          1,3,3,1,1,3,3,1,
                          2,0,0,2,2,0,0,2,
                          2,0,0,2,2,0,0,2};         // phXmix2dqf;  CMR 4/11/18 fix for DQF phase cycle
static int table7[8] =  {0,0,0,0,0,0,0,0};          // phXspc5
static int table8[8] =  {0,1,0,1,0,1,0,1};          // phXspc5ref

// none of the phases during mixing should be cycled from scan to scan (unless doing DQF) CMR 4/15/15
static int table10[8] = {0,2,0,2,2,0,2,0};          // phHpar needs NMR test
static int table14[16] ={0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0};          // phXr2t

static int table12[8] = {0,2,0,2,2,0,2,0};          // phXdream

static int table11[8] = {0,0,0,0,0,0,0,0};          // phXpar

static int table15[16] ={1,1,3,3,3,3,1,1,
                         1,1,3,3,3,3,1,1};          // ph90Xdream1
static int table16[16] ={3,3,1,1,1,1,3,3,
                         3,3,1,1,1,1,3,3};          // ph90Xdream2;

static int table6[32] = {0,2,0,2,2,0,2,0,
                         2,0,2,0,0,2,0,2,
                         1,3,1,3,3,1,3,1,
                         3,1,3,1,1,3,1,3};          // phRec;
static int table19[32]= {0,2,0,2,2,0,2,0,
                         2,0,2,0,0,2,0,2,
                         1,3,1,3,3,1,3,1,
                         3,1,3,1,1,3,1,3};          // phRecsoft;
static int table20[16]= {0,2,0,2,2,0,2,0,
                         2,0,2,0,0,2,0,2};          // phRecsoftnomix;
static int table24[4] = {0,0,0,0};                  // phCompY1
static int table25[4] = {1,1,1,1};                  // phCompY2


#define phH90 t1
#define phHhx t2
#define phXhx t3
#define phXhx_soft t21
#define phXmix1 t4
#define phXmix2 t5
#define phRec t6
#define phXspc5 t7
#define phXspc5ref t8
#define phXmix2dqf t9
#define phXpar t11
#define phXdream t12
#define phXr2t t14

#define ph90Xdream1 t15
#define ph90Xdream2 t16

#define phXshp1 t18
#define phRecsoft t19
#define phRecsoftnomix t20
#define phX90 t22
#define phX90_soft t23
#define phCompY1 t24
#define phCompY2 t25


int id2_,id3_, xsel;

void pulsesequence() {

    check_array();

    // Define Variables and Objects and Get Parameter Values

    double d2_, duty;

    CP hx = getcp("HX",0.0,0.0,0,1);
    strcpy(hx.fr,"dec");
    strcpy(hx.to,"obs");
    putCmd("frHX='dec'\n");
    putCmd("toHX='obs'\n");

    char mMix[MAXSTR];
    getstr("mMix",mMix);
    
    char echo[MAXSTR];
    getstr("echo",echo);
    double tECHO = getval("tECHO");
    double tRF = getval("tRF");
    if (strcmp(echo,"n") != 0) {
        tRF = tRF - tECHO;
    }
    double pwXshp1 = getval("pwXshp1");
    if (tECHO < 0.0) {
        tECHO = 0.0;
    }
    PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);
    double pwY90 = getval("pwY90");

    DSEQ dec;
    dec = getdseq("H");
    DSEQ dec2;
    dec2 = getdseq("Y");    // For acq H evolution
    DSEQ dec_soft;
    dec_soft = getdseq("Hsoft");

    char cp[MAXSTR];
    getstr("cp",cp);

    char cpboth[MAXSTR];
    getstr("cpboth",cpboth);  //1 is normalmixing 2 is dqf 

    // Initialize mixing parameters using consolidated library
    double tXmix = getval("tXmix");
    MixingParams mixing = init_mixing_params(mMix, tXmix);
    
    // Setup sequences for X nucleus
    setup_mixing_sequences(&mixing, "X");
    
    // Set phase table references
    mixing.phMix1 = phXmix1;
    mixing.phMix2 = phXmix2;
    mixing.phMix2dqf = phXmix2dqf;
    mixing.phSpc5 = phXspc5;
    mixing.phSpc5ref = phXspc5ref;
    
    // Validate parameters
    validate_mixing_parameters(&mixing);

    // Set Mixing Period to N Rotor Cycles 
    double taur=1;
    double srate = getval("srate");
    if (srate >= 500.0) {
        taur = roundoff((1.0/srate), 0.125e-6);
    }
    else {
        printf("ABORT: Spin Rate (srate) must be greater than 500\n");
        psg_abort(1);
    }
    tXmix = roundoff(tXmix,taur);

    // Set tPAR to n * pwX360
    double tPAR;
    tPAR = getval("tPAR");
    tPAR = roundoff(tPAR,4.0*getval("pwX90")*4095/getval("aXpar"));

    // Calculate duty cycle using consolidated function
    duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 + getval("ad") + getval("rd") + at;
    
    // Dutycycle Protection
    if (strcmp(echo,"n") != 0) {
        duty += getval("tECHO");
    }
    
    // Only calculate mixing duty cycle if mixing is enabled
    if (strcmp(mMix, "n") != 0) {
        duty = calculate_mixing_duty_cycle(&mixing, duty, duty + getval("d1") + 4.0e-6);
    } else {
        duty = duty/(duty + getval("d1") + 4.0e-6);
    }
    
    // 5% duty cycle limit for C-detected sequences (high-power decoupling on X channel)
    // NOTE: Future enhancement should make duty cycle power-dependent:
    //   - High-power decoupling (>50 kHz): 5% limit (current conservative assumption)
    //   - Medium-power decoupling (20-50 kHz): could allow 7-10%
    //   - Low-power decoupling (<20 kHz): could allow 10-15%
    //   This requires integrating decoupling power (aH, aY) into duty cycle calculation
    if (duty > 0.05) {
        abort_message("Duty cycle %.1f%% >5%%. Abort!\n", duty*100);
    }

    // Create Phasetables
    settable(phH90,16,table1);
    settable(phHhx,8,table2);
    settable(phXhx,8,table3);
    settable(phXhx_soft,8,table21);
    settable(phXmix1,16,table4);
    settable(phXmix2,32,table5);
    settable(phRec,32,table6);
    settable(phRecsoft,32,table19);
    settable(phRecsoftnomix,16,table20);
    settable(phXspc5,8,table7);
    settable(phXspc5ref,8,table8);
    settable(phXmix2dqf,32,table9);
    settable(phXpar,8,table11);
    settable(phXdream,8,table12);
    settable(phXr2t,16,table14);
    settable(ph90Xdream1,16,table15);
    settable(ph90Xdream2,16,table16);
    settable(phXshp1,16,table18);
    settable(phX90,16,table22);
    settable(phX90_soft,16,table23);
    settable(phCompY1,4,table24);
    settable(phCompY2,4,table25);


    id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4); /* invert the phases of the CP pulse and */
        tsadd(phRecsoft,2*id2_,4); /* invert the phases of the CP pulse and */
        tsadd(phRecsoftnomix,2*id2_,4); /* ditto */
        tsadd(phXmix1,2*id2_,4); /* the receiver for FAD to displace the axial peaks */
    }

    if (phase1 == 2) {
        tsadd(phXmix1,3,4);  // CMR 8/7/2018 removed phXpar and phXdream from here; all States-TPPI selection with phXmix1
    }

    if (strcmp(echo,"n") == 0 && strcmp(mMix,"n") != 0) {
        setreceiver(phRec);
    }
    else if (strcmp(echo,"soft") == 0 && strcmp(mMix,"n") != 0) {
        setreceiver(phRecsoft);
    }
    else if (strcmp(echo,"soft") != 0 && strcmp(mMix,"n") == 0) {
        setreceiver(phRecsoftnomix);
    }
    else {
        abort_message("Must have soft pulse (echo == 'soft') or mixing (mMix != 'n')");
    }

    // Begin Sequence
    //
    // External trigger on channel 1 number 1
    splineon(1);
    delay(2.0e-6);
    splineoff(1);

    if (strcmp(echo,"n") == 0) {
        txphase(phXhx);
    }
    else {
        txphase(phXhx_soft);
    }
    
    decphase(phH90);
    obspwrf(getval("aXhx")); decpwrf(getval("aH90"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    // H to X Cross Polarization
    
    if (strcmp(cp,"y") == 0) {
        if (strcmp(cpboth,"y") == 0)
        {
            obspwrf(getval("aX90"));
            if (strcmp(echo,"n") == 0) {
                rgpulse(getval("pwX90"), phX90, 0.0, 0.0);
            }
            else {
                rgpulse(getval("pwX90"), phX90_soft, 0.0, 0.0);
            }      
        }
        decrgpulse(getval("pwH90"),phH90,0.0,0.0);
        decphase(phHhx);
        if (strcmp(echo,"n") == 0) {
            _cp_(hx,phHhx,phXhx);
        }
        else {
            _cp_(hx,phHhx,phXhx_soft);
        }
    }
    else if (strcmp(cp,"n") == 0) {
        obspwrf(getval("aX90"));
        if (strcmp(echo,"n") == 0) {
            rgpulse(getval("pwX90"), phX90, 0.0, 0.0);
        }
        else {
            rgpulse(getval("pwX90"), phX90_soft, 0.0, 0.0);
        }
    }
    else {
        abort_message("Must set cp to y, n, or both!");
    }

    // F2 Indirect Period for X
    obspwrf(getval("aX90"));
    _dseqon(dec);
    if (d2 > 4.0*pwY90) 
    {
        dec2pwrf(getval("aY90"));
        delay(d2/2.0-2.0*pwY90);
        dec2rgpulse(pwY90,phCompY1,0.0,0.0);
        dec2rgpulse(2.0*pwY90,phCompY2,0.0,0.0);
        dec2rgpulse(pwY90,phCompY1,0.0,0.0);
        delay(d2/2.0-2.0*pwY90);
    }
    else {
        delay(d2);
    }
    _dseqoff(dec);

    if (strcmp(echo,"soft") == 0) {
       _dseqon(dec_soft);
       delay(tECHO/2.0);
       _pboxpulse(shp1,phXshp1);
       delay(tECHO/2.0);
       _dseqoff(dec_soft);
    }
// CMR 11/1/2018:  commented back IN the _dseqoff(dec), _dseqon(dec_soft), _dseqoff(dec_soft) lines above

    // Execute mixing sequence using consolidated library
    if (strcmp(mMix, "n") != 0) {
        execute_mixing_sequence(&mixing);
    }

    // Begin Acquisition
    _dseqon(dec);  _dseqon(dec2); 
//    _dseqon(dec3);
    d2_ = (ni-1)/getval("sw1") - d2;
    if (d2_<0) {
        d2_ = 0.0;
    }

    obsblank(); _blank34();
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec); _dseqoff(dec2);
//    _dseqoff(dec3);


    _dseqon(dec);

    if (tRF <= (ni-1)/getval("sw1")) {
        d2_ = (ni-1)/getval("sw1") - d2;
        if (d2_<0) {
            d2_ = 0.0;
            }
        delay(d2_);
    }
    else {
        delay(tRF-d2);
    }

    _dseqoff(dec);
    obsunblank(); decunblank(); _unblank34();
}
