/*
 * hXX.c - CANONICAL VERSION - Homonuclear XX Correlation Sequence
 *
 * A sequence to provide XX homonuclear correlation with multiple mixing schemes
 *
 * CONSOLIDATION HISTORY:
 * - Original: J. Rapp 09/17/09, CMR test 11/4/09, LJS & MT 8/18/10
 * - CMR 11/1/2018: Renamed hXX_dream.c to hXX.c, fixed soft pulse decoupling
 * - July 2025: Refactored with biosolidmixing_simple.h consolidation
 * - Nov 2025: CANONICAL VERSION - Consolidated from multiple variants
 *
 * CANONICAL VERSION FEATURES:
 * - Uses biosolid_jmc.h for maximum compatibility (proven in production)
 * - 5% duty cycle limit enforced (correct for C-detected experiments)
 * - Complete mixing support: SPC5, C6, C7, RFDR, R2T, DREAM, PAR, RAD, PARIS
 * - Soft pulse echo capability with proper decoupling
 * - Clean, well-documented code structure
 * - All important features from refactored versions preserved
 *
 * DUTY CYCLE STANDARD:
 * - 5% duty cycle limit for C-detected sequences (SAFETY_STANDARDS.md Section 1)
 * - Protects probe from thermal damage during high-power decoupling
 * - Calculation includes all RF components: pulses, CP, mixing, decoupling
 */

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"
#include "biosolid_jmc.h"

// ================================================================
// PHASE TABLE DEFINITIONS - Organized by Sequence Element
// ================================================================

// 1. Excitation and Preparation
static int table1[16] = {1,1,1,1,3,3,3,3,
                         3,3,3,3,1,1,1,1};          // phH90: H excitation pulse
static int table2[8] =  {0,0,0,0,0,0,0,0};          // phHhx: H channel during H→X CP
static int table3[8] =  {0,2,0,2,2,0,2,0};          // phXhx: X channel during H→X CP
static int table21[8] = {0,0,0,0,2,2,2,2};          // phXhx_soft: X soft CP mode
static int table22[16] ={1,3,1,3,1,3,1,3,
                         3,1,3,1,3,1,3,1};          // phX90: X direct excitation
static int table23[16] ={1,1,1,1,3,3,3,3,
                         3,3,3,3,1,1,1,1};          // phX90_soft: X soft excitation

// 2. Evolution Period
static int table24[4] = {0,0,0,0};                  // phCompY1: Y compensation pulse 1
static int table25[4] = {1,1,1,1};                  // phCompY2: Y compensation pulse 2
static int table18[16]= {0,1,0,1,0,1,0,1,
                         0,1,0,1,0,1,0,1};          // phXshp1: Soft echo pulse

// 3. Mixing Sequences
static int table4[16] = {3,3,1,1,1,1,3,3,
                         3,3,1,1,1,1,3,3};          // phXmix1: Storage pulse (CMR 4/9/18)
static int table5[32] = {1,1,3,3,1,1,3,3,
                         1,1,3,3,1,1,3,3,
                         2,2,0,0,2,2,0,0,
                         2,2,0,0,2,2,0,0};          // phXmix2: Readout pulse (CMR 4/9/18)
static int table9[32] = {1,3,3,1,1,3,3,1,
                         1,3,3,1,1,3,3,1,
                         2,0,0,2,2,0,0,2,
                         2,0,0,2,2,0,0,2};          // phXmix2dqf: DQF readout (CMR 4/11/18)
static int table7[8] =  {0,0,0,0,0,0,0,0};          // phXspc5: SPC5 base phase
static int table8[8] =  {0,1,0,1,0,1,0,1};          // phXspc5ref: SPC5 DQF reference
static int table11[8] = {0,0,0,0,0,0,0,0};          // phXpar: PAR mixing
static int table12[8] = {0,2,0,2,2,0,2,0};          // phXdream: DREAM mixing
static int table14[16] ={0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0};          // phXr2t: R2T mixing
static int table15[16] ={1,1,3,3,3,3,1,1,
                         1,1,3,3,3,3,1,1};          // ph90Xdream1: DREAM 90 pulse 1
static int table16[16] ={3,3,1,1,1,1,3,3,
                         3,3,1,1,1,1,3,3};          // ph90Xdream2: DREAM 90 pulse 2

// 4. Receiver Phases
static int table6[32] = {0,2,0,2,2,0,2,0,
                         2,0,2,0,0,2,0,2,
                         1,3,1,3,3,1,3,1,
                         3,1,3,1,1,3,1,3};          // phRec: Standard receiver
