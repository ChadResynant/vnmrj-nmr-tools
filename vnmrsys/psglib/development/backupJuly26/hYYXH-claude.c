/*hYYXH_4D.c - A 4D sequence to obtain YYXH correlation with H detection
                    HY cp, t1 Y evolution (F4) H dec
                    Y-Y mixing 
                    t2 Y evolution (F3) H dec  
		    YX cp, t3 X evolution (F2) H dec
		    XH cp, potential HH rfdr mixing
		    H acq (F1) with X,Y dec.

                    Based on hYyXH.c and following hXYXX_4D conventions
                    4D NUS implementation included */

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"
#include "biosolidnus.h"  /* Include NUS header */

// Define Values for Phasetables

static int table1[8] = 	{0,0,0,0,2,2,2,2};          // phH90
static int table2[4] = 	{1,1,1,1};                  // phHhy
static int table3[4] = 	{0,0,0,0};                  // phYhy
static int table4[4] = 	{3,3,3,3};                  // ph1Y90 store F4
static int table5[4] = 	{1,1,3,3};                  // ph2Y90 recover F4
static int table6[8] = 	{0,0,0,0,2,2,2,2};          // phYmix1 Y-Y mixing
static int table7[8] = 	{1,1,1,1,1,1,1,1};          // phYmix2 Y-Y mixing

static int table8[4] = 	{3,3,3,3};                  // ph3Y90 store F3
static int table9[4] = 	{1,1,3,3};                  // ph4Y90 recover F3
static int table10[8] = {0,0,0,0,2,2,2,2};          // phYyx Y to X CP
static int table11[4] = {0,0,0,0};                  // phXyx Y to X CP

static int table12[4] = {1,1,1,1};                  // ph1X90 store F2
static int table13[4] = {1,1,3,3};                  // ph2X90 recover F2
static int table14[4] = {0,0,0,0};                  // phXxh X to H CP
static int table15[4] = {1,1,1,1};                  // phHxh X to H CP

static int table16[8] = {0,0,2,2,2,2,0,0};          // ph1H90 H-H mixing
static int table17[8] = {2,2,0,0,0,0,2,2};          // ph2H90 H-H mixing
static int table18[4] = {0,0,0,0};                  // phHrfdr H-H mixing
static int table19[4] = {0,1,0,1};                  // phHpxy H presat

// Soft pulse phase tables
static int table20[8] = {0,1,0,1,1,0,1,0};          // phYsoft1 F4
static int table21[8] = {0,0,0,0,1,1,1,1};          // phYhard1 F4
static int table22[8] = {0,1,0,1,1,0,1,0};          // phYsoft2 F3  
static int table23[8] = {0,0,0,0,1,1,1,1};          // phYhard2 F3

// Y-Y mixing sequences
static int table24[8] = {0,0,0,0,0,0,0,0};          // phYspc1
static int table25[8] = {0,1,0,1,2,3,2,3};          // phYspcref1
static int table26[8] = {0,0,0,0,0,0,0,0};          // phYspc2
static int table27[8] = {0,1,0,1,2,3,2,3};          // phYspcref2
static int table28[4] = {0,0,0,0};                  // phYrfdr1
static int table29[4] = {0,0,0,0};                  // phYrfdr2

// Receiver phases
static int table30[16]= {0,2,2,0,2,0,0,2,2,0,0,2,0,2,2,0};    // phRec
static int table31[16]= {0,0,2,2,2,2,0,0,2,2,0,0,0,0,2,2};    // phRecsoft1
static int table32[16]= {2,2,0,0,0,0,2,2,0,0,2,2,2,2,0,0};    // phRecsoft2  
static int table33[16]= {2,0,0,2,0,2,2,0,0,2,2,0,2,0,0,2};    // phRecsoft12

#define phH90       t1
#define phHhy       t2
#define phYhy       t3
#define ph1Y90      t4
#define ph2Y90      t5
#define phYmix1     t6
#define phYmix2     t7
#define ph3Y90      t8
#define ph4Y90      t9
#define phYyx       t10
#define phXyx       t11
#define ph1X90      t12
#define ph2X90      t13
#define phXxh       t14
#define phHxh       t15
#define ph1H90      t16
#define ph2H90      t17
#define phHrfdr     t18
#define phHpxy      t19
#define phYsoft1    t20
#define phYhard1    t21
#define phYsoft2    t22
#define phYhard2    t23
#define phYspc1     t24
#define phYspcref1  t25
#define phYspc2     t26
#define phYspcref2  t27
#define phYrfdr1    t28
#define phYrfdr2    t29
#define phRec       t30
#define phRecsoft1  t31
#define phRecsoft2  t32
#define phRecsoft12 t33

