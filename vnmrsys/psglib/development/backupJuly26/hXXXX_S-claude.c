/*  hXXXX_S.c - A 4D sequence to provide XXXX homonuclear correlation with RAD, SPC5, POSTC7 or RFDR mixing
    LJS and MT 08/18/10   
    CMR 3/10/12: add soft pulse options in each indirect dimension
    CMR 3/14/12: build hXXhhX from hXXX version with soft pulses
    JMC 07/02/15: added PAR mixing option to both dimensions
    DWP 12/14/15: made into NUS version (see comments containing 'NUS' for additions)
    CGB ~March 2025 + July 8 2025: constructed 4D code from hXXX_S
    CMR 7/9/25: (1) added various duty cycle protections (also in setup macro); (2) rewrote full phase cycle
    SIMPLIFIED: Using unified biosolidnus.h header for consistent NUS implementation
*/

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"
#include "biosolidnus.h"  /* Unified NUS header */

// Define Values for Phasetables CMR 7/9/25 starting over from first principles
static int table1[8]   = {0,2,0,2,0,2,0,2};                           // phH90: spin temp alternation essential 
static int table2[8]   = {1,1,1,1,1,1,1,1};                           // phHhx
static int table3[8]   = {0,0,1,1,2,2,3,3};                           // phXhx
static int table41[16] = {3,3,0,0,1,1,2,2,3,3,0,0,1,1,2,2};           // phXmix5  storage pulse A dim  = phXhx + 1
static int table42[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};           // phXmix6  readout pulse A dim  = readout to X
static int table4[16]  = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};           // phXmix1  storage pulse Z dim  = phXmix6 + 2
static int table5[16]  = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};           // phXmix2  readout pulse Z dim  = readout to X
static int table10[16] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};	        // phXmix3  storage pulse Y dim
static int table11[16] = {1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0};           // phXmix4  readout pulse Y dim  = with phH90 spin temp, determines phRec
static int table6[16]  = {0,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1};           // phRec

// Soft pulse phase tables (CMR 7/9/25: soft pulse options will not work properly at this stage)
static int table7[8] =  {0,0,0,0,0,0,0,0};                           // phXspc5
static int table8[8] =  {0,1,0,1,0,1,0,1};                           // phXspc5ref
static int table9[16]=  {3,1,0,2,1,3,2,0,3,1,0,2,1,3,2,0};           // phXmix2dq;
static int table15[8] = {0,1,0,1,3,2,3,2};                          // phXsoft 
static int table16[8] = {0,0,1,1,3,3,2,2};                          // phXhard 
static int table21[8] = {0,2,3,1,2,0,1,3};                          // phXhx for softpul='y'
static int table17[8] = {0,0,0,0,0,0,0,0};                          // phYhard 
static int table18[16]= {1,0,1,0,2,3,2,3,0,1,0,1,3,2,3,2};          // phXsoft2 
static int table19[8] = {1,1,0,0,2,2,3,3};                          // phXhard2 
static int table22[16]= {3,1,2,0,1,3,0,2,1,3,0,2,3,1,2,0};          // phXmix2 for softpul2='y';
static int table20[8] = {0,0,0,0,0,0,0,0};                          // phYhard2 
static int table31[4] = {0,0,0,0};                                  // phCompX1
static int table32[4] = {1,1,1,1};                                  // phCompX2
static int table33[4] = {0,0,0,0};                                  // phCompY1
static int table34[4] = {1,1,1,1};                                  // phCompY2

static int table35[8] = {0,0,0,0,0,0,0,0};                          // phXpar
static int table36[16] ={1,1,3,3,3,3,1,1,1,1,3,3,3,3,1,1};          // ph90Xzf1
static int table37[16] ={3,3,1,1,1,1,3,3,3,3,1,1,1,1,3,3};          // ph90Xzf2;

// Added for hXXXX_S
static int table47[16] ={3,3,1,1,1,1,3,3,3,3,1,1,1,1,3,3};           // ph90Xzf3;
static int table43[4] = {0,0,0,0};                                   // phCompZ1
static int table44[4] = {1,1,1,1};                                   // phCompZ2

