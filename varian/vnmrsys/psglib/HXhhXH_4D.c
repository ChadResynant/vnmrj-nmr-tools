/*HXhhXH_4D.c -                 */
/* CMR 7/20/24. Moved "HXXH.c" from Ayrshire backups. This is named inconsistently with our current conventions. */
/* Determined that this code was used for the APR17 work on vns500 at UIUC */
/* Renamed macro and pulse code */
/* Testing as of 7/20/24 on Taurus. See GB1 data sets from 7/20 to 7/23 */
/* Key operational notes: */
/*   1. Start with working hXH and run HXhhXH_4D setup macro. */
/*   2. Calibrate conditions with SPARSE='n'. */
/*   3. For NUS, change SPARSE='y' and define value of sparse_file and ndim */
/*   4. Perform dps to error check and examine parsing of the NUS schedule in Text Output */
/*   5. ni and nrows must both be set to the total number of rows to run in NUS schedule */
/*   6. ni2 and ni3 must be set to 1 for NUS mode. */
/*   7. Be sure to set phase3, phase2, phase according to experiment type */
/*   8. Good luck! */
/* CMR 7/28/24: adding skiprows capability and testing */
/*   unresolved issues: (1) why necessary to set ni=nrows?; (2) CT mode operation with NUS not functional */

#include "standard.h"
#include "biosolidstandard.h"

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

