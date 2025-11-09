/*hXYXX_4D_S.c  DCP with F1 13C, F2 15N and F3 13C

AJN UIUC Rienstra Group 12/15/2009 from 
dcptan3drad D.Rice 10/12/05
DWP 2/21/17: add 4th dimension to hXYX.c code
DWP 5/11/17: add NUS to code (remember to use setup nus macro ("sunus") for this code)
DWP 7/10/17: add SPCn to code
DWP 7/21/17: add softpul2 to code              
CMR 7/26/17: improved phase cycle for soft pulses; added pwX180 for pi pulse (sometimes not quite 2*pwX90)
CMR 7/26/17 6 pm:  substantially modified phase cycle optimized primarily for soft pulses on; may not be optimal with soft pulses off (and I don't care)
CMR 7/24/24: Now I care. */

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"

/* NUS Pt.1 - Declare variables used for specifying d2 & d3 values and dimensionality */
#define MAXFIDS 400000
#define MAXDIM 4 /* maximum dimensionality for experiment */
static int sel[MAXFIDS][MAXDIM-1];
static double d4const, d3const, d2const;                                     
/* NUS Pt.1 - END */

// probably CMR
// 7/26/24 4 am fixing DC offset issue with hard pulse version; need spin temp alternation on H for scans 1-2 and Y for scans 3-4

// Define Values for Phasetables
//static int table1[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};  // phH90
static int table1[32] = {0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0};  // phH90
static int table2[8]  = {3,3,3,3,3,3,3,3};   // phHhx
static int table4[8]  = {0,0,0,0,0,0,0,0};   // phXhx
static int table5[8]  = {1,1,1,1,1,1,1,1};   // phHxy (not part of phase cycle)
static int table6[8]  = {0,0,0,0,0,0,0,0};   // phXxy 
static int table17[8] = {0,0,1,1,0,0,1,1};   //*phXsoft 
static int table18[8] = {1,1,1,1,1,1,1,1};   //*phXhard  12/24/22 shift from 0 to 1 CMR
// 7/26/24 CMR
//static int table7[8]  = {0,0,0,0,0,0,0,0};   // phYxy
static int table7[8]  = {0,0,2,2,0,0,2,2};   // phYxy
static int table8[8]  = {3,3,3,3,3,3,3,3};   // phHyx (not part of phase cycle)
static int table9[16] = {0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2};   // phYyx (invert scans 9 through 16 to setup CP condition CMR 4/14/23 
static int table10[8] = {1,1,1,1,3,3,3,3};   //*phXyx
static int table11[8] = {0,0,0,0,0,0,0,0};   // phXmix1
static int table12[8] = {1,1,1,1,1,1,1,1};   // phXmix2
static int table51[8] = {0,0,0,0,0,0,0,0};   // phXspc5 (not intended to be ... part of phase cycle)  CMR 7/26/17 00002222 cause of methyl twist?
static int table52[8] = {0,0,1,1,0,0,1,1};   // phXspc5ref (not part of phase cycle)
static int table53[8] = {0,1,2,3,2,3,0,1};   // phXmix2dqf (not implemented properly)
static int table19[8] = {0,0,0,0,0,0,0,0};   // phYhard (not part of phase cycle)
static int table54[8] = {0,1,0,1,0,1,0,1};   //*phXsoft2 
static int table55[16]= {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};   //*phXhard2 
static int table56[8] = {0,0,0,0,0,0,0,0};   // phYhard2 (not part of phase cycle)
// BDZ an older declaration
//static int table20[32]= {0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,2,2,2,2};   // phRec
// 7/26/24 CMR now change all recr in pattern 0220 on top of previous. 
//static int table20[32]= {2,2,2,2,0,0,0,0, 0,0,0,0,2,2,2,2, 0,0,0,0,2,2,2,2, 2,2,2,2,0,0,0,0};  // phRec  7/24 modulate 9-16 fixed!
static int   table20[32]= {2,0,0,2,0,2,2,0, 0,2,2,0,2,0,0,2, 0,2,2,0,2,0,0,2, 2,0,0,2,0,2,2,0};  // phRec  7/24 modulate 9-16 fixed!
//static int table21[32]= {0,0,2,2,2,2,0,0, 2,2,0,0,0,0,2,2, 2,2,0,0,0,0,2,2, 0,0,2,2,2,2,0,0};  // phRecsoft1
static int   table21[32]= {0,2,0,2,2,0,2,0, 2,0,2,0,0,2,0,2, 2,0,2,0,0,2,0,2, 0,2,0,2,2,0,2,0};  // phRecsoft1
//static int table22[32]= {2,0,2,0,0,2,0,2, 2,0,2,0,0,2,0,2, 0,2,0,2,2,0,2,0, 0,2,0,2,2,0,2,0};  // phRecsoft2
static int   table22[32]= {2,2,0,0,0,0,2,2, 2,2,0,0,0,0,2,2, 0,0,2,2,2,2,0,0, 0,0,2,2,2,2,0,0};  // phRecsoft2
//static int table23[32]= {0,2,2,0,2,0,0,2, 0,2,2,0,2,0,0,2, 2,0,0,2,0,2,2,0, 2,0,0,2,0,2,2,0};  // phRecsoft12
static int   table23[32]= {0,0,0,0,2,2,2,2, 0,0,0,0,2,2,2,2, 2,2,2,2,0,0,0,0, 2,2,2,2,0,0,0,0};  // phRecsoft12 

