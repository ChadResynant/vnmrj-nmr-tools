/*HhYXH_4D.c - 4D H-H-Y-X-H correlation sequence with H detection
 * 
 * Based on HhXH_S.c but extended to 4D with Y (15N) dimension
 * 
 * Magnetization transfer: H(F4) → H-H mixing → Y(F3) → X(F2) → H(F1) detection
 * Features constant time options for F2, F3, and F4 dimensions
 * 
 * Key applications: Protein backbone assignment, side chain correlations
 */

#include "standard.h"
#include "biosolidstandard.h" 
#include "biosolidpboxpulse.h"
#include "biosolidnus.h"  /* Unified NUS header */

// Define Values for Phasetables
static int table1[8]   = {0,0,0,0,2,2,2,2};         // phH90
static int table10[16] = {0,0,0,0,0,0,0,0,
                          2,2,2,2,2,2,2,2}; // ph1H90 (H-H mixing)
static int table11[8]  = {0,0,0,0,0,0,0,0};         // phHrfdr
static int table12[16] = {2,2,2,2,2,2,2,2,
                          0,0,0,0,0,0,0,0}; // ph2H90 (H-H mixing)

// H-Y transfer phases
static int table20[8]  = {1,1,1,1,1,1,1,1};         // phHhy
static int table21[8]  = {0,0,0,0,0,0,0,0};         // phYhy

// Y evolution and mixing phases
static int table22[8]  = {3,3,3,3,3,3,3,3};         // ph1Y90 (storage)
static int table23[8]  = {1,1,3,3,1,1,3,3};         // ph2Y90 (readout)
static int table24[8]  = {0,0,0,0,2,2,2,2};         // phYmix1
static int table25[8]  = {1,1,1,1,1,1,1,1};         // phYmix2

// Y-X transfer phases
static int table26[8]  = {0,0,0,0,0,0,0,0};         // phYyx
static int table27[8]  = {0,2,0,2,0,2,0,2};         // phXyx

// X evolution phases
static int table5[8]   = {3,3,3,3,3,3,3,3};         // ph1X90
static int table7[8]   = {1,3,1,3,1,3,1,3};         // ph2X90

// Water suppression and final transfer
static int table6[8]   = {0,0,0,0,0,0,0,0};         // phHpxy
static int table9[8]   = {0,0,0,0,0,0,0,0};         // phXxh
static int table8[8]   = {1,1,1,1,1,1,1,1};         // phHxh

// Constant time 180 pulses
static int table18[16] = {0,0,0,0,1,1,1,1,
                          0,0,0,0,1,1,1,1}; // phH180CT (F4)
static int table28[16] = {0,0,1,1,0,0,1,1,
                          0,0,1,1,0,0,1,1}; // phY180CT (F3)
static int table16[8]  = {0,0,1,1,0,0,1,1};         // phX180CT (F2)

// Receiver phases
static int table14[16] = {0,2,0,2,0,2,0,2,
                          2,0,2,0,2,0,2,0}; // phRec
static int table17[16] = {0,2,2,0,2,0,0,2,
                          2,0,0,2,0,2,2,0}; // phRecCT

// H echo phase
static int table15[8]  = {1,1,1,1,1,1,1,1};         // phHecho

#define phH90 t1
#define ph1H90 t10
#define phHrfdr t11
#define ph2H90 t12
#define phHhy t20
#define phYhy t21
#define ph1Y90 t22
#define ph2Y90 t23
#define phYmix1 t24
#define phYmix2 t25
#define phYyx t26
#define phXyx t27
#define ph1X90 t5
#define phHpxy t6
#define ph2X90 t7
#define phHxh t8
#define phXxh t9
#define phHecho t15
#define phRec t14
#define phX180CT t16
#define phRecCT t17
#define phH180CT t18
#define phY180CT t28

