/*  hXXX.c - A sequence to provide XXX homonuclear correlation with the option of using RAD, SPC5, POSTC7 or RFDR mixing
    LJS and MT 08/18/10   
    CMR 3/10/12: add soft pulse options in each indirect dimension
    CMR 3/14/12: build hXXhhX from hXXX version with soft pulses
    JMC 07/02/15: added PAR mixing option to both dimensions
    DWP 12/14/15: made into NUS version (see comments containing 'NUS' for additions)
*/

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"


/* NUS Pt.1 - Declare variables used for specifying d2 & d3 values (from example code, gnoesyNhsqc.c)*/
#define MAXFIDS 400000
static int sel[MAXFIDS][2];                                                             
static double d3const, d2const;                                     
/* NUS Pt.1 - END */


// Define Values for Phasetables
// Mei Hong's phase cycle JMR 2010 202, 203-210
// CMR 8/2/22 modify for nt=2 good for DARR/DARR only, preserve direct polarization
// hXXX_S_nt2.c is a prototype version only for this mode of operation
// refer back to hXXX_S.c as a default version in case troubles arise
// 8/6/22 modify to invert phH90, phX90, phRec to cancel T1 relaxation artifact for nt=1 3D
//static int table1[8] = {2,2,2,2,2,2,2,2};                           // phH90
static int table1[8] = {0,0,0,0,0,0,0,0};                           // phH90
// phH90 changed to constant 0 phase; also to be used for 13C 90 preceding CP
static int table2[8] = {1,1,1,1,1,1,1,1};                           // phHhx
//static int table38[8]= {1,1,2,2,3,3,0,0}; 			// phX90 initial DP
static int table38[8]= {3,3,0,0,1,1,2,2}; 			// phX90 initial DP
static int table3[8] = {0,0,1,1,2,2,3,3};                           // phXhx
static int table4[8] = {1,1,2,2,3,3,0,0};                           // phXmix1
static int table5[16]= {3,3,0,0,1,1,2,2,3,3,0,0,1,1,2,2};           // phXmix2
static int table10[16]={3,3,0,0,1,1,2,2,1,1,2,2,3,3,0,0};	        // phXmix3
static int table11[8] ={1,3,0,2,3,1,2,0};                           // phXmix4
// two step long mixing phase cycle, +z and -z to cancel T1 relaxation
//static int table6[16] ={2,0,1,3,0,2,3,1,0,2,3,1,2,0,1,3};           // phRec
static int table6[16] ={0,2,3,1,2,0,1,3,2,0,1,3,0,2,3,1};           // phRec
static int table7[8] = {0,0,0,0,0,0,0,0};                           // phXspc5
static int table8[8] = {0,1,0,1,0,1,0,1};                           // phXspc5ref
static int table9[16]= {3,1,0,2,1,3,2,0,3,1,0,2,1,3,2,0};           // phXmix2dq;
static int table15[8] = {0,1,0,1,3,2,3,2};                          // phXsoft 
static int table16[8] = {0,0,1,1,3,3,2,2};                          // phXhard 
static int table21[8] = {0,2,3,1,2,0,1,3};                          // phXhx for softpul='y'
static int table17[8] = {0,0,0,0,0,0,0,0};                          // phYhard 
static int table18[16]= {1,0,1,0,2,3,2,3,0,1,0,1,3,2,3,2};          // phXsoft2 
static int table19[8] = {1,1,0,0,2,2,3,3};                          // phXhard2 
static int table22[16]= {3,1,2,0,1,3,0,2,1,3,0,2,3,1,2,0};          // phXmix2 for softpul2='y';
static int table20[8] = {0,0,0,0,0,0,0,0};                          // phYhard2 
static int table31[4] = {0,0,0,0};                                  // phCompX1
static int table32[4] = {1,1,1,1};                                  // phCompX2
static int table33[4] = {0,0,0,0};                                  // phCompY1
static int table34[4] = {1,1,1,1};                                  // phCompY2
static int table35[8] = {0,0,0,0,0,0,0,0};                          // phXpar
static int table36[16] ={1,1,3,3,3,3,1,1,1,1,3,3,3,3,1,1};          // ph90Xzf1
static int table37[16] ={3,3,1,1,1,1,3,3,3,3,1,1,1,1,3,3};          // ph90Xzf2;

//{2,0,2,0} after phXmix2


