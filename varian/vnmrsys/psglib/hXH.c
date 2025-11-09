/*hXH.c -     A sequence to obtain an HX correlation with H
              detection using MISS (Multiple Intense Solvent
              Suppression), constant time with X decoupling.

              D. Rice 7/3/04   AJN 3/29/10                  */

#include "standard.h"
#include "biosolidstandard.h"

// Define Values for Phasetables

static int table1[4] =   {0,0,2,2};           // phH90
static int table2[8] =   {0,0,0,0,2,2,2,2};           // phXhx
static int table16[16] = {0,0,2,2,1,1,3,3,0,0,2,2,1,1,3,3}; // phX180CT
static int table3[2]   = {1,1};           // phHhx
static int table5[2]   = {3,3};           // ph1X90
static int table7[2]   = {1,3};           // ph2X90
static int table9[16]  = {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};           // phXxh
static int table8[16]  = {1,1,2,2,1,1,2,2, 1,1,2,2,1,1,2,2};           // phHxh

static int table10[8]  = {0,0,1,1,2,2,3,3};          // ph1H90
static int table11[4]  = {0,0,0,0};      // phHrfdr
static int table12[8]  = {2,2,3,3,0,0,1,1};          // ph2H90
static int table14[32] = {0,2,3,1,2,0,1,3, 0,2,3,1,2,0,1,3, 0,2,3,1,2,0,1,3, 0,2,3,1,2,0,1,3};          // phRec 
static int table17[32] = {0,2,3,1,0,2,3,1, 0,2,3,1,0,2,3,1, 0,2,3,1,0,2,3,1, 0,2,3,1,0,2,3,1};          // phRecCT
static int table18[32] = {0,2,3,1,0,2,3,1, 2,0,1,3,2,0,1,3, 0,2,3,1,0,2,3,1, 2,0,1,3,2,0,1,3};          // phRecHecho
static int table19[32] = {0,2,3,1,2,0,1,3, 2,0,1,3,0,2,3,1, 0,2,3,1,2,0,1,3, 2,0,1,3,0,2,3,1};          // phRecCTHecho

static int table15[32] = {1,1,0,0,0,0,1,1, 2,2,1,1,1,1,2,2, 3,3,2,2,2,2,3,3, 0,0,1,1,1,1,0,0};          // phHecho - solid echo cycle
static int table6[2] = {0,0};           // phHpxy

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
#define phRecHecho t18
#define phRecCTHecho t19

