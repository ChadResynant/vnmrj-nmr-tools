/*hYXH.c - A sequence to obtain an YXH correlation with H detection using MISSISSIPPI,
                    HY cp, t1 Y evolution H dec
		    YX cp, t2 X evolution H dec
		    XH cp, potential HH rfdr mixing
		    H acq with X dec. 		    

		    no Y-Y dream mixing yet but will add

                    from hXH.c  AJN 03/29/10    
		    based on hYXH.c DHZ 02/07 for inova   
CMR 7/3/25 merging NUS capabilities into this code         */

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"

// Define Values for Phasetables
// CMR phase cycle adjustments 3/24 to reduce ringdown at nt=2 and improve solvent suppression
// focusing changes on the version with soft pulse present on C (softpul='y')
// should also work for softpul='n' but this is the lower priority
// v2: put spin temp on H every nt=2. much better.
// v3: constant phase on phHpxy
// v4: invert phHpxy every nt=4
// v5: invert phYyx every nt=4
// v6: no inversion on phHpxy
static int table1[4] =  {0,2,0,2};          // phH90 v2: spin temp nt=2
static int table2[2] =  {1,1};          // phHhy
static int table3[2] =  {0,0};          // phYhy
static int table18[8] = {0,1,0,1,1,0,1,0};      // phYsoft
static int table19[8] = {0,0,0,0,1,1,1,1};      // phYhard
static int table4[2] =  {3,3};          // ph1Y90 store 
static int table17[2] = {1,1};          // ph2Y90 recover
static int table5[8] =  {0,0,2,2,0,0,2,2};          // phYyx
static int table6[2] =  {0,0};          // phXyx
static int table7[2] =  {1,1};          // ph1X90 store
static int table8[2] =  {1,3};          // ph2X90 recover
static int table9[2] =  {0,0};          // phXxhwft
static int table10[2] = {1,1};          // phHxh
static int table11[4] = {0,0,2,2};      // ph1H90
static int table22[8] = {0,0,0,0,2,2,2,2};      // ph1H90soft
static int table12[4] = {2,2,0,0};      // ph2H90
static int table21[8] = {2,2,2,2,0,0,0,0};      // ph2H90soft

static int table13[8] = {0,0,2,2,0,0,2,2};          // phRec
static int table20[8] = {0,2,2,0,0,2,2,0}; // phRecsoft  
static int table14[4] = {0,0,0,0};      // phHrfdr H-H mixing
static int table15[8] = {0,0};          // phHecho - solid echo cycle
static int table16[4] = {0,0,0,0};           // phHpxy  H presat pulses

#define phH90 t1
#define phHhy t2
#define phYhy t3
#define phYsoft t18
#define phYhard t19
#define ph1Y90 t4
#define ph2Y90 t17
#define phYyx t5
#define phXyx t6
#define ph1X90 t7
#define ph2X90 t8
#define phXxh t9
#define phHxh t10
#define ph1H90 t11
#define ph1H90soft t22
#define ph2H90 t12
#define ph2H90soft t21
#define phRec t13
#define phRecsoft t20
#define phHrfdr t14
#define phHecho t15
#define phHpxy  t16


/* NUS Pt.1 - Declare variables used for specifying d2 & d3 values (from example code, gnoesyNhsqc.c)*/
#define MAXFIDS 400000
static int sel[MAXFIDS][2];
static double d3const, d2const;
/* NUS Pt.1 - END */