// 7/26/24 CMR: note that with spin temp alternation on H and Y, soft12 phase cycle will be suboptimal in 4 scans
// may need to define two separate logical cases for phH90 and phHxy to enable optimal performance for both use cases

static int table13[4] = {0,0,0,0};           // phCompX1 (not part of phase cycle)
static int table14[4] = {1,1,1,1};           // phCompX2 (not part of phase cycle)
static int table15[4] = {0,0,0,0};           // phCompY1 (not part of phase cycle)
static int table16[4] = {1,1,1,1};           // phCompY2 (not part of phase cycle)

#define phH90 t1
#define phHhx t2
#define phXhx t4
#define phHxy t5
#define phXxy t6
#define phXsoft t17
#define phXhard t18
#define phYxy t7
#define phHyx t8
#define phYyx t9
#define phXyx t10
#define phXmix1 t11
#define phXmix2 t12
#define phYhard t19
#define phXsoft2 t54
#define phXhard2 t55
#define phYhard2 t56
#define phRec t20
#define phRecsoft1 t21
#define phRecsoft2 t22
#define phRecsoft12 t23
#define phXspc5 t51
#define phXspc5ref t52
#define phXmix2dqf t53
#define phCompX1 t13
#define phCompX2 t14
#define phCompY1 t15
#define phCompY2 t16

void pulsesequence() {

   check_array();

// Define Variables and Objects and Get Parameter Values

   double duty,dutyon;
   double tRF,d2_,d3_,d4_;

/* NUS Pt.2 - define used NUS variables */
   int ni = getval("ni");   /* used for NUS */ 
   int ni2 = getval("ni2");   /* used for NUS */ 
   int ni3 = getval("ni3");   /* used for NUS */ 
   double sw1 = getval("sw1");
   double sw2 = getval("sw2");
   double sw3 = getval("sw3");
   int ndim = getval("ndim");   /* used for NUS */ 
   int nrows = getval("nrows");   /* used for NUS */ 
   int skiprows = getval("skiprows"); /* used for NUS concatenation CMR 7/1/22 */
/* NUS Pt.2 - END */

   CP hx = getcp("HX",0.0,0.0,0,1);
   strcpy(hx.fr,"dec");
   strcpy(hx.to,"obs");
   putCmd("frHX='dec'\n"); 
   putCmd("toHX='obs'\n");

   CP xy = getcp("XY",0.0,0.0,0,1);
   strcpy(xy.fr,"obs");
   strcpy(xy.to,"dec2");
   putCmd("frYX='obs'\n");
   putCmd("toYX='dec2'\n");

   CP yx = getcp("YX",0.0,0.0,0,1);
   strcpy(yx.fr,"dec2");
   strcpy(yx.to,"obs");
   putCmd("frYX='dec2'\n");
   putCmd("toYX='obs'\n");

   char softpul[MAXSTR];
   getstr("softpul",softpul);

   PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);

   char softpul2[MAXSTR];
   getstr("softpul2",softpul2);

   PBOXPULSE shp2 = getpboxpulse("shp2X",0,1);

   DSEQ dec = getdseq("H");
   DSEQ decsoft = getdseq("Hsoft");
   
   char mMix[MAXSTR];
   getstr("mMix",mMix);   

   char dqf_flag[MAXSTR];
   getstr("dqf_flag",dqf_flag);  //1 is normalmixing 2 is dqf

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
   pwX180= getval("pwX180");

