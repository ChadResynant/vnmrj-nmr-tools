/*HhXX.c - A sequence to provide XX homonuclear correlation with the option of using RAD, SPC5, POSTC7 or RFDR mixing

                J. Rapp 09/17/09, CMR test 11/4/09                                    */
/*     GCC making HhXX from the hXX code 08/05/10 */
/*     MT add softpulse on 1H 09/02/10 */

#include "standard.h"
#include "solidstandard.h"

// NOTE BDZ 10-24-23
// 
// This is including stock OVJ's pboxpulse.h instead of
// our biosolidpboxpulse.h. I think this is correct. The
// above includes are stock OVJ as well.
//

#include "pboxpulse.h"

// Define Values for Phasetables


static int table1[16] = {3,3,3,3,1,1,1,1,1,1,1,1,3,3,3,3};           // phH90
static int table2[8] = {0,0,0,0,0,0,0,0};           // phHhx
static int table3[8] = {0,0,0,0,0,0,0,0};           // phXhx
static int table4[16] = {3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1};           // phXmix1
static int table5[8] = {1,2,1,2,1,2,1,2};           // phXmix2;
static int table9[8] = {1,0,1,0,1,0,1,0};           // phXmix2dq;
static int table6[8] = {0,1,2,3,2,3,0,1}; // receiver
static int table7[8] = {0,0,0,0,0,0,0,0};	    // phXspc5
static int table8[8] = {0,1,0,1,0,1,0,1};	    // phXspc5ref
static int table10[8] = {1,1,1,1,1,1,1,1};           // phHstore
static int table11[8] = {3,3,3,3,3,3,3,3};           // phHreadout
static int table13[8] = {0,2,1,3,0,2,1,3};   // phHecho - solid echo cycle 

//static int table1[16] = {0,0,0,0,2,2,2,2,0,0,0,0,2,2,2,2};           // phH90
//static int table2[8] = {0,1,2,3,2,3,0,1};           // phHhx
//static int table3[16] = {0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0};           // phXhx
//static int table4[8] = {2,2,2,2,0,0,0,0};           // phXmix1
//static int table5[8] = {1,2,1,2,3,0,3,0};           // phXmix2;
//static int table9[8] = {1,0,1,0,1,0,1,0};           // phXmix2dq;
//static int table6[16] = {2,3,0,1,2,3,0,1,0,1,2,3,0,1,2,3}; // receiver
//static int table7[8] = {0,0,0,0,0,0,0,0};	    // phXspc5
//static int table8[8] = {0,1,0,1,0,1,0,1};	    // phXspc5ref
//static int table10[16] = {3,3,3,3,1,1,1,1,3,3,3,3,1,1,1,1};           // phXmix3
//static int table11[16] = {3,3,3,3,1,1,1,1,3,3,3,3,1,1,1,1};           // phXmix4
//static int table13[8] = {0,2,1,3,0,2,1,3};   // phHecho - solid echo cycle 


#define phH90 t1
#define phHhx t2
#define phXhx t3
#define phXmix1 t4
#define phXmix2 t5
#define phRec t6
#define phXspc5 t7
#define phXspc5ref t8
#define phXmix2dq t9
#define phHstore t10
#define phHreadout t11
#define phHecho t13

int id2_,id3_, xsel; /*during y evolution, 
                        xsel: -1---soft Pi only; +1---soft, then hard Pi; 0---neither*/
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
   getstr("mMix",mMix); 

   char hMix[MAXSTR];   
   getstr("hMix",hMix); 
   
   char hsel[MAXSTR];   
   getstr("hsel",hsel); 

   PBOXPULSE shp = getpboxpulse("shpH",0,1);
   strcpy(shp.ch,"dec");
   putCmd("chHshp ='dec'\n");

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

   MPSEQ spc5 = getspc5("spc5X",0,0.0,0.0,0,1);
   MPSEQ spc5ref = getspc5("spc5X",spc5.iSuper,spc5.phAccum,spc5.phInt,1,1); 
   strcpy(spc5.ch,"obs");
   putCmd("chXspc5='obs'\n");
   
   MPSEQ c7 = getpostc7("c7X",0,0.0,0.0,0,1);
   MPSEQ c7ref = getpostc7("c7X",c7.iSuper,c7.phAccum,c7.phInt,1,1);
   strcpy(c7.ch,"obs");
   putCmd("chXc7='obs'\n");

   PARIS paris = getparis("H");

   DSEQ dec = getdseq("H");

   char spc5_flag[MAXSTR];
   getstr("spc5_flag",spc5_flag);  //1 is normalmixing 2 is dqf 
   
  // Set Mixing Period to N Rotor Cycles 
   
   double taur=1,mix,hmix, srate;
   mix =  getval("tXmix"); 
   hmix =  getval("tHmix"); 
   srate =  getval("srate");
   if (srate >= 500.0) {
      taur = roundoff((1.0/srate), 0.125e-6);
   }
   else {
      printf("ABORT: Spin Rate (srate) must be greater than 500\n");
      psg_abort(1);
   }
   mix = roundoff(mix,taur);
  
