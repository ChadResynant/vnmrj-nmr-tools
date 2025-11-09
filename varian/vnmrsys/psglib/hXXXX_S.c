/*  hXXXX.c - A sequence to provide XXXX homonuclear correlation with the option of using RAD, SPC5, POSTC7 or RFDR mixing
    LJS and MT 08/18/10   
    CMR 3/10/12: add soft pulse options in each indirect dimension
    CMR 3/14/12: build hXXhhX from hXXX version with soft pulses
    JMC 07/02/15: added PAR mixing option to both dimensions
    DWP 12/14/15: made into NUS version (see comments containing 'NUS' for additions)
    CGB ~March 2025 + July 8 2025: constructed 4D code from hXXX_S
    CMR 7/9/25: (1) added various duty cycle protections (also in setup macro); (2) rewrote full phase cycle
*/

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"


/* NUS Pt.1 - Declare variables used for specifying d2 & d3 values (from example code, gnoesyNhsqc.c)*/
#define MAXFIDS 400000
#define MAXDIM 4 /* maximum dimensionality for experiment */
static int sel[MAXFIDS][MAXDIM-1];
static double d4const, d3const, d2const;    
double duty ;                                 
/* NUS Pt.1 - END */

// Define Values for Phasetables CMR 7/9/25 starting over from first principles
static int table1[8]   = {0,2,0,2,0,2,0,2};                           // phH90: spin temp alternation essential 
static int table2[8]   = {1,1,1,1,1,1,1,1};                           // phHhx
static int table3[8]   = {0,0,1,1,2,2,3,3};                           // phXhx
static int table41[16] = {3,3,0,0,1,1,2,2, 3,3,0,0,1,1,2,2};           // phXmix5  storage pulse A dim  = phXhx - 1
static int table42[16] = {1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1};           // phXmix6  readout pulse A dim  = readout to X
static int table4[16]  = {3,3,3,3,3,3,3,3, 1,1,1,1,1,1,1,1};           // phXmix1  storage pulse Z dim  = phXmix6 + 2, alternate every 8 scans for relax during long mix
static int table5[16]  = {1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1};           // phXmix2  readout pulse Z dim  = readout to X
static int table10[16] = {3,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3};	        // phXmix3  storage pulse Y dim
static int table11[16] = {1,2,3,0,1,2,3,0, 1,2,3,0,1,2,3,0};           // phXmix4  readout pulse Y dim  = with phH90 spin temp, determines phRec
static int table6[16]  = {0,3,2,1,0,3,2,1, 2,1,0,3,2,1,0,3};           // phRec

// CMR 7/9/25: soft pulse options will not work properly at this stage
static int table7[8] =  {0,0,0,0,0,0,0,0};                           // phXspc5
static int table8[8] =  {0,1,0,1,0,1,0,1};                           // phXspc5ref
static int table9[16]=  {3,1,0,2,1,3,2,0,3,1,0,2,1,3,2,0};           // phXmix2dq;
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

// added for hXXXX_S
static int table47[16] ={3,3,1,1,1,1,3,3,3,3,1,1,1,1,3,3};           // ph90Xzf3;
static int table43[4] = {0,0,0,0};                                   // phCompZ1
static int table44[4] = {1,1,1,1};                                   // phCompZ2

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

// added for hXXXX_S
#define phXmix5 t41
#define phXmix6 t42
#define ph90Xzf3 t47
#define phCompZ1 t43
#define phCompZ2 t44

int id2_,id3_,id4_, xsel; /*during y evolution, xsel: -1---soft Pi only; +1---soft, then hard Pi; 0---neither*/