// Set Mixing Period to N Rotor Cycles

   double taur,tXmix,srate;
   tXmix =  getval("tXmix");
   srate =  getval("srate");
   taur = 0.0;
   if (srate >= 500.0)
      taur = roundoff((1.0/srate), 0.125e-6);
   else {
      printf("ABORT: Spin Rate (srate) must be greater than 500\n");
      psg_abort(1);
   }
   tXmix = roundoff(tXmix,taur);



/* NUS Pt.3 - Read-in NUS schedule file: '~/vnmrsys/nus_sched/XYXX_4D_S.hdr_3' */
    char sparse_file[MAXSTR],sparse[MAXSTR];
    char read_char[MAXSTR];
    char check_str[MAXSTR];
    char tmp_str[MAXSTR];
    int check_len;
    int inc_max;
    int i, j;
 /*   int *dim_counts[MAXDIM-1] = {&ni, &ni2, &ni3};  */
    getstr("SPARSE",sparse);
    if(sparse[0]=='y') {
        int active_dims[MAXDIM-1];
        int nis_correct = 1;
        int first_ni_checked = 0;
 
 /*  next several lines unnecessary and cause many errors; use previous NUS method developed by CMR/DWP v1. Set nrows=ni*ni2 or otherwise as desired  */ 
 /*  what we do instead of making some large ni2 value equal to nrows... set active_dims values by brute force */
 	first_ni_checked = 1;
 	if(ni>0){active_dims[0]=1;}
 	if(ni2>0){active_dims[1]=1;}
 	if(ni3>0){active_dims[2]=1;}
 		       
/*        for(j=0; j < MAXDIM-1; j++) {
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
        
        */
        
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
                                printf("Line %d is malformed.\n",i+1);
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
    }
    /* NUS Pt.3 - END */

// Dutycycle Protection

//   dutyon = 4.0e-6 + getval("pwY90") + getval("pwH90") + getval("tHX") + d4 +
//            getval("tYX") + d3 + getval("tXY") + d2 + 2.0*getval("pwX90") + getval("ad") +
//             getval("rd") + at;
// bad version below
   dutyon = 4.0e-6 + getval("pwY90") + getval("pwH90") + getval("tHX") +
             getval("tYX") + getval("tXY") + 2.0*getval("pwX90") + getval("ad") +
             getval("rd") + at + d2 + d3 + d4;
   printf("d2 is %.6f s: ",d2);
   printf("d3 is %.6f s: ",d3);
   printf("d4 is %.6f s: ",d4); 
  
   printf("\ndutyon is %.1f ms ",dutyon*1000);
   double rf_time = dutyon;  // Save RF time for minimum d1 calculation
   duty = dutyon/(dutyon + d1 + 4.0e-6);
   printf("duty is %.1f%%  ",duty*100);
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
  else {      printf("Duty cycle %.1f%% <5%%. All good!\n", duty*100);  }