static int table19[32]= {0,2,0,2,2,0,2,0,
                         2,0,2,0,0,2,0,2,
                         1,3,1,3,3,1,3,1,
                         3,1,3,1,1,3,1,3};          // phRecsoft: Soft pulse receiver
static int table20[16]= {0,2,0,2,2,0,2,0,
                         2,0,2,0,0,2,0,2};          // phRecsoftnomix: No mixing receiver

// Phase assignments - logical sequence order
#define phH90 t1
#define phHhx t2
#define phXhx t3
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
#define phXhx_soft t21
#define phX90 t22
#define phX90_soft t23
#define phCompY1 t24
#define phCompY2 t25

int id2_,id3_, xsel;

void pulsesequence() {

    check_array();

    // ================================================================
    // PARAMETER SETUP AND VALIDATION
    // ================================================================

    double d2_, duty;

    // Cross-Polarization Setup
    CP hx = getcp("HX",0.0,0.0,0,1);
    strcpy(hx.fr,"dec");
    strcpy(hx.to,"obs");
    putCmd("frHX='dec'\n");
    putCmd("toHX='obs'\n");

    // Mixing Sequence Objects
    MPSEQ spc5, spc5ref;
    MPSEQ c6, c6ref;
    MPSEQ c7, c7ref;
    MPSEQ rfdr;
    DSEQ paris;
    R2TCKL r2t;
    spc5.t = spc5ref.t = 0;
    c6.t = c6ref.t = 0;
    c7.t = c7ref.t = 0;

    // Get sequence parameters
    char mMix[MAXSTR];
    getstr("mMix",mMix);

    // Initialize mixing sequences based on mMix parameter
    if (strcmp(mMix, "spc5") == 0) {
        spc5 = getspc5("spc5X",0,0.0,0.0,0,1);
        spc5ref = getspc5("spc5X",spc5.iSuper,spc5.phAccum,spc5.phInt,1,1);
        strcpy(spc5.ch,"obs");
        putCmd("chXspc5='obs'\n");
    }
    else if (strcmp(mMix, "c6") == 0) {
        c6 = getpostc6("c6X",0,0.0,0.0,0,1);
        c6ref = getpostc6("c6X",c6.iSuper,c6.phAccum,c6.phInt,1,1);
        strcpy(c6.ch,"obs");
        putCmd("chXc6='obs'\n");
    }
    else if (strcmp(mMix, "c7") == 0) {
        c7 = getpostc7("c7X",0,0.0,0.0,0,1);
        c7ref = getpostc7("c7X",c7.iSuper,c7.phAccum,c7.phInt,1,1);
        strcpy(c7.ch,"obs");
        putCmd("chXc7='obs'\n");
    }
    else if (strcmp(mMix, "rfdr") == 0) {
        rfdr = getrfdrxy8("rfdrX",0,0.0,0.0,0,1);
        strcpy(rfdr.ch,"obs");
        putCmd("chXrfdr='obs'\n");
    }
    else if (strcmp(mMix, "r2t") == 0 || strcmp(mMix, "dream") == 0) {
        r2t = getr2tckl("Xr2t",0,0.0,0.0,0,1);
        strcpy(r2t.ch,"obs");
    }
    else if (strcmp(mMix, "paris") == 0) {
        paris = getdseq("Hm");
    }

    // Sequence mode parameters
    char cp[MAXSTR];
    getstr("cp",cp);

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

    // Decoupling sequences
    DSEQ dec;
    dec = getdseq("H");
    DSEQ dec2;
    dec2 = getdseq("Y");
    DSEQ dec_soft;
    dec_soft = getdseq("Hsoft");
    DSEQ dec_mix;
    dec_mix = getdseq("Hmix");

    char dqf_flag[MAXSTR];
    getstr("dqf_flag",dqf_flag);

    char cpboth[MAXSTR];
    getstr("cpboth",cpboth);

    // ================================================================
    // MIXING PARAMETER SETUP
    // ================================================================

    // Set Mixing Period to N Rotor Cycles
    double taur=1,tXmix,srate;
    tXmix =  getval("tXmix");
    srate =  getval("srate");
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
    tRF = getval("tRF");

    // ================================================================
    // DUTY CYCLE PROTECTION - 5% LIMIT FOR C-DETECTED
    // ================================================================
    // C-detected sequences use high-power C decoupling which generates
    // significant heat. 5% duty cycle limit protects probe components.
    // See SAFETY_STANDARDS.md Section 1: Duty Cycle Limits

    duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 + getval("ad") + getval("rd") + at + tRF;

    // Add echo time if enabled
    if (strcmp(echo,"n") != 0) {
        duty += getval("tECHO");
    }

    double rf_time;    // Save for minimum d1 calculation
    double overhead;   // Overhead time added to denominator

    // Add mixing-specific contributions
    if ((strcmp(mMix, "rad") == 0) || (strcmp(mMix, "paris") == 0)) {
        duty += 2.0*getval("pwX90") + (getval("aHmix")/4095) * (getval("aHmix")/4095) * getval("tXmix");
        rf_time = duty;
        overhead = getval("tXmix") + 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "c7") == 0) {
        duty += 2.0*getval("pwX90");
        if (strcmp(dqf_flag,"2") == 0) {
            duty += c7ref.t;
        }
        else {
            duty += c7.t;
        }
        rf_time = duty;
        overhead = 4.0e-6 + 2.0*getval("tZF");
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "c6") == 0) {
        duty += 2.0*getval("pwX90");
        if (strcmp(dqf_flag,"2") == 0) {
            duty += c6ref.t;
        }
        else {
            duty += c6.t;
        }
        rf_time = duty;
        overhead = 4.0e-6 + 2.0*getval("tZF");
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "spc5") == 0) {
        duty += 2.0*getval("pwX90");
        if (!strcmp(dqf_flag,"2")) {
            duty += spc5ref.t;
        }
        else {
            duty += spc5.t;
        }
        rf_time = duty;
        overhead = 4.0e-6 + 2.0*getval("tZF");
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "par") == 0) {
        duty += 2.0*getval("pwX90") + getval("tPAR");
        rf_time = duty;
        overhead = 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "rfdr") == 0) {
        duty += 2.0*taur*getval("qXrfdr") + 2.0*getval("pwX90");
        rf_time = duty;
        overhead = 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }
    else if (strcmp(mMix, "r2t") == 0 || strcmp(mMix, "dream") == 0) {
        duty += 2*getval("tZF") + getval("tXr2t_in") + getval("tXr2t_mix") + getval("tXr2t_out") + 2.0*getval("pwX90");
        rf_time = duty;
        overhead = 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }
    else {
        // Default/unknown mixing mode
        rf_time = duty;
        overhead = 4.0e-6;
        duty = duty/(duty + d1 + overhead);
    }

    // Enforce 5% duty cycle limit with specific advice
    if (duty > 0.05) {
        double min_d1 = (rf_time / 0.05) - rf_time - overhead;
        abort_message("Duty cycle %.1f%% exceeds 5%% limit. Increase d1 to at least %.3f s. Abort!\n",
                      duty*100, min_d1);
    }
    else {
        printf("Duty cycle %.1f%% < 5%%. Safe to proceed. Good luck!\n", duty*100);
    }

    // ================================================================
    // PHASE TABLE SETUP
    // ================================================================

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

    // F2 indirect dimension phase cycling (States-TPPI)
    id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);
        tsadd(phRecsoft,2*id2_,4);
        tsadd(phRecsoftnomix,2*id2_,4);
        tsadd(phXmix1,2*id2_,4);
    }

    if (phase1 == 2) {
        tsadd(phXmix1,3,4);  // CMR 8/7/2018: States-TPPI selection via phXmix1
    }

    // Receiver selection based on sequence mode
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

    // ================================================================
    // PULSE SEQUENCE EXECUTION
    // ================================================================

    // External trigger on channel 1
    splineon(1);
    delay(2.0e-6);
    splineoff(1);

    // Initial phase setup
    if (strcmp(echo,"n") == 0) {
        txphase(phXhx);
    }
    else {
        txphase(phXhx_soft);
    }

    decphase(phH90);
    obspwrf(getval("aXhx"));
    decpwrf(getval("aH90"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    // ================================================================
    // H TO X CROSS POLARIZATION
    // ================================================================

    if (strcmp(cp,"y") == 0) {
        // Optional X pre-pulse
        if (strcmp(cpboth,"y") == 0) {
            obspwrf(getval("aX90"));
            if (strcmp(echo,"n") == 0) {
                rgpulse(getval("pwX90"), phX90, 0.0, 0.0);
            }
            else {
                rgpulse(getval("pwX90"), phX90_soft, 0.0, 0.0);
            }
        }
        // H excitation + H→X CP
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
        // Direct X excitation (no CP)
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

    // ================================================================
    // F2 INDIRECT EVOLUTION PERIOD FOR X
    // ================================================================

    obspwrf(getval("aX90"));
    _dseqon(dec);

    // Y compensation pulses during evolution (if d2 long enough)
    if (d2 > 4.0*pwY90) {
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

    // ================================================================
    // SOFT PULSE ECHO REFOCUSING (if enabled)
    // ================================================================

    if (strcmp(echo,"soft") == 0) {
       _dseqon(dec_soft);
       delay(tECHO/2.0);
       _pboxpulse(shp1,phXshp1);
       delay(tECHO/2.0);
       _dseqoff(dec_soft);
    }

    // ================================================================
    // MIXING SEQUENCES
    // ================================================================

    // R2T/DREAM mixing
    if (strcmp(mMix, "r2t") == 0 || strcmp(mMix, "dream") == 0) {
        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decoff();

        decpwrf(getval("aHZF"));
        decon();
        delay(getval("tZF"));
        decoff();

        if (strcmp(mMix, "dream") == 0) {
            decpwrf(getval("aH90"));
            decon();
            obspwrf(getval("aX90"));
            rgpulse(getval("pwX90"),ph90Xdream1,0.0,0.0);
            decoff();
        }
        _dseqon(dec_mix);
        _r2tckl(r2t, phXr2t, phXr2t, phXr2t);
        _dseqoff(dec_mix);

        if (strcmp(mMix, "dream") == 0) {
            decpwrf(getval("aH90"));
            decon();
            obspwrf(getval("aX90"));
            rgpulse(getval("pwX90"),ph90Xdream2,0.0,0.0);
            decoff();
        }

        decpwrf(getval("aHZF"));
        decon();
        delay(getval("tZF"));
        decoff();

        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
        decpwrf(getval("aH90"));
    }

    // PAR mixing
    if (strcmp(mMix, "par") ==0) {
        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decoff();

        decpwrf(getval("aHZF"));
        decon();
        delay(getval("tZF"));
        decoff();

        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),ph90Xdream1,0.0,0.0);
        decoff();

        obsunblank();
        decpwrf(getval("aHpar"));
        obspwrf(getval("aXpar"));
        txphase(phXpar);
        xmtron();
        decon();
        delay(getval("tPAR"));
        decoff();
        xmtroff();
        obsblank();

        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),ph90Xdream2,0.0,0.0);
        decoff();

        decpwrf(getval("aHZF"));
        decon();
        delay(getval("tZF"));
        decoff();

        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
        decpwrf(getval("aH90"));
    }

    // SPC5/C6/C7 Recoupling
    if (strcmp(mMix, "spc5") == 0 || strcmp(mMix, "c6") == 0 || strcmp(mMix, "c7") == 0) {
        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decoff();

        if (strcmp(mMix, "spc5") == 0) {
            obspwrf(getval("aXspc5"));
        }
        else if (strcmp(mMix, "c6") == 0) {
            obspwrf(getval("aXc6"));
        }
        else if (strcmp(mMix, "c7") == 0) {
            obspwrf(getval("aXc7"));
        }

        decpwrf(getval("aHZF"));
        txphase(phXmix1);

        decon();
        delay(getval("tZF"));
        decoff();

        if (strcmp(mMix, "spc5") == 0) {
            decpwrf(getval("aHmixspc5"));
            decon();
            _mpseq(spc5, phXspc5);
        }
        else if (strcmp(mMix, "c6") == 0) {
            decpwrf(getval("aHmixc6"));
            decon();
            _mpseq(c6, phXspc5);
        }
        else if (strcmp(mMix, "c7") == 0) {
            obspwrf(getval("aXc7"));
            decpwrf(getval("aHmixc7"));
            decon();
            _mpseq(c7, phXspc5);
        }

        if (strcmp(dqf_flag,"2") == 0) {
            if (strcmp(mMix, "spc5") == 0) {
                decpwrf(getval("aHmixspc5"));
                decon();
                _mpseq(spc5, phXspc5ref);
            }
            else if (strcmp(mMix, "c6") == 0) {
                decpwrf(getval("aHmixc6"));
                decon();
                _mpseq(c6, phXspc5ref);
            }
            else if (strcmp(mMix, "c7") == 0) {
                obspwrf(getval("aXc7"));
                decpwrf(getval("aHmixc7"));
                decon();
                _mpseq(c7, phXspc5ref);
            }
        }
        decoff();

        decpwrf(getval("aHZF"));
        obspwrf(getval("aX90"));

        xmtrphase(zero);
        txphase(phXmix2);

        decon();
        delay(getval("tZF"));
        decoff();

        decpwrf(getval("aH90"));

        if (strcmp(dqf_flag,"2") == 0) {
            decon();
            rgpulse(getval("pwX90"),phXmix2dqf,0.0,0.0);
        }
        else {
            decon();
            rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        }
        decoff();
    }

    // RAD (DARR) Mixing
    if (strcmp(mMix, "rad") == 0) {
        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decpwrf(getval("aHmix"));
        delay(tXmix);
        decpwrf(getval("aH90"));
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
    }

    // PARIS Mixing
    if (strcmp(mMix, "paris") == 0) {
        decpwrf(getval("aH90"));
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        _dseqon(paris);
        delay(tXmix);
        _dseqoff(paris);
        decpwrf(getval("aH90"));
        decon();
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
    }

    // RFDR Mixing
    if (strcmp(mMix, "rfdr") == 0) {
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

    // ================================================================
    // ACQUISITION
    // ================================================================

    _dseqon(dec);
    _dseqon(dec2);

    d2_ = (ni-1)/getval("sw1") - d2;
    if (d2_<0) {
        d2_ = 0.0;
    }

    obsblank(); _blank34();
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec);
    _dseqoff(dec2);

    // ================================================================
    // RECOVERY DELAY
    // ================================================================

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

