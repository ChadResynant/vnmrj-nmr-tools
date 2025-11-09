/*  hYXX.c  tangent ramped hNCC 3D with specific cp 
    and several C-C mixing schemes
    AJN 11/13/09 from dcp2tan3drad.c  D.Rice 10/12/05    
    MT 3/29/11 add dqf_flag in SPC5 and C7 
    AJN, MT 5/31/11 change ofdecN during d3
    CGB added water edited hhYXX 3D capabilities 11/28/21
*/

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"

// 10-25-23: BDZ commenting this out. It is a mistake to
//   include both bisolidstandard.h and solidstandard.h
//
//#include "solidstandard.h" //CGB


/* NUS Pt.1 - Declare variables used for specifying d2 & d3 values (from example code, gnoesyNhsqc.c)*/
#define MAXFIDS 400000
static int sel[MAXFIDS][2];
static double d3const, d2const;
/* NUS Pt.1 - END */


// Define Values for Phasetables
//static int table1[8]  = {0,0,0,0,0,0,0,0};                          // phH90 CGB commented out and redefined below
static int table2[8]  = {3,3,3,3,3,3,3,3};                          // phHhy
static int table3[8]  = {1,1,1,1,1,1,1,1};                          // phY90
static int table4[8]  = {0,0,0,0,0,0,0,0};                          // phYhy
static int table24[8] = {0,0,0,0,0,0,0,0};                          // phY180
static int table5[8]  = {0,0,0,0,0,0,0,0};                          // phHyx
static int table6[8]  = {0,2,0,2,0,2,0,2};                          // phYyx
static int table7[8]  = {0,0,2,2,0,0,2,2};                          // phXyx

static int table8[8]  = {3,3,1,1,3,3,1,1};                          // phXmix1
static int table9[8]  = {0,0,1,1,2,2,3,3};                          // phXmix2
static int table21[8] = {0,0,0,0,2,2,2,2};                          // phXspc5
static int table22[8] = {0,0,1,1,2,2,3,3};                          // phXspc5ref
static int table23[8] = {2,2,1,1,0,0,3,3};                          // phXmix2dqf;
static int table10[8] = {0,2,1,3,2,0,3,1};                          // phRec
static int table11[8] = {0,0,0,0,2,2,2,2};                          // phYyx_soft
static int table12[8] = {0,0,0,0,1,1,1,1};                          // phXyx_soft
static int table15[8] = {0,1,0,1,3,2,3,2};                          // phXsoft 
static int table16[8] = {0,0,1,1,3,3,2,2};                          // phXhard 
static int table17[8] = {0,0,0,0,0,0,0,0};                          // phYhard 
static int table18[8] = {1,1,1,1,0,0,0,0};                          // phXmix1_soft
static int table19[8] = {0,0,2,2,1,1,3,3};                          // phXmix2_soft
static int table20[8] = {2,0,2,0,3,1,3,1};                          // phRec_soft 
static int table31[4] = {0,0,0,0};                                  // phCompX1
static int table32[4] = {1,1,1,1};                                  // phCompX2
static int table33[4] = {0,0,0,0};                                  // phCompY1
static int table34[4] = {1,1,1,1};                                  // phCompY2

static int table35[8] = {0,0,0,0,0,0,0,0};                          // phXpar
static int table36[16] ={1,1,3,3,3,3,1,1,1,1,3,3,3,3,1,1};          // ph90Xzf1
static int table37[16] ={3,3,1,1,1,1,3,3,3,3,1,1,1,1,3,3};			// ph90Xzf2;
// CGB added for HhYXX experiment
// CMR updated phase cycle 3/13/2024
static int table1[16] = {3,3,3,3,1,1,1,1,3,3,3,3,1,1,1,1};          // phH90	CGB added 11/28/21
static int table40[8] = {0,0,0,0,0,0,0,0};          			// phHhy	CGB added 11/28/21
static int table41[8] = {1,1,3,3,1,1,3,3};         			// phHstore	CGB added 11/28/21
static int table42[8] = {3,3,3,3,1,1,1,1};          			// phHreadout	CGB added 11/28/21
static int table43[8] = {0,2,1,3,0,2,1,3};   				// phHecho - solid echo cycle 	CGB added 11/28/21