#define phH90 t1
#define phHhx t2
#define phXhx t3
#define phXmix1 t4
#define phXmix2 t5
#define phRec t6
#define phXspc5 t7
#define phXspc5ref t8
#define phXmix2dq t9
#define phXmix3 t10
#define phXmix4 t11
#define phCompX1 t31
#define phCompX2 t32
#define phCompY1 t33
#define phCompY2 t34
#define phXsoft t15
#define phXhard t16
#define phYhard t17
#define phXsoft2 t18
#define phXhard2 t19
#define phYhard2 t20
#define phXhx_soft t21
#define phXmix2_soft t22
#define phXpar t35
#define ph90Xzf1 t36
#define ph90Xzf2 t37
#define phX90 t38

int id2_,id3_, xsel; /*during y evolution, xsel: -1---soft Pi only; +1---soft, then hard Pi; 0---neither*/

void pulsesequence() {

    check_array();

    // Define Variables and Objects and Get Parameter Values

    double tRF,d2_,d3_;

    double duty;

    CP hx = getcp("HX",0.0,0.0,0,1);
    strcpy(hx.fr,"dec");
    strcpy(hx.to,"obs");
    putCmd("frHX='dec'\n");
    putCmd("toHX='obs'\n");

    char mMix[MAXSTR];
    char mMix1[MAXSTR];

    int ni2 = getval("ni2");   /* used for NUS */ 
   
    getstr("mMix",mMix);   
    getstr("mMix1",mMix1);   

    char softpul[MAXSTR];
    getstr("softpul",softpul);
    char softpul2[MAXSTR];
    getstr("softpul2",softpul2);

    MPSEQ spc5 = getspc5("spc5X",0,0.0,0.0,0,1);
    MPSEQ spc5ref = getspc5("spc5X",spc5.iSuper,spc5.phAccum,spc5.phInt,1,1); 
    strcpy(spc5.ch,"obs");
    putCmd("chXspc5='obs'\n");

    MPSEQ c7 = getpostc7("c7X",0,0.0,0.0,0,1);
    MPSEQ c7ref = getpostc7("c7X",c7.iSuper,c7.phAccum,c7.phInt,1,1);
    strcpy(c7.ch,"obs");
    putCmd("chXc7='obs'\n");

    MPSEQ rfdr = getrfdrxy8("rfdrX",0,0.0,0.0,0,1); 
    strcpy(rfdr.ch,"obs");
    putCmd("chXrfdr='obs'\n");

   MPSEQ spcn = getspcn("spcnX",0,0.0,0.0,0,1);
   MPSEQ spcnref = getspcn("spcnX",spcn.iSuper,spcn.phAccum,spcn.phInt,1,1); 
   strcpy(spcn.ch,"obs");
   putCmd("chXspcn='obs'\n");

   int NXspcn, qXspcn;
   NXspcn = (int)getval("NXspcn"); 
   qXspcn = (int)getval("qXspcn"); 

   if (qXspcn % NXspcn != 0) {
      printf("ABORT: qXspcn should be a multiple of NXspcn\n");
      psg_abort(1);
   }

    PARIS paris = getparis("H");

    DSEQ dec = getdseq("H");

    double pwX90,pwY90;
    pwX90 = getval("pwX90");
    pwY90 = getval("pwY90");

    char spc5_flag[MAXSTR];
    getstr("spc5_flag",spc5_flag);  //1 is normalmixing 2 is dqf 
    char c7_flag[MAXSTR];
    getstr("c7_flag",c7_flag);  //1 is normalmixing 2 is dqf 
    char dqf_flag[MAXSTR];
    getstr("dqf_flag",dqf_flag);  //1 is normalmixing 2 is dqf 

    PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);
    PBOXPULSE shp2 = getpboxpulse("shp2X",0,1);

    // Set Mixing Period to N Rotor Cycles 

    double taur=1,tXmix,tXmix1,srate;
    tXmix1 =  getval("tXmix1"); 
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
    tXmix1 = roundoff(tXmix1,taur);

    // Dutycycle Protection

   if (!strcmp(mMix, "rad")) {
       duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 +
              tXmix + tXmix1 + 2.0*getval("pwX90") + getval("ad") + getval("rd") + at;
       duty = duty/(duty + d1 + 4.0e-6);
   }
   else if (!strcmp(mMix, "c7")) {
       duty = 4.0e-6 + getval("pwH90") + getval("tHX") + 2.0*getval("tZF") + c7.t + 
              d2+ tXmix1 + c7ref.t + getval("ad") + getval("rd") + at;
       duty = duty/(duty + d1 + 4.0e-6 + 2.0*getval("tZF"));
   }
   else if (!strcmp(mMix, "spc5")) {
       duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 +
              2.0*getval("pwX90") + tXmix1 + getval("ad") + getval("rd") + at;
       duty = duty/(duty + d1 + 4.0e-6);
   }

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
                    printf("i = %5d, ni = %4d, d2 = %6.3fms, ni2 = %4d, d3 = %6.3fms\n",
                        i,
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


    // Create Phasetables

    settable(phH90,8,table1);
    settable(phHhx,8,table2);
    //  settable(phXhx,8,table3); see below
    settable(phX90,8,table38);
    settable(phXmix1,8,table4);
    settable(phRec,16,table6);
    settable(phXspc5,8,table7);
    settable(phXmix3,16,table10);
    settable(phXmix4,8,table11);

    settable(phCompX1,4,table31);
    settable(phCompX2,4,table32);
    settable(phCompY1,4,table33);
    settable(phCompY2,4,table34); 

    settable(phXpar, 8, table35); 
    settable(ph90Xzf1, 16, table36); 
    settable(ph90Xzf2, 16, table37); 

    if (!strcmp(softpul, "n")) {
        settable(phXhx,8,table3);
    }
    if (!strcmp(softpul, "y")) {  
        settable(phXsoft,8,table15);
        settable(phXhard,8,table16);
        settable(phYhard,8,table17);
        settable(phXhx,8,table21);
    }
    if (!strcmp(softpul2, "y")) {  
        settable(phXsoft2,16,table18);
        settable(phXhard2,8,table19);
        settable(phYhard2,8,table20);
    }

    if ( (!strcmp(spc5_flag,"2") && !strcmp(mMix, "spc5")) || (!strcmp(c7_flag,"2") && !strcmp(mMix, "c7")) || (!strcmp(dqf_flag,"2") && !strcmp(mMix, "spcn")) ) {
        settable(phXspc5ref,8,table8);
        settable(phXmix2,16,table9);
    }
    else {
            settable(phXspc5ref,8,table7);
        if (!strcmp(softpul2, "y")) {
            settable(phXmix2,16,table22);
        }
        else {
            settable(phXmix2,16,table5);
        }
    }

    // Set tPAR to n * pwX360
    double tPAR;
    tPAR = getval("tPAR");
    tPAR = roundoff(tPAR,4.0*getval("pwX90")*4095/getval("aXpar"));

    id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4); /* invert the phases of the CP pulse and */
        tsadd(phXmix3,2*id2_,4); /* the receiver for FAD to displace the axial peaks */
    }

    if (phase1 == 2) {tsadd(phXmix3,3,4);}

    // tsadd(phXhx,1,4) was used previously
    // add 270 degrees to storage pulse to effectively have States-TPPI 
    // but this also avoids phase twisting on resonance due to T1 relaxation during t1 evol.

    id3_ = (int) (d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,2*id3_,4); /* invert the phases of the CP pulse and */
        tsadd(phXmix1,2*id3_,4); /* the receiver for FAD to displace the axial peaks */
    }  
    if (phase2 == 2) {tsadd(phXmix1,3,4);}

    setreceiver(phRec);

    // Begin Sequence

    // External trigger on channel 1 number 1
    splineon(1);
    delay(2.0e-6);
    splineoff(1);

    txphase(phX90); decphase(phH90);
    obspwrf(getval("aXhx")); decpwrf(getval("aH90"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    // H to X Cross Polarization

//    decrgpulse(getval("pwH90"),phH90,0.0,0.0);
    sim3pulse(pwX90,getval("pwH90"),0,phX90,phH90,0,0,0);
    decphase(phHhx); txphase(phXhx);
    _cp_(hx,phHhx,phXhx);

    // F1 Indirect Period for X

    obspwrf(getval("aX90"));

    _dseqon(dec); 
    if (!strcmp(softpul, "n")) {
        if (d3 > 4.0*pwY90) {
            dec2pwrf(getval("aY90"));
            delay(d3/2.0-2.0*pwY90);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            dec2rgpulse(2.0*pwY90,phCompY2,0.0,0.0);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            delay(d3/2.0-2.0*pwY90);
        }
        else {
            delay(d3);
        }
    }
    if (!strcmp(softpul, "y")) {
        delay(d3/2.0);
        _pboxpulse(shp1,phXsoft);
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));
        sim3pulse(2.0*pwX90,0.0,2.0*pwY90,phXhard,zero,phYhard,0.0,0.0);
        delay(d3/2.0);
    }

    _dseqoff(dec);

    // Mixing with SPC5 Recoupling

    if (!strcmp(mMix, "spc5")) {
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        decon();
        obspwrf(getval("aX90"));
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        obspwrf(getval("aXspc5"));
        xmtrphase(v1); txphase(phXmix1);

        delay(getval("tZF"));
        decoff();

        decpwrf(getval("aHmixspc5"));
        decon();
        _mpseq(spc5, phXspc5);

        if (!strcmp(spc5_flag,"2")) {
            xmtrphase(v2); txphase(phXmix2);
            _mpseq(spc5ref, phXspc5ref);
        }
        decoff();
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        decon();
        obspwrf(getval("aX90"));
        xmtrphase(zero); txphase(phXmix2);
        delay(getval("tZF"));
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
    }



    // Mixing with C7 Recoupling
    if (!strcmp(mMix, "c7")) {
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        obspwrf(getval("aXc7"));
        xmtrphase(v1); txphase(phXmix1);
        delay(getval("tZF"));

        decoff();
        decpwrf(getval("aHmixc7"));
        decunblank();
        decon();
        _mpseq(c7, phXspc5);
        decoff();
        decpwrf(getval("aHmixc7"));
        decunblank();
        if (!strcmp(c7_flag,"2")) {
            decon();
            _mpseq(c7ref, phXspc5ref);
            decoff();
        }
        obspwrf(getval("aX90"));
        xmtrphase(zero); txphase(phXmix2);
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        decon();
        delay(getval("tZF"));
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        decoff();
    }