void pulsesequence() {

   // AJN trying to prevent the backwards incrementing of d3
   // and d2 phases. check for phase,phase2 array bug.

   check_array();

// Define Variables and Objects and Get Parameter Values

   double duty;

   int decmode = getval("decmode");

   double d2_ = getval("d2max") - getval("d2") - getval("pwX90");
   double d3_ = getval("d3max") - getval("d3") - getval("pwY90");
   if (d2_ < 0.0) d2_ = 0.0;
   if (d3_ < 0.0) d3_ = 0.0;


   CP hy = getcp("HY",0.0,0.0,0,1);
   strcpy(hy.fr,"obs");
   strcpy(hy.to,"dec2");  // BDZ bug fixed here on 9-13-23: only "dec" was getting copied into "to" field.
   putCmd("frHY='obs'\n");
   putCmd("toHY='dec2'\n");

   CP yx = getcp("YX",0.0,0.0,0,1);
   strcpy(yx.fr,"dec2");  // BDZ bug fixed here on 9-13-23: only "dec" was getting copied into "fr" field.
   strcpy(yx.to,"dec");
   putCmd("frYX='dec2'\n");
   putCmd("toYX='dec'\n");

   CP xh = getcp("XH",0.0,0.0,0,1);
   strcpy(xh.fr,"dec");
   strcpy(xh.to,"obs");
   putCmd("frXH='dec'\n");
   putCmd("toXH='obs'\n");

   MPSEQ pxy = getpxy("pxyH",0,0.0,0.0,0,1);
   strcpy(pxy.ch,"obs");
   putCmd("chHpxy='obs'\n");

   MPSEQ rfdr = getrfdrxy8("rfdrH",0,0.0,0.0,0,1);
   strcpy(rfdr.ch,"obs");
   putCmd("chHrfdr='obs'\n");

/*   MPSEQ dream = getr2t("dreamY",0.0,0.0,0,1);
   strcpy(dream.ch,"dec2");  // BDZ bug fixed here on 9-13-23: only "dec" was getting copied into "ch" field.
   putCmd("chYdream='dec2'\n");
*/

   DSEQ dec = getdseq("H");
   DSEQ dec2 = getdseq("X");
   DSEQ dec3 = getdseq("Y");

   char softpul[MAXSTR];
   getstr("softpul",softpul);

   PBOXPULSE shp1 = getpboxpulse("shp1Y",0,1);

   double t1Hechoinit = getval("tHecho")/2;
   double t2Hechoinit = getval("tHecho")/2;
   double t1Hecho  = t1Hechoinit - getval("pwH90");
   if (t1Hecho < 0.0) t1Hecho = 0.0;
   double t2Hecho  = t2Hechoinit - getval("pwH90") - getval("rd");
   if (t2Hecho < 0.0) t2Hecho = 0.0;

// Dutycycle Protection

   duty = 4.0e-6 + 3* getval("pwH90") + getval("tHX") + getval("tXH") + t1Hecho + t2Hecho + getval("ad") + getval("rd") + at;

   duty = duty/(duty + d1 + 4.0e-6);
   if (duty > 0.1) {
      printf("Duty cycle %.1f%% >10%%. Abort!\n", duty*100);
      psg_abort(1);
   }



    /* NUS Pt.2 - Read-in NUS schedule file: '~/vnmrsys/nus_sched/hXXX_S.hdr_3' */
    char sparse_file[MAXSTR],sparse[MAXSTR];
    char read_char[MAXSTR];
    char check_str[MAXSTR];
    int check_len;
    int ni = getval("ni");
    int ni2 = getval("ni2");

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
            }
        }
        if(ni==1 || ni2==1) {
            d2 = d2const+sel[(int)(ix-1)/2][0]/sw1;
            d3 = d3const+sel[(int)(ix-1)/2][1]/sw2;
        }
        else {
            d2 = d2const+sel[(int)(ix-1)/4][0]/sw1;
            d3 = d3const+sel[(int)(ix-1)/4][1]/sw2;
        }
    }
    /* NUS Pt.2 - END */


// Set Phase Tables

   settable(phH90,4,table1);
   settable(phHhy,2,table2);
   settable(phYhy,2,table3);
   settable(phYsoft,8,table18);
   settable(phYhard,8,table19);
   settable(ph1Y90,2,table4);
   settable(ph2Y90,2,table17);
   settable(phYyx,8,table5);
   settable(phXyx,2,table6);
   settable(ph1X90,2,table7);
   settable(ph2X90,2,table8);
   settable(phXxh,2,table9);
   settable(phHxh,2,table10);
   settable(ph1H90,4,table11);
   settable(ph1H90soft,8,table22);
   settable(ph2H90,4,table12);
   settable(ph2H90soft,8,table21);
   settable(phRec,8,table13);
   settable(phRecsoft,8,table20);
   settable(phHrfdr,4,table14);
   settable(phHecho,2,table15);
   settable(phHpxy,2,table16);

   int id2_ = (int) (d2 * getval("sw1") + 0.1);
   if ((phase1 == 1) || (phase1 == 2)) {
     tsadd(phRec,2*id2_,4);  /* invert the phases of the storage 90 and */
     tsadd(phRecsoft,2*id2_,4);
     tsadd(ph1X90,2*id2_,4); /* the receiver for FAD to displace the axial peaks */
   }
   if (phase1 == 2) {tsadd(ph1X90,3,4);} /* hypercomplex*/

   int id3_ = (int) (d3 * getval("sw2") + 0.1);
   if ((phase2 == 1) || (phase2 == 2)) {
     tsadd(phRec,2*id3_,4);  /* invert the phases of the storage and */
     tsadd(phRecsoft,2*id3_,4);
     tsadd(ph1Y90,2*id3_,4); /* the receiver for FAD to displace the axial peaks */
   }
   if (phase2 == 2) {tsadd(ph1Y90,3,4);} /* hypercomplex*/

   if (!strcmp(softpul, "y"))   {setreceiver(phRecsoft);}
   else {   setreceiver(phRec);}