// Dutycycle Protection
   
   if (!strcmp(mMix, "rad")) {
      duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 +
             mix + 2.0*getval("pwX90") + getval("ad") + getval("rd") + at;
      duty = duty/(duty + d1 + 4.0e-6);
   }
   else if (!strcmp(mMix, "c7")) {
      duty = 4.0e-6 + getval("pwH90") + getval("tHX") + 2.0*getval("tZF") + c7.t + 
             d2 + c7ref.t + getval("ad") + getval("rd") + at;

      duty = duty/(duty + d1 + 4.0e-6 + 2.0*getval("tZF"));   }
   else if (!strcmp(mMix, "spc5")) {
      duty = 4.0e-6 + getval("pwH90") + getval("tHX") + d2 +
             2.0*getval("pwX90") + getval("ad") + getval("rd") + at;
      duty = duty/(duty + d1 + 4.0e-6);
   }
  
// Create Phasetables

   settable(phH90,16,table1);
   settable(phHhx,8,table2);
   settable(phXhx,8,table3);
   settable(phXmix1,16,table4);
   settable(phRec,8,table6);
   settable(phXspc5,8,table7);
   settable(phHecho,8,table13);
   settable(phHstore,8,table10);  
   settable(phHreadout,8,table11);
   
   if (!strcmp(spc5_flag,"2"))
   {
	  settable(phXspc5ref,8,table8);
   	  settable(phXmix2,8,table9);
   }
   else
   {
      settable(phXspc5ref,8,table7);
   	  settable(phXmix2,8,table5);
   }

   id2_ = (int) (d2 * getval("sw1") + 0.1);
   if ((phase1 == 1) || (phase1 == 2)) {
      tsadd(phRec,2*id2_,4); /* invert the phases of the CP pulse and */
      tsadd(phXmix1,2*id2_,4); /* the receiver for FAD to displace the axial peaks */
   }  

   if (phase1 == 2){
      tsadd(phXmix1,3,4);
   }


// tsadd(phXhx,1,4) was used previously
// add 270 degrees to storage pulse to effectively have States-TPPI 
// but this also avoids phase twisting on resonance due to T1 relaxation during t1 evol.
 
   id3_ = (int) (d3 * getval("sw2") + 0.1);   
   if ((phase2 == 1) || (phase2 == 2)) {
      tsadd(phRec,2*id3_,4); 
	  tsadd(phHstore,2*id3_,4); 
      if (phase2 == 2) {
         tsadd(phHstore,3,4);
      }
   }
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

// T2  filter (Hahn Echo), add softpulse
 
   if (!strcmp(hsel, "y")) { 
      decpwrf(getval("aHshp"));
      _pboxpulse(shp,phH90);
   }
   else {
      decrgpulse(getval("pwH90"),phH90,0.0,0.0);
   }
   decphase(phHhx);

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
         delay(fabs(tECHO/2.0-soft1-getval("pwH90"))); //compensate one rotor period for softpulse.
      }
      else {
         delay((tECHO/2.0));
      }
   
      decrgpulse(pwHecho,phHecho,0.0,0.0);  
      if((tECHO/2.0) > 0.1) {
         delay((tECHO/2.0));
      }
      else {
         delay(tECHO/2.0);
      }
   }

// F3 Indirect Period for H

   delay(d3); 


// H-H Mixing For H

   if (!strcmp(hMix, "y")) {
      decrgpulse(getval("pwH90"),phHstore,0.0,0.0);
      delay(hmix);
      decrgpulse(getval("pwH90"),phHreadout,0.0,0.0);
   }
   decoff();


// H to X Cross Polarization

   decphase(phHhx);
   _cp_(hx,phHhx,phXhx);

// F2 Indirect Period for X

   obspwrf(getval("aX90"));
   _dseqon(dec);
   delay(d2);
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

      if (!strcmp(spc5_flag,"2"))
      {
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
      _mpseq(c7, phXmix1);
      decoff();
      decpwrf(getval("aHmixc7"));
      decunblank();
      decon();
      _mpseq(c7ref, phXmix2);
      decoff();
      obspwrf(getval("aX90"));
      xmtrphase(zero); txphase(phXmix2);
      decpwrf(getval("aHZF")); //AJN 031510 control aHzf
      decon();
      delay(getval("tZF"));
      rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
      decoff();
   }

// RAD(DARR) Mixing For X

   if (!strcmp(mMix, "rad")) {
      decpwrf(getval("aHmix"));
      decunblank(); decon();
      rgpulse(getval("pwX90"),phXmix1,0.0,0.0);
      delay(mix);
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
      delay(mix);
      decprgoff();
      decpwrf(getval("aH90"));
      decunblank(); decon();
      rgpulse(getval("pwX90"),phXmix2,0.0,0.0);
   }
   decoff();


// Begin Acquisition

   _dseqon(dec);   
   d2_ = (ni-1)/getval("sw1") - d2;
   if (d2_<0)
      d2_ = 0.0;     
     
   obsblank(); _blank34();
   delay(getval("rd"));
   startacq(getval("ad"));
   acquire(np, 1/sw);
   endacq();
   _dseqoff(dec);

   decon();
  
   tRF = getval("tRF");
   if (tRF <= (ni-1)/getval("sw1")) {
      d2_ = (ni-1)/getval("sw1") - d2;
      if (d2_<0) d2_ = 0.0;
   	     delay(d2_);
   }
   else
      delay(tRF-d2);

   decoff();
   obsunblank(); decunblank(); _unblank34();
}