// Create Phasetables 

   settable(phH90,32,table1);
   settable(phHhx,8,table2);
   settable(phXhx,8,table4);
   settable(phHxy,8,table5);
   settable(phYxy,8,table6);
   settable(phXsoft,8,table17);
   settable(phXhard,8,table18);
   settable(phXxy,8,table7);
   settable(phHyx,8,table8);
   settable(phYyx,16,table9);
   settable(phXyx,8,table10);
   settable(phXmix1,8,table11);
   settable(phXmix2,8,table12);
   settable(phXspc5,8,table51);
   settable(phXspc5ref,8,table52);
   settable(phXmix2dqf,8,table53);
   settable(phYhard,8,table19);
   settable(phXsoft2,8,table54);
   settable(phXhard2,16,table55);
   settable(phYhard2,8,table56);
   settable(phCompX1,4,table13);
   settable(phCompX2,4,table14);
   settable(phCompY1,4,table15);
   settable(phCompY2,4,table16);

   settable(phRec,32,table20);
   settable(phRecsoft1,32,table21);
   settable(phRecsoft2,32,table22);
   settable(phRecsoft12,32,table23);
    
   int id2_,id3_,id4_;

   id2_ = (int) (d2 * getval("sw1") + 0.1);   
   if ((phase1 == 1) || (phase1 == 2)) {
     tsadd(phXyx,2*id2_,4); 
     tsadd(phRec,2*id2_,4); 
     tsadd(phRecsoft1,2*id2_,4); 
     tsadd(phRecsoft2,2*id2_,4); 
     tsadd(phRecsoft12,2*id2_,4); 
     if (phase1 == 2) {tsadd(phXyx,1,4);}
   }

   id3_ = (int) (d3 * getval("sw2") + 0.1);
   if ((phase2 == 1) || (phase2 == 2)) {
     tsadd(phYxy,2*id3_,4); 
     tsadd(phRec,2*id3_,4); 
     tsadd(phRecsoft1,2*id3_,4); 
     tsadd(phRecsoft2,2*id3_,4); 
     tsadd(phRecsoft12,2*id3_,4); 
     if (phase2 == 2) {tsadd(phYxy,1,4);}
   }

   id4_ = (int) (d4 * getval("sw3") + 0.1);   
   if ((phase3 == 1) || (phase3 == 2)) {
     tsadd(phXhx,2*id4_,4); 
     tsadd(phRec,2*id4_,4); 
     tsadd(phRecsoft1,2*id4_,4); 
     tsadd(phRecsoft2,2*id4_,4); 
     tsadd(phRecsoft12,2*id4_,4); 
     if (phase3 == 2) {tsadd(phXhx,1,4);}
   }

  if ((!strcmp(softpul, "n")) && (!strcmp(softpul2, "n")))
    {setreceiver(phRec);}
  if ((!strcmp(softpul, "y")) && (!strcmp(softpul2, "n")))
    {setreceiver(phRecsoft1);}
  if ((!strcmp(softpul, "n")) && (!strcmp(softpul2, "y")))
    {setreceiver(phRecsoft2);}
  if ((!strcmp(softpul, "y")) && (!strcmp(softpul2, "y")))
    {setreceiver(phRecsoft12);}
   
// Begin Sequence

   txphase(phXyx); decphase(phH90);
   obspwrf(getval("aXyx")); decpwrf(getval("aH90")); dec2pwrf(getval("aY90"));
   obsunblank(); decunblank(); _unblank34();
   delay(d1);
   sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

// H to X Cross Polarization 

/*   obsphase(phXhx); not sure if we need; seems to be causing trouble */
   obspwrf(getval("aXhx"));

   decrgpulse(getval("pwH90"),phH90,0.0,0.0);
   decphase(phHhx);
   _cp_(hx,phHhx,phXhx);

// F1 Indirect Period For X

   _dseqon(dec); 

   if (!strcmp(softpul, "n"))
   {
   	if (d4 > 4.0*pwY90) 
	{
       		dec2pwrf(getval("aY90"));
                delay(d4/2.0-2.0*pwY90);
       		dec2rgpulse(pwY90,phCompY1,0.0,0.0);
       		dec2rgpulse(2.0*pwY90,phCompY2,0.0,0.0);
       		dec2rgpulse(pwY90,phCompY1,0.0,0.0);
       		delay(d4/2.0-2.0*pwY90);
 	}
   	else delay(d4);
   }
   if (!strcmp(softpul, "y"))
   {       
        delay(d4/2.0);
	_dseqoff(dec);
	_dseqon(decsoft);
	_pboxpulse(shp1,phXsoft);
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));
	_dseqoff(decsoft);
  	sim3pulse(pwX180,2*pwY90,2.0*pwY90,phXhard,zero,phYhard,0.0,0.0);
	_dseqon(dec);
       	delay(d4/2.0);
   }

	_dseqoff(dec);