// Begin Sequence

// Initial Homospoil Pulse

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

// Y F1 Period with H XiX Decoupling

   _dseqon(dec);

   decpwrf(getval("aX90"));


   if (!strcmp(softpul, "y"))
   {
        if (d3>2*getval("pwX90"))
          {delay(0.5*d3-2.0*getval("pwX90"));
           decrgpulse(2*getval("pwX90"),0,0,0);
          }
        else{delay(d3/2.0);}

        _dseqoff(dec);
        obspwrf(getval("aHshp1"));
	obsunblank(); xmtron();
	_pboxpulse(shp1,phYsoft);
        dec2pwrf(getval("aY90"));
	dec2rgpulse(2*getval("pwY90"),phYhard,0.0,0.0);
        xmtroff();
        _dseqon(dec);
       	delay(d3/2.0);
    }

   if (!strcmp(softpul, "n"))
    {
   	if (d3>0)
   	  {
       	if (d3>2*getval("pwX90"))
       		 {delay(0.5*d3-getval("pwX90"));
       		 decrgpulse(2*getval("pwX90"),0,0.0,0.0);
       		 delay(0.5*d3 - getval("pwX90"));
       		 }
        else {delay(d3);}
   	  }
     }

   dec2rgpulse(getval("pwY90"),ph1Y90,0.0,0.0);
   dec2phase(ph2Y90);
   delay(d3_);

   _dseqoff(dec);

   delay(getval("hstconst"));

   decphase(phXyx);txphase(one);
   obspwrf(getval("aHyx"));
   obsunblank(); xmtron();

   dec2rgpulse(getval("pwY90"),ph2Y90,0.0,0.0);
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
       if (d2>2*getval("pwY90")) {delay(0.5*d2-getval("pwY90"));
       dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
       delay(0.5*d2 - getval("pwY90"));
       }
     }
   else {delay(d2);}

   decrgpulse(getval("pwX90"),ph1X90,0.0,0.0);
   txphase(phHxh);
   decunblank();
   delay(d2_);

   _dseqoff(dec);

//   obsblank();
//   delay(getval("hstconst"));
//   obsunblank();

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

// XY8 Mixing

   if (getval("qHrfdr")>0)
   {
   obspwrf(getval("aH90"));
   if(!strcmp(softpul,"y")) {
       rgpulse(getval("pwH90"),ph1H90soft,0.0,0.0);
   }
   else {
       rgpulse(getval("pwH90"),ph1H90,0.0,0.0);
   }

   _mpseq(rfdr,phHrfdr);

   obspwrf(getval("aH90"));
   if(!strcmp(softpul,"y")) {
       rgpulse(getval("pwH90"),ph2H90soft,0.0,0.0);
   }
   else {
       rgpulse(getval("pwH90"),ph2H90,0.0,0.0);
   }
   }

// Begin X Decoupling

   status(C);
   _dseqon(dec2); _dseqon(dec3);

// H Hahn Echo

   if(t1Hecho>0.0)
   {
   txphase(phHecho);
   obspwrf(getval("aH90"));
   delay(t1Hecho);
   rgpulse(2*getval("pwH90"),phHecho,0.0,0.0);
   delay(t2Hecho);
   }

// Begin H Acquisition

   obsblank(); _blank34();
   delay(getval("rd"));
   startacq(getval("ad"));
   acquire(np, 1/sw);
   endacq();
   _dseqoff(dec2);_dseqoff(dec3);
   obsunblank(); decunblank(); _unblank34();
}