//static int table10[8] = {0,2,1,3,2,0,3,1};                          // phRec (same as above)

#define phH90 t1
//#define phHhy t2
#define phY90 t3
#define phYhy t4
#define phHyx t5
#define phYyx t6
#define phXyx t7
#define phXmix1 t8
#define phXmix2 t9
#define phY180 t24

#define phXmix1_soft t18
#define phXmix2_soft t19

#define phCompX1 t31
#define phCompX2 t32
#define phCompY1 t33
#define phCompY2 t34
#define phXsoft t15
#define phXhard t16
#define phYhard t17

#define phRec t10

#define phYyx_soft t11
#define phXyx_soft t12
#define phRec_soft t20

#define phXspc5 t21
#define phXspc5ref t22
#define phXmix2dqf t23

#define phXpar t35
#define ph90Xzf1 t36
#define ph90Xzf2 t37

#define phHhy t40		//CGB added 11/28/21
#define phHstore t41	//CGB added 11/28/21
#define phHreadout t42	//CGB added 11/28/21
#define phHecho t43		//CGB added 11/28/21

void pulsesequence() {

    check_array();

    // Define Variables and Objects and Get Parameter Values

    double duty;
    double tRF,d2_,d3_,d4;
    int id2_,id3_,id4_;

    CP hy = getcp("HY",0.0,0.0,0,1);
    strcpy(hy.fr,"dec");
    strcpy(hy.to,"dec2");
    putCmd("frHY='dec'\n");
    putCmd("toHY='dec2'\n");

    CP yx = getcp("YX",0.0,0.0,0,1);
    strcpy(yx.fr,"dec2");
    strcpy(yx.to,"obs");
    putCmd("frYX='dec2'\n");
    putCmd("toYX='obs'\n");

    char decN[MAXSTR];
    getstr("decN",decN);

 
    char hMix[MAXSTR];   		// CGB added 11/28/21
    getstr("hMix",hMix); 		// CGB added 11/28/21
    
    char hsel[MAXSTR];  		//CGB added 11/28/21
    getstr("hsel",hsel);		//CGB added 11/28/21

    PBOXPULSE shpH = getpboxpulse("shpH",0,1); //CGB changed shp to shpH to fix naming convention involving multiple shaped pulses 4/25/22
    strcpy(shpH.ch,"dec"); //
    putCmd("chHshp ='dec'\n"); //

    double dof = getval("dof");
    double ofdecN = getval("ofdecN");

    DSEQ dec = getdseq("H");

    PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);
	// CGB added 11/28/21
    char echo[MAXSTR];
    getstr("echo",echo);  
 
    double aHecho = getval("aHecho");
    double pwHecho = getval("pwHecho");
 
    double tECHO = getval("tECHO");
    tECHO  = tECHO - pwHecho;
    if (tECHO < 0.0) tECHO = 0.0;
 
    double tECHOmax = getval("tECHOmax");
    if (strcmp(echo,"n")) {
    tECHOmax = tECHOmax - tECHO;}
    if (tECHOmax < 0.0) tECHOmax = 0.0;

    char ctN[MAXSTR];
    getstr("ctN",ctN);

    char softpul[MAXSTR];
    getstr("softpul",softpul);

    char mMix[MAXSTR];
    getstr("mMix",mMix);   

    char dqf_flag[MAXSTR];
    getstr("dqf_flag",dqf_flag);  //n is normalmixing y is dqf 

    MPSEQ spc5 = getspc5("spc5X",0,0.0,0.0,0,1);
    MPSEQ spc5ref = getspc5("spc5X",spc5.iSuper,spc5.phAccum,spc5.phInt,1,1); 
    strcpy(spc5.ch,"obs");
    putCmd("chXspc5='obs'\n");

    MPSEQ c7 = getpostc7("c7X",0,0.0,0.0,0,1);
    MPSEQ c7ref = getpostc7("c7X",c7.iSuper,c7.phAccum,c7.phInt,1,1);
    strcpy(c7.ch,"obs");
    putCmd("chXc7='obs'\n");

    double pwX90,pwY90;
    pwX90 = getval("pwX90");
    pwY90 = getval("pwY90");

    // Set Mixing Period to N Rotor Cycles

