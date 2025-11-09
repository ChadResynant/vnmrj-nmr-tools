/*hYXH.c - A sequence to obtain an YXH correlation with H detection using MISSISSIPPI,
                    HY cp, t1 Y evolution H dec
		    YX cp, t2 X evolution H dec
		    XH cp, potential HH rfdr mixing
		    H acq with X dec.

		    no Y-Y dream mixing yet but will add

                    from hXH.c  AJN 03/29/10
		    based on hYXH.c DHZ 02/07 for inova            
		    
		    NUS IMPLEMENTATION ADDED - Consistent with other sequences
		    FIXED: Removed ni/ni2 variable conflicts */

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"
#include "biosolidnus.h"

/* REMOVED NUS Pt.1 - Legacy static arrays that caused variable conflicts
   These are now handled internally by biosolidnus.h */

// Define Values for Phasetables

static int table1[4] = 	{0,0,2,2};              // phH90
static int table2[2] = 	{1,1};                  // phHhy
static int table3[2] = 	{0,0};                  // phYhy
static int table18[8] = {0,1,0,1,1,0,1,0};      // phYsoft
static int table19[8] = {0,0,0,0,1,1,1,1};      // phYhard
static int table4[2] = 	{3,3};                  // ph1Y90 store
static int table17[2] = {1,1};                  // ph2Y90 recover
static int table5[8] = 	{0,0,0,0,2,2,2,2};      // phYyx
static int table6[2] = 	{0,0};                  // phXyx
static int table7[2] = 	{1,1};                  // ph1X90 store
static int table8[2] = 	{1,3};                  // ph2X90 recover
static int table9[2] = 	{0,0};                  // phXxh
static int table10[2] = {1,1};                  // phHxh
static int table11[4] = {0,0,2,2};              // ph1H90
static int table22[8] = {0,0,0,0,2,2,2,2};      // ph1H90soft
static int table12[4] = {2,2,0,0};              // ph2H90
static int table21[8] = {2,2,2,2,0,0,0,0};      // ph2H90soft

static int table13[8] = {0,2,2,0,2,0,0,2};      // phRec
static int table20[8] = {0,0,2,2,2,2,0,0};      // phRecsoft
static int table14[4] = {0,0,0,0};              // phHrfdr H-H mixing
static int table16[2] = {0,1};                  // phHpxy  H presat pulses

static int table23[8] = {0,0,0,0,0,0,0,0};      // phYspc
static int table24[8] = {0,1,0,1,2,3,2,3};      // phYspcref
static int table25[4] = {0,0,0,0};              // phYrfdr

#define phH90       t1
#define phHhy       t2
#define phYhy       t3
#define ph1Y90      t4
#define phYyx       t5
#define phXyx       t6
#define ph1X90      t7
#define ph2X90      t8
#define phXxh       t9
#define phHxh       t10
#define ph1H90      t11
#define ph2H90      t12
#define phRec       t13
#define phHrfdr     t14
#define phHpxy      t16
#define ph2Y90      t17
#define phYsoft     t18
#define phYhard     t19
#define phRecsoft   t20
#define ph2H90soft  t21
#define ph1H90soft  t22
#define phYspc      t23
#define phYspcref   t24
#define phYrfdr     t24

