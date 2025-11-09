/*  hYXX.c  tangent ramped hNCC 3D with specific cp 
    and several C-C mixing schemes
    AJN 11/13/09 from dcp2tan3drad.c  D.Rice 10/12/05    
    MT 3/29/11 add dqf_flag in SPC5 and C7 
    AJN, MT 5/31/11 change ofdecN during d3
    DWP 1/10/17: made into NUS version (see comments containing 'NUS' for additions)
    DWP 2/7/17: add general spc-n mixing
    CMR 8/6/18-8/7/18:  cleanup code
	1.  remove composite pwH90 pulses (which also saves memory for tables 39, 40, 41 etc.)
	2.  cleanup phase cycle lists (put in the logical order, remove unnecessary tables)
	3.  fix ctN mode timings, including case for SPARSE='y' (note that CT timing calcs. must come after NUS timing calcs.)
	4.  fix tRF operational mode so that this value can remain the same for both CT modes, and if desired t3max arrayed to measure N T2'
	5.  test phase cycles with NMR... AUG18/200to299 around records 230-240
    SW 5/8/22: change the hard pulse in soft-hard echo from pwX180 and aX180 to 2*pwX90 and aX90 (line 569-571) to be consistent with the non-NUS version
CONCLUSIONS:
1.  Phase cycles in all 4 cases give expected amount of signal with proper pulse widths and powers
2.  If only one pi pulse option is on (either ctN='y' OR softpul='y'), then nt=4 should be an acceptable phase cycle for 2D or 3D exps
3.  If only one option is on, but mixing time is long (comparable to 13C T1), nt=8 is recommended
4.  If both options are on (ctN='y' and softpul='y'), nt=4 might work but nt=8 is recommended phase cycle 
5.  If both options are on, and mixing time is long (comparable to 13C T1), nt=16 is strongly recommended.
6.  Be sure to calibrate pwY90 with aY90, pwX180 with aX180 and pwXshp1 with aXshp1.
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
// CMR 8/6/18 reorganizing phase tables in order used, and adjusting for proper relative receiver phase (right hand convention)
static int table1[8]  = {0,0,0,0,0,0,0,0};                          // phH90
static int table2[8]  = {3,3,3,3,3,3,3,3};                          // phHhy
static int table4[8]  = {0,0,0,0,0,0,0,0};                          // phYhy    always start with magnetization along X for 15N

static int table5[8]  = {0,0,0,0,0,0,0,0};                          // phHyx
static int table6[8]  = {0,2,0,2,0,2,0,2};                          // phYyx    steps 1,2 always to select for NC xfer
static int table7[8]  = {0,0,0,0,2,2,2,2};                          // phXyx	steps 5-8 spin temp 13C for long mixing
static int table8[16] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};          // phXmix1
static int table9[8]  = {1,1,2,2,1,1,2,2};                          // phXmix2  steps 3,4 remove quad image direct dim.
static int table10[8] = {0,2,1,3,2,0,3,1};                          // phRec

static int table11[8] = {0,2,0,2,0,2,0,2};                          // phYyx_soft (for softpul='y')
static int table12[8] = {0,0,0,0,0,0,0,0};                          // phXyx_soft
static int table15[8] = {0,0,1,1,0,0,1,1};                          // phXsoft   soft pi pulse on 13C, spin ping these two pi's scans 3-8
static int table16[8] = {0,0,0,0,1,1,1,1};                          // phXhard   hard pi pulse following 13C soft pi pulse
static int table17[8] = {0,0,0,0,0,0,0,0};                          // phYhard   simultaneous 15N pi with 13C hard pi pulse
static int table18[16]= {3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1};          // phXmix1_soft  use 16 steps if soft pulse and long mixing
static int table19[8] = {1,1,2,2,1,1,2,2};                          // phXmix2_soft
static int table20[16]= {0,2,3,1,2,0,1,3,2,0,1,3,0,2,3,1};          // phRec_soft

static int table24[16]= {0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0};        // phY180_CT (for ctN='y').  select every 4 scans, pref. 8 step, 16 for long mix
static int table39[16]= {0,2,3,1,2,0,1,3,2,0,1,3,0,2,3,1};        // phRec_CT

static int table40[16]= {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0};	  // phY180_CTsoft (for ctN='y' and softpul='y')
static int table38[16]= {0,2,3,1,0,2,3,1,0,2,3,1,0,2,3,1};        // phRec_CTsoft

static int table21[8] = {0,0,0,0,2,2,2,2};                          // phXspc5   self-contained DQF option (no need to change phRec)  
static int table22[8] = {0,0,1,1,2,2,3,3};                          // phXspc5ref  probably this should be named "phXspc5dqf" but I will leave for now CMR 8/6/18
static int table23[8] = {2,2,1,1,0,0,3,3};                          // phXmix2dqf;
static int table31[4] = {0,0,0,0};                                  // phCompX1
static int table32[4] = {1,1,1,1};                                  // phCompX2
static int table33[4] = {0,0,0,0};                                  // phCompY1
static int table34[4] = {1,1,1,1};                                  // phCompY2
static int table35[8] = {0,0,0,0,0,0,0,0};                          // phXpar
static int table36[16] ={1,1,3,3,3,3,1,1,1,1,3,3,3,3,1,1};          // ph90Xzf1
static int table37[16] ={3,3,1,1,1,1,3,3,3,3,1,1,1,1,3,3};          // ph90Xzf2;

static int table42[8] = {2,2,2,2,2,2,2,2};                          // phHsl
static int table43[8] = {1,1,1,1,1,1,1,1};                          // phHflip


#define phH90 t1
#define phHhy t2
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
#define phHsl t42
#define phHflip t43

#define phRec_CTsoft t38
#define phRec_CT t39

void pulsesequence() {

    // check for phase,phase2 array bug
    
    check_array();

    // Define Variables and Objects and Get Parameter Values

    double duty;
    double tRF,d2_,d3_;
    int id2_,id3_;

    int ni2 = getval("ni2");   /* used for NUS */ 
    int ni2max_s = getval("ni2max_s");
    double t3max, Ndelay_a=0.0, Ndelay_b=0.0, Ndelay_c=0.0; // CMR 8/4/18 X chn pi pulse must be halfway through evolution period
    t3max = getval("t3max"); 

    if(t3max <= (ni2max_s/getval("sw2"))){
		    	abort_message("ABORT: Please check that t3max is long enough.");
    }

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

