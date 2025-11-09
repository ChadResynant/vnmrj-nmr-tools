/*hYYXH_4D.c - A 4D sequence to obtain YYXH correlation with H detection
                    HY cp, t1 Y evolution (F4) H dec
                    Y-Y mixing 
                    t2 Y evolution (F3) H dec  
		    YX cp, t3 X evolution (F2) H dec
		    XH cp, potential HH rfdr mixing
		    H acq (F1) with X,Y dec.

                    Based on hYyXH.c and following hXYXX_4D conventions
                    4D NUS implementation included
                    FIXED: Removed ni/ni2/ni3 variable conflicts */

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"
#include "biosolidnus.h"  /* Include NUS header */

// Define Values for Phasetables

static int table1[8] =  {0,0,0,0,2,2,2,2};          // phH90 - improved cycling
static int table2[4] =  {1,1,1,1};                  // phHhy
static int table3[4] =  {0,0,0,0};                  // phYhy
static int table4[4] =  {3,3,3,3};                  // ph1Y90 store F4
static int table5[4] =  {1,1,3,3};                  // ph2Y90 recover F4 - added cycling
static int table6[8] =  {0,0,0,0,2,2,2,2};          // phYmix1 Y-Y mixing - added cycling

static int table8[4] =  {3,3,3,3};                  // ph3Y90 store F3
static int table9[4] =  {1,1,3,3};                  // ph4Y90 recover F3 - improved cycling
static int table10[8] = {0,0,0,0,2,2,2,2};          // phYyx Y to X CP - simplified from 16 to 8
static int table11[4] = {0,0,0,0};                  // phXyx Y to X CP

static int table12[4] = {1,1,1,1};                  // ph1X90 store F2
static int table13[4] = {1,1,3,3};                  // ph2X90 recover F2 - improved cycling
static int table14[4] = {0,0,0,0};                  // phXxh X to H CP
static int table15[4] = {1,1,1,1};                  // phHxh X to H CP

static int table16[8] = {0,0,2,2,2,2,0,0};          // ph1H90 H-H mixing - added cycling
static int table17[8] = {2,2,0,0,0,0,2,2};          // ph2H90 H-H mixing - added cycling
static int table18[4] = {0,0,0,0};                  // phHrfdr H-H mixing
static int table19[4] = {0,1,0,1};                  // phHpxy H presat - added cycling

// Soft pulse phase tables
static int table20[8] = {0,1,0,1,1,0,1,0};          // phYsoft1 F4
static int table21[8] = {0,0,0,0,1,1,1,1};          // phYhard1 F4
static int table22[8] = {0,1,0,1,1,0,1,0};          // phYsoft2 F3  
static int table23[8] = {0,0,0,0,1,1,1,1};          // phYhard2 F3

// Receiver phases
static int table30[16]= {0,2,2,0,2,0,0,2, 2,0,0,2,0,2,2,0};    // phRec
static int table31[16]= {0,0,2,2,2,2,0,0, 2,2,0,0,0,0,2,2};    // phRecsoft1
static int table32[16]= {2,2,0,0,0,0,2,2, 0,0,2,2,2,2,0,0};    // phRecsoft2  
static int table33[16]= {2,0,0,2,0,2,2,0, 0,2,2,0,2,0,0,2};    // phRecsoft12