//    double taur,tXmix,srate; CGB 4/25/22
    double taur,tXmix,hmix,srate;
    tXmix =  getval("tXmix");
	hmix =  getval("tHmix"); 

    srate =  getval("srate");
    taur = 0.0;
    if (srate >= 500.0) {
        taur = roundoff((1.0/srate), 0.125e-6);
    }
    else {
        printf("ABORT: Spin Rate (srate) must be greater than 500\n");
        psg_abort(1);
    }
    tXmix = roundoff(tXmix,taur);
    
    tRF = getval("tRF");


    /* NUS Pt.2 - Read-in NUS schedule file: '~/vnmrsys/nus_sched/hXXX_S.hdr_3' */
    {
        char sparse_file[MAXSTR],sparse[MAXSTR];
        char read_char[MAXSTR];
        char check_str[MAXSTR];
        int check_len;
        getstr("SPARSE",sparse);
        if(sparse[0]=='y') {
            if(ix==1) {
                int i;
                FILE *fsparse_table;
                getstr("sparse_file",sparse_file);
                fsparse_table=fopen(sparse_file,"r");
                if(NULL==fsparse_table) {
                    printf("File %s not found\n",sparse_file);
                    psg_abort(1);
                }
                d3const=d3;
                d2const=d2;
                for(i=0;i<ni*ni2;i++) {
                    if(i>=MAXFIDS) {
                        printf("Too many FIDs \n");
                        psg_abort(1);
                    }
                    // These methods ensure that a line in a schedule exactly follows the pattern "%d %d\n"
                    if(1!=fscanf(fsparse_table,"%d",(int*)sel+2*i)) {
                        printf("Line %d is malformed. Or you have too few rows in file. \n",i+1);
                        psg_abort(1);
                    }
                    fgets(read_char, 2, fsparse_table);
                    if(strcmp(read_char, " ")) {
                        printf("Line %d is malformed. The first digit is followed by \"%s\" rather than a space\n",i+1, read_char);
                        psg_abort(1);
                    }
                    if(1!=fscanf(fsparse_table,"%d",(int*)sel+2*i+1)) {
                        printf("Too few lines in %s\n",sparse_file);
                        psg_abort(1);
                    }
                    fgets(read_char, 2, fsparse_table);
                    if(strcmp(read_char, "\n")) {
                        printf("Line %d is malformed. It ends with \"%s\" rather than a newline\n",i+1, read_char);
                        psg_abort(1);
                    }
                    // Recreate the line from the digits that were read
                    sprintf(check_str, "%d %d\n", sel[i][0], sel[i][1]);
                    // measure the length of the recreated lien
                    check_len = strlen(check_str);
                    // rewind the file to the predicted beginning of the previous line
                    fseek(fsparse_table, -(check_len), SEEK_CUR);
                    // read that line
                    fgets(read_char, check_len+1, fsparse_table);
                    // compare the recreated line with the actual line to make sure they match
                    if(strcmp(check_str, read_char)) {
                        printf("Line %d is malformed.\n",i+1);
                        psg_abort(1);
                    }
                    
                }
                fclose(fsparse_table);
                printf("Sampling table was read from %s\n",sparse_file);
                printf("\nNUS sampling list (will be applied upon 'gos'):\n");
                for(i=0; i<ni*ni2; i++) {
                    printf("ni = %4d, d2 = %6.3fms, ni2 = %4d, d3 = %6.3fms\n",
                           sel[i][0],
                           1000*(d2const+sel[i][0]/sw1),
                           sel[i][1],
                           1000*(d3const+sel[i][1]/sw2)
                           );
                    if(tRF <= (d2const+sel[i][0]/sw1)+(d3const+sel[i][1]/sw2)) {
                        abort_message("ABORT: The maximum (d2+d3) must be less than tRF and tRF should be set to a safe value");
                    }
                }
            }
            
            /* CMR 12/17/2016 for NUS; for 2D planes must increment time every 2 rows, for 3D every 4 rows to enable hypercomplex sampling */
            if(ni==1 || ni2==1)
            {
                d2 = d2const+sel[(int)(ix-1)/2][0]/sw1;
                d3 = d3const+sel[(int)(ix-1)/2][1]/sw2;
            }
            else
            {
                d2 = d2const+sel[(int)(ix-1)/4][0]/sw1;
                d3 = d3const+sel[(int)(ix-1)/4][1]/sw2;
            }
        }
    }
    /* NUS Pt.2 - END */

    // N constant time calc
    double t3max, Ndelay1=0.0, Ndelay2=0.0;
    t3max = getval("t3max");
    if (t3max <= (getval("ni2")*1.0/getval("sw2")+2.0*pwY90)) {
        t3max = (roundoff(getval("ni2")*1.0/getval("sw2")/2.0,taur)+taur)*2.0;
    }
    if (pwY90 > 2.0*pwX90) {
        Ndelay1 = t3max/2.0 - pwY90 + d3/2.0;
        if (Ndelay1 < 0) {
            Ndelay1 = 0.0;
        }
        Ndelay2 = t3max/2.0 - pwY90 - d3/2.0;
        if (Ndelay2 < 0) {
            Ndelay2 = 0.0;
        }
    }
    else {
        Ndelay1 = t3max/2.0 - 2.0*pwX90 + d3/2.0; 
        if (Ndelay1 < 0) {
            Ndelay1 = 0.0; 
        }
        Ndelay2 = t3max/2.0 - 2.0*pwX90 - d3/2.0;
        if (Ndelay2 < 0) {
            Ndelay2 = 0.0;
        }
    }

    // Dutycycle Protection  CMR 8/7/18 must be calculatd after actual d3 value is computed by NUS schedule 