// CMR 11/1/2018 the next 4 lines enable (in theory) offset of the H decoupling during N evolution; not sure if this ever worked
//  but we are going to leave it here just in case removing it would break other things (see also around line 529)
    char decN[MAXSTR];
    getstr("decN",decN);

    double dof = getval("dof");
    double ofdecN = getval("ofdecN");

    DSEQ dec = getdseq("H");
    DSEQ dec2 = getdseq("Y");
    DSEQ dec_soft = getdseq("Hsoft");

    PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);

    char ctN[MAXSTR];
    getstr("ctN",ctN);

    char softpul[MAXSTR];
    getstr("softpul",softpul);

    char mMix[MAXSTR];
    getstr("mMix",mMix);   

    char dqf_flag[MAXSTR];
    getstr("dqf_flag",dqf_flag);  //1 is normalmixing 2 is dqf

    MPSEQ spc5 = getspc5("spc5X",0,0.0,0.0,0,1);
    MPSEQ spc5ref = getspc5("spc5X",spc5.iSuper,spc5.phAccum,spc5.phInt,1,1); 
    strcpy(spc5.ch,"obs");
    putCmd("chXspc5='obs'\n");

    MPSEQ c7 = getpostc7("c7X",0,0.0,0.0,0,1);
    MPSEQ c7ref = getpostc7("c7X",c7.iSuper,c7.phAccum,c7.phInt,1,1);
    strcpy(c7.ch,"obs");
    putCmd("chXc7='obs'\n");

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

    double pwX90,pwY90,pwX180;
    pwX90 = getval("pwX90");
    pwY90 = getval("pwY90");
    pwX180 = getval("pwX180");




    // Set Mixing Period to N Rotor Cycles
    double taur,tXmix,srate;
    tXmix =  getval("tXmix");
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
                    // measure the length of the recreated line
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

   if (!strcmp(ctN, "y")) 
     {
		    if(tRF <= (d2const+sel[i][0]/sw1)+t3max)
		     {
		    	abort_message("ABORT: The maximum (d2+t3max) must be less than tRF and tRF should be set to a safe value");
		     }

     }
   else
     {
		    if(tRF <= (d2const+sel[i][0]/sw1)+(d3const+sel[i][1]/sw2)) 
		     {
		    	abort_message("ABORT: The maximum (d2+d3) must be less than tRF and tRF should be set to a safe value");
		     }
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

    // N constant time calc (MUST BE after NUS calculations!)  CMR 8/7/18
    if (t3max <= (getval("ni2")*1.0/getval("sw2")+2.0*pwY90+pwX180)) {
        t3max = (roundoff(getval("ni2")*1.0/getval("sw2")/2.0,taur)+taur)*2.0;
    }   // CMR 8/7/18 protection added for case when initial d3 value is non-zero, so CT interval will be long enough

    if(!strcmp(ctN,"y")){
    Ndelay_a = t3max/2.0 - pwY90 - d3/2.0 ;
    Ndelay_b = t3max/2.0 - pwY90 - pwX180 ;
    Ndelay_c = d3/2.0  ;
    if (Ndelay_a < 0) {  
		    	abort_message("ABORT: t3max must be greater than ni2/sw2 + d3 (recommend d3=0 in most cases)");
		     }
    if (Ndelay_b < 0) {  Ndelay_b = 0.0; }
    if (Ndelay_c < 0) {  Ndelay_c = 0.0; }
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

    duty = duty/(duty + d1 + 4.0e-6);
    // 5% duty cycle limit for C-detected sequences (high-power decoupling on X channel)
    // NOTE: Future enhancement should make duty cycle power-dependent:
    //   - High-power decoupling (>50 kHz): 5% limit (current conservative assumption)
    //   - Medium-power decoupling (20-50 kHz): could allow 7-10%
    //   - Low-power decoupling (<20 kHz): could allow 10-15%
    //   This requires integrating decoupling power and sequence type into duty cycle calculation
    if (duty > 0.05) {
        printf("Duty cycle %.1f%% >5%%. Abort!\n", duty*100);
	abort_message("ABORT: duty cycle error");
    }
    


    // Create Phasetables

    settable(phH90,8,table1);
    settable(phHsl,8,table42);
    settable(phHflip,8,table43);

    settable(phHhy,8,table2);
    settable(phYhy,8,table4);
    settable(phHyx,8,table5);   
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

    if (!strcmp(softpul, "n")) { 
        settable(phRec,8,table10);
        settable(phYyx,8,table6);
        settable(phXyx,8,table7);
        settable(phXmix1,16,table8);
        settable(phXmix2,8,table9);
	settable(phRec_CT,16,table39);
        if(!strcmp(ctN,"y")){
         settable(phY180,16,table24);
        }
    }
    if (!strcmp(softpul, "y")) {
        settable(phRec_soft,16,table20);
        settable(phYyx,8,table11);
        settable(phXyx,8,table12);
        settable(phXsoft,8,table15);
        settable(phXhard,8,table16);
        settable(phYhard,8,table17);
        settable(phXmix1,16,table18);
        settable(phXmix2,8,table19);
        settable(phRec_CTsoft,16,table38);
        if(!strcmp(ctN,"y")){
         settable(phY180,16,table40);
        }
    }

    
    // Set tPAR to n * pwX360
    double tPAR;
    tPAR = getval("tPAR");
    tPAR = roundoff(tPAR,4.0*getval("pwX90")*4095/getval("aXpar"));

    id2_ = (int) (d2 * getval("sw1") + 0.1);

    if ((phase1 == 1) || (phase1 == 2)) {
        if(!strcmp(softpul, "y")) {
            tsadd(phRec_soft,2*id2_,4);// the receiver for FAD to displace the axial peaks 
            tsadd(phXyx,2*id2_,4);
            tsadd(phXsoft,2*id2_,4); 
            tsadd(phXhard,2*id2_,4); 
            tsadd(phRec_CTsoft,2*id2_,4); // CMR 8/3/18 
       } 
// if(!strcmp(softpul, "n"))
        else {
            tsadd(phXyx,2*id2_,4);//invert the phases of the pre-evolution pulse and 
            tsadd(phRec,2*id2_,4);// the receiver for FAD to displace the axial peaks 
	    tsadd(phRec_CT,2*id2_,4); // CMR 8/4/18
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
        if(!strcmp(softpul, "y")) {
            tsadd(phRec_soft,2*id3_,4);
            tsadd(phRec_CTsoft,2*id3_,4); // CMR 8/3/18 
       }
        else {
            tsadd(phRec,2*id3_,4);
	    tsadd(phRec_CT,2*id3_,4); // CMR 8/4/18
        }
        if (phase2 == 2) {
            tsadd(phYhy,1,4); 
        }
 
    }

    // Begin Sequence

    // External trigger on channel 1 number 1
    splineon(1);
    delay(2.0e-6);
    splineoff(1);  

    if(!strcmp(ctN,"n"))
     {
      if(!strcmp(softpul, "y")) { setreceiver(phRec_soft);    }
      else 			{ setreceiver(phRec);         }
     }
    else
     {
      if(!strcmp(softpul, "y")) { setreceiver(phRec_CTsoft);    }
      else 			{ setreceiver(phRec_CT);         }
     }

    txphase(phXyx); decphase(phH90); dec2phase(0.0);
    obspwrf(getval("aXyx")); decpwrf(getval("aH90")); dec2pwrf(getval("aY90"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    // H to Y Cross Polarization with a Y Prepulse
    dec2phase(phYhy);
    dec2pwrf(getval("aYhy"));

    decrgpulse(getval("pwH90"),phH90,0.0,0.0);

    decphase(phHhy);
    _cp_(hy,phHhy,phYhy);

    // F1 Indirect Period For Y

    if (!strcmp(ctN, "y")) {
        obspwrf(getval("aX180"));
        dec2pwrf(getval("aY90"));

        _dseqon(dec);

        delay(Ndelay_a);
        dec2rgpulse(2.0*pwY90,phY180,0.0,0.0);
	delay(Ndelay_b);
	rgpulse(pwX180,0,0,0);
        delay(Ndelay_c);

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

    if (!strcmp(softpul, "n")) {
        _dseqon(dec); 
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
    }
    if (!strcmp(softpul, "y")) {    
        _dseqon(dec);
        delay(d2/2.0);
        _dseqoff(dec);
        _dseqon(dec_soft);
        _pboxpulse(shp1,phXsoft);
        obspwrf(getval("aX180"));
        dec2pwrf(getval("aY90"));
        sim3pulse(pwX180,0.0,2.0*pwY90,phXhard,zero,phYhard,0.0,0.0);
        _dseqoff(dec_soft);
        _dseqon(dec);
        delay(d2/2.0);
        _dseqoff(dec);
    }


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

        if (!strcmp(dqf_flag,"2")) {
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
        if (!strcmp(dqf_flag,"2")) {
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

        if (!strcmp(dqf_flag,"2")) {
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
        if (!strcmp(dqf_flag,"2")) {
            rgpulse(getval("pwX90"),phXmix2dqf,0.0,0.0);
        }
        else {
            rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
        }
        decoff();
    }

    // Mixing with SPCN Recoupling
    if (!strcmp(mMix, "spcn")) {
        decpwrf(getval("aH90")); 
        decon();
        obspwrf(getval("aX90"));
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        obspwrf(getval("aXspcn"));
        xmtrphase(v1); txphase(phXmix1);

        delay(getval("tZF"));
        decoff();

        decpwrf(getval("aHmixspcn"));
        decon();
        _mpseq(spcn, phXspc5);

        if (!strcmp(dqf_flag,"2")) {
            xmtrphase(v2); txphase(phXmix2);
            _mpseq(spcnref, phXspc5ref);
        }

        decoff();
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        decon();
        obspwrf(getval("aX90"));
        xmtrphase(zero); txphase(phXmix2);
        delay(getval("tZF"));
        decpwrf(getval("aH90")); 
        if (!strcmp(dqf_flag,"2")) {
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

        decpwrf(getval("aHpar"));
        obspwrf(getval("aXpar"));
        decon();
        rgpulse(getval("tPAR"),phXpar,0.0,0.0);
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

    // Begin Acquisition
    _dseqon(dec);
    _dseqon(dec2);
    obsblank();
    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec);
    _dseqoff(dec2);
    decphase(zero);
    decon();

   if (!strcmp(ctN, "y")) 
   {
    if (tRF > (d2+t3max))
	{
        delay(tRF-d2-t3max);
        }
    }
   else
    {
      if (tRF > (d2+d3)) 
	{  
        delay(tRF-d2-d3);
        }
    }

    decoff();
    obsunblank(); decunblank(); dec2unblank();
}

