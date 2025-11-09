/*HXhhXH_4D.c - 4D H-X correlation with H-H mixing and H detection
 * 
 * CMR 7/20/24: Moved "HXXH.c" from Ayrshire backups. This is named inconsistently with our current conventions.
 * Determined that this code was used for the APR17 work on vns500 at UIUC
 * Renamed macro and pulse code
 * Testing as of 7/20/24 on Taurus. See GB1 data sets from 7/20 to 7/23
 * CMR 7/28/24: adding skiprows capability and testing
 * 
 * SIMPLIFIED: Using unified biosolidnus.h header for consistent NUS implementation
 * 
 * Magnetization transfer: H(F4) → X(F3) → X(F2) → H(F1) with optional H-H mixing
 * 
 * Key operational notes:
 *   1. Start with working hXH and run HXhhXH_4D setup macro.
 *   2. Calibrate conditions with SPARSE='n'.
 *   3. For NUS, change SPARSE='y' and define sparse_file
 *   4. Use unified NUS parameter setup (no need to set ni=nrows anymore!)
 *   5. Good luck!
 */

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidnus.h"  /* Unified NUS header */

// Define Values for Phasetables
static int table1[4] =      {0,0,2,2};              // phH90
static int table3[2] =      {1,1};                  // phHhx1
static int table2[8] =      {0,0,0,0,0,0,0,0};      // phXhx1
static int table5[8] =      {3,3,3,3,3,3,3,3};      // ph1X90
static int table7[8] =      {3,3,3,3,3,3,3,3};      // ph2X90
static int table9[8] =      {0,0,0,0,0,0,0,0};      // phXxh4
static int table8[8] =      {1,1,1,1,1,1,1,1};      // phHxh4
static int table10[2] =     {1,1};                  // phH180_CT
static int table14[8] =     {3,1,1,3,3,1,1,3};      // phRec
static int table6[2] =      {0,0};                  // phHpxy

static int table15[2] =     {3,3};                  // ph3X90
static int table16[2] =     {1,3};                  // ph4X90

static int table17[2] =     {0,0};                  // phX180_CT_1
static int table18[2] =     {0,0};                  // phX180_CT_2

static int table19[2] =     {0,0};                  // phHrfdr

static int table21[8] =     {1,1,1,1,1,1,1,1};      // phHxh2
static int table22[8] =     {0,0,0,0,0,0,0,0};      // phXxh2
static int table23[8] =     {0,0,0,0,0,0,0,0};      // phH90_mix_1
static int table24[8] =     {0,0,0,0,2,2,2,2};      // phH90_mix_2
static int table25[8] =     {0,0,0,0,0,0,0,0};      // phXhx3
static int table26[8] =     {1,1,1,1,1,1,1,1};      // phHhx3
static int table28[8] =     {1,3,3,1,3,1,1,3};      // phRec_Hmix

#define phH90 t1
#define phXhx1 t2
#define phHhx1 t3
#define ph1X90 t5
#define phHpxy t6
#define ph2X90 t7
#define phHxh4 t8
#define phXxh4 t9
#define phH180_CT t10
#define phRec t14
#define ph3X90 t15
#define ph4X90 t16
#define phX180_CT_1 t17
#define phX180_CT_2 t18
#define phHrfdr t19

#define phHxh2 t21
#define phXxh2 t22
#define phH90_mix_1 t23
#define phH90_mix_2 t24
#define phXhx3 t25
#define phHhx3 t26
#define phRec_Hmix t28