//(conservative calculation maybe double counting tRF and d3)
    if(!strcmp(ctN,"n")){

    duty = 4.0e-6 + getval("pwY90") + getval("pwH90") + getval("tHY") + d2 +
    getval("tYX") + d3 + 2.0*getval("pwX90") + getval("ad") +
    getval("rd") + at + getval("tRF");
    }
    else
    {
    duty = 4.0e-6 + getval("pwY90") + getval("pwH90") + getval("tHY") + d2 +
    getval("tYX") + t3max + 2.0*getval("pwX90") + getval("ad") +
    getval("rd") + at + getval("tRF");
    }

    double rf_time = duty;  // Save RF time for minimum d1 calculation
    duty = duty/(duty + d1 + 4.0e-6);
    // 5% duty cycle limit for C-detected sequences (high-power decoupling on X channel)
    // NOTE: Future enhancement should make duty cycle power-dependent:
    //   - High-power decoupling (>50 kHz): 5% limit (current conservative assumption)
    //   - Medium-power decoupling (20-50 kHz): could allow 7-10%
    //   - Low-power decoupling (<20 kHz): could allow 10-15%
    //   This requires integrating decoupling power and sequence type into duty cycle calculation
    if (duty > 0.05) {
        double min_d1 = (rf_time / 0.05) - rf_time - 4.0e-6;
        abort_message("Duty cycle %.1f%% exceeds 5%% limit. Increase d1 to at least %.3f s. Abort!\n",
                      duty*100, min_d1);
    }
    

    // Create Phasetables
    settable(phH90,16,table1); //Changed from 6 to 16