#define phH90       t1
#define phHhy       t2
#define phYhy       t3
#define ph1Y90      t4
#define ph2Y90      t5
#define phYmix1     t6
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
    
    /* REMOVED: Local ni, ni2, and ni3 variables that caused conflicts
       These are protected PSG variables and should not be redeclared */

    // Initialize timing variables
    double tmd4 = getval("d4max") - getval("d4") - pwX90;
    if (tmd4 < 0.0) tmd4 = 0.0;

    double tmd3 = getval("d3max") - getval("d3") - pwX90;
    if (tmd3 < 0.0) tmd3 = 0.0;

    double tmd2 = getval("d2max") - getval("d2") - pwX90;
    if (tmd2 < 0.0) tmd2 = 0.0;
    
   int ni3 = getval("ni3");   /* used for NUS */ 
   int ndim = getval("ndim");   /* used for NUS  */ 
   int nrows = getval("nrows");   /* used for NUS; must be equal to ni*ni2*ni3 to work properly; need to address this later CMR 7/9/25 */ 
   int skiprows = getval("skiprows"); /* used for NUS concatenation CMR 7/1/22 */
    tRF = getval("tRF");

    /* NUS Implementation for 4D - handles d2, d3, and d4 calculation internally */
    if (NUS_ACTIVE()) {
        if(ix==1)
         {
          if (NUS_INIT(4) != 0) { psg_abort(1);}  // 4D experiment
          if (NUS_SAFETY_CHECK(tRF, "n", 0.0) != 0) { psg_abort(1); }
          }
        if (NUS_CALC_DELAYS() != 0) { psg_abort(1); }
        
//        printf("LINE 124 d2 is %.3f \n",d2);
//        printf("LINE 125 d3 is %.3f \n",d3);
//        printf("LINE 126 d4 is %.3f \n",d4);
        
        // Recalculate timing after NUS adjustment
        tmd4 = getval("d4max") - d4 - pwX90;
        if (tmd4 < 0.0) tmd4 = 0.0;
        
        tmd3 = getval("d3max") - d3 - pwX90;
        if (tmd3 < 0.0) tmd3 = 0.0;
        
        tmd2 = getval("d2max") - d2 - pwX90;
        if (tmd2 < 0.0) tmd2 = 0.0;
        
// CMR 8/5/25 using this to debug new NUS protocol        
//        if(ni>1) {printf("Please use nrows for incrementing. Set ni=1"); psg_abort(1);}
//        if(ni2>1) {printf("Please use nrows for incrementing. Set ni2=1"); psg_abort(1);}
//        if(ni3>1) {printf("Please use nrows for incrementing. Set ni3=1"); psg_abort(1);}
    }
    // NUS Safety Check. CMR 8/5/25. This safety check does not behave properly after 'go'. It works for dps but then accounting fails when ni>1. 

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

    MPSEQ Yrfdr = getrfdrxy8("rfdrY",0,0.0,0.0,0,1);
    strcpy(Yrfdr.ch,"dec2");
    putCmd("chYrfdr='dec2'\n");

    MPSEQ Yspcn = getspcn("spcnY",0,0.0,0.0,0,1);
    MPSEQ Yspcnref = getspcn("spcnY",Yspcn.iSuper,Yspcn.phAccum,Yspcn.phInt,1,1);
    strcpy(Yspcn.ch,"dec2");
    putCmd("chYspcn='dec2'\n");

    int NYspcn = (int)getval("NYspcn");
    int qYspcn = (int)getval("qYspcn");
    
    MPSEQ Yspc5 = getspc5("spc5Y",0,0.0,0.0,0,1);
    MPSEQ Yspc5ref = getspc5("spc5Y",Yspc5.iSuper,Yspc5.phAccum,Yspc5.phInt,1,1);
    strcpy(Yspc5.ch,"dec2");
    putCmd("chYspc5='dec2'\n");

    int NYspc5 = (int)getval("NYspc5");
    int qYspc5 = (int)getval("qYspc5");

    char mMix[MAXSTR];
    getstr("mMix",mMix);  // First Y-Y mixing

    // Decoupling sequences
    DSEQ dec = getdseq("H");        // For Y evolution periods
    DSEQ dec2 = getdseq("X");       // For H acquisition
    DSEQ dec3 = getdseq("Y");       // For H acquisition
    DSEQ dec4 = getdseq("Z");

    // Dutycycle Protection
    duty = 4.0e-6 + 4*pwY90 + 3*pwH90 + getval("tHY") + getval("tYX") + getval("tXH") + 
           d2 + d3 + d4 + tmd2 + tmd3 + tmd4 + getval("ad") + getval("rd") + at;

    double rf_time = duty;  // Save RF time for minimum d1 calculation
    duty = duty/(duty + d1 + 4.0e-6);
    // H-detected sequence: Low-power H decoupling during acquisition allows 15% duty cycle
    // See SAFETY_STANDARDS.md Section 6: Power-Dependent Duty Cycle Limits
    if (duty > 0.15) {
        double min_d1 = (rf_time / 0.15) - rf_time - 4.0e-6;
        abort_message("Duty cycle %.1f%% exceeds 15%% limit. Increase d1 to at least %.3f s. Abort!\n",
                      duty*100, min_d1);
    }
    else {
        printf("Duty cycle %.1f%% < 15%%. Safe to proceed.\n", duty*100);
    }

    // Set Phase Tables
    settable(phH90,8,table1);
    settable(phHhy,4,table2);
    settable(phYhy,4,table3);
    settable(ph1Y90,4,table4);
    settable(ph2Y90,4,table5);
    settable(phYmix1,8,table6);
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

    char arraycheck[MAXSTR];
    getstr("array",arraycheck);
    printf("array=%s\n",arraycheck);    
        if( !strcmp(arraycheck,"phase,phase2,phase3") || !strcmp(arraycheck,"phase,phase3,phase2") || !strcmp(arraycheck,"phase2,phase3,phase") || !strcmp(arraycheck,"phase2,phase,phase3") || !strcmp(arraycheck,"phase3,phase,phase2"))
    	{ abort_message("change array to phase3,phase2,phase for 4D exp\n");}  
    if( !strcmp(arraycheck,"phase3,phase2,phase") && ((ni==0) || (ni2==0) || (ni3==0))) { abort_message("all ni's must be non-zero to run 4D");}
    if( !strcmp(arraycheck,"phase3,phase2,phase") && (ndim != 4) ) { abort_message("ndim must be = 4 for 4D exp");}


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

    else {
        if (d4>2*getval("pwX90")) {
                delay(0.5*d4-getval("pwX90"));
                decrgpulse(2*getval("pwX90"),0,0.0,0.0);
                delay(0.5*d4 - getval("pwX90"));
            } else {
                delay(d4);
            }
        
    }

    dec2rgpulse(getval("pwY90"),ph1Y90,0.0,0.0);
    dec2phase(ph2Y90);

    delay(tmd4);
    
    // First Y-Y Mixing
    if (!strcmp(mMix, "spcn"))  // CMR 8/24/25 tests on Ayrshire indicate frequency offset behavior is not properly working; DD specific bug? Worked on Taurus with same code
    				// tried recompiling but shapes start with non-zero phases and polarization transfer is discontinuous function of offset frequency
     {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        obspwrf(getval("aHmixspcn"));
        xmtron();
        _mpseq(Yspcn, 0);
        dec2pwrf(getval("aY90"));
        xmtroff();

        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();
        _dseqon(dec);
     } 
      else if (!strcmp(mMix, "spc5"))  
     {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        obspwrf(getval("aHmixspc5"));
        xmtron();
        _mpseq(Yspc5, 0);
        dec2pwrf(getval("aY90"));
        xmtroff();

        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();
        _dseqon(dec);
    } 
     else if (!strcmp(mMix, "rfdr")) 
    {
        _dseqoff(dec);
        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();

        if (getval("qYrfdr1")>0) {
            _mpseq(Yrfdr,0);
        }
        dec2pwrf(getval("aY90"));

        obspwrf(getval("aHZF"));
        xmtron();
        delay(getval("tZF"));
        xmtroff();
        _dseqon(dec);
    }

    _dseqoff(dec);

    // Y mixing pulses
    obspwrf(getval("aY90"));
    dec2rgpulse(getval("pwY90"),ph2Y90,0.0,0.0);

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

    else {
        
            if (d3>2*getval("pwX90")) {
                delay(0.5*d3-getval("pwX90"));
                decrgpulse(2*getval("pwX90"),0,0.0,0.0);
                delay(0.5*d3 - getval("pwX90"));
            } else {
                delay(d3);
            }
        
    }

    dec2rgpulse(getval("pwY90"),ph3Y90,0.0,0.0);
    dec2phase(ph4Y90);

    delay(tmd3);
    
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
    if (d2>0) 
     {
        if (d2>2*getval("pwY90")) {
            delay(0.5*d2-getval("pwY90"));
            dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
            delay(0.5*d2 - getval("pwY90"));
        }
     } 
    else 
     {
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
