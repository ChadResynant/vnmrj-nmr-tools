/*hXhhX.c - A sequence to provide XX homonuclear correlation with the option of using PDSD                              */
/* 08/13/10 GCC and JJL creating hXhhX from hXX code */
/* CMR fixed phase cycle 091010 test on 500 100210 works in 16 scans */
/* 7/6/21 CMR fixed phase cycle again for nt=16 version; 9/8/21 fixed steps 17-32 for table 5 */

#include "standard.h"
#include "biosolidstandard.h"

// Define Values for Phasetables

// New phase cycle 9/12/10, trying to use pwX90 purge pulses before or after CP, instead of Zed filters
// H spin temp every 2 scans
static int table1[16] =  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};        // phH90
static int table2[16] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};        // phHhx
static int table3[16] =  {0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2};        // phXhx
// select for first CP every 2 scans
static int table5[32] =  {0,2,0,2,2,0,2,0,0,2,0,2,2,0,2,0,0,2,0,2,2,0,2,0,0,2,0,2,2,0,2,0};       // phXxh
// select for second CP every 4 scans
static int table4[32] =  {1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3};       // phHxh
// spin temp on H here instead of initial 90
static int table6[16] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};       // phHmix1
static int table7[16] =  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};       // phHmix2;
static int table8[32] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};       // phHhx2
static int table9[16] =  {0,0,2,2,0,0,2,2,1,1,3,3,1,1,3,3};       // phXhx2
// select for third CP every 8 scans
static int table14[32] = {0,2,2,0,2,0,0,2,1,3,3,1,3,1,1,3,2,0,0,2,0,2,2,0,3,1,1,3,1,3,3,1};       // phRec
// quad image suppression every 16 scans (should not be necessary with DDR)
// another spin temp on phHxh relative to everything else on scans 17-32 (also redundant)

// alternate first ZF every 16 scans
//static int table10[16] = {3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1};       // phZF1;  
//static int table11[16] = {1,1,1,1,1,1,1,1,3,3,3,3,3,3,3,3};       // phZF2;

//no net effect on final phase, but alternate periodically to avoid effect of T1 relaxation during ZF
// alternate last ZF every 8 scans
static int table15[16] = {0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2};        //phPURGE1  CMR 7/6/21 pulse removed
static int table16[16] = {0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2};       //phPURGE2   CMR 7/6/21 pulse removed
static int table17[16] = {0,0,0,0,2,2,2,2,0,0,0,0,2,2,2,2};       // phPURGE3  CMR 7/6/21 pulse removed
static int table12[16] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};       // phZF3    CMR 7/6/21 remove
static int table13[16] = {1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0};       // phZF4    CMR 7/6/21 remove


// Phase cycle below gives bad methyl twisting (NAV) in 4 or 8 scans; good with 16 scans (CMR 9/10/10)
//static int table1[16] =  {1,1,1,1,1,1,1,1,3,3,3,3,3,3,3,3};        // phH90
//static int table2[16] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};        // phHhx
//static int table3[16] =  {0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2};        // phXhx
//static int table4[16] =  {0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0};        // phHxh
//static int table5[16] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};        // phXxh
//static int table6[16] =  {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};       // phHmix1
//static int table7[16] =  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};       // phHmix2;
//static int table8[16] =  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};       // phHhx2
//static int table9[16] =  {0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3};       // phXhx2
//static int table10[16] = {1,1,1,1,1,1,1,1,3,3,3,3,3,3,3,3};       // phZF1;
//static int table11[16] = {3,3,3,3,3,3,3,3,1,1,1,1,1,1,1,1};       // phZF2;
//static int table12[16] = {1,2,3,0,3,0,1,2,1,2,3,0,3,0,1,2};       // phZF3;
//static int table13[16] = {3,0,1,2,1,2,3,0,3,0,1,2,1,2,3,0};       // phZF4;
//static int table14[16] = {0,3,2,1,2,1,0,3,0,3,2,1,2,1,0,3};       // phRec;


#define phH90 t1
#define phHhx t2
#define phXhx t3
#define phHxh t4
#define phXxh t5
#define phHmix1 t6
#define phHmix2 t7
#define phHhx2 t8
#define phXhx2 t9
//#define phZF1 t10
//#define phZF2 t11
#define phZF3 t12
#define phZF4 t13 
#define phRec t14
#define phPURGE1 t15
#define phPURGE2 t16
#define phPURGE3 t17

 int id2_,id3_, xsel; /*during y evolution, 
                        xsel: -1---soft Pi only; +1---soft, then hard Pi; 0---neither*/