void pulsesequence() {

    //AJN trying to prevent the backwards incrementing of d3 and d2 phases
    // check for phase,phase2 array bug
    
    check_array();

    // Define Variables and Objects and Get Parameter Values
    double duty;
    double tRF;

    double pwH90 = getval("pwH90");
    double pwX90 = getval("pwX90");
    double pwY90 = getval("pwY90");
    double pwZ90 = getval("pwZ90");
    
    /* REMOVED: Local ni and ni2 variables that caused conflicts
       These are protected PSG variables and should not be redeclared */

    double max_pw_XZ = (pwX90 > pwZ90 ? pwX90 : pwZ90);
    double max_pw_YZ = (pwY90 > pwZ90 ? pwY90 : pwZ90);
    double max_pw_XYZ = max_pw_YZ > pwX90 ? max_pw_YZ : pwX90;

    double tmd3 = getval("d3max") - getval("d3") - pwX90;
    if (tmd3 < 0.0) {
        tmd3 = 0.0;
    }

    double tmd2 = getval("d2max") - getval("d2") - pwX90;
    if (tmd2 < 0.0) {
        tmd2 = 0.0;
    }

    CP hy = getcp("HY",0.0,0.0,0,1);
    strcpy(hy.fr,"obs");
    strcpy(hy.to,"dec2");  // BDZ bug fixed here on 9-13-23: only "dec" was getting copied into "to" field.
    putCmd("frHY='obs'\n");
    putCmd("toHY='dec2'\n");

    CP yx = getcp("YX",0.0,0.0,0,1);
    strcpy(yx.fr,"dec2");  // BDZ bug fixed here on 9-13-23: only "dec" was getting copied into "fr" field.
    strcpy(yx.to,"dec");
    putCmd("frYX='dec2'\n");
    putCmd("toYX='dec'\n");

    CP xh = getcp("XH",0.0,0.0,0,1);
    strcpy(xh.fr,"dec");
    strcpy(xh.to,"obs");
    putCmd("frXH='dec'\n");
    putCmd("toXH='obs'\n");

    char softpul[MAXSTR];
    getstr("softpul",softpul);
    PBOXPULSE shp1 = getpboxpulse("shp1Y",0,1);

    MPSEQ pxy = getpxy("pxyH",0,0.0,0.0,0,1);
    strcpy(pxy.ch,"obs");
    putCmd("chHpxy='obs'\n");

    MPSEQ Hrfdr = getrfdrxy8("rfdrH",0,0.0,0.0,0,1);
    strcpy(Hrfdr.ch,"obs");
    putCmd("chHrfdr='obs'\n");

    MPSEQ Yrfdr = getrfdrxy8("rfdrY",0,0.0,0.0,0,1);
    strcpy(Yrfdr.ch,"dec2");  // BDZ bug fixed here on 9-13-23: only "dec" was getting copied into "ch" field.
    putCmd("chYrfdr='dec2'\n");

    MPSEQ Yspcn = getspcn("spcnY",0,0.0,0.0,0,1);
    MPSEQ Yspcnref = getspcn("spcnY",Yspcn.iSuper,Yspcn.phAccum,Yspcn.phInt,1,1);
    strcpy(Yspcn.ch,"dec2");  // BDZ bug fixed here on 9-13-23: only "dec" was getting copied tinto "ch" field.
    putCmd("chYspcn='dec2'\n");
    int NYspcn, qYspcn;
    NYspcn = (int)getval("NYspcn");
    qYspcn = (int)getval("qYspcn");
    char dqf_flag[MAXSTR];
    getstr("dqf_flag",dqf_flag);  // "1" is normal mixing "2" is dqf

    char mMix[MAXSTR];
    getstr("mMix",mMix);

    DSEQ dec = getdseq("H");        //For t1 X evolution
    DSEQ dec2 = getdseq("X");       // For acq H evolution
    DSEQ dec3 = getdseq("Y");
    DSEQ dec4 = getdseq("Z");

    tRF = getval("tRF");

    /* NUS Implementation - handles d2 and d3 calculation internally */
    if (NUS_ACTIVE()) {
        if (NUS_INIT(3) != 0) psg_abort(1);  // 3D experiment
        if (NUS_CALC_DELAYS() != 0) psg_abort(1);
        if (NUS_SAFETY_CHECK(tRF, "n", 0.0) != 0) psg_abort(1);
    }
    
    // Recalculate tmd2 and tmd3 after NUS adjustment
    tmd3 = getval("d3max") - d3 - pwX90;
    if (tmd3 < 0.0) {
        tmd3 = 0.0;
    }

    tmd2 = getval("d2max") - d2 - pwX90;
    if (tmd2 < 0.0) {
        tmd2 = 0.0;
    }

    // Dutycycle Protection
    // H-detected sequence: Low-power H decoupling during acquisition allows 15% duty cycle
    // See SAFETY_STANDARDS.md Section 6: Power-Dependent Duty Cycle Limits
    duty = 4.0e-6 + 3* getval("pwH90") + getval("tHY") + getval("tYX") + getval("tXH") +
           d2 + d3 + tmd2 + tmd3 + getval("ad") + getval("rd") + at;

    duty = duty/(duty + d1 + 4.0e-6);
    if (duty > 0.15) {
        printf("Duty cycle %.1f%% >15%%. Abort!\n", duty*100);
        psg_abort(1);
    }

    // Set Phase Tables

    settable(phH90,4,table1);
    settable(phHhy,2,table2);
    settable(phYhy,2,table3);
    settable(ph1Y90,2,table4);
    settable(phYyx,8,table5);
    settable(phXyx,2,table6);
    settable(ph1X90,2,table7);
    settable(ph2X90,2,table8);
    settable(phXxh,2,table9);
    settable(phHxh,2,table10);
    settable(ph1H90,4,table11);
    settable(ph2H90,4,table12);
    settable(phRec,8,table13);
    settable(phHrfdr,4,table14);
    settable(phHpxy,2,table16);
    settable(ph2Y90,2,table17);
    settable(phYsoft,8,table18);
    settable(phYhard,8,table19);
    settable(phRecsoft,8,table20);
    settable(ph2H90soft,8,table21);
    settable(ph1H90soft,8,table22);
    settable(phYspc,8,table23);
    settable(phYspcref,8,table24);
    settable(phYrfdr,4,table25);

    int id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);  /* invert the phases of the storage 90 and */
        tsadd(phRecsoft,2*id2_,4);
        tsadd(ph1X90,2*id2_,4); /* the receiver for FAD to displace the axial peaks */
    }
    if (phase1 == 2) { /* hypercomplex*/
        tsadd(ph1X90,3,4);
    }

    int id3_ = (int) (d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,2*id3_,4);  /* invert the phases of the storage and */
        tsadd(phRecsoft,2*id3_,4);
        tsadd(ph1Y90,2*id3_,4); /* the receiver for FAD to displace the axial peaks */
    }
    if (phase2 == 2) { /* hypercomplex*/
        tsadd(ph1Y90,3,4);
    }

    if (!strcmp(softpul, "y")) {
        setreceiver(phRecsoft);
    }
    else {
        setreceiver(phRec);
    }

    // Begin Sequence

    // Initial Homospoil Pulse

    status(B);
    hsdelay(getval("hst"));
    status(A);

    txphase(phH90); dec2phase(phYhy);
    obspwrf(getval("aH90")); dec2pwrf(getval("aYhy"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    // H to Y Cross Polarization

    rgpulse(getval("pwH90"),phH90,0.0,0.0);
    txphase(phHhy);
    _cp_(hy,phHhy,phYhy);
    dec2phase(ph1Y90);
    dec2pwrf(getval("aY90"));

    // Y F1 Period with H XiX Decoupling

    _dseqon(dec);

    decpwrf(getval("aX90"));


    if (!strcmp(softpul, "y")) {
        if (d3>2*getval("pwX90")) {
            delay(0.5*d3-2.0*getval("pwX90"));
            decrgpulse(2*getval("pwX90"),0,0,0);
        }
        else {
            delay(d3/2.0);
        }
        _dseqoff(dec);
        obspwrf(getval("aHshp1"));
        obsunblank(); xmtron();
        _pboxpulse(shp1,phYsoft);
        dec2pwrf(getval("aY90"));
        dec2rgpulse(2*getval("pwY90"),phYhard,0.0,0.0);
        xmtroff();
        _dseqon(dec);
        delay(d3/2.0);
    }

    if (!strcmp(softpul, "n")) {
        if (d3>0) {
            if (d3>2*getval("pwX90")) {
                delay(0.5*d3-getval("pwX90"));
                decrgpulse(2*getval("pwX90"),0,0.0,0.0);
                delay(0.5*d3 - getval("pwX90"));
            }
            else {
                delay(d3);
            }
        }
    }

    dec2rgpulse(getval("pwY90"),ph1Y90,0.0,0.0);
    dec2phase(ph2Y90);
    delay(tmd3);

    if (!strcmp(mMix, "spcn")) {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        obspwrf(getval("aHmixspcn"));
        xmtron();

        _mpseq(Yspcn, phYspc);
        if (!strcmp(dqf_flag,"2")) {
            _mpseq(Yspcnref, phYspcref);
        }
        dec2pwrf(getval("aY90"));
        xmtroff();

        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();
        _dseqon(dec);
    }
    else if (!strcmp(mMix, "rfdr")) {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        if (getval("qYrfdr")>0) {
            _mpseq(Yrfdr,phYrfdr);
        }
        dec2pwrf(getval("aY90"));

        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();
        _dseqon(dec);
    }


    _dseqoff(dec);
    delay(getval("hstconst"));

    decphase(phXyx);txphase(one);
    obspwrf(getval("aHyx"));
    obsunblank(); xmtron();

    dec2rgpulse(getval("pwY90"),ph2Y90,0.0,0.0);
    dec2unblank();dec2phase(phYyx);

    // Y to X Cross Polarization
    _cp_(yx,phYyx,phXyx);
    xmtroff();

    dec2pwrf(getval("aY90"));

    // X F2 Period with H XiX Decoupling

    _dseqon(dec);
    decphase(ph1X90);decpwrf(getval("aX90"));
    if (d2>0) {
        if (d2>2*getval("pwY90")) {
            delay(0.5*d2-getval("pwY90"));
            dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
            delay(0.5*d2 - getval("pwY90"));
        }
    }
    else {
        delay(d2);
    }

    decrgpulse(getval("pwX90"),ph1X90,0.0,0.0);
    txphase(phHxh);
    decunblank();
    delay(tmd2);

    _dseqoff(dec);

    // Homospoil Delay
    status(B);
    hsdelay(getval("hst"));
    status(A);
    delay(getval("hstconst"));

    // Saturation Pulses

    _mpseq(pxy,phHpxy);
    txphase(phHxh);

    // X to H Cross Polarization
    obspwrf(getval("aH90"));
    obsunblank(); xmtron();
    decrgpulse(getval("pwX90"),ph2X90,0.0,0.0);
    xmtroff();
    decphase(phXxh);
    _cp_(xh,phXxh,phHxh);

    // XY8 Mixing

    if (getval("qHrfdr")>0) {
        obspwrf(getval("aH90"));
        if(!strcmp(softpul,"y")) {
            rgpulse(getval("pwH90"),ph1H90soft,0.0,0.0);
        }
        else {
            rgpulse(getval("pwH90"),ph1H90,0.0,0.0);
        }

        _mpseq(Hrfdr,phHrfdr);

        obspwrf(getval("aH90"));
        if(!strcmp(softpul,"y")) {
            rgpulse(getval("pwH90"),ph2H90soft,0.0,0.0);
        }
        else {
            rgpulse(getval("pwH90"),ph2H90,0.0,0.0);
        }
    }

    // Begin X Decoupling

    status(C);
    _dseqon(dec2); _dseqon(dec3);

    // Begin H Acquisition

    obsblank(); _blank34();
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec2); _dseqoff(dec3);

    decon();

    if (tRF > d2+d3) {
        delay(tRF-d2-d3);
    }

    decoff();
    obsunblank(); decunblank(); _unblank34();
}