//    settable(phHhy,8,table2); //CGB commented out 12/6/21
    settable(phY90,8,table3);
    settable(phYhy,8,table4);
    settable(phHyx,8,table5);   
    settable(phY180,8,table24);
    settable(phCompX1,4,table31);
    settable(phCompX2,4,table32);
    settable(phCompY1,4,table33);
    settable(phCompY2,4,table34); 
    settable(phXspc5,8,table21);
    settable(phXspc5ref,8,table22);
    settable(phXmix2dqf,8,table23);
    settable(phXpar, 8, table35); 
    settable(ph90Xzf1, 16, table36); 
    settable(ph90Xzf2, 16, table37); 
    
    settable(phHhy,8,table40); 		//CGB added 11/28/21
    settable(phHstore,8,table41);		//CGB added 11/28/21
    settable(phHreadout,8,table42);		//CGB added 11/28/21
    settable(phHecho,8,table43);		//CGB added 11/28/21

    if (!strcmp(softpul, "n")) { 
        settable(phRec,8,table10);
        settable(phYyx,8,table6);
        settable(phXyx,8,table7);
        settable(phXmix1,8,table8);
        settable(phXmix2,8,table9);
    }
    if (!strcmp(softpul, "y")) {
        settable(phRec_soft,8,table20);
        settable(phYyx,8,table11);
        settable(phXyx,8,table12);
        settable(phXsoft,8,table15);
        settable(phXhard,8,table16);
        settable(phYhard,8,table17);
        settable(phXmix1,8,table18);
        settable(phXmix2,8,table19);
    }
    

    id2_ = (int) (d2 * getval("sw1") + 0.1);

    if ((phase1 == 1) || (phase1 == 2)) {
        if(!strcmp(softpul, "n")) {
            tsadd(phXyx,2*id2_,4);//invert the phases of the pre-evolution pulse and 
            tsadd(phRec,2*id2_,4);// the receiver for FAD to displace the axial peaks 
        }
        if(!strcmp(softpul, "y")) {
            tsadd(phRec_soft,2*id2_,4);// the receiver for FAD to displace the axial peaks 
            tsadd(phXyx,2*id2_,4);
            tsadd(phXsoft,2*id2_,4); 
            tsadd(phXhard,2*id2_,4); 
        } 

        //hypercomplex
        if (phase1 == 2 && !strcmp(softpul, "y")) {
            tsadd(phXyx,1,4); tsadd(phXsoft,1,4); tsadd(phXhard,1,4);
        }
        if (phase1 == 2 && !strcmp(softpul, "n")) {
            tsadd(phXyx,1,4);
        }
    } 

    id3_ = (int) (d3 * getval("sw2") + 0.1);   
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phYhy,2*id3_,4); 
        tsadd(phY90,2*id3_,4);
        if(!strcmp(softpul, "y")) {
            tsadd(phRec_soft,2*id3_,4);
        }
        else {
            tsadd(phRec,2*id3_,4);
        }
        if (phase2 == 2) {
            tsadd(phYhy,1,4); tsadd(phY90,1,4);
        }
    }
    
    // Begin Sequence

    // External trigger on channel 1 number 1
    splineon(1);
    delay(2.0e-6);
    splineoff(1);  

    if(!strcmp(softpul, "y")) {
        setreceiver(phRec_soft);
    }
    else {
        setreceiver(phRec);
    }

    txphase(phXyx); decphase(phH90); dec2phase(phY90);
    obspwrf(getval("aXyx")); decpwrf(getval("aH90")); dec2pwrf(getval("aY90"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

// T2  filter (Hahn Echo), add softpulse CGB added 11/28/21 

   if (!strcmp(hsel, "y")) { 
      decpwrf(getval("aHshp"));
      _pboxpulse(shpH,phH90);} //CGB changed shp to shpH to fix naming convention involving multiple shaped pulses 4/25/22
   else {
   decrgpulse(getval("pwH90"),phH90,0.0,0.0); }
   decphase(phHhy);

   if (!strcmp(echo, "y")) { 
   decphase(phHecho);
   decpwrf(aHecho);
     if (!strcmp(hsel, "y")) {
   int n = (int) ((getval("pwHshp") + 4.95e-6)/taur);
   if (n%2 != 0) n = n + 1;
   double soft1 = 0.5*((double) (n*taur - getval("pwHshp") - 5.0e-6));
   while (soft1 < 0.0) {
      n = n + 2;
      soft1 = 0.5*((double) (n*taur - getval("pwHshp") - 5.0e-6));
       			 }
   delay(fabs(tECHO/2.0-soft1-getval("pwH90")));} //compensate one rotor period for softpulse.
     else {delay((tECHO/2.0));}
   
   decrgpulse(pwHecho,phHecho,0.0,0.0);
  
   if((tECHO/2.0) > 0.1) {delay((tECHO/2.0));}
   else{delay(tECHO/2.0);}
   }

 
// H-H Mixing For H

  if (!strcmp(hMix, "y")) {
   decpwrf(getval("aH90"));   
   decrgpulse(getval("pwH90"),phHstore,0.0,0.0);
   delay(hmix);
   decrgpulse(getval("pwH90"),phHreadout,0.0,0.0);
   }
   decoff();

//

    // H to Y Cross Polarization with a Y Prepulse
    dec2phase(phYhy);
    dec2pwrf(getval("aYhy"));
//    decrgpulse(getval("pwH90"),phH90,0.0,0.0);
    decphase(phHhy);
    _cp_(hy,phHhy,phYhy);

    // F1 Indirect Period For Y
    if (!strcmp(decN, "y")) {
        decoffset(ofdecN);
    }

    if (!strcmp(ctN, "y")) {
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));

        _dseqon(dec);

        delay(Ndelay1);

        _dseqoff(dec); decpwrf(getval("aH90"));    
        sim3pulse(2.0*pwX90,2.0*pwY90,2.0*pwY90,phCompX1,zero,phY180,0.0,0.0);
        _dseqon(dec);
        delay(Ndelay2);
        _dseqoff(dec);
    }
    else {
        obspwrf(getval("aX90"));
        _dseqon(dec);
        if (d3 > 4.0*pwX90) {
            delay(d3/2.0-2.0*pwX90);
            rgpulse(pwX90,phCompX1,0.0,0.0);
            rgpulse(2.0*pwX90,phCompX2,0.0,0.0);
            rgpulse(pwX90,phCompX1, 0.0,0.0);
            delay(d3/2.0-2.0*pwX90);
        }
        else {
            delay(d3);
        }
        _dseqoff(dec);
    }

    if (!strcmp(decN, "y")) {
        decoffset(dof);
    }

    // Y to X Cross Polarization
    decphase(phHyx);
    dec2phase(phYyx);
    decpwrf(getval("aHyx"));
    dec2pwrf(getval("aYyx"));
    obspwrf(getval("aXyx"));
    decunblank(); decon();
    _cp_(yx,phYyx,phXyx);
    decphase(phHhy);
    decoff();

    // F2 Indirect Period for X

    _dseqon(dec); 
    if (!strcmp(softpul, "n")) {
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
    }
    if (!strcmp(softpul, "y")) {    
        delay(d2/2.0);
        _pboxpulse(shp1,phXsoft);
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));
        sim3pulse(2.0*pwX90,0.0,2.0*pwY90,phXhard,zero,phYhard,0.0,0.0);
        delay(d2/2.0);
    }
    _dseqoff(dec);

    // Mixing with SPC5 Recoupling
    if (!strcmp(mMix, "spc5")) {
        decpwrf(getval("aH90")); 
        decon();
        obspwrf(getval("aX90"));
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        obspwrf(getval("aXspc5"));
        xmtrphase(v1); txphase(phXmix1);

        delay(getval("tZF"));
        decoff();

        decpwrf(getval("aHmixspc5"));
        decon();
        _mpseq(spc5, phXspc5);

        if (!strcmp(dqf_flag,"y")) {
            xmtrphase(v2); txphase(phXmix2);
            _mpseq(spc5ref, phXspc5ref);
        }

        decoff();
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        decon();
        obspwrf(getval("aX90"));
        xmtrphase(zero); txphase(phXmix2);
        delay(getval("tZF"));
        decpwrf(getval("aH90")); 
        if (!strcmp(dqf_flag,"y")) {
            rgpulse(getval("pwX90"),phXmix2dqf,0.0,0.0);
        }
        else {
            rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        }
        decoff();
    }

    // Mixing with C7 Recoupling
    if (!strcmp(mMix, "c7")) {
        decpwrf(getval("aH90")); 
        decon();
        obspwrf(getval("aX90"));
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        obspwrf(getval("aXc7"));
        xmtrphase(v1); txphase(phXmix1);

        delay(getval("tZF"));
        decoff();

        decpwrf(getval("aHmixc7"));
        decon();
        _mpseq(c7, phXspc5);

        if (!strcmp(dqf_flag,"y")) {
            xmtrphase(v2); txphase(phXmix2);
            _mpseq(c7ref, phXspc5ref);
        }
        decoff();
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        decon();
        obspwrf(getval("aX90"));
        xmtrphase(zero); txphase(phXmix2);
        delay(getval("tZF"));
        decpwrf(getval("aH90")); 
        if (!strcmp(dqf_flag,"y")) {
            rgpulse(getval("pwX90"),phXmix2dqf,0.0,0.0);
        }
        else {
            rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        }
        decoff();
    }


    // RAD(DARR) Mixing For X

    if (!strcmp(mMix, "rad")) {
        decpwrf(getval("aH90"));
        decunblank(); decon();
        obspwrf(getval("aX90"));
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decpwrf(getval("aHmix"));
        delay(tXmix);
        decpwrf(getval("aH90"));
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
    }

    // PAR mixing
    if (!strcmp(mMix, "par") ) {
        decpwrf(getval("aH90")); 
        decphase(0);
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
        rgpulse(getval("pwX90"),ph90Xzf1,0.0,0.0);
        decoff();

		obsunblank();
        decpwrf(getval("aHpar"));
        obspwrf(getval("aXpar"));
        xmtrphase(phXpar);
        decon();
        xmtron();
        delay(getval("tPAR"));
        xmtroff();
        decoff();

        decpwrf(getval("aH90")); 
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),ph90Xzf2,0.0,0.0);
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
/*CGB needs to figure everything below here out. How to include the incrementation time for d4 and 
if we even want a d4 or if any changes are necessary here on out for the constant time aspects below. */
    // Begin Acquisition
    _dseqon(dec);
    obsblank(); _blank34();
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec);
    decphase(zero);
    decon();
    tRF = getval("tRF");

    if (tRF <= (getval("ni2")-1)/getval("sw2") + (ni-1)/getval("sw1")) {
        d3_ = (getval("ni2")-1)/getval("sw2") - d3;
        d2_ = (ni-1)/getval("sw1") - d2;
        if (d3_<0) d3_ = 0.0;
        if (d2_<0) d2_ = 0.0;
        if (!strcmp(ctN, "n")) {
            delay(d3_);
        } 
        delay(d2_);
    }
    else {
        if (!strcmp(ctN, "y")) {
            delay(tRF-d2-t3max);
        }
        else {
            delay(tRF-d2-d3);
        }
    }

    decoff();
    obsunblank(); decunblank(); _unblank34();
}