void pulsesequence() {

    check_array();

    // Define Variables and Objects and Get Parameter Values

    double duty;

    int decmode = getval("decmode");
    int decmodem = getval("decmodem");

	int i = 0;

    double tmd2 = getval("tconst") - getval("d2") - getval("pwX90");
    if (tmd2 < 0.0) {tmd2 = 0.0;}

    char CT[MAXSTR],HS[MAXSTR];

    getstr("CT",CT); 
    getstr("HS",HS);  

    CP hx = getcp("HX",0.0,0.0,0,1);
    strcpy(hx.fr,"obs");
    strcpy(hx.to,"dec");
    putCmd("frHX='obs'\n");
    putCmd("toHX='dec'\n");

    MPSEQ pxy = getpxy("pxyH",0,0.0,0.0,0,1);
    strcpy(pxy.ch,"obs");
    putCmd("chHpxy='obs'\n");

    MPSEQ pxy2 = getpxy("pxy2H",0,0.0,0.0,0,1);
    strcpy(pxy2.ch,"obs");
    putCmd("chHpxy2='obs'\n");

    MPSEQ rfdr = getrfdrxy8("rfdrH",0,0.0,0.0,0,1);
    strcpy(rfdr.ch,"obs");
    putCmd("chHrfdr='obs'\n");

    CP xh = getcp("XH",0.0,0.0,0,1);
    strcpy(xh.fr,"dec");
    strcpy(xh.to,"obs");
    putCmd("frXH='dec'\n");
    putCmd("toXH='obs'\n");

    DSEQ dec = getdseq("H");    //For t1 X evolution
    DSEQ dec2 = getdseq("X");    // For acq H evolution
    DSEQ dec3 = getdseq("Y");
    DSEQ dec4 = getdseq("Z");

    double t1Hechoinit = getval("tHecho")/2;
    double t2Hechoinit = getval("tHecho")/2;
    double t1Hecho  = t1Hechoinit - (getval("pwH180")/2.0);
    if (t1Hecho < 0.0) {t1Hecho = 0.0;}
    double t2Hecho  = t2Hechoinit - (getval("pwH180")/2.0);
    if (t2Hecho < 0.0) {t2Hecho = 0.0;}

    // Dutycycle Protection

    duty = 4.0e-6 + 3*getval("pwH90") + getval("tHX") + getval("tconst") + getval("d2") + getval("tXH") + t1Hecho + t2Hecho + getval("ad") + getval("rd") + at;

    duty = duty/(duty + d1 + 4.0e-6);
    if (duty > 0.15) {
        printf("Duty cycle %.1f%% >15%%. Abort!\n", duty*100);
        psg_abort(1);
    }

    double Gradlvl = (getval("qHpxy2")*2*getval("pwHpxy2")*getval("gzlvl1"));

    Gradlvl = abs(Gradlvl);
    printf("Grad Power %.1f%%.\n", (100*Gradlvl)/300);

    if (Gradlvl > 300) {
        printf("Grad Power %.1f%%. Abort!\n", (100*Gradlvl)/300);
        psg_abort(1);
    }

    // Set Phase Tables

    settable(phH90,4,table1);
    settable(phXhx,8,table2);
    settable(phHhx,2,table3);
    settable(ph1X90,2,table5);
    settable(phHpxy,2,table6);
    settable(ph2X90,2,table7);
    settable(phHxh,16,table8);
    settable(phXxh,16,table9);
    settable(ph1H90,8,table10);
    settable(phHrfdr,4,table11);
    settable(ph2H90,8,table12);
    settable(phRec,32,table14);
    settable(phHecho,32,table15);
    settable(phX180CT,16,table16);
    settable(phRecCT,32,table17);
    settable(phRecHecho,32,table18);
    settable(phRecCTHecho,32,table19);

    int id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);  /* invert the phases of the CP and */
        tsadd(phRecCT,2*id2_,4);
        tsadd(phRecHecho,2*id2_,4);
        tsadd(phRecCTHecho,2*id2_,4);
        tsadd(phXhx,2*id2_,4); /* the receiver for FAD to displace the axial peaks */
    }  
    if (phase1 == 2) {tsadd(ph1X90,3,4);} /* hypercomplex*/

    if (!strcmp(CT,"y") && getval("tHecho")==0){setreceiver(phRecCT);}
    if (!strcmp(CT,"y") && getval("tHecho")>0){setreceiver(phRecCTHecho);}
    if (!strcmp(CT,"n") && getval("tHecho")==0){setreceiver(phRec);}
    if (!strcmp(CT,"n") && getval("tHecho")>0){setreceiver(phRecHecho);}

    // Begin Sequence

    // Initial Homospoil Pulse

