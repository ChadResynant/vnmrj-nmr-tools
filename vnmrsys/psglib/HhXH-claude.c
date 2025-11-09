/*HhXH_S.c - 3D H-H-X-H correlation sequence with H detection
 * 
 * JMC 20/02/2015
 * CGB 1/14/23
 * 
 * ENHANCED: Added unified NUS implementation using biosolidnus.h header
 * 
 * Magnetization transfer: H(F3) → H-H mixing → X(F2) → H(F1) detection
 * Features constant time options for both F2 and F3 dimensions
 */

#include "standard.h"
#include "biosolidstandard.h" 
#include "biosolidpboxpulse.h"
#include "biosolidnus.h"  /* Unified NUS header */

static int table1[4]   = {0,0,0,0};         // phH90
static int table10[16] = {0,0,0,0,0,0,0,0,
                          2,2,2,2,2,2,2,2}; // ph1H90
static int table11[4]  = {0,0,0,0};         // phHrfdr
static int table12[16] = {2,2,2,2,2,2,2,2,
                          0,0,0,0,0,0,0,0}; // ph2H90
static int table3[4]   = {1,1,1,1};         // phHhx
static int table2[4]   = {0,0,0,0};         // phXhx
static int table5[4]   = {3,3,3,3};         // ph1X90
static int table7[4]   = {1,3,1,3};         // ph2X90
static int table6[4]   = {0,0,0,0};         // phHpxy
static int table9[4]   = {0,0,0,0};         // phXxh
static int table8[4]   = {1,1,1,1};         // phHxh
static int table14[8]  = {0,2,0,2,0,2,0,2}; // phRec
static int table18[16] = {0,0,0,0,1,1,1,1,
                          0,0,0,0,1,1,1,1}; // phH180CT
static int table16[4]  = {0,0,1,1};         // phX180CT
static int table17[16] = {0,2,2,0,2,0,0,2,
                          0,2,2,0,2,0,0,2}; // phRecCT
static int table15[4]  = {1,1,1,1};         // phHecho - solid echo cycle

#define phH90 t1
#define phXhx t2
#define phHhx t3
#define ph1X90 t5
#define phHpxy t6
#define ph2X90 t7
#define phHxh t8
#define phXxh t9
#define ph1H90 t10
#define phHrfdr t11
#define ph2H90 t12
#define phHecho t15
#define phRec t14
#define phX180CT t16
#define phRecCT t17
#define phH180CT t18