void pulsesequence() {

   check_array();

   // Define Variables and Objects and Get Parameter Values

   double tRF,d2_;

   double duty;
   
   CP hx = getcp("HX",0.0,0.0,0,1);
   strcpy(hx.fr,"dec");
   strcpy(hx.to,"obs");
   putCmd("frHX='dec'\n");
   putCmd("toHX='obs'\n"); 
   CP xh = getcp("XH",0.0,0.0,0,1);
   strcpy(xh.fr,"obs");
   strcpy(xh.to,"dec");
   putCmd("frXH='obs'\n");
   putCmd("toXH='dec'\n");
   
   CP hx2 = getcp("HX2",0.0,0.0,0,1);
   strcpy(hx2.fr,"dec");
   strcpy(hx2.to,"obs");
   putCmd("frHX2='dec'\n");
   putCmd("toHX2='obs'\n");

   char ZF_flag[MAXSTR];      
   getstr("ZF_flag",ZF_flag); 

   char hMix[MAXSTR];      
   getstr("hMix",hMix);   

   DSEQ dec = getdseq("H");
   
  // Set Mixing Period to N Rotor Cycles 
   
   double taur=1,tHmix,srate;
   tHmix =  getval("tHmix"); 
   srate =  getval("srate");
      if (srate >= 500.0) {
         taur = roundoff((1.0/srate), 0.125e-6);
      }
      else {
      printf("ABORT: Spin Rate (srate) must be greater than 500\n");
      psg_abort(1);
      }
   tHmix = roundoff(tHmix,taur);
  
// Dutycycle Protection
   

   duty = 4.0e-6 + getval("pwH90") + getval("tHX") + getval("tHX2")+ getval("tXH") + d2 +
          tHmix + 2.0*getval("pwH90") + getval("ad") + getval("rd") + at;
   duty = duty/(duty + d1 + 4.0e-6);
   


// Create Phasetables

   settable(phH90,16,table1);
   settable(phHhx,16,table2);
   settable(phXhx,16,table3);
   settable(phHxh,32,table4);
   settable(phXxh,32,table5);
   settable(phHmix1,16,table6);
   settable(phHmix2,16,table7);
   settable(phHhx2,32,table8);
   settable(phXhx2,16,table9);
//   settable(phZF1,16,table10);
//  settable(phZF2,16,table11);
   settable(phZF3,16,table12);
   settable(phZF4,16,table13);
   settable(phRec,32,table14);
   settable(phPURGE1,16,table15);
   settable(phPURGE2,16,table16);
   settable(phPURGE3,16,table17);

  
   id2_ = (int) (d2 * getval("sw1") + 0.1);
   if ((phase1 == 1) || (phase1 == 2)) {
     tsadd(phRec,2*id2_,4); /* invert the phases of the CP pulse and */
     tsadd(phXxh,2*id2_,4); /* the receiver for FAD to displace the axial peaks */
     tsadd(phPURGE2,2*id2_,4); /* the purge pulse must always follow the phXxh */
   }  

   if (phase1 == 2) {tsadd(phXxh,3,4); tsadd(phPURGE2,3,4);} /* see above! */
// tsadd(phXhx,1,4) was used previously
// add 270 degrees to storage pulse to effectively have States-TPPI 
// but this also avoids phase twisting on resonance due to T1 relaxation during t1 evol.

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

   decpwrf(getval("aH90")); //AJN 031510 control aHzf
   decon();obspwrf(getval("aX90"));
//   rgpulse(getval("pwX90"),phPURGE1,0.0,0.0); 
// decpwrf(getval("aHzf"));
//   delay(getval("tZF"));
//   rgpulse(getval("pwX90"),phZF2,0.0,0.0);
   decoff(); 
// F2 Indirect Period for X

    _dseqon(dec);
   delay(d2);
   _dseqoff(dec);

// X to H Cross Polarization
   decpwrf(getval("aH90")); //AJN 031510 control aHzf
   decphase(phHxh); decon();   obspwrf(getval("aX90"));
//   rgpulse(getval("pwX90"),phPURGE2,0.0,0.0); 
   decoff();
   txphase(phXxh);
   obspwrf(getval("aXxh"));
   decphase(phHxh);
    _cp_(xh,phXxh,phHxh);

   
   
// longitudinal Mixing For 1H

   decpwrf(getval("aH90")); 
// AJN for now just using one mixing type
//   if (!strcmp(hMix, "y")) {
   if(tHmix > 0.0) {
   decunblank(); 
   decrgpulse(getval("pwH90"),phHmix1,0.0,0.0);
   delay(tHmix);
   decrgpulse(getval("pwH90"),phHmix2,0.0,0.0);
  }

// H to X Cross Polarization 2
   decpwrf(getval("aH90")); //AJN 031510 control aHzf
   decphase(phHhx2); decon();obspwrf(getval("aX90"));
//   rgpulse(getval("pwX90"),phPURGE3,0.0,0.0); 
   txphase(phXhx2);
   decoff();
   decpwrf(getval("aHhx2"));
   obspwrf(getval("aXhx2"));
   decphase(phHhx2);
    _cp_(hx2,phHhx2,phXhx2);
 
// Z filter
 
//if(!strcmp(ZF_flag, "on"))
 //  {
// decpwrf(getval("aH90"));
// decon(); obspwrf(getval("aX90"));
// rgpulse(getval("pwX90"),phZF3,0.0,0.0);
// decpwrf(getval("aHZF"));
// delay(getval("tZF"));
// decpwrf(getval("aH90"));
// rgpulse(getval("pwX90"),phZF4,0.0,0.0);
// decoff();
 //  }

// Begin Acquisition

   _dseqon(dec);   
   d2_ = (ni-1)/getval("sw1") - d2;
   if (d2_<0) d2_ = 0.0;     
     
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
   } else delay(tRF-d2);
 
 decoff();
 obsunblank(); decunblank(); _unblank34();
}