void pulsesequence() {

    // Check for phase,phase2,phase3 array bug
    check_array();

    // Define Variables and Get Parameter Values
    double pwH90 = getval("pwH90");
    double pwX90 = getval("pwX90");
    double pwY90 = getval("pwY90");
    double pwZ90 = getval("pwZ90");

    double max_pw_YZ = (pwY90 > pwZ90 ? pwY90 : pwZ90);
    double max_pw_XYZ = max_pw_YZ > pwX90 ? max_pw_YZ : pwX90;

    // Constant time parameters
    char CT[MAXSTR], CT2[MAXSTR], CT3[MAXSTR];
    getstr("CT",CT);     // F2 constant time
    getstr("CT2",CT2);   // F3 constant time  
    getstr("CT3",CT3);   // F4 constant time

    double tmd2 = getval("tconst") - getval("d2") - pwX90;
    if (tmd2 < 0.0) tmd2 = 0.0;

    double tmd3 = getval("tconst2") - getval("d3") - pwX90;
    if (tmd3 < 0.0) tmd3 = 0.0;

    double tmd4 = getval("tconst3") - getval("d4") - pwX90;
    if (tmd4 < 0.0) tmd4 = 0.0;

    // NUS Implementation - MUCH SIMPLER!
    if (NUS_ACTIVE()) {
        if (NUS_INIT(4) != 0) {  // 4D experiment
            psg_abort(1);
        }
        if (NUS_CALC_DELAYS() != 0) {
            psg_abort(1);
        }
        
        // Recalculate constant time delays after NUS adjustment
        tmd2 = getval("tconst") - d2 - pwX90;
        if (tmd2 < 0.0) tmd2 = 0.0;
        
        tmd3 = getval("tconst2") - d3 - pwX90;
        if (tmd3 < 0.0) tmd3 = 0.0;
        
        tmd4 = getval("tconst3") - d4 - pwX90;
        if (tmd4 < 0.0) tmd4 = 0.0;
    }

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

    MPSEQ Hrfdr = getrfdrxy8("rfdrH",0,0.0,0.0,0,1);
    strcpy(Hrfdr.ch,"obs");
    putCmd("chHrfdr='obs'\n");

    char mMix[MAXSTR];
    getstr("mMix",mMix);

    // Decoupling sequences
    DSEQ dec = getdseq("H");        // For X evolution periods
    DSEQ dec2 = getdseq("X");       // For H acquisition
    DSEQ dec3 = getdseq("Y");
    DSEQ dec4 = getdseq("Z");

    // NUS Safety Check
    if (NUS_ACTIVE()) {
        double tRF = getval("tRF");
        if (NUS_SAFETY_CHECK(tRF, "n", 0.0) != 0) {
            psg_abort(1);
        }
    }

    // Dutycycle Protection
    // H-detected sequence: Low-power H decoupling during acquisition allows 15% duty cycle
    // See SAFETY_STANDARDS.md Section 6: Power-Dependent Duty Cycle Limits
    double total_proton_on_time = pwH90 + getval("tHX") + 4 * pwX90 +
                                  getval("tconst2") + getval("tconst") + getval("tXH");
    if (!strcmp(CT3,"y")) {
        total_proton_on_time += 2*pwH90;
    }

    double cycle_time = 4e-6 + getval("d1") + total_proton_on_time + tmd3 + tmd2 +
                        2*getval("hst") + 2*getval("hstconst") +
                        getval("qHpxy")*2*getval("pwHpxy") + getval("ad") + getval("rd") + at;
    double duty = total_proton_on_time / cycle_time;

    if (duty > 0.15) {
        printf("Duty cycle %.1f%% >15%%. Abort!\n", duty*100);
        psg_abort(1);
    }

    // Gradient power protection
    double Gradlvl = (getval("qHpxy")*2*getval("pwHpxy")*getval("gzlvl1"));
    if (Gradlvl > 300) {
        printf("Grad Power %.1f%%. Abort!\n", (100*Gradlvl)/300);
        psg_abort(1);
    }

    // Array validation
    char arraycheck[MAXSTR];
    getstr("array",arraycheck);
    int ndim = getval("ndim");
    int ni = getval("ni");
    int ni2 = getval("ni2");
    int ni3 = getval("ni3");
    
    if (ndim == 3) {
        if (!strcmp(arraycheck,"phase,phase2")) {
            printf("ABORT: change array to 'phase2,phase' for 3D\n");
            psg_abort(1);
        }
        if (!strcmp(arraycheck,"phase,phase3")) {
            printf("ABORT: change array to 'phase3,phase' for 3D\n");
            psg_abort(1);
        }
        if (!strcmp(arraycheck,"phase2,phase3")) {
            printf("ABORT: change array to 'phase3,phase2' for 3D\n");
            psg_abort(1);
        }
    } else if (ndim == 4 && ni != 0 && ni2 != 0 && ni3 != 0) {
        if (strcmp(arraycheck,"phase3,phase2,phase")) {
            printf("ABORT: change array to 'phase3,phase2,phase' for 4D\n");
            psg_abort(1);
        }
    }

    // Set Phase Tables
    settable(phH90,4,table1);
    settable(phXhx1,8,table2);
    settable(phHhx1,2,table3);
    settable(ph1X90,8,table5);
    settable(phHpxy,2,table6);
    settable(ph2X90,8,table7);
    settable(phHxh4,8,table8);
    settable(phXxh4,8,table9);
    settable(phH180_CT,2,table10);
    settable(phRec,8,table14);
    settable(ph3X90,2,table15);
    settable(ph4X90,2,table16);
    settable(phX180_CT_1,2,table17);
    settable(phX180_CT_2,2,table18);
    settable(phHrfdr,2,table19);

    settable(phHxh2, 8, table21);
    settable(phXxh2, 8, table22);
    settable(phH90_mix_1, 8, table23);
    settable(phH90_mix_2, 8, table24);
    settable(phXhx3, 8, table25);
    settable(phHhx3, 8, table26);
    settable(phRec_Hmix, 8, table28);

    // Phase cycling for indirect dimensions
    int id4_ = (int) (d4 * getval("sw3") + 0.1);
    if ((phase3 == 1) || (phase3 == 2)) {
        tsadd(phRec,2*id4_,4);
        tsadd(phRec_Hmix,2*id4_,4);
        tsadd(phH90,2*id4_,4);
    }
    if (phase3 == 2) {
        tsadd(phH90,1,4);
    }

    int id3_ = (int) (d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,2*id3_,4);
        tsadd(phRec_Hmix,2*id3_,4);
        tsadd(phXhx1,2*id3_,4);
    }
    if (phase2 == 2) {
        tsadd(ph1X90,3,4);
    }

    int id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);
        tsadd(phRec_Hmix,2*id2_,4);
        tsadd(phXxh4,2*id2_,4);
    }
    if (phase1 == 2) {
        tsadd(ph3X90,3,4);
    }

    // Set receiver based on mixing
    if (!strcmp(mMix,"n")) {
        setreceiver(phRec);
    } else {
        setreceiver(phRec_Hmix);
    }

    // Begin Pulse Sequence
    status(B);
    hsdelay(getval("hst"));
    status(A);
    delay(getval("hstconst"));

    txphase(phH90); decphase(phXhx1);
    obspwrf(getval("aH90")); decpwrf(getval("aXhx"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    // H excitation Pulse
    rgpulse(pwH90,phH90,0.0,0.0);

    // H F4 Period (first indirect dimension)
    if (!strcmp(CT3,"n")) {
        if (d4 > 2*max_pw_XYZ) {
            delay(d4/2 - max_pw_XYZ);
            sim4pulse(0.0, 2*pwX90, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
            delay(d4/2 - max_pw_XYZ);
        } else if (d4 > 0) {
            delay(d4);
        }
    } else if (!strcmp(CT3,"y")) {
        if (getval("tconst3") < ni3/getval("sw3")) {
            printf("ABORT: tconst3 < maximum d4! Consider adjusting parameters.\n");
            psg_abort(1);
        }
        if (getval("tconst3") < d4) {
            printf("ABORT: tconst3 < d4! Check constant time setup!\n");
            psg_abort(1);
        }
        if (getval("tconst3") > 2*pwH90) {
            delay((getval("tconst3") - d4)/2 - pwH90);
            rgpulse(2*pwH90, phH180_CT, 0.0, 0.0);
            delay((getval("tconst3") - d4)/2 - pwH90);
        } else {
            rgpulse(2*pwH90, phH180_CT, 0.0, 0.0);
        }
        if (d4 > 2*max_pw_XYZ) {
            delay(d4/2 - max_pw_XYZ);
            sim4pulse(0.0, 2*pwX90, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
            delay(d4/2 - max_pw_XYZ);
        } else if (d4 > 0) {
            delay(d4);
        }
    }

    // H→X Cross Polarization
    txphase(phHhx1);
    _cp_(hx,phHhx1,phXhx1);
    decphase(ph1X90);
    decpwrf(getval("aX90"));

    // X F3 Period (second indirect dimension)
    _dseqon(dec);
    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));
    
    if (!strcmp(CT2,"n")) {
        if (d3 > 0) {
            if (d3 > 2*max_pw_YZ) {
                delay(d3/2 - max_pw_YZ);
                sim4pulse(0.0, 0.0, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
                delay(d3/2 - max_pw_YZ);
            } else {
                delay(d3);
            }
        }
        _dseqoff(dec);
        obspwrf(getval("aH90"));
        obsunblank();
        simpulse(pwX90, pwX90, 0, ph1X90, 0.0, 0.0);
        _dseqon(dec);
        decphase(ph2X90);
        decunblank();
        delay(tmd3);
    } else if (!strcmp(CT2,"y")) {
        if (getval("tconst2") < ni2/getval("sw2")) {
            printf("ABORT: tconst2 < maximum d3! Consider adjusting parameters.\n");
            psg_abort(1);
        }
        if (getval("tconst2") < d3) {
            printf("ABORT: tconst2 < d3! Check constant time setup!\n");
            psg_abort(1);
        }
        if (getval("tconst2") > 2*pwX90) {
            delay((getval("tconst2") - d3)/2 - pwX90);
            decrgpulse(2*pwX90, phX180_CT_2, 0.0, 0.0);
            delay((getval("tconst2") - d3)/2 - pwX90);
        } else {
            decrgpulse(2*pwX90, phX180_CT_2, 0.0, 0.0);
        }
        if (d3 > 2*max_pw_YZ) {
            delay(d3/2 - max_pw_YZ);
            _dseqoff(dec);
            sim4pulse(0.0, 0.0, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
            _dseqon(dec);
            delay(d3/2 - max_pw_YZ);
        } else if (d3 > 0) {
            delay(d3);
        }
        _dseqoff(dec);
        obspwrf(getval("aH90"));
        obsunblank();
        simpulse(pwX90, pwX90, 0, ph1X90, 0.0, 0.0);
        decphase(ph2X90);
        decunblank();
        _dseqon(dec);
    }
    
    obspwrf(getval("aH90"));
    decpwrf(getval("aX90"));
    obsunblank();
    decphase(ph2X90);
    _dseqoff(dec);
    simpulse(pwX90, pwX90, 0, ph2X90, 0.0, 0.0);

    // X-X Mixing via H-H mixing
    if (!strcmp(mMix,"Hrfdr")) {
        txphase(phHxh2);
        decphase(phXxh2);
        _cp_(xh,phXxh2,phHxh2);

        obspwrf(getval("aH90"));
        decpwrf(getval("aX90"));
        txphase(phH90_mix_1);
        rgpulse(pwH90, phH90_mix_1, 0.0, 0.0);

        if (getval("qHrfdr") > 0) {
            _mpseq(Hrfdr, phHrfdr);
        }

        obspwrf(getval("aH90"));
        decpwrf(getval("aX90"));
        txphase(phH90_mix_2);
        rgpulse(pwH90, phH90_mix_2, 0.0, 0.0);

        txphase(phHhx3);
        decphase(phXhx3);
        _cp_(hx,phHhx3,phXhx3);
    }

    // X F2 Period (third indirect dimension)
    _dseqon(dec);
    decpwrf(getval("aX90"));
    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));
    
    if (!strcmp(CT,"n")) {
        if (d2 > 0) {
            if (d2 > 2*max_pw_YZ) {
                delay(d2/2 - max_pw_YZ);
                sim4pulse(0.0, 0.0, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
                delay(d2/2 - max_pw_YZ);
            } else {
                delay(d2);
            }
        }
        _dseqoff(dec);
        obspwrf(getval("aH90"));
        obsunblank();
        simpulse(pwX90, pwX90, 0, ph3X90, 0.0, 0.0);
        decphase(ph4X90);
        decunblank();
        delay(tmd2);
    } else if (!strcmp(CT,"y")) {
        if (getval("tconst") < ni/getval("sw1")) {
            printf("ABORT: tconst < maximum d2! Consider adjusting parameters.\n");
            psg_abort(1);
        }
        if (getval("tconst") < d2) {
            printf("ABORT: tconst < d2! Check constant time setup!\n");
            psg_abort(1);
        }
        if (getval("tconst") > 2*pwX90) {
            delay((getval("tconst") - d2)/2 - pwX90);
            decrgpulse(2*pwX90, phX180_CT_1, 0.0, 0.0);
            delay((getval("tconst") - d2)/2 - pwX90);
        } else {
            decrgpulse(2*pwX90, phX180_CT_1, 0.0, 0.0);
        }
        if (d2 > 2*max_pw_YZ) {
            delay(d2/2 - max_pw_YZ);
            sim4pulse(0.0, 0.0, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
            delay(d2/2 - max_pw_YZ);
        } else if (d2 > 0) {
            delay(d2);
        }
        _dseqoff(dec);
        obspwrf(getval("aH90"));
        obsunblank();
        simpulse(pwX90, pwX90, 0, ph3X90, 0.0, 0.0);
        decphase(ph4X90);
        decunblank();
    }

    // MISSISSIPPI Water Suppression
    txphase(phHxh4);
    obspwrf(getval("aHpxy"));

    status(B);
    hsdelay(getval("hst"));
    status(A);
    delay(getval("hstconst"));

    // Saturation Pulses
    for (int i = 0; i < getval("nHpxyrep"); i++) {
        delay(getval("resetgap"));
        _mpseq(pxy,phHpxy);
    }
    txphase(phHxh4);

    // X→H Cross Polarization
    obspwrf(getval("aH90"));
    obsunblank();
    simpulse(pwX90, pwX90, 0, ph4X90, 0.0, 0.0);
    decphase(phXxh4);
    _cp_(xh,phXxh4,phHxh4);

    // Begin Acquisition
    status(C);
    _dseqon(dec2); _dseqon(dec3); _dseqon(dec4);

    obsblank();
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec2); _dseqoff(dec3); _dseqoff(dec4);
    obsunblank(); decunblank(); _unblank34();
}