void pulsesequence() {

    // Check for phase,phase2,phase3 array bug
    check_array();

    // Define Variables and Objects and Get Parameter Values
    double duty;

    // Initial timing calculations (will be updated after NUS)
    double tmd2 = getval("tconst_X") - getval("d2") - getval("pwX90");
    if (tmd2 < 0.0) tmd2 = 0.0;

    double tmd3 = getval("tconst_Y") - getval("d3") - getval("pwY90");
    if (tmd3 < 0.0) tmd3 = 0.0;

    double tmd4 = getval("tconst_H") - getval("d4") - getval("pwH90");
    if (tmd4 < 0.0) tmd4 = 0.0;

    // NUS Implementation for 4D
    if (NUS_ACTIVE()) {
        if (NUS_INIT(4) != 0) {  // 4D experiment
            psg_abort(1);
        }
        if (NUS_CALC_DELAYS() != 0) {
            psg_abort(1);
        }
        
        // Recalculate timing after NUS adjustment
        tmd2 = getval("tconst_X") - d2 - getval("pwX90");
        if (tmd2 < 0.0) tmd2 = 0.0;
        
        tmd3 = getval("tconst_Y") - d3 - getval("pwY90");
        if (tmd3 < 0.0) tmd3 = 0.0;
        
        tmd4 = getval("tconst_H") - d4 - getval("pwH90");
        if (tmd4 < 0.0) tmd4 = 0.0;
    }

    // Constant time and mixing options
    char CT_X[MAXSTR], CT_Y[MAXSTR], CT_H[MAXSTR], HS[MAXSTR];
    getstr("CT_X",CT_X);   // Constant time for X dimension
    getstr("CT_Y",CT_Y);   // Constant time for Y dimension  
    getstr("CT_H",CT_H);   // Constant time for H dimension
    getstr("HS",HS);       // Homospoil method

    char mMixY[MAXSTR];
    getstr("mMixY",mMixY); // Y-Y mixing option

    double tHmix = getval("tHmix");
    double tYmix = getval("tYmix");

    // Cross-polarization setup
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

    // Pulse sequences
    MPSEQ pxy = getpxy("pxyH",0,0.0,0.0,0,1);
    strcpy(pxy.ch,"obs");
    putCmd("chHpxy='obs'\n");

    MPSEQ pxy2 = getpxy("pxy2H",0,0.0,0.0,0,1);
    strcpy(pxy2.ch,"obs");
    putCmd("chHpxy2='obs'\n");

    MPSEQ Hrfdr = getrfdrxy8("rfdrH",0,0.0,0.0,0,1);
    strcpy(Hrfdr.ch,"obs");
    putCmd("chHrfdr='obs'\n");

    MPSEQ Yrfdr = getrfdrxy8("rfdrY",0,0.0,0.0,0,1);
    strcpy(Yrfdr.ch,"dec2");
    putCmd("chYrfdr='dec2'\n");

    // Decoupling sequences
    DSEQ dec = getdseq("H");    // For Y and X evolution
    DSEQ dec2 = getdseq("X");   // For H acquisition
    DSEQ dec3 = getdseq("Y");   // For H acquisition
    DSEQ dec4 = getdseq("Z");

    // H echo timing
    double t1Hechoinit = getval("tHecho")/2;
    double t2Hechoinit = getval("tHecho")/2;
    double t1Hecho = t1Hechoinit - getval("pwH90");
    if (t1Hecho < 0.0) t1Hecho = 0.0;
    
    double t2Hecho = t2Hechoinit - getval("pwH90") - getval("rd");
    if (t2Hecho < 0.0) t2Hecho = 0.0;

    // NUS Safety Check
    if (NUS_ACTIVE()) {
        double tRF = getval("tRF");
        if (NUS_SAFETY_CHECK(tRF, "n", 0.0) != 0) {
            psg_abort(1);
        }
    }

    // Enhanced Dutycycle Protection for 4D
    duty = 4.0e-6 + 4*getval("pwH90") + 2*getval("pwY90") + 3*getval("pwX90") + 
           getval("tHY") + getval("tYX") + getval("tXH") + 
           getval("tconst_H") + getval("tconst_Y") + getval("tconst_X") + 
           t1Hecho + t2Hecho + getval("ad") + getval("rd") + at;

    duty = duty/(duty + d1 + 4.0e-6);
    if (duty > 0.15) {
        printf("Duty cycle %.1f%% >15%%. Abort!\n", duty*100);
        psg_abort(1);
    }

    // Gradient power protection
    double Gradlvl = (getval("qHpxy2")*2*getval("pwHpxy2")*getval("gzlvl1"));
    Gradlvl = abs(Gradlvl);
    printf("Grad Power %.1f%%.\n", (100*Gradlvl)/300);

    if (Gradlvl > 300) {
        printf("Grad Power %.1f%%. Abort!\n", (100*Gradlvl)/300);
        psg_abort(1);
    }

    // Array validation for 4D
    char arraycheck[MAXSTR];
    getstr("array",arraycheck);
    int ni = getval("ni");
    int ni2 = getval("ni2");
    int ni3 = getval("ni3");
    
    if (strcmp(arraycheck,"phase3,phase2,phase")) {
        printf("ABORT: For 4D experiment, set array='phase3,phase2,phase'\n");
        psg_abort(1);
    }
    
    if ((ni==0) || (ni2==0) || (ni3==0)) {
        printf("ABORT: All ni values must be non-zero for 4D experiment\n");
        psg_abort(1);
    }

    // Set Phase Tables
    settable(phH90,8,table1);
    settable(ph1H90,16,table10);
    settable(phHrfdr,8,table11);
    settable(ph2H90,16,table12);
    settable(phHhy,8,table20);
    settable(phYhy,8,table21);
    settable(ph1Y90,8,table22);
    settable(ph2Y90,8,table23);
    settable(phYmix1,8,table24);
    settable(phYmix2,8,table25);
    settable(phYyx,8,table26);
    settable(phXyx,8,table27);
    settable(ph1X90,8,table5);
    settable(phHpxy,8,table6);
    settable(ph2X90,8,table7);
    settable(phHxh,8,table8);
    settable(phXxh,8,table9);
    settable(phRec,16,table14);
    settable(phHecho,8,table15);
    settable(phX180CT,8,table16);
    settable(phRecCT,16,table17);
    settable(phH180CT,16,table18);
    settable(phY180CT,16,table28);

    // Phase cycling for indirect dimensions
    int id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);
        tsadd(phRecCT,2*id2_,4);
        tsadd(ph1X90,2*id2_,4);
    }
    if (phase1 == 2) {
        tsadd(ph1X90,3,4);
    }

    int id3_ = (int) (d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,2*id3_,4);
        tsadd(phRecCT,2*id3_,4);
        tsadd(ph1Y90,2*id3_,4);
    }
    if (phase2 == 2) {
        tsadd(ph1Y90,3,4);
    }

    int id4_ = (int) (d4 * getval("sw3") + 0.1);
    if ((phase3 == 1) || (phase3 == 2)) {
        tsadd(phRec,2*id4_,4);
        tsadd(phRecCT,2*id4_,4);
        tsadd(phH90,2*id4_,4);
    }
    if (phase3 == 2) {
        tsadd(phH90,3,4);
    }

    // Set receiver based on constant time mode
    if (!strcmp(CT_X,"y") || !strcmp(CT_Y,"y") || !strcmp(CT_H,"y")) {
        setreceiver(phRecCT);
    } else {
        setreceiver(phRec);
    }

    // Begin Pulse Sequence

    // Initial Homospoil or Gradient
    if (!strcmp(HS,"hs")) {
        status(B);
        hsdelay(getval("hst"));
        status(A);
        delay(getval("hstconst"));
    }

    if (!strcmp(HS,"z")) {
        rgradient('z',getval("gzlvl1"));
        delay(getval("tzgrad"));
        rgradient('z',0.0);
        delay(getval("tzgradconst"));
    }

    txphase(phH90); dec2phase(phYhy);
    obspwrf(getval("aH90")); dec2pwrf(getval("aYhy"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    obspwrf(getval("aH90"));
    decpwrf(getval("aX90"));
    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));

    // H excitation
    rgpulse(getval("pwH90"),phH90,0.0,0.0);

    // H F4 Period (first indirect dimension)
    if (!strcmp(CT_H,"n")) {
        if (d4 > 0) {
            if (d4 > 2*(getval("pwX90") + getval("pwY90") + getval("pwZ90"))) {
                delay(0.5*d4 - getval("pwX90") - getval("pwY90") - getval("pwZ90"));
                dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
                decrgpulse(2*getval("pwX90"),0,0.0,0.0);
                dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
                delay(0.5*d4 - getval("pwX90") - getval("pwY90") - getval("pwZ90"));
            } else {
                delay(d4);
            }
        }
    } else if (!strcmp(CT_H,"y")) {
        if (getval("tconst_H") < getval("ni3")/getval("sw3")) {
            printf("ABORT: tconst_H < maximum d4 evolution! Check parameters.\n");
            psg_abort(1);
        }
        if (getval("tconst_H") < d4) {
            printf("ABORT: tconst_H < d4! Check that ni3/sw3 < tconst_H\n");
            psg_abort(1);
        }
        delay((getval("tconst_H")-d4)/2.0 - getval("pwH90"));
        rgpulse(2*getval("pwH90"),phH180CT,0.0,0.0);
        delay((getval("tconst_H")-d4)/2.0 - getval("pwH90") - 2*getval("pwX90") - 
              2*getval("pwY90") - 2*getval("pwZ90") - getval("pwH90")*(2./3.14159265358979323846));
        delay(d4/2.0);
        dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
        decrgpulse(2*getval("pwX90"),0,0.0,0.0);
        dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
        delay(d4/2.0);
    }

    // H-H Mixing (RFDR or simple delay)
    if (getval("qHrfdr") > 0 || tHmix > 0) {
        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph1H90,0.0,0.0);
        
        if (getval("qHrfdr") > 0 && getval("aHrfdr") > 0) {
            _mpseq(Hrfdr,phHrfdr);
        } else if (tHmix > 0) {
            delay(tHmix);
        }
        
        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph2H90,0.0,0.0);
    }

    // H→Y Cross Polarization
    txphase(phHhy);
    _cp_(hy,phHhy,phYhy);
    dec2phase(ph1Y90);
    dec2pwrf(getval("aY90"));

    // Y F3 Period (second indirect dimension) with H decoupling
    _dseqon(dec);
    decpwrf(getval("aX90"));
    dec3pwrf(getval("aZ90"));
    
    if (!strcmp(CT_Y,"n")) {
        if (d3 > 0) {
            if (d3 > 2*(getval("pwX90") + getval("pwZ90"))) {
                delay(0.5*d3 - getval("pwX90") - getval("pwZ90"));
                decrgpulse(2*getval("pwX90"),0,0.0,0.0);
                dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
                delay(0.5*d3 - getval("pwX90") - getval("pwZ90"));
            } else {
                delay(d3);
            }
        }
        
        dec2rgpulse(getval("pwY90"),ph1Y90,0.0,0.0);
        dec2phase(ph2Y90);
        delay(tmd3);
    } else if (!strcmp(CT_Y,"y")) {
        if (getval("tconst_Y") < getval("ni2")/getval("sw2")) {
            printf("ABORT: tconst_Y < maximum d3 evolution! Check parameters.\n");
            psg_abort(1);
        }
        if (getval("tconst_Y") < d3) {
            printf("ABORT: tconst_Y < d3! Check that ni2/sw2 < tconst_Y\n");
            psg_abort(1);
        }
        delay((getval("tconst_Y")-d3)/2.0 - getval("pwY90"));
        dec2rgpulse(2*getval("pwY90"),phY180CT,0.0,0.0);
        delay((getval("tconst_Y")-d3)/2.0 - getval("pwY90") - 2*getval("pwX90") - 
              2*getval("pwZ90") - getval("pwY90")*(2./3.14159265358979323846));
        delay(d3/2.0);
        decrgpulse(2*getval("pwX90"),0,0.0,0.0);
        dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
        delay(d3/2.0);
        dec2rgpulse(getval("pwY90"),ph1Y90,0.0,0.0);
        dec2phase(ph2Y90);
    }

    // Y-Y Mixing (optional)
    if (!strcmp(mMixY,"rfdr") && getval("qYrfdr") > 0) {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        if (getval("qYrfdr") > 0) {
            _mpseq(Yrfdr,phYmix1);
        }
        dec2pwrf(getval("aY90"));

        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();
        _dseqon(dec);
    } else if (tYmix > 0) {
        dec2rgpulse(getval("pwY90"),phYmix1,0.0,0.0);
        delay(tYmix);
        dec2rgpulse(getval("pwY90"),phYmix2,0.0,0.0);
    }

    _dseqoff(dec);

    // Y→X Cross Polarization
    decphase(phXyx);
    dec2phase(phYyx);
    decpwrf(getval("aXyx"));
    dec2pwrf(getval("aYyx"));
    decunblank(); decon();
    _cp_(yx,phYyx,phXyx);
    decoff();

    // X F2 Period (third indirect dimension) with H decoupling
    _dseqon(dec);
    decpwrf(getval("aX90"));
    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));
    
    if (!strcmp(CT_X,"n")) {
        if (d2 > 0) {
            if (d2 > 2*(getval("pwY90") + getval("pwZ90"))) {
                delay(0.5*d2 - getval("pwY90") - getval("pwZ90"));
                dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
                dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
                delay(0.5*d2 - getval("pwY90") - getval("pwZ90"));
            } else {
                delay(d2);
            }
        }
        
        decrgpulse(getval("pwX90"),ph1X90,0.0,0.0);
        decphase(ph2X90);
        decunblank();
        delay(tmd2);
    } else if (!strcmp(CT_X,"y")) {
        if (getval("tconst_X") < getval("ni")/getval("sw1")) {
            printf("ABORT: tconst_X < maximum d2 evolution! Check parameters.\n");
            psg_abort(1);
        }
        if (getval("tconst_X") < d2) {
            printf("ABORT: tconst_X < d2! Check that ni/sw1 < tconst_X\n");
            psg_abort(1);
        }
        delay((getval("tconst_X")-d2)/2.0 - getval("pwX90"));
        decrgpulse(2*getval("pwX90"),phX180CT,0.0,0.0);
        delay((getval("tconst_X")-d2)/2.0 - getval("pwX90") - 2*getval("pwY90") - 
              2*getval("pwZ90") - getval("pwX90")*(2./3.14159265358979323846));
        delay(d2/2.0);
        dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
        dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
        delay(d2/2.0);
        decrgpulse(getval("pwX90"),ph1X90,0.0,0.0);
        decphase(ph2X90);
    }

    _dseqoff(dec);

    txphase(phHxh);
    obspwrf(getval("aHpxy"));

    // Homospoil Delay
    if (!strcmp(HS,"hs")) {
        status(B);
        hsdelay(getval("hst"));
        status(A);
        delay(getval("hstconst"));
    }

    // Water Suppression Pulses
    if (!strcmp(HS,"z")) {
        rgradient('z',getval("gzlvl1"));
        _mpseq(pxy2,phHpxy);
        rgradient('z',0.0);
    }

    _mpseq(pxy,phHpxy);
    txphase(phHxh);

    // X→H Cross Polarization
    obspwrf(getval("aH90"));
    obsunblank(); xmtron();
    decrgpulse(getval("pwX90"),ph2X90,0.0,0.0);
    xmtroff();
    decphase(phXxh);
    _cp_(xh,phXxh,phHxh);

    // Begin X,Y,Z Decoupling for H detection
    if (!strcmp(HS,"hs")) {
        status(C);
    }
    _dseqon(dec2); _dseqon(dec3); _dseqon(dec4);
    obsblank(); _blank34();

    // H Hahn Echo (if enabled)
    if (t1Hecho > 0.0) {
        txphase(phHecho);
        obspwrf(getval("aH90"));
        delay(t1Hecho);
        rgpulse(2*getval("pwH90"),phHecho,0.0,0.0);
        delay(t2Hecho);
    }

    // Begin H Acquisition
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec2); _dseqoff(dec3); _dseqoff(dec4);

    decon();

    // tRF recovery delay
    if (NUS_ACTIVE()) {
        double tRF = getval("tRF");
        if (tRF > d2+d3+d4) {
            delay(tRF-d2-d3-d4);
        }
    }

    decoff();
    obsunblank(); decunblank(); _unblank34();
}