/* Acode errors on pfg board 12/4/23 CMR
    if(!strcmp(HS,"hs")) {
        status(B);
        hsdelay(getval("hst"));
        status(A);
        delay(getval("hstconst"));
    }

    if(!strcmp(HS,"z")) {
    rgradient('z',getval("gzlvl1"));
    delay(getval("tzgrad"));
    rgradient('z',0.0);
    delay(getval("tzgradconst"));
    }
*/

    txphase(phH90); decphase(phXhx);
    obspwrf(getval("aH90")); decpwrf(getval("aXhx"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    // H to X Cross Polarization

    rgpulse(getval("pwH90"),phH90,0.0,0.0);
    txphase(phHhx);
    _cp_(hx,phHhx,phXhx);
    decphase(ph1X90);
    decpwrf(getval("aX90"));

    // X F1 Period with H XiX (via TPPM) or SPINAL Decoupling and Constant Time N Evolution

    _dseqon(dec);

    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));
    if (!strcmp(CT,"n")) {
        if (d2>0) {
            if (d2>2*getval("pwY90")) {delay(0.5*d2-getval("pwY90")-getval("pwZ90"));
                dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
                dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
                delay(0.5*d2 - getval("pwY90")-getval("pwZ90"));
            }
        }
        else {delay(d2);}

        decrgpulse(getval("pwX90"),ph1X90,0.0,0.0);
        decphase(ph2X90);
        decunblank();
        delay(tmd2);
    }
    if (!strcmp(CT,"y")) {
        if (getval("tconst")< ni/sw1) {
            printf("tconst< tmd2 evol!! abort.\nMay consider setting ni=0.");
            psg_abort(1);
        }
        if (getval("tconst")< d2) {
            printf("tconst< d2 evol!! abort. check that ni/sw<tconst\n");
            psg_abort(1);
        }
        delay( (getval("tconst")-d2)/2.0 - (getval("pwX90"))  );
        decrgpulse(2*getval("pwX90"),phX180CT,0.0,0.0);
        delay((getval("tconst")-d2)/2.0 - getval("pwX90") - 2*getval("pwY90") -2*getval("pwZ90") - getval("pwX90")*(2./3.14159265358979323846) );
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

/*    if(!strcmp(HS,"hs")) {
        status(B);
        hsdelay(getval("hst"));
        status(A);
        delay(getval("hstconst"));
    }
*/


    // Saturation Pulses



/*    if(!strcmp(HS,"z")) {
        rgradient('z',getval("gzlvl1"));
        _mpseq(pxy2,phHpxy);
        rgradient('z',0.0);
    }
*/

    for(i = 0; i < getval("nHpxyrep"); i++) {
        delay(getval("resetgap"));
        _mpseq(pxy,phHpxy);
    }
    txphase(phHxh);  

    // X to H Cross Polarization
    obspwrf(getval("aH90"));
    obsunblank();
    simpulse(getval("pwX90"), getval("pwX90"), phHxh, ph2X90, 0.0, 0.0);
    decphase(phXxh);
    
    if(getval("tXH")>10e-6)
    { _cp_(xh,phXxh,phHxh); } // use this line if you need independent adjustment of HX and XH
    else
    { _cp_(hx,phXxh,phHxh); } // use this line if you don't
  
    //Chemical Exchange
    if (getval("tEX")>0)
    {obspwrf(getval("aH90"));txphase(ph1H90);
    rgpulse(getval("pwH90"),ph1H90,0.0,0.0);
    delay(getval("tEX"));
    rgpulse(getval("pwH90"),ph2H90,0.0,0.0);}
    

    // XY8 Mixing

    if (getval("qHrfdr")>0) {
        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph1H90,0.0,0.0);
      if(getval("aHrfdr")>0) {

        _mpseq(rfdr,phHrfdr);
        }

        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph2H90,0.0,0.0);
    }

    // Begin X Decoupling

//    if (!strcmp(HS,"hs")) {
//        status(C);
//    }
 
 
    // H Hahn Echo

    if(t1Hecho>0.0) {
        delay(t1Hecho);
        txphase(phHecho);
        obspwrf(getval("aH180"));
        rgpulse(getval("pwH180"),phHecho,0.0,0.0);
        delay(t2Hecho);
    }
    // Begin H Acquisition
       _dseqon(dec2); _dseqon(dec3); _dseqon(dec4);
 
    obsblank(); 
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec2); _dseqoff(dec3); _dseqoff(dec4);
    obsunblank(); decunblank(); _unblank34();
}