// X to Y Cross Polarization

   decphase(phHxy); dec2phase(phYxy);
   decpwrf(getval("aHxy"));
   decunblank(); decon();
   decphase(phHxy);
   _cp_(xy,phXxy,phYxy);
   decoff();

// F2 Indirect Period for Y

   obspwrf(getval("aX90"));
   _dseqon(dec);
   if (d3 > 4.0*pwX90) {
       delay(d3/2.0-2.0*pwX90);
       rgpulse(pwX90,phCompX1,0.0,0.0);
       rgpulse(2.0*pwX90,phCompX2,0.0,0.0);
       rgpulse(pwX90,phCompX1, 0.0,0.0);
       delay(d3/2.0-2.0*pwX90);
}
   else delay(d3);
   _dseqoff(dec);

// Y to X Cross Polarization

   decphase(phHyx); dec2phase(phYyx);
   decpwrf(getval("aHyx"));
   decunblank(); decon();
   decphase(phHyx);
   _cp_(yx,phYyx,phXyx);
   decoff();

// F3 Indirect Period for X (DWP 2/21/17) 

    obspwrf(getval("aX90"));
    _dseqon(dec); 
    if (!strcmp(softpul2, "n")) {		// softpul2 obtained from hXXX_original F2
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
        _dseqoff(dec);
        _dseqon(decsoft);
        _pboxpulse(shp2,phXsoft2);
        obspwrf(getval("aX90"));
        dec2pwrf(getval("aY90"));
        _dseqoff(decsoft);				
        sim3pulse(pwX180,2.0*pwY90,2.0*pwY90,phXhard2,zero,phYhard2,0.0,0.0);   
// or?  sim3pulse(2.0*pwX90,2*pwY90,2.0*pwY90,phXhard2,zero,phYhard2,0.0,0.0);
		_dseqon(dec);				
        delay(d2/2.0);
    }
    _dseqoff(dec);

// Mixing with SPCN Recoupling (DWP 7/10/17)

    if (!strcmp(mMix, "spcn")) {
        decpwrf(getval("aH90")); 
        decon();
        obspwrf(getval("aX90"));
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        decpwrf(getval("aHZF")); //AJN 031510 control aHzf
        obspwrf(getval("aXspcn"));
//        xmtrphase(v1); txphase(phXmix1);

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
//        xmtrphase(zero); txphase(phXmix2);
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
        obspwrf(getval("aX90"));
        decpwrf(getval("aHmix"));
        decunblank(); decon();
        rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
        delay(tXmix);
        rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
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

   decphase(zero);

   decon();

   tRF = getval("tRF");
   if(sparse[0]=='y') {
       if (tRF > d2+d3+d4) {
           delay(tRF-d2-d3-d4);
       }
   }
   else {
       if (tRF <= (getval("ni3")-1)/getval("sw3") + (getval("ni2")-1)/getval("sw2") + (getval("ni")-1)/getval("sw1"))
          {
          d4_ = (getval("ni3")-1)/getval("sw3") - d4;
          d3_ = (getval("ni2")-1)/getval("sw2") - d3;
          d2_ = (getval("ni")-1)/getval("sw1") - d2;
          if (d4_<0) d4_ = 0.0;
          if (d3_<0) d3_ = 0.0;
          if (d2_<0) d2_ = 0.0;
          delay(d4_); delay(d3_); delay(d2_);
          } 
    else if(tRF-d2-d3-d4 > 0) {delay(tRF-d2-d3-d4);}
    }

   decoff();
  
   obsunblank(); decunblank(); _unblank34();
}