/*
 * ================================================================
 * CONSOLIDATION DECISIONS AND RATIONALE
 * ================================================================
 *
 * VERSION COMPARISON:
 * -------------------
 * 1. hXX.c (original, 663 lines)
 *    - Production-proven code from CMR 2018
 *    - Uses biosolid_jmc.h (stable, widely compatible)
 *    - Complete mixing support: SPC5, C6, C7, RFDR, R2T, DREAM, PAR, RAD, PARIS
 *    - Correct 5% duty cycle limit
 *    - Well-tested in real experiments
 *
 * 2. hXX-claude.c (July 2025, 369 lines)
 *    - Uses biosolidmixing_simple.h for mixing consolidation
 *    - Shorter code (~44% reduction)
 *    - Same 5% duty cycle limit
 *    - Good structure but depends on newer library
 *
 * 3. hXX_refactored.c (338 lines)
 *    - Uses multiple new headers: biosolidevolution.h, biosolidcp.h, biosolidvalidation.h
 *    - Most compact code (~49% reduction)
 *    - 10% duty cycle limit (INCORRECT for C-detected!)
 *    - Depends on multiple untested consolidation libraries
 *
 * 4. hXX_complete_refactor.c (538 lines)
 *    - Extensively documented with phase cycling details
 *    - Uses all new consolidation headers
 *    - 10% duty cycle limit (INCORRECT)
 *    - Verbose comments increase line count
 *
 * CANONICAL VERSION CHOICES:
 * --------------------------
 * 1. BASE: hXX.c (original)
 *    WHY: Production-proven, uses stable biosolid_jmc.h, complete functionality
 *
 * 2. DUTY CYCLE: 5% limit from original
 *    WHY: Correct for C-detected sequences (SAFETY_STANDARDS.md Section 1)
 *    REJECTED: 10% limit from refactored versions (too high for probe protection)
 *
 * 3. CODE STRUCTURE: Enhanced from original with better comments
 *    WHY: Balance between clarity and compatibility
 *    - Organized phase tables by sequence element (from complete_refactor)
 *    - Clear section headers (from refactored versions)
 *    - Production code structure maintained (from original)
 *
 * 4. LIBRARIES: biosolid_jmc.h only
 *    WHY: Proven stable, widely compatible
 *    REJECTED: Newer consolidation libraries (biosolidmixing_simple.h,
 *              biosolidevolution.h, etc.) - not yet proven in production
 *
 * 5. MIXING SUPPORT: All original methods preserved
 *    WHY: Users depend on multiple mixing schemes
 *    KEPT: SPC5, C6, C7, RFDR, R2T, DREAM, PAR, RAD, PARIS
 *
 * IMPROVEMENTS OVER ORIGINAL:
 * ---------------------------
 * - Better code organization with clear section headers
 * - Improved phase table documentation
 * - Enhanced comments explaining duty cycle rationale
 * - Clearer variable naming in some sections
 * - Better consolidation documentation
 *
 * BACKWARD COMPATIBILITY:
 * -----------------------
 * - All parameter names unchanged
 * - All mixing methods preserved
 * - Same phase cycling behavior
 * - Same duty cycle limits
 * - Drop-in replacement for original hXX.c
 *
 * TESTING STATUS:
 * ---------------
 * This canonical version should be tested against original hXX.c:
 * 1. Verify identical spectra with standard samples
 * 2. Test all mixing methods (SPC5, C6, C7, RFDR, etc.)
 * 3. Verify duty cycle calculations
 * 4. Test soft pulse echo mode
 * 5. Test CP and direct excitation modes
 */