// Mixing with SPCN Recoupling

   if (!strcmp(mMix, "spcn")) {
       decpwrf(getval("aHZF"));
       decon();
       obspwrf(getval("aX90"));
       rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
       obspwrf(getval("aXspcn"));
       txphase(phXmix1);
       delay(getval("tZF"));

       decoff();
       decpwrf(getval("aHmixspcn"));
       decon();
       _mpseq(spcn, phXspc5);
       if (!strcmp(dqf_flag,"2")) {
          _mpseq(spcnref, phXspc5ref);
       }
       decoff();
       decpwrf(getval("aHZF"));
       decon();
       obspwrf(getval("aX90"));
       xmtrphase(zero); txphase(phXmix2);
       delay(getval("tZF"));
       rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
       decoff();
  }

    // RAD(DARR) Mixing For X

    if (!strcmp(mMix, "rad")) {
        decpwrf(getval("aHmix"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        delay(tXmix);
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
    }
    decoff();

    // PARIS Mixing For X
    if (!strcmp(mMix, "paris")) {
        decpwrf(getval("aH90"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decpwrf(getval("aHparis"));
        _paris(paris);
        delay(tXmix);
        decprgoff();
        decpwrf(getval("aH90"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
    }
    decoff();

    //RFDR
    if (!strcmp(mMix, "rfdr")) {
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

    // PAR mixing
    if (!strcmp(mMix, "par") ) {
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
        rgpulse(getval("pwX90"),ph90Xzf1,0.0,0.0);
        decoff();

        obsunblank();
        decpwrf(getval("aHpar"));
        obspwrf(getval("aXpar"));
        txphase(phXpar);
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

    // F2 Indirect Period for X

    obspwrf(getval("aX90"));
    _dseqon(dec); 
    if (!strcmp(softpul2, "n")) {
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
    if (!strcmp(softpul2, "y")) {
        delay(d2/2.0);
        _pboxpulse(shp2,phXsoft2);
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));
        sim3pulse(2.0*pwX90,0.0,2.0*pwY90,phXhard2,zero,phYhard2,0.0,0.0);
        delay(d2/2.0);
    }
    _dseqoff(dec);

    // RAD(DARR) Mixing For X
    if (!strcmp(mMix1, "rad")) {
        decpwrf(getval("aHmix"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix3,0.0,0.0);
        delay(tXmix1);
        rgpulse(getval("pwX90"),phXmix4,0.0,0.0);
        decoff();
    }
    // PAR mixing
    if (!strcmp(mMix1, "par") ) {
        decpwrf(getval("aH90")); 
        obspwrf(getval("aX90"));
        decon();
        rgpulse(getval("pwX90"),phXmix3,0.0,0.0);
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

        decpwrf(getval("aHpar1"));
        obspwrf(getval("aXpar1"));
        decon();
        rgpulse(getval("tPAR1"),phXpar,0.0,0.0);
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
        rgpulse(getval("pwX90"),phXmix4,0.0,0.0);
        decoff();
        decpwrf(getval("aH90")); 
    }

    // Begin Acquisition

    _dseqon(dec);

    obsblank(); _blank34();
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec);

    decon();

    if (tRF > d2+d3) {
        delay(tRF-d2-d3);
    }

    decoff();
    obsunblank(); decunblank(); _unblank34();
}