void pulsesequence() {

    // Check for phase,phase2 array bug
    check_array();

    // Define Variables and Objects and Get Parameter Values
    double duty;

    int decmode = getval("decmode");
    int decmodem = getval("decmodem");

    // Initial timing calculations (will be updated after NUS)
    double tmd2 = getval("tconst_N") - getval("d2") - getval("pwX90");
    if (tmd2 < 0.0) tmd2 = 0.0;

    double tmd3 = getval("tconst_H") - getval("d3") - getval("pwH90");
    if (tmd3 < 0.0) tmd3 = 0.0;

    // NUS Implementation - MUCH SIMPLER!
    if (NUS_ACTIVE()) {
        if (NUS_INIT(3) != 0) {  // 3D experiment
            psg_abort(1);
        }
        if (NUS_CALC_DELAYS() != 0) {
            psg_abort(1);
        }
        
        // Recalculate timing after NUS adjustment
        tmd2 = getval("tconst_N") - d2 - getval("pwX90");
        if (tmd2 < 0.0) tmd2 = 0.0;
        
        tmd3 = getval("tconst_H") - d3 - getval("pwH90");
        if (tmd3 < 0.0) tmd3 = 0.0;
    }

    // Constant time and homospoil options
    char CT_N[MAXSTR], CT_H[MAXSTR], HS[MAXSTR];
    getstr("CT_N",CT_N);   // Constant time for X dimension
    getstr("CT_H",CT_H);   // Constant time for H dimension
    getstr("HS",HS);       // Homospoil method

    double tHmix = getval("tHmix");

    // Cross-polarization setup
    CP hx = getcp("HX",0.0,0.0,0,1);
    strcpy(hx.fr,"obs");
    strcpy(hx.to,"dec");
    putCmd("frHX='obs'\n");
    putCmd("toHX='dec'\n");

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

    MPSEQ rfdr = getrfdrxy8("rfdrH",0,0.0,0.0,0,1);
    strcpy(rfdr.ch,"obs");
    putCmd("chHrfdr='obs'\n");

    // Decoupling sequences
    DSEQ dec = getdseq("H");    // For X evolution
    DSEQ dec2 = getdseq("X");   // For H acquisition
    DSEQ dec3 = getdseq("Y");
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
        if (NUS_SAFETY_CHECK(tRF, CT_N, getval("tconst_N")) != 0) {
            psg_abort(1);
        }
    }

    // Dutycycle Protection
    // H-detected sequence: Low-power H decoupling during acquisition allows 15% duty cycle
    // See SAFETY_STANDARDS.md Section 6: Power-Dependent Duty Cycle Limits
    duty = 4.0e-6 + 3*getval("pwH90") + getval("tHX") + getval("tconst_N") +
           getval("tXH") + t1Hecho + t2Hecho + getval("ad") + getval("rd") + at;

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

    // Set Phase Tables
    settable(phH90,4,table1);
    settable(phXhx,4,table2);
    settable(phHhx,4,table3);
    settable(ph1X90,4,table5);
    settable(phHpxy,4,table6);
    settable(ph2X90,4,table7);
    settable(phHxh,4,table8);
    settable(phXxh,4,table9);
    settable(ph1H90,16,table10);
    settable(phHrfdr,4,table11);
    settable(ph2H90,16,table12);
    settable(phRec,8,table14);
    settable(phHecho,4,table15);
    settable(phX180CT,4,table16);
    settable(phH180CT,16,table18);
    settable(phRecCT,16,table17);

    // Phase cycling for indirect dimensions
    int id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);
        tsadd(phRecCT,2*id2_,4);
        tsadd(phXhx,2*id2_,4);
    }
    if (phase1 == 2) {
        tsadd(ph1X90,3,4);
    }

    int id3_ = (int) (d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,2*id3_,4);
        tsadd(phRecCT,2*id3_,4);
        tsadd(ph1H90,2*id3_,4);
    }
    if (phase2 == 2) {
        tsadd(ph1H90,3,4);
    }

    // Set receiver based on constant time mode
    if (!strcmp(CT_N,"y")) {
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

    txphase(phH90); decphase(phXhx);
    obspwrf(getval("aH90")); decpwrf(getval("aXhx"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    obspwrf(getval("aH90"));
    decpwrf(getval("aX90"));
    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));

    // H excitation
    rgpulse(getval("pwH90"),phH90,0.0,0.0);

    // H F3 Period (first indirect dimension)
    if (!strcmp(CT_H,"n")) {
        if (d3 > 0) {
            if (d3 > 2*(getval("pwX90") + getval("pwY90") + getval("pwZ90"))) {
                delay(0.5*d3 - getval("pwX90") - getval("pwY90") - getval("pwZ90"));
                dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
                decrgpulse(2*getval("pwX90"),0,0.0,0.0);
                dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
                delay(0.5*d3 - getval("pwX90") - getval("pwY90") - getval("pwZ90"));
            } else {
                delay(d3);
            }
        }
    } else if (!strcmp(CT_H,"y")) {
        if (getval("tconst_H") < getval("ni2")/getval("sw2")) {
            printf("ABORT: tconst_H < maximum d3 evolution! Consider setting ni2=0.\n");
            psg_abort(1);
        }
        if (getval("tconst_H") < d3) {
            printf("ABORT: tconst_H < d3! Check that ni2/sw2 < tconst_H\n");
            psg_abort(1);
        }
        delay((getval("tconst_H")-d3)/2.0 - getval("pwH90"));
        rgpulse(2*getval("pwH90"),phH180CT,0.0,0.0);
        delay((getval("tconst_H")-d3)/2.0 - getval("pwH90") - 2*getval("pwX90") - 
              2*getval("pwY90") - 2*getval("pwZ90") - getval("pwH90")*(2./3.14159265358979323846));
        delay(d3/2.0);
        dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
        decrgpulse(2*getval("pwX90"),0,0.0,0.0);
        dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
        delay(d3/2.0);
    }

    // H-H Mixing (RFDR or simple delay)
    if (getval("qHrfdr") > 0 || tHmix > 0) {
        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph1H90,0.0,0.0);
        
        if (getval("qHrfdr") > 0 && getval("aHrfdr") > 0) {
            _mpseq(rfdr,phHrfdr);
        } else if (tHmix > 0) {
            delay(tHmix);
        }
        
        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph2H90,0.0,0.0);
    }

    // H→X Cross Polarization
    txphase(phHhx);
    _cp_(hx,phHhx,phXhx);
    decphase(ph1X90);
    decpwrf(getval("aX90"));

    // X F2 Period (second indirect dimension) with H decoupling
    _dseqon(dec);
    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));
    
    if (!strcmp(CT_N,"n")) {
        if (d2 > 0) {
            if (d2 > 2*getval("pwY90")) {
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
    } else if (!strcmp(CT_N,"y")) {
        if (getval("tconst_N") < getval("ni")/getval("sw1")) {
            printf("ABORT: tconst_N < maximum d2 evolution! Consider setting ni=0.\n");
            psg_abort(1);
        }
        if (getval("tconst_N") < d2) {
            printf("ABORT: tconst_N < d2! Check that ni/sw1 < tconst_N\n");
            psg_abort(1);
        }
        delay((getval("tconst_N")-d2)/2.0 - getval("pwX90"));
        decrgpulse(2*getval("pwX90"),phX180CT,0.0,0.0);
        delay((getval("tconst_N")-d2)/2.0 - getval("pwX90") - 2*getval("pwY90") - 
              2*getval("pwZ90") - getval("pwX90")*(2./3.14159265358979323846));
        delay(d2/2.0);
        dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
        dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
        delay(d2/2.0);
        decrgpulse(getval("pwX90"),ph1X90,0.0,0.0);
        decphase(ph1X90);
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
        if (tRF > d2+d3) {
            delay(tRF-d2-d3);
        }
    }

    decoff();
    obsunblank(); decunblank(); _unblank34();
}