static int table17[2] =     {0,0};                  // phH180_CT_1
static int table18[2] =     {0,0};                  // phH180_CT_2

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
    double pwH90 = getval("pwH90");
    double pwX90 = getval("pwX90");
    double pwY90 = getval("pwY90");
    double pwZ90 = getval("pwZ90");
    int ni = getval("ni");
    int ni2 = getval("ni2");
    int ni3 = getval("ni3");
    double sw1 = getval("sw1");
    double sw2 = getval("sw2");
    double sw3 = getval("sw3");
    int ndim = getval("ndim");
    int nrows = getval("nrows");
    int skiprows = getval("skiprows"); /* used for NUS concatenation CMR 7/28/24 */
    #define MAXFIDS 400000
    #define MAXDIM 4 /* maximum dimensionality for experiment */
    static int sel[MAXFIDS][MAXDIM-1];

    double max_pw_YZ = (pwY90 > pwZ90 ? pwY90 : pwZ90);
    double max_pw_XYZ = max_pw_YZ > pwX90 ? max_pw_YZ : pwX90;

    char CT[MAXSTR], CT2[MAXSTR], CT3[MAXSTR];
    double d4const = 0;
    double d3const = 0;
    double d2const = 0;
    double tmd4 = getval("tconst3") - getval("d4") - pwX90;
    if (tmd4 < 0.0) {
        tmd4 = 0.0;
    }
    getstr("CT3",CT3);

    double tmd3 = getval("tconst2") - getval("d3") - pwX90;
    if (tmd3 < 0.0) {
        tmd3 = 0.0;
    }
    getstr("CT2",CT2);

    double tmd2 = getval("tconst") - getval("d2") - pwX90;
    if (tmd2 < 0.0) {
        tmd2 = 0.0;
    }
    getstr("CT",CT);

    CP hx = getcp("HX",0.0,0.0,0,1);
    strcpy(hx.fr,"obs");
    strcpy(hx.to,"dec");
    putCmd("frHX='obs'\n");
    putCmd("toHX='dec'\n");

    MPSEQ pxy = getpxy("pxyH",0,0.0,0.0,0,1);
    strcpy(pxy.ch,"obs");
    putCmd("chHpxy='obs'\n");

    MPSEQ Hrfdr = getrfdrxy8("rfdrH",0,0.0,0.0,0,1);
    strcpy(Hrfdr.ch,"obs");
    putCmd("chHrfdr='obs'\n");

    CP xh = getcp("XH",0.0,0.0,0,1);
    strcpy(xh.fr,"dec");
    strcpy(xh.to,"obs");
    putCmd("frXH='dec'\n");
    putCmd("toXH='obs'\n");

    char mMix[MAXSTR];
    getstr("mMix",mMix);

    DSEQ dec = getdseq("H");        //For t1 X evolution
    DSEQ dec2 = getdseq("X");       // For acq H evolution
    DSEQ dec3 = getdseq("Y");
    DSEQ dec4 = getdseq("Z");

    // Dutycycle Protection
    double total_proton_on_time;
    total_proton_on_time = pwH90 + getval("tHX") + 4 * pwX90 + getval("tconst2") + getval("tconst") + + getval("tXH");
    if (!strcmp(CT3,"y")) {
        total_proton_on_time += 2*pwH90;
    }
    double cycle_time = 4e-6 + getval("d1") + total_proton_on_time + tmd3 + tmd2 + 2*getval("hst") + 2*getval("hstconst") + getval("qHpxy")*2*getval("pwHpxy") + getval("ad") + getval("rd") + at ;
    double duty = total_proton_on_time / cycle_time;

    if (duty > 0.1) {
        printf("Duty cycle %.1f%% >10%%. Abort!\n", duty*100);
        psg_abort(1);
    }

    double Gradlvl = (getval("qHpxy")*2*getval("pwHpxy")*getval("gzlvl1"));
    if (Gradlvl > 300) {
        printf("Grad Power %.1f%%. Abort!\n", (100*Gradlvl)/300);
        psg_abort(1);
    }



    /* NUS */
    char sparse_file[MAXSTR],sparse[MAXSTR];
    char read_char[MAXSTR];
    char check_str[MAXSTR];
    char tmp_str[MAXSTR];
    int check_len;
    int inc_max;
    int i, j;
    int *dim_counts[MAXDIM-1] = {&ni, &ni2, &ni3};
    getstr("SPARSE",sparse);
    if(sparse[0]=='y') {
        int active_dims[MAXDIM-1];
        int nis_correct = 1;
        int first_ni_checked = 0;
        for(j=0; j < MAXDIM-1; j++) {
            if(*(dim_counts[j]) > 0) {
                active_dims[j] = 1;
                if(!first_ni_checked) {
                    first_ni_checked = 1;
                    if(*(dim_counts[j]) != nrows) {
                        nis_correct = 0;
                        if(j==0) {
                            printf("Set ni = nrows (%d)\n", nrows);
                        }
                        else {
                            printf("Set ni%d = nrows (%d)\n", j+1, nrows);
                        }
                    }
                }
                else {
                    if(*(dim_counts[j]) != 1) {
                        nis_correct = 0;
                        if(j==0) {
                            printf("Set ni = 1\n");
                        }
                        else {
                            printf("Set ni%d = 1\n", j+1);
                        }
                    }
                }
            }
            else {
                active_dims[j] = 0;
            }
        }
        if(!nis_correct) {
            psg_abort(1);
        }
        printf("[%d %d %d]\n", active_dims[0], active_dims[1], active_dims[2]);
        int dim;
        if(ix==1) {
            FILE *fsparse_table;
            getstr("sparse_file",sparse_file);
            fsparse_table = fopen(sparse_file,"r");
            if(NULL==fsparse_table) {
                printf("File %s not found\n",sparse_file);
                psg_abort(1);
            }
            d4const = d4;
            d3const = d3;
            d2const = d2;

            if(nrows >= MAXFIDS) {
                printf("Too many FIDs \n");
                psg_abort(1);
            }
            printf("Maximum Increments: %d\n", nrows);

/* CMR 7/28/24 skiprows */
	    for(i=0; i < skiprows; i++) { fgets(read_char, 4*(MAXDIM-1), fsparse_table); }
	    

            for(i=0; i < nrows; i++) {
                dim = 1;
                for(j = 0; j < MAXDIM-1; j++) {
                    if(active_dims[j]) {
                        if(1 != fscanf(fsparse_table, "%d", &(sel[i][j]))) {
                            printf("Line %d is malformed. Or you have too few rows in file. \n",i+1);
                            printf("Check your sparse_file argument and ndim value.");
                            psg_abort(1);
                        }
                        fgets(read_char, 2, fsparse_table);
                        if(dim < ndim - 1) {
                            if(strcmp(read_char, " ")) {
                                printf("Line %d is malformed.\n",i+1);
                                printf("Check your sparse_file argument and ndim value.");
                                psg_abort(1);
                            }
                        }
                        else {
                            if(strcmp(read_char, "\n")) {
                                printf("Line %d is too long. (\"%s\")\n",i+1,read_char);
                                printf("Check that your sparse_file has the correct number of rows.");
                                psg_abort(1);
                            }
                        }
                        dim = dim + 1;
                    }
                    else {
                        sel[i][j] = 0;
                    }
                }
                dim = 0;
                memset(check_str, 0, sizeof(check_str));
                for(j = 0; j < MAXDIM-1; j++) {
                    if(active_dims[j]) {
                        sprintf(tmp_str, "%d", sel[i][j]);
                        if(dim > 0) {
                            strcat(check_str, " ");
                        }
                        strcat(check_str, tmp_str);
                        dim = dim + 1;
                    }
                }
                strcat(check_str, "\n");

                // measure the length of the recreated lien
                check_len = strlen(check_str);
                // rewind the file to the predicted beginning of the previous line
                fseek(fsparse_table, -(check_len), SEEK_CUR);
                // read that line
                fgets(read_char, check_len+1, fsparse_table);
                // compare the recreated line with the actual line to make sure they match
                if(strcmp(check_str, read_char)) {
                    printf("Line %d is malformed.\n",i+1);
                    printf("Expected: \"%s\"\n", check_str);
                    printf("Got: \"%s\"\n", read_char);
                    printf("Check your sparse_file argument and ndim value.");
                    psg_abort(1);
                }
            }
            fclose(fsparse_table);
            printf("Sampling table was read from %s\n",sparse_file);
            printf("\nNUS sampling list (will be applied upon 'gos'):\n");
            printf("  row   ni d2(ms)  ni2 d3(ms)  ni3 d4(ms) \n");
            printf("------------------------------------\n");
            for(i = 0; i < nrows; i++) {
                printf("%5d %4d %6.3f %4d %6.3f %4d %6.3f\n", i+1, sel[i][0], d2const+sel[i][0]/sw1, sel[i][1], d3const+sel[i][1]/sw2, sel[i][2], d4const+sel[i][2]/sw3);
            }

        }

        /* for each dimension, set the dwell according to the current ix value */
        d2 = d2const+sel[(int)((ix-1)/pow(2,ndim-1))][0]/sw1;
        d3 = d3const+sel[(int)((ix-1)/pow(2,ndim-1))][1]/sw2;
        d4 = d4const+sel[(int)((ix-1)/pow(2,ndim-1))][2]/sw3;
    }
    /* NUS Pt.2 - END */




    /******** Start Phase Cycle Wrangling ********/
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

    int id4_ = (int) (d4 * getval("sw3") + 0.1);
    if ((phase3 == 1) || (phase3 == 2)) {
        tsadd(phRec,2*id4_,4);
        tsadd(phRec_Hmix,2*id4_,4);
        tsadd(phH90,2*id4_,4); /* the receiver for FAD to displace the axial peaks */
    }
    if (phase3 == 2) { /* hypercomplex*/
        tsadd(phH90,1,4);
    }

    int id3_ = (int) (d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,2*id3_,4);
        tsadd(phRec_Hmix,2*id3_,4);
        tsadd(phXhx1,2*id3_,4); /* the receiver for FAD to displace the axial peaks */
    }
    if (phase2 == 2) { /* hypercomplex*/
        tsadd(ph1X90,3,4);
    }

    int id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);
        tsadd(phRec_Hmix,2*id2_,4);
        tsadd(phXxh4,2*id2_,4); /* the receiver for FAD to displace the axial peaks */
    }
    if (phase1 == 2) { /* hypercomplex*/
        tsadd(ph3X90,3,4);
    }
    if (!strcmp(mMix,"n")) {
        setreceiver(phRec);
    }
    else {
        setreceiver(phRec_Hmix);
    }
    /******** End Phase Cycle Wrangling ********/


    char arraycheck[MAXSTR];
    getstr("array",arraycheck);
    if(ndim == 3) {
        if(!strcmp(arraycheck,"phase,phase2")) {
            abort_message("change array to phase2,phase\n");
        }
        if(!strcmp(arraycheck,"phase,phase3")) {
            abort_message("change array to phase3,phase\n");
        }
        if(!strcmp(arraycheck,"phase2,phase3")) {
            abort_message("change array to phase3,phase2\n");
        }
    }
    else if(ndim == 4 && ni != 0 && ni2 != 0 && ni3 != 0) {
        if(strcmp(arraycheck,"phase3,phase2,phase")) {
            abort_message("change array to phase3,phase2,phase\n");
        }
    }


    // Begin Sequence

    // Initial Homospoil Pulse
    status(B);
    hsdelay(getval("hst"));
    status(A);
    delay(getval("hstconst"));

    txphase(phH90); decphase(phXhx1);
    obspwrf(getval("aH90")); decpwrf(getval("aXhx"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    /******** H excitation Pulse ********/
    rgpulse(pwH90,phH90,0.0,0.0);

    /******** Start H F1 Period ********/
    if (!strcmp(CT3,"n")) {
        if (d4 > 2*max_pw_XYZ) {
            delay(d4/2 - max_pw_XYZ);
            sim4pulse(0.0, 2*pwX90, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
            delay(d4/2 - max_pw_XYZ);
        }
        else if (d4 > 0){
            delay(d4);
        }
    }
    else if (!strcmp(CT3,"y")) {
        if (getval("tconst3") < ni3/sw3) {
            printf("tconst3 < tmd4! Consider setting ni3 = 0.\n");
            psg_abort(1);
        }
        if (getval("tconst3") < d4) {
            printf("tconst3 < d4! Check that ni3/sw3 < tconst3!\n");
            psg_abort(1);
        }
        if (getval("tconst3") > 2*pwH90) {
            delay((getval("tconst3") - d4)/2 - pwH90);
            rgpulse(2*pwH90, phH180_CT, 0.0, 0.0);
            delay((getval("tconst3") - d4)/2 - pwH90);
        }
        else {
            rgpulse(2*pwH90, phH180_CT, 0.0, 0.0);
        }
        if (d4 > 2*max_pw_XYZ) {
            delay(d4/2 - max_pw_XYZ);
            sim4pulse(0.0, 2*pwX90, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
            delay(d4/2 - max_pw_XYZ);
        }
        else if (d4 > 0){
            delay(d4);
        }
    }
    /******** End H F1 Period ********/

    /******** Start HX Cross Polarization ********/
    txphase(phHhx1);
    _cp_(hx,phHhx1,phXhx1);
    decphase(ph1X90);
    decpwrf(getval("aX90"));
    /******** END HX Cross Polarization ********/

    /******** Start X F2 Period ********/
    _dseqon(dec);
    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));
    if (!strcmp(CT2,"n")) {
        if (d3>0) {
            if (d3 > 2*max_pw_YZ) {
                delay(d3/2 - max_pw_YZ);
                sim4pulse(0.0, 0.0, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
                delay(d3/2 - max_pw_YZ);
            }
        }
        else {
            delay(d3);
        }
        _dseqoff(dec);
        obspwrf(getval("aH90"));
        obsunblank();
        simpulse(pwX90, pwX90, 0, ph1X90, 0.0, 0.0);
        _dseqon(dec);
        decphase(ph2X90);
        decunblank();
        delay(tmd3);
    }
    else if (!strcmp(CT2,"y")) {
        if (getval("tconst2") < ni2/sw2) {
            printf("tconst2 < tmd3! Consider setting ni2 = 0.\n");
            psg_abort(1);
        }
        if (getval("tconst2") < d3) {
            printf("tconst2 < d3! Check that ni2/sw2 < tconst2!\n");
            psg_abort(1);
        }
        if (getval("tconst2") > 2*pwX90) {
            delay((getval("tconst2") - d3)/2 - pwX90);
            decrgpulse(2*pwX90, phX180_CT_2, 0.0, 0.0);
            delay((getval("tconst2") - d3)/2 - pwX90);
        }
        else {
            decrgpulse(2*pwX90, phX180_CT_2, 0.0, 0.0);
        }
        if (d3 > 2*max_pw_YZ) {
            delay(d3/2 - max_pw_YZ);
            _dseqoff(dec);
            sim4pulse(0.0, 0.0, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
            _dseqon(dec);
            delay(d3/2 - max_pw_YZ);
        }
        else if (d3 > 0){
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

    /******** End X F2 Period ********/

    /******** Start X-X Mixing ********/
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
    /******** End X-X Mixing ********/

    /******** Start X F1 Period ********/
    _dseqon(dec);
    decpwrf(getval("aX90"));
    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));
    if (!strcmp(CT,"n")) {
        if (d2>0) {
            if (d2 > 2*max_pw_YZ) {
                delay(d2/2 - max_pw_YZ);
                sim4pulse(0.0, 0.0, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
                delay(d2/2 - max_pw_YZ);
            }
        }
        else {
            delay(d2);
        }
        _dseqoff(dec);
        obspwrf(getval("aH90"));
        obsunblank();
        simpulse(pwX90, pwX90, 0, ph3X90, 0.0, 0.0);
        decphase(ph4X90);
        decunblank();
        delay(tmd2);
    }
    else if (!strcmp(CT,"y")) {
        if (getval("tconst") < ni/sw1) {
            printf("tconst < tmd2! Consider setting ni = 0.\n");
            psg_abort(1);
        }
        if (getval("tconst") < d2) {
            printf("tconst < d2! Check that ni/sw1 < tconst!\n");
            psg_abort(1);
        }
        if (getval("tconst") > 2*pwX90) {
            delay((getval("tconst") - d2)/2 - pwX90);
            decrgpulse(2*pwX90, phX180_CT_1, 0.0, 0.0);
            delay((getval("tconst") - d2)/2 - pwX90);
        }
        else {
            decrgpulse(2*pwX90, phX180_CT_1, 0.0, 0.0);
        }
        if (d2 > 2*max_pw_YZ) {
            delay(d2/2 - max_pw_YZ);
            sim4pulse(0.0, 0.0, 2*pwY90, 2*pwZ90, 0, 0, 0, 0, 0.0, 0.0);
            delay(d2/2 - max_pw_YZ);
        }
        else if (d2 > 0){
            delay(d2);
        }
        _dseqoff(dec);
        obspwrf(getval("aH90"));
        obsunblank();
        simpulse(pwX90, pwX90, 0, ph3X90, 0.0, 0.0);
        decphase(ph4X90);
        decunblank();
    }
    /******** End X F1 Period ********/

    /******** Start MISSISSIPPI ********/
    txphase(phHxh4);
    obspwrf(getval("aHpxy"));

    // Homospoil Delay
    status(B);
    hsdelay(getval("hst"));
    status(A);
    delay(getval("hstconst"));

    // Saturation Pulses
    for(i = 0; i < getval("nHpxyrep"); i++) {
        delay(getval("resetgap"));
        _mpseq(pxy,phHpxy);
    }
    txphase(phHxh4);
    
    /******** End MISSISSIPPI ********/

    /******** Start XH Cross Polarization ********/
    obspwrf(getval("aH90"));
    obsunblank();
    simpulse(pwX90, pwX90, 0, ph4X90, 0.0, 0.0);
    decphase(phXxh4);
    _cp_(xh,phXxh4,phHxh4);
    /******** End XH Cross Polarization ********/


    /******** Start Acquisition ********/
    // Begin X Decoupling
    status(C);
    _dseqon(dec2); _dseqon(dec3); _dseqon(dec4);

    // Begin H Acquisition
    obsblank();
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec2); _dseqoff(dec3); _dseqoff(dec4);
    obsunblank(); decunblank(); _unblank34();
    /******** End Acquisition ********/
}