void pulsesequence() {
    // Define Variables and Objects and Get Parameter Values
    double tRF,d2_,d3_,d4_;

/* NUS Pt.2 - define used NUS variables */
   int ni = getval("ni");   /* used for NUS */ 
   int ni2 = getval("ni2");   /* used for NUS */ 
   int ni3 = getval("ni3");   /* used for NUS */ 
   double sw1 = getval("sw1");
   double sw2 = getval("sw2");
   double sw3 = getval("sw3");
   int ndim = getval("ndim");   /* used for NUS  */ 
   int nrows = getval("nrows");   /* used for NUS; must be equal to ni*ni2*ni3 to work properly; need to address this later CMR 7/9/25 */ 
   nrows=ni*ni2*ni3; /* should we do this or another solution? */
   int skiprows = getval("skiprows"); /* used for NUS concatenation CMR 7/1/22 */

/* NUS Pt.2 - END */
    CP hx = getcp("HX",0.0,0.0,0,1);
    strncpy(hx.fr,"dec",3);
    strncpy(hx.to,"obs",3);
    putCmd("frHX='dec'\n");
    putCmd("toHX='obs'\n");

    char mMix[MAXSTR];
    char mMix1[MAXSTR];
    char mMix2[MAXSTR];

    getstr("mMix",mMix);   
    getstr("mMix1",mMix1);   
    getstr("mMix2",mMix2);   // 

    char softpul[MAXSTR];
    getstr("softpul",softpul);
    char softpul2[MAXSTR];
    getstr("softpul2",softpul2);

    MPSEQ spc5 = getspc5("spc5X",0,0.0,0.0,0,1);
    MPSEQ spc5ref = getspc5("spc5X",spc5.iSuper,spc5.phAccum,spc5.phInt,1,1); 
    strncpy(spc5.ch,"obs",3);
    putCmd("chXspc5='obs'\n");

    MPSEQ c7 = getpostc7("c7X",0,0.0,0.0,0,1);
    MPSEQ c7ref = getpostc7("c7X",c7.iSuper,c7.phAccum,c7.phInt,1,1);
    strncpy(c7.ch,"obs",3);
    putCmd("chXc7='obs'\n");

    MPSEQ rfdr = getrfdrxy8("rfdrX",0,0.0,0.0,0,1); 
    strncpy(rfdr.ch,"obs",3);
    putCmd("chXrfdr='obs'\n");

	MPSEQ spcn = getspcn("spcnX",0,0.0,0.0,0,1);
	MPSEQ spcnref = getspcn("spcnX",spcn.iSuper,spcn.phAccum,spcn.phInt,1,1); 
	strncpy(spcn.ch,"obs",3);
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

    double taur=1,tXmix,tXmix1,tXmix2,srate;
    tXmix2 =  getval("tXmix2"); //
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
    tXmix2 = roundoff(tXmix2,taur);

    tRF = getval("tRF");

/* NUS Pt.3 - Read-in NUS schedule file: '~/vnmrsys/nus_sched/XYXX_4D_S.hdr_3' */
    char sparse_file[MAXSTR],sparse[MAXSTR];
    char read_char[MAXSTR];
    char check_str[MAXSTR];
    char tmp_str[MAXSTR];
    int check_len;
    int inc_max;
    int i, j;
    getstr("SPARSE",sparse);
    if(sparse[0]=='y') {
        int active_dims[MAXDIM-1];
        int nis_correct = 1;
        int first_ni_checked = 0;
 	first_ni_checked = 1;
 	if(ni>0){active_dims[0]=1;}
 	if(ni2>0){active_dims[1]=1;}
 	if(ni3>0){active_dims[2]=1;}         
        printf("active dims [%d %d %d]\n", active_dims[0], active_dims[1], active_dims[2]);  
        
        int dim;
        if(ix==1) 
        {
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

/* CMR 7/1/22 skiprows */
	    for(i=0; i < skiprows; i++) { fgets(read_char, 4*(MAXDIM-1), fsparse_table); }
	    

            for(i=0; i < nrows; i++) 
            {
                dim = 1;
                for(j = 0; j < MAXDIM-1; j++) 
                {
                    if(active_dims[j]) 
                    {
                        if(1 != fscanf(fsparse_table, "%d", &(sel[i][j]))) 
                        {
                            printf("Line %d is malformed. Or you have too few rows in file. \n",i+1);
                            psg_abort(1);
                        }
                        fgets(read_char, 2, fsparse_table);
                        if(dim < ndim - 1) 
                        {
                            if(strcmp(read_char, " ")) 
                            {
                                printf("Line %d is malformed. Need each desired dim to have a non-zero ni/ni2/ni3 value.\n",i+1);
                                psg_abort(1);
                            }
                        }
   /*     //                else {
        //                    if(strcmp(read_char, "\n")) {
        //                       printf("Line %d is too long. (\"%s\")\n",i+1,read_char);
        //                       psg_abort(1);
        //                    }
                        } */
                        dim = dim + 1;
                    }
                    else 
                    {
                        sel[i][j] = 0;
                    }
                }
                dim = 0;
                memset(check_str, 0, sizeof(check_str));
                for(j = 0; j < MAXDIM-1; j++) 
                {
                    if(active_dims[j]) 
                    {
                        sprintf(tmp_str, "%d", sel[i][j]);
                        if(dim > 0) {
                            strcat(check_str, " ");
                        }
                        strcat(check_str, tmp_str);
                        dim = dim + 1;
                    }
                }
                strcat(check_str, "\n");

                // measure the length of the recreated line
                check_len = strlen(check_str);
                // rewind the file to the predicted beginning of the previous line
                fseek(fsparse_table, -(check_len), SEEK_CUR);
                // read that line
                fgets(read_char, check_len+1, fsparse_table);
                // compare the recreated line with the actual line to make sure they match
    /*            if(strcmp(check_str, read_char)) {
                    printf("Line %d is malformed.\n",i+1);
                    printf("Expected: \"%s\"\n", check_str);
                    printf("Got: \"%s\"\n", read_char);
                    psg_abort(1);
                }  */
            }
             
            fclose(fsparse_table);
            printf("sparse_file: %s\n",sparse_file);
            printf("skipping this many rows: %4d ",skiprows);
            printf("\nNUS sampling list (will be applied upon 'gos'):\n");
/*            printf("The outputs are now actually in ms CMR 6/21/22\n"); */
            printf(" row#   ni  d2(ms) ni2  d3(ms) ni3  d4(ms) \n");
            printf("-----------------------------------------\n");
            for(i = 0; i < nrows; i++) {
                printf("%5d %4d %6.3f %4d %6.3f %4d %6.3f\n", i+1, sel[i][0], (d2const+sel[i][0]/sw1)*1000., sel[i][1], (d3const+sel[i][1]/sw2)*1000., sel[i][2], (d4const+sel[i][2]/sw3)*1000.);
            }
            printf("-----------------------------------------\n");
            printf(" row#   ni  d2(ms) ni2  d3(ms) ni3  d4(ms) \n");
            printf("-----------------------------------------\n");

        }

        /* for each dimension, set the dwell according to the current ix value */
        d2 = d2const+sel[(int)((ix-1)/pow(2,ndim-1))][0]/sw1;
        d3 = d3const+sel[(int)((ix-1)/pow(2,ndim-1))][1]/sw2;
        d4 = d4const+sel[(int)((ix-1)/pow(2,ndim-1))][2]/sw3;
        printf("d2 = %.6f, d3 = %.6f, d4 = %.6f \n",d2,d3,d4);
    }
    /* NUS Pt.3 - END */


    // Dutycycle Protection  CMR 7/9/25 add d3 and d4, actual DARR power deposition calc., and other safeties
    tRF = getval("tRF");
   if (!strcmp(mMix, "rad")) {
       duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 + d3 + d4 + tRF + (getval("aHmix")/4095) * (getval("aHmix")/4095) * (tXmix + tXmix1 + tXmix2) +
              2.0*getval("pwX90") + getval("ad") + getval("rd") + at;
       duty = duty/(duty + d1 + tXmix + tXmix1 + tXmix2 + 4.0e-6);
   }
   else if (!strcmp(mMix, "c7")) {
       duty = 4.0e-6 + getval("pwH90") + getval("tHX") + 2.0*getval("tZF") + c7.t + d3 + d4 + tRF + 
              d2+ tXmix1 + c7ref.t + getval("ad") + getval("rd") + at;
       duty = duty/(duty + d1 + 4.0e-6 + 2.0*getval("tZF"));
   }
   else if (!strcmp(mMix, "spc5")) {
       duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 + d3 + d4 + tRF +
              2.0*getval("pwX90") + tXmix1 + getval("ad") + getval("rd") + at;
       duty = duty/(duty + d1 + 4.0e-6);
   }

    if (duty > 0.05) {
        abort_message("Duty cycle %.1f%% >5%%. Check d1, d2, d3, d4, tRF, at, tXmix, tXmix1, tXmix2. Abort!\n", duty*100);
    }
    else {
        printf("Duty cycle %.1f%% < 5%%. Safe so far (only row 1 is calculated by dps). \n", duty*100);
        } 
    if( d2 > 10e-3) { abort_message("d2 = %.6f which is > 10 ms. Aborting!",d2); } 
    if( d3 > 10e-3) { abort_message("d3 = %.6f which is > 10 ms. Aborting!",d3); }
    if( d4 > 10e-3) { abort_message("d4 = %.6f which is > 10 ms. Aborting!",d4); }


    // Create Phasetables

    settable(phH90,8,table1);
    settable(phHhx,8,table2);
    //  settable(phXhx,8,table3); see below
    settable(phXmix1,16,table4);
    settable(phRec,16,table6);
    settable(phXspc5,8,table7);
    settable(phXmix3,16,table10);
    settable(phXmix4,16,table11);
    settable(phXmix5,16,table41);
    settable(phXmix6,16,table42);
    

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

	// hXXXX  CMR 7/9/25 fixed logic to use phase3 and phXmix5 here
    id4_ = (int) (d4 * getval("sw3") + 0.1);
    if ((phase3 == 1) || (phase3 == 2)) {
        tsadd(phRec,2*id4_,4); /* invert the phases of the CP pulse and */
        tsadd(phXmix5,2*id4_,4); /* the receiver for FAD to displace the axial peaks */
    }  
    if (phase3 == 2) {tsadd(phXmix5,3,4);}



    // check for phase,phase2,phase3 array bug
	// hXXXX
    char arraycheck[MAXSTR];
    getstr("array",arraycheck);
    printf("array=%s\n",arraycheck);
    if( !strcmp(arraycheck,"phase,phase2,phase3") || !strcmp(arraycheck,"phase,phase3,phase2") || !strcmp(arraycheck,"phase2,phase3,phase") || !strcmp(arraycheck,"phase2,phase,phase3") || !strcmp(arraycheck,"phase3,phase,phase2"))
    	{ abort_message("change array to phase3,phase2,phase for 4D exp\n");}  
    if( !strcmp(arraycheck,"phase3,phase2,phase") && ((ni==0) || (ni2==0) || (ni3==0))) { abort_message("all ni's must be non-zero to run 4D");}
// CMR 7/10/25 need a trap here for making sure ndim is set properly according to the phase array. If you run with array='phase3,phase2,phase' but ndim=3, it won't work
    if( !strcmp(arraycheck,"phase3,phase2,phase") && (ndim != 4) ) { abort_message("ndim must be = 4 for 4D exp");}

// CMR 7/9/25 may want to add other error checking for various 3D and 2D cases but this is tedious so leaving for later
//    if( !strcmp(arraycheck,"phase,phase3") || !strcmp(arraycheck,"phase,phase3") || !strcmp(arraycheck,"phase2,phase3,phase") || !strcmp(arraycheck,"phase2,phase,phase3") || !strcmp(arraycheck,"phase3,phase,phase2"))
//    	{ abort_message("change array to phase3,phase for AZX test exp\n");}  
//    if( !strcmp(arraycheck,"phase,phase2,phase3") || !strcmp(arraycheck,"phase,phase3,phase2") || !strcmp(arraycheck,"phase2,phase3,phase") || !strcmp(arraycheck,"phase2,phase,phase3") || !strcmp(arraycheck,"phase3,phase,phase2"))
//    	{ abort_message("change array to phase3,phase2,phase for 4D exp\n");}  

    setreceiver(phRec);

    // Begin Sequence

    // External trigger on channel 1 number 1
    splineon(1);
    delay(2.0e-6);
    splineoff(1);

    txphase(phXhx); decphase(phH90);
    obspwrf(getval("aXhx")); decpwrf(getval("aH90"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    // H to X Cross Polarization

    decrgpulse(getval("pwH90"),phH90,0.0,0.0);
    decphase(phHhx);
    _cp_(hx,phHhx,phXhx);

    // F4 d4 sw3 phXmix5 (A dim of 4D) Indirect Period for X nucleus

    obspwrf(getval("aX90"));

    _dseqon(dec); 
    if (!strcmp(softpul, "n")) {
        if (d4 > 4.0*pwY90) {
            dec2pwrf(getval("aY90"));
            delay(d4/2.0-2.0*pwY90);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            dec2rgpulse(2.0*pwY90,phCompY2,0.0,0.0);
            dec2rgpulse(pwY90,phCompY1,0.0,0.0);
            delay(d4/2.0-2.0*pwY90);
        }
        else {
            delay(d4);
        }
    }
    if (!strcmp(softpul, "y")) {
        delay(d4/2.0);
        _pboxpulse(shp1,phXsoft);
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));
        sim3pulse(2.0*pwX90,0.0,2.0*pwY90,phXhard,zero,phYhard,0.0,0.0);
        delay(d4/2.0);
    }

    _dseqoff(dec);

    // RAD(DARR) Mixing For X

    if (!strcmp(mMix, "rad")) {
        decpwrf(getval("aHmix"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix5,0.0,0.0);
        delay(tXmix);
        rgpulse(getval("pwX90"),phXmix6,0.0,0.0);
    }
    decoff();


    // F2 Indirect Period for Z

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

    // RAD(DARR) Mixing For X

    if (!strcmp(mMix, "rad")) {
        decpwrf(getval("aHmix"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        delay(tXmix1);
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
    }
    decoff();    
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

    decoff();
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
        delay(tXmix2);
        rgpulse(getval("pwX90"),phXmix4,0.0,0.0);
        decoff();
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

    if (tRF > d2+d3+d4) {
        delay(tRF-d2-d3-d4);
    }

    decoff();
    obsunblank(); decunblank(); _unblank34();
}