#define phH90 t1
#define phHhx t2
#define phXhx t3
#define phXmix1 t4
#define phXmix2 t5
#define phRec t6
#define phXspc5 t7
#define phXspc5ref t8
#define phXmix2dq t9
#define phXmix3 t10
#define phXmix4 t11
#define phCompX1 t31
#define phCompX2 t32
#define phCompY1 t33
#define phCompY2 t34
#define phXsoft t15
#define phXhard t16
#define phYhard t17
#define phXsoft2 t18
#define phXhard2 t19
#define phYhard2 t20
#define phXhx_soft t21
#define phXmix2_soft t22
#define phXpar t35
#define ph90Xzf1 t36
#define ph90Xzf2 t37

// Added for hXXXX_S
#define phXmix5 t41
#define phXmix6 t42
#define ph90Xzf3 t47
#define phCompZ1 t43
#define phCompZ2 t44

void pulsesequence() {

    // Check for phase,phase2,phase3 array bug early
    check_array();

    // Define Variables and Objects and Get Parameter Values
    double duty;
    double tRF;

    // NUS Implementation - MUCH SIMPLER!
    if (NUS_ACTIVE()) {
        if (NUS_INIT(4) != 0) {  // 4D experiment
            psg_abort(1);
        }
        if (NUS_CALC_DELAYS() != 0) {
            psg_abort(1);
        }
    }

    // Cross-polarization setup
    CP hx = getcp("HX",0.0,0.0,0,1);
    strcpy(hx.fr,"dec");
    strcpy(hx.to,"obs");
    putCmd("frHX='dec'\n");
    putCmd("toHX='obs'\n");

    // Mixing sequence parameters
    char mMix[MAXSTR];
    char mMix1[MAXSTR];
    char mMix2[MAXSTR];
    getstr("mMix",mMix);   
    getstr("mMix1",mMix1);   
    getstr("mMix2",mMix2);

    // Soft pulse options
    char softpul[MAXSTR];
    getstr("softpul",softpul);
    char softpul2[MAXSTR];
    getstr("softpul2",softpul2);

    // Recoupling sequences
    MPSEQ spc5 = getspc5("spc5X",0,0.0,0.0,0,1);
    MPSEQ spc5ref = getspc5("spc5X",spc5.iSuper,spc5.phAccum,spc5.phInt,1,1); 
    strcpy(spc5.ch,"obs");
    putCmd("chXspc5='obs'\n");

    MPSEQ c7 = getpostc7("c7X",0,0.0,0.0,0,1);
    MPSEQ c7ref = getpostc7("c7X",c7.iSuper,c7.phAccum,c7.phInt,1,1);
    strcpy(c7.ch,"obs");
    putCmd("chXc7='obs'\n");

    MPSEQ rfdr = getrfdrxy8("rfdrX",0,0.0,0.0,0,1); 
    strcpy(rfdr.ch,"obs");
    putCmd("chXrfdr='obs'\n");

    MPSEQ spcn = getspcn("spcnX",0,0.0,0.0,0,1);
    MPSEQ spcnref = getspcn("spcnX",spcn.iSuper,spcn.phAccum,spcn.phInt,1,1); 
    strcpy(spcn.ch,"obs");
    putCmd("chXspcn='obs'\n");

    int NXspcn = (int)getval("NXspcn"); 
    int qXspcn = (int)getval("qXspcn"); 

    if (qXspcn % NXspcn != 0) {
        printf("ABORT: qXspcn should be a multiple of NXspcn\n");
        psg_abort(1);
    }

    PARIS paris = getparis("H");
    DSEQ dec = getdseq("H");

    double pwX90 = getval("pwX90");
    double pwY90 = getval("pwY90");

    // Flags for recoupling sequences
    char spc5_flag[MAXSTR];
    getstr("spc5_flag",spc5_flag);  //1 is normalmixing 2 is dqf 
    char c7_flag[MAXSTR];
    getstr("c7_flag",c7_flag);  //1 is normalmixing 2 is dqf 
    char dqf_flag[MAXSTR];
    getstr("dqf_flag",dqf_flag);  //1 is normalmixing 2 is dqf 

    PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);
    PBOXPULSE shp2 = getpboxpulse("shp2X",0,1);

    // Set Mixing Periods to N Rotor Cycles 
    double taur, tXmix, tXmix1, tXmix2, srate;
    tXmix2 = getval("tXmix2");
    tXmix1 = getval("tXmix1"); 
    tXmix = getval("tXmix"); 
    srate = getval("srate");
    
    if (srate >= 500.0) {
        taur = roundoff((1.0/srate), 0.125e-6);
    } else {
        printf("ABORT: Spin Rate (srate) must be greater than 500\n");
        psg_abort(1);
    }
    
    tXmix = roundoff(tXmix,taur);
    tXmix1 = roundoff(tXmix1,taur);
    tXmix2 = roundoff(tXmix2,taur);

    tRF = getval("tRF");

    // NUS Safety Check
    if (NUS_ACTIVE()) {
        if (NUS_SAFETY_CHECK(tRF, "n", 0.0) != 0) {
            psg_abort(1);
        }
    }

    // Enhanced Dutycycle Protection CMR 7/9/25
    if (!strcmp(mMix, "rad")) {
        duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 + d3 + d4 + tRF + 
               (getval("aHmix")/4095) * (getval("aHmix")/4095) * (tXmix + tXmix1 + tXmix2) +
               2.0*getval("pwX90") + getval("ad") + getval("rd") + at;
        duty = duty/(duty + d1 + tXmix + tXmix1 + tXmix2 + 4.0e-6);
    } else if (!strcmp(mMix, "c7")) {
        duty = 4.0e-6 + getval("pwH90") + getval("tHX") + 2.0*getval("tZF") + c7.t + d3 + d4 + tRF + 
               d2+ tXmix1 + c7ref.t + getval("ad") + getval("rd") + at;
        duty = duty/(duty + d1 + 4.0e-6 + 2.0*getval("tZF"));
    } else if (!strcmp(mMix, "spc5")) {
        duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 + d3 + d4 + tRF +
               2.0*getval("pwX90") + tXmix1 + getval("ad") + getval("rd") + at;
        duty = duty/(duty + d1 + 4.0e-6);
    }

    if (duty > 0.05) {
        printf("ABORT: Duty cycle %.1f%% > 5%%. Check d1, d2, d3, d4, tRF, at, tXmix parameters.\n", duty*100);
        psg_abort(1);
    } else {
        printf("Duty cycle %.1f%% < 5%%. Safe (calculated for first increment only).\n", duty*100);
    } 

    // Safety checks for evolution periods
    if (d2 > 10e-3) {
        printf("ABORT: d2 = %.6f s which is > 10 ms. Too long!\n", d2);
        psg_abort(1);
    } 
    if (d3 > 10e-3) {
        printf("ABORT: d3 = %.6f s which is > 10 ms. Too long!\n", d3);
        psg_abort(1);
    }
    if (d4 > 10e-3) {
        printf("ABORT: d4 = %.6f s which is > 10 ms. Too long!\n", d4);
        psg_abort(1);
    }

    // Array validation for 4D experiment
    char arraycheck[MAXSTR];
    getstr("array",arraycheck);
    printf("array=%s\n",arraycheck);
    
    if (!strcmp(arraycheck,"phase,phase2,phase3") || !strcmp(arraycheck,"phase,phase3,phase2") || 
        !strcmp(arraycheck,"phase2,phase3,phase") || !strcmp(arraycheck,"phase2,phase,phase3") || 
        !strcmp(arraycheck,"phase3,phase,phase2")) {
        printf("ABORT: change array to 'phase3,phase2,phase' for 4D experiment\n");
        psg_abort(1);
    }
    
    if (!strcmp(arraycheck,"phase3,phase2,phase")) {
        int ni = getval("ni");
        int ni2 = getval("ni2");
        int ni3 = getval("ni3");
        int ndim = getval("ndim");
        
        if ((ni==0) || (ni2==0) || (ni3==0)) {
            printf("ABORT: all ni values must be non-zero for 4D experiment\n");
            psg_abort(1);
        }
        if (ndim != 4) {
            printf("ABORT: ndim must be = 4 for 4D experiment\n");
            psg_abort(1);
        }
    }

    // Create Phasetables
    settable(phH90,8,table1);
    settable(phHhx,8,table2);
    settable(phXmix1,16,table4);
    settable(phXmix2,16,table5);
    settable(phRec,16,table6);
    settable(phXspc5,8,table7);
    settable(phXmix3,16,table10);
    settable(phXmix4,16,table11);
    settable(phXmix5,16,table41);
    settable(phXmix6,16,table42);

    settable(phCompX1,4,table31);
    settable(phCompX2,4,table32);
    settable(phCompY1,4,table33);
    settable(phCompY2,4,table34); 
    settable(phCompZ1,4,table43);
    settable(phCompZ2,4,table44);

    settable(phXpar, 8, table35); 
    settable(ph90Xzf1, 16, table36); 
    settable(ph90Xzf2, 16, table37); 
    settable(ph90Xzf3, 16, table47);

    // Soft pulse phase tables
    if (!strcmp(softpul, "n")) {
        settable(phXhx,8,table3);
    }
    if (!strcmp(softpul, "y")) {  
        settable(phXsoft,8,table15);
        settable(phXhard,8,table16);
        settable(phYhard,8,table17);
        settable(phXhx,8,table21);
    }
    if (!strcmp(softpul2, "y")) {  
        settable(phXsoft2,16,table18);
        settable(phXhard2,8,table19);
        settable(phYhard2,8,table20);
    }

    // DQF phase tables
    if ((!strcmp(spc5_flag,"2") && !strcmp(mMix, "spc5")) || 
        (!strcmp(c7_flag,"2") && !strcmp(mMix, "c7")) || 
        (!strcmp(dqf_flag,"2") && !strcmp(mMix, "spcn"))) {
        settable(phXspc5ref,8,table8);
        settable(phXmix2dq,16,table9);
    } else {
        settable(phXspc5ref,8,table7);
        if (!strcmp(softpul2, "y")) {
            settable(phXmix2dq,16,table22);
        } else {
            settable(phXmix2dq,16,table5);
        }
    }

    // Set tPAR to n * pwX360
    double tPAR = getval("tPAR");
    tPAR = roundoff(tPAR,4.0*getval("pwX90")*4095/getval("aXpar"));

    // Phase cycling for indirect dimensions
    int id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);
        tsadd(phXmix3,2*id2_,4);
    }
    if (phase1 == 2) {tsadd(phXmix3,3,4);}

    int id3_ = (int) (d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,2*id3_,4);
        tsadd(phXmix1,2*id3_,4);
    }  
    if (phase2 == 2) {tsadd(phXmix1,3,4);}

    int id4_ = (int) (d4 * getval("sw3") + 0.1);
    if ((phase3 == 1) || (phase3 == 2)) {
        tsadd(phRec,2*id4_,4);
        tsadd(phXmix5,2*id4_,4);
    }  
    if (phase3 == 2) {tsadd(phXmix5,3,4);}

    setreceiver(phRec);

    // Begin Pulse Sequence
    splineon(1);
    delay(2.0e-6);
    splineoff(1);

    txphase(phXhx); decphase(phH90);
    obspwrf(getval("aXhx")); decpwrf(getval("aH90"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    // H to X Cross Polarization
    decrgpulse(getval("pwH90"),phH90,0.0,0.0);
    decphase(phHhx);
    _cp_(hx,phHhx,phXhx);

    // F4 (d4/sw3/phXmix5) - A dimension of 4D - Indirect Period for X nucleus
    obspwrf(getval("aX90"));
    _dseqon(dec); 
    if (!strcmp(softpul, "n")) {
        if (d4 > 4.0*pwY90) {
            dec2pwrf(getval("aY90"));
            delay(d4/2.0-2.0*pwY90);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            dec2rgpulse(2.0*pwY90,phCompY2,0.0,0.0);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            delay(d4/2.0-2.0*pwY90);
        } else {
            delay(d4);
        }
    }
    if (!strcmp(softpul, "y")) {
        delay(d4/2.0);
        _pboxpulse(shp1,phXsoft);
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));
        sim3pulse(2.0*pwX90,0.0,2.0*pwY90,phXhard,zero,phYhard,0.0,0.0);
        delay(d4/2.0);
    }
    _dseqoff(dec);

    // First X-X Mixing Period
    if (!strcmp(mMix, "rad")) {
        decpwrf(getval("aHmix"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix5,0.0,0.0);
        delay(tXmix);
        rgpulse(getval("pwX90"),phXmix6,0.0,0.0);
        decoff();
    }

    // F3 (d3/sw2/phXmix1) - Z dimension - Indirect Period for X nucleus
    obspwrf(getval("aX90"));
    _dseqon(dec); 
    if (!strcmp(softpul, "n")) {
        if (d3 > 4.0*pwY90) {
            dec2pwrf(getval("aY90"));
            delay(d3/2.0-2.0*pwY90);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            dec2rgpulse(2.0*pwY90,phCompY2,0.0,0.0);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            delay(d3/2.0-2.0*pwY90);
        } else {
            delay(d3);
        }
    }
    if (!strcmp(softpul, "y")) {
        delay(d3/2.0);
        _pboxpulse(shp1,phXsoft);
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));
        sim3pulse(2.0*pwX90,0.0,2.0*pwY90,phXhard,zero,phYhard,0.0,0.0);
        delay(d3/2.0);
    }
    _dseqoff(dec);

    // Second X-X Mixing Period
    if (!strcmp(mMix, "rad")) {
        decpwrf(getval("aHmix"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        delay(tXmix1);
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
    } else if (!strcmp(mMix, "spc5")) {
        decpwrf(getval("aHZF"));
        decon();
        obspwrf(getval("aX90"));
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        obspwrf(getval("aXspc5"));
        xmtrphase(v1); txphase(phXmix1);
        delay(getval("tZF"));
        decoff();

        decpwrf(getval("aHmixspc5"));
        decon();
        _mpseq(spc5, phXspc5);
        if (!strcmp(spc5_flag,"2")) {
            xmtrphase(v2); txphase(phXmix2);
            _mpseq(spc5ref, phXspc5ref);
        }
        decoff();
        
        decpwrf(getval("aHZF"));
        decon();
        obspwrf(getval("aX90"));
        xmtrphase(zero); txphase(phXmix2);
        delay(getval("tZF"));
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
    } else if (!strcmp(mMix, "rfdr")) {
        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decpwrf(getval("aHZF"));
        delay(getval("tZF"));
        decpwrf(getval("aHrfdr"));
        _mpseq(rfdr,0.0);
        decpwrf(getval("aHZF"));
        delay(getval("tZF"));
        obspwrf(getval("aX90"));
        decpwrf(getval("aH90"));
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
    }

    // F2 (d2/sw1/phXmix3) - Y dimension - Indirect Period for X nucleus
    obspwrf(getval("aX90"));
    _dseqon(dec); 
    if (!strcmp(softpul2, "n")) {
        if (d2 > 4.0*pwY90) {
            dec2pwrf(getval("aY90"));
            delay(d2/2.0-2.0*pwY90);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            dec2rgpulse(2.0*pwY90,phCompY2,0.0,0.0);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            delay(d2/2.0-2.0*pwY90);
        } else {
            delay(d2);
        }
    }
    if (!strcmp(softpul2, "y")) {
        delay(d2/2.0);
        _pboxpulse(shp2,phXsoft2);
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));
        sim3pulse(2.0*pwX90,0.0,2.0*pwY90,phXhard2,zero,phYhard2,0.0,0.0);
        delay(d2/2.0);
    }
    _dseqoff(dec);

    // Third X-X Mixing Period
    if (!strcmp(mMix1, "rad")) {
        decpwrf(getval("aHmix"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix3,0.0,0.0);
        delay(tXmix2);
        rgpulse(getval("pwX90"),phXmix4,0.0,0.0);
        decoff();
    }

    // Begin Acquisition
    _dseqon(dec);
    obsblank(); _blank34();
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec);

    decon();

    // tRF recovery delay
    if (tRF > d2+d3+d4) {
        delay(tRF-d2-d3-d4);
    }

    decoff();
    obsunblank(); decunblank(); _unblank34();
}