void pulsesequence() {

    // Check for phase,phase2,phase3 array bug
    check_array();

    // Define Variables and Objects and Get Parameter Values
    double duty;
    double tRF;

    double pwH90 = getval("pwH90");
    double pwX90 = getval("pwX90");
    double pwY90 = getval("pwY90");
    double pwZ90 = getval("pwZ90");
    
    int ni = getval("ni");           /* used for NUS */ 
    int ni2 = getval("ni2");         /* used for NUS */ 
    int ni3 = getval("ni3");         /* used for NUS */ 

    // Initialize timing variables
    double tmd4 = getval("d4max") - getval("d4") - pwX90;
    if (tmd4 < 0.0) tmd4 = 0.0;

    double tmd3 = getval("d3max") - getval("d3") - pwX90;
    if (tmd3 < 0.0) tmd3 = 0.0;

    double tmd2 = getval("d2max") - getval("d2") - pwX90;
    if (tmd2 < 0.0) tmd2 = 0.0;

    // NUS Implementation for 4D
    if (NUS_ACTIVE()) {
        if (NUS_INIT(4) != 0) {  // 4D experiment
            psg_abort(1);
        }
        if (NUS_CALC_DELAYS() != 0) {
            psg_abort(1);
        }
        
        // Recalculate timing after NUS adjustment
        tmd4 = getval("d4max") - d4 - pwX90;
        if (tmd4 < 0.0) tmd4 = 0.0;
        
        tmd3 = getval("d3max") - d3 - pwX90;
        if (tmd3 < 0.0) tmd3 = 0.0;
        
        tmd2 = getval("d2max") - d2 - pwX90;
        if (tmd2 < 0.0) tmd2 = 0.0;
    }

    // Cross-polarization objects
    CP hy = getcp("HY",0.0,0.0,0,1);
    strcpy(hy.fr,"obs");
    strcpy(hy.to,"dec2");
    putCmd("frHY='obs'\n");
    putCmd("toHY='dec2'\n");

    CP yx = getcp("YX",0.0,0.0,0,1);
    strcpy(yx.fr,"dec2");
    strcpy(yx.to,"dec");
    putCmd("frYX='dec2'\n");
    putCmd("toYX='dec'\n");

    CP xh = getcp("XH",0.0,0.0,0,1);
    strcpy(xh.fr,"dec");
    strcpy(xh.to,"obs");
    putCmd("frXH='dec'\n");
    putCmd("toXH='obs'\n");

    // Pulse sequence objects
    char softpul[MAXSTR];
    getstr("softpul",softpul);
    char softpul2[MAXSTR];
    getstr("softpul2",softpul2);
    
    PBOXPULSE shp1 = getpboxpulse("shp1Y",0,1);
    PBOXPULSE shp2 = getpboxpulse("shp2Y",0,1);

    MPSEQ pxy = getpxy("pxyH",0,0.0,0.0,0,1);
    strcpy(pxy.ch,"obs");
    putCmd("chHpxy='obs'\n");

    MPSEQ Hrfdr = getrfdrxy8("rfdrH",0,0.0,0.0,0,1);
    strcpy(Hrfdr.ch,"obs");
    putCmd("chHrfdr='obs'\n");

    MPSEQ Yrfdr1 = getrfdrxy8("rfdrY1",0,0.0,0.0,0,1);
    strcpy(Yrfdr1.ch,"dec2");
    putCmd("chYrfdr1='dec2'\n");

    MPSEQ Yrfdr2 = getrfdrxy8("rfdrY2",0,0.0,0.0,0,1);
    strcpy(Yrfdr2.ch,"dec2");
    putCmd("chYrfdr2='dec2'\n");

    MPSEQ Yspcn1 = getspcn("spcnY1",0,0.0,0.0,0,1);
    MPSEQ Yspcnref1 = getspcn("spcnY1",Yspcn1.iSuper,Yspcn1.phAccum,Yspcn1.phInt,1,1);
    strcpy(Yspcn1.ch,"dec2");
    putCmd("chYspcn1='dec2'\n");

    MPSEQ Yspcn2 = getspcn("spcnY2",0,0.0,0.0,0,1);
    MPSEQ Yspcnref2 = getspcn("spcnY2",Yspcn2.iSuper,Yspcn2.phAccum,Yspcn2.phInt,1,1);
    strcpy(Yspcn2.ch,"dec2");
    putCmd("chYspcn2='dec2'\n");

    int NYspcn1 = (int)getval("NYspcn1");
    int qYspcn1 = (int)getval("qYspcn1");
    int NYspcn2 = (int)getval("NYspcn2");
    int qYspcn2 = (int)getval("qYspcn2");
    
    char dqf_flag1[MAXSTR];
    getstr("dqf_flag1",dqf_flag1);  // "1" is normal mixing "2" is dqf
    char dqf_flag2[MAXSTR];
    getstr("dqf_flag2",dqf_flag2);  // "1" is normal mixing "2" is dqf

    char mMix1[MAXSTR];
    getstr("mMix1",mMix1);  // First Y-Y mixing
    char mMix2[MAXSTR];
    getstr("mMix2",mMix2);  // Second Y-Y mixing

    // Decoupling sequences
    DSEQ dec = getdseq("H");        // For Y evolution periods
    DSEQ dec2 = getdseq("X");       // For H acquisition
    DSEQ dec3 = getdseq("Y");       // For H acquisition
    DSEQ dec4 = getdseq("Z");

    tRF = getval("tRF");

    // NUS Safety Check
    if (NUS_ACTIVE()) {
        if (NUS_SAFETY_CHECK(tRF, "n", 0.0) != 0) {
            psg_abort(1);
        }
    }

    // Dutycycle Protection
    duty = 4.0e-6 + 4*pwY90 + 3*pwH90 + getval("tHY") + getval("tYX") + getval("tXH") + 
           d2 + d3 + d4 + tmd2 + tmd3 + tmd4 + getval("ad") + getval("rd") + at;

    duty = duty/(duty + d1 + 4.0e-6);
    if (duty > 0.2) {
        printf("Duty cycle %.1f%% >20%%. Abort!\n", duty*100);
        psg_abort(1);
    }

    // Set Phase Tables
    settable(phH90,8,table1);
    settable(phHhy,4,table2);
    settable(phYhy,4,table3);
    settable(ph1Y90,4,table4);
    settable(ph2Y90,4,table5);
    settable(phYmix1,8,table6);
    settable(phYmix2,8,table7);
    settable(ph3Y90,4,table8);
    settable(ph4Y90,4,table9);
    settable(phYyx,8,table10);
    settable(phXyx,4,table11);
    settable(ph1X90,4,table12);
    settable(ph2X90,4,table13);
    settable(phXxh,4,table14);
    settable(phHxh,4,table15);
    settable(ph1H90,8,table16);
    settable(ph2H90,8,table17);
    settable(phHrfdr,4,table18);
    settable(phHpxy,4,table19);
    settable(phYsoft1,8,table20);
    settable(phYhard1,8,table21);
    settable(phYsoft2,8,table22);
    settable(phYhard2,8,table23);
    settable(phYspc1,8,table24);
    settable(phYspcref1,8,table25);
    settable(phYspc2,8,table26);
    settable(phYspcref2,8,table27);
    settable(phYrfdr1,4,table28);
    settable(phYrfdr2,4,table29);
    settable(phRec,16,table30);
    settable(phRecsoft1,16,table31);
    settable(phRecsoft2,16,table32);
    settable(phRecsoft12,16,table33);

    // Phase cycling for indirect dimensions
    int id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);
        tsadd(phRecsoft1,2*id2_,4);
        tsadd(phRecsoft2,2*id2_,4);
        tsadd(phRecsoft12,2*id2_,4);
        tsadd(ph1X90,2*id2_,4);
    }
    if (phase1 == 2) {
        tsadd(ph1X90,3,4);
    }

    int id3_ = (int) (d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,2*id3_,4);
        tsadd(phRecsoft1,2*id3_,4);
        tsadd(phRecsoft2,2*id3_,4);
        tsadd(phRecsoft12,2*id3_,4);
        tsadd(ph3Y90,2*id3_,4);
    }
    if (phase2 == 2) {
        tsadd(ph3Y90,3,4);
    }

    int id4_ = (int) (d4 * getval("sw3") + 0.1);
    if ((phase3 == 1) || (phase3 == 2)) {
        tsadd(phRec,2*id4_,4);
        tsadd(phRecsoft1,2*id4_,4);
        tsadd(phRecsoft2,2*id4_,4);
        tsadd(phRecsoft12,2*id4_,4);
        tsadd(ph1Y90,2*id4_,4);
    }
    if (phase3 == 2) {
        tsadd(ph1Y90,3,4);
    }

    // Set receiver based on soft pulse usage
    if ((!strcmp(softpul, "n")) && (!strcmp(softpul2, "n"))) {
        setreceiver(phRec);
    }
    if ((!strcmp(softpul, "y")) && (!strcmp(softpul2, "n"))) {
        setreceiver(phRecsoft1);
    }
    if ((!strcmp(softpul, "n")) && (!strcmp(softpul2, "y"))) {
        setreceiver(phRecsoft2);
    }
    if ((!strcmp(softpul, "y")) && (!strcmp(softpul2, "y"))) {
        setreceiver(phRecsoft12);
    }

    // Begin Pulse Sequence
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

    // Y F4 Period (First Y evolution) with H XiX Decoupling
    _dseqon(dec);
    decpwrf(getval("aX90"));

    if (!strcmp(softpul, "y")) {
        if (d4>2*getval("pwX90")) {
            delay(0.5*d4-2.0*getval("pwX90"));
            decrgpulse(2*getval("pwX90"),0,0,0);
        } else {
            delay(d4/2.0);
        }
        _dseqoff(dec);
        obspwrf(getval("aHshp1"));
        obsunblank(); xmtron();
        _pboxpulse(shp1,phYsoft1);
        dec2pwrf(getval("aY90"));
        dec2rgpulse(2*getval("pwY90"),phYhard1,0.0,0.0);
        xmtroff();
        _dseqon(dec);
        delay(d4/2.0);
    }

    if (!strcmp(softpul, "n")) {
        if (d4>0) {
            if (d4>2*getval("pwX90")) {
                delay(0.5*d4-getval("pwX90"));
                decrgpulse(2*getval("pwX90"),0,0.0,0.0);
                delay(0.5*d4 - getval("pwX90"));
            } else {
                delay(d4);
            }
        }
    }

    dec2rgpulse(getval("pwY90"),ph1Y90,0.0,0.0);
    dec2phase(ph2Y90);
    delay(tmd4);

    // First Y-Y Mixing
    if (!strcmp(mMix1, "spcn")) {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        obspwrf(getval("aHmixspcn1"));
        xmtron();
        _mpseq(Yspcn1, phYspc1);
        if (!strcmp(dqf_flag1,"2")) {
            _mpseq(Yspcnref1, phYspcref1);
        }
        dec2pwrf(getval("aY90"));
        xmtroff();

        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();
        _dseqon(dec);
    } else if (!strcmp(mMix1, "rfdr")) {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        if (getval("qYrfdr1")>0) {
            _mpseq(Yrfdr1,phYrfdr1);
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

    // Y mixing pulses
    obspwrf(getval("aY90"));
    dec2rgpulse(getval("pwY90"),ph2Y90,0.0,0.0);
    dec2rgpulse(getval("pwY90"),phYmix1,0.0,0.0);
    delay(getval("tYmix"));
    dec2rgpulse(getval("pwY90"),phYmix2,0.0,0.0);
    dec2phase(ph3Y90);

    _dseqon(dec);

    // Y F3 Period (Second Y evolution) with H XiX Decoupling
    decpwrf(getval("aX90"));

    if (!strcmp(softpul2, "y")) {
        if (d3>2*getval("pwX90")) {
            delay(0.5*d3-2.0*getval("pwX90"));
            decrgpulse(2*getval("pwX90"),0,0,0);
        } else {
            delay(d3/2.0);
        }
        _dseqoff(dec);
        obspwrf(getval("aHshp2"));
        obsunblank(); xmtron();
        _pboxpulse(shp2,phYsoft2);
        dec2pwrf(getval("aY90"));
        dec2rgpulse(2*getval("pwY90"),phYhard2,0.0,0.0);
        xmtroff();
        _dseqon(dec);
        delay(d3/2.0);
    }

    if (!strcmp(softpul2, "n")) {
        if (d3>0) {
            if (d3>2*getval("pwX90")) {
                delay(0.5*d3-getval("pwX90"));
                decrgpulse(2*getval("pwX90"),0,0.0,0.0);
                delay(0.5*d3 - getval("pwX90"));
            } else {
                delay(d3);
            }
        }
    }

    dec2rgpulse(getval("pwY90"),ph3Y90,0.0,0.0);
    dec2phase(ph4Y90);
    delay(tmd3);

    // Second Y-Y Mixing (if different from first)
    if (!strcmp(mMix2, "spcn")) {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        obspwrf(getval("aHmixspcn2"));
        xmtron();
        _mpseq(Yspcn2, phYspc2);
        if (!strcmp(dqf_flag2,"2")) {
            _mpseq(Yspcnref2, phYspcref2);
        }
        dec2pwrf(getval("aY90"));
        xmtroff();

        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();
        _dseqon(dec);
    } else if (!strcmp(mMix2, "rfdr")) {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        if (getval("qYrfdr2")>0) {
            _mpseq(Yrfdr2,phYrfdr2);
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

    dec2rgpulse(getval("pwY90"),ph4Y90,0.0,0.0);
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
    } else {
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

    // H-H RFDR Mixing
    if (getval("qHrfdr")>0) {
        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph1H90,0.0,0.0);
        _mpseq(Hrfdr,phHrfdr);
        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph2H90,0.0,0.0);
    }

    // Begin X and Y Decoupling
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

    // tRF recovery delay
    if (tRF > d2+d3+d4) {
        delay(tRF-d2-d3-d4);
    }

    decoff();
    obsunblank(); decunblank(); _unblank34();
}
