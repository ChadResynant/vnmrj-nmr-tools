/*hX.c - A sequence to form a constant, linear or tangent ramped CP
              followed by a shaped refocussing pulse, with MPSEQ TPPM or SPINAL
              decoupling.

             Replace the DSEQ module with the MPSEQ module
             D. Rice for UIUC 23/07/08;  J.Rapp 09/17/09; G.Comellas 11/09/09;
	     Additional edits AJN 11/14/09; G.Comellas more edits 12/21/09
   	     AJN and LJS 02/26/10 added X flip and fixed tECHO timing bug
   		GCC and MT 03/21/12 solved bug with softpulse decoupling on 1H
   		Add PIPS decoupling June, 2016*/

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"

// Define Values for Phasetables

static int table1[4] =  {2,2,2,2};           // phH90 softpulse

static int table2[4] =  {0,3,0,3};           // phXhx softpulse
static int table3[4] =  {1,1,1,1};           // phHhx softpulse
static int table4[8] =  {2,2,2,2,2,2,2,2};           // phHspnlm
static int table5[8] =  {0,0,1,1,0,0,1,1};           // phXshp1
static int table6[8] =  {1,1,1,1,1,1,1,1};           // phHspnl
static int table7[4] =  {0,1,2,3};           // phRec softpulse phRecsftp

static int table8[16] = {0,2,0,2,0,2,0,2, 2,0,2,0,2,0,2,0};           // phH90 just CP

static int table9[4] =  {0,0,1,1};           // phXhx just CP
static int table10[4] = {3,3,3,3};           // phHhx just CP
static int table11[16]= {0,2,1,3,0,2,1,3,2,0,3,1,2,0,3,1};           // phRec just CP

static int table12[16]= {1,3,2,0,1,3,2,0, 2,0,1,3,2,0,1,3};           // phX90
static int table16[16]= {0,2,1,3,0,2,1,3, 3,1,2,0,3,1,2,0}; // phRecsoftdp
static int table14[4] = {1,3,2,0};   	    // phX90flip

static int table13[8] = {0,2,1,3,1,3,2,0};   // phXecho - solid echo cycle
static int table15[16]= {0,2,1,3,2,0,3,1,2,0,3,1,0,2,1,3};   // phRechard  CMR 5/7/24 fixed for nt=16 case

#define phH90sftp t1
#define phXhxsftp t2
#define phHhxsftp t3
#define phHspnlm t4
#define phXshp1 t5
#define phHspnl t6
#define phRecsftp t7
#define phH90 t8
#define phXhx t9
#define phHhx t10
#define phRec t11
#define phX90 t12
#define phX90flip t14
#define phXecho t13
#define phRechard t15
#define phRecsoftdp t16

void pulsesequence() {

// Define Variables and Objects and Get Parameter Values

   double duty = 0.1;

   int decmode = getval("decmode");
   int decmodem = getval("decmodem");

   CP hx = getcp("HX",0.0,0.0,0,1);
   strcpy(hx.fr,"dec");
   strcpy(hx.to,"obs");
   putCmd("frHX='dec'\n");
   putCmd("toHX='obs'\n");

   PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);

   DSEQ dec = getdseq("H");
   DSEQ dec2 = getdseq("Y");
//   DSEQ dec3 = getdseq("Hsoft"); // Apr 14, 2015 CMR this is no longer used, and never really was... because of an old bug
   DSEQ dec_soft = getdseq("Hsoft");  // 11/1/2018 CMR renaming to be consistent with hXX and hYXX etc. code
   
   char cp[MAXSTR];
   getstr("cp",cp);

   char echo[MAXSTR];
   getstr("echo",echo);

   char flip[MAXSTR];
   getstr("flip",flip);

   char T1[MAXSTR];
   getstr("T1",T1);

   double aXecho = getval("aXecho");
   double pwXecho = getval("pwXecho");
   double tECHO = getval("tECHO");
   tECHO  = tECHO - pwXecho;
   if (tECHO < 0.0) tECHO = 0.0;

   double tECHOmax = getval("tECHOmax");
   if (strcmp(echo,"n")) {
   tECHOmax = tECHOmax - tECHO;}
   if (tECHOmax < 0.0) tECHOmax = 0.0;

// Dutycycle Protection

   if (!strcmp(cp, "n"))  {
   duty = 4.0e-6 + getval("pwH90") + getval("tHX") + getval("ad") + getval("rd") + at;
   }
   else if (!strcmp(cp, "y"))  {
   duty = 4.0e-6 + getval("pwH90") + getval("tHX") + getval("ad") + getval("rd") + at;
   }
   else if (!strcmp(echo, "y")) {
   duty = 4.0e-6 + getval("pwH90") + getval("tHX") + tECHO + pwXecho + tECHOmax + getval("ad") + getval("rd") + at;
   }

   duty = duty/(duty + d1 + 4.0e-6);

   if (duty > 0.2) {
      abort_message("Duty cycle >20%%. Abort!\n");
   }

// Set Phase Tables

   settable(phH90sftp,4,table1);
   settable(phXhxsftp,4,table2);
   settable(phHhxsftp,4,table3);
   settable(phHspnlm,8,table4);
   settable(phXshp1,8,table5);
   settable(phHspnl,8,table6);
   settable(phRecsftp,4,table7);
   settable(phH90,16,table8);

   settable(phX90flip,4,table14);
   settable(phXhx,4,table9);
   settable(phHhx,4,table10);
   settable(phRec,16,table11);
   settable(phRechard,16,table15);
   settable(phRecsoftdp,16,table16);
   setreceiver(phRec);
   settable(phX90,16,table12);

   if (!strcmp(cp, "n")) 
    {
     setreceiver(phRec);
     if (!strcmp(echo, "soft")) {setreceiver(phRecsoftdp);}
    }
 
   else 
    { 
     if (!strcmp(echo, "hard")) 
      {
       settable(phXecho,8,table13);
       setreceiver(phRechard);
      }
     if (!strcmp(echo, "soft")) 
      {
       setreceiver(phRecsftp);
      }
    }

// Begin Sequence



   if (!strcmp(cp,"n")) {
   txphase(phX90); decphase(zero);
   obspwrf(getval("aX90"));
  }

   if (!strcmp(cp, "y")) {
   txphase(phXhx); decphase(phH90);
   obspwrf(getval("aXhx")); decpwrf(getval("aH90"));
  }

   if (!strcmp(echo, "hard")) {
   txphase(phXhx); decphase(phH90);
   obspwrf(getval("aXhx")); decpwrf(getval("aH90"));
  }
   if (!strcmp(echo, "soft")) {
   txphase(phXhxsftp); decphase(phH90sftp);dec2phase(zero);
   obspower(tpwr);
   obspwrf(getval("aXhx")); decpwrf(getval("aH90"));
  }

   obsunblank(); decunblank(); dec2unblank();dec3unblank();
   obsblank();
   delay(d1);
   obsunblank();
   
//H T1 Inversion Recovery

if (!strcmp(T1,"y"))
   {
   if (!strcmp(cp,"y")) {
      decrgpulse(2*getval("pwH90"),0,0.0,0.0);
      delay(getval("d2"));
      }
   }


 // External trigger on channel 1 number 1
   sp1on(); sp4on(); delay(2.0e-6); sp1off(); sp4off(); // proteus delay(2.0e-6);  // CMR 10/11/23 use sp1on() for scope trigger and F19 Tomco amp
   rcvroff();
// unblank CH3 for F19 CMR 10/10/23 see ELN for more details
//   sp2on(); 
//	delay(10e-6);
//	delay(d2);
//
// H to X Cross Polarization
    if (!strcmp(cp, "n")) {
        if (!strcmp(T1, "n")){
            obspwrf(getval("aX90"));
            rgpulse(getval("pwX90"),phX90,0.0,0.0);
        }
        else if (!strcmp(T1, "y")){
            obspwrf(getval("aX90"));
            rgpulse(2*getval("pwX90"),0,0.0,0.0);
	        sp4off(); delay(getval("d2")); sp4on(); delay(100e-6);
	        rgpulse(getval("pwX90"),phX90,0.0,0.0);
        }
    }
    else if (!strcmp(cp, "y")) {
        if (!strcmp(echo, "soft")) {
            decrgpulse(getval("pwH90"),phH90sftp,0.0,0.0);
            decphase(phHhxsftp);
            _cp_(hx,phHhxsftp,phXhxsftp);
        }
        else {
            decrgpulse(getval("pwH90"),phH90,0.0,0.0);
            decphase(phHhx);
            _cp_(hx,phHhx,phXhx);
        }
   }






// Decoupling
//    sp4off();
    _dseqon(dec);
    if (NUMch > 2) _dseqon(dec2);
    dec3blank();

// X flip back

if (!strcmp(flip,"y"))
   {
   txphase(phX90flip);
   obspwrf(getval("aX90"));
   rgpulse(getval("pwX90"),phX90flip,0.0,0.0);
   }

// Shaped Refocussing Pulse

    if (!strcmp(echo,"soft")) {
       delay(tECHO/2.0);
       _dseqoff(dec);  
       _dseqon(dec_soft);  
       _pboxpulse(shp1,phXshp1);
       _dseqoff(dec_soft); 
       _dseqon(dec);  
// the 5 lines above have been modified 11/1/2018 CMR so that soft pulse decoupling parameters match those in hXX and hYXX code
       if ((tECHO/2.0) > 0.1) {
       delay((tECHO/2.0));
          }
       else {
       delay(tECHO/2.0);
            }
    }

// X Hahn Echo
      if (!strcmp(echo, "hard")) {
      txphase(phXecho);
      obspwrf(aXecho);
      delay(tECHO/2.0);
      rgpulse(pwXecho,phXecho,0.0,0.0);
      sp4off();
      if(((tECHO/2.0)-getval("rd"))> 0.1) {delay((tECHO/2.0)-getval("rd"));}
      else{delay(tECHO/2.0);}
	}

// Begin Acquisition

   obsblank(); 
   sp4off();
//   sp2off(); // blank F19 CHN 3
//   sp1off(); // use sp1 for scope trigger and F19 Tomco amp
   if (NUMch > 2) {_dseqoff(dec2);} 
   dec2blank();
   delay(getval("rd"));
   startacq(getval("ad"));
   acquire(np, 1/sw);
   endacq();

// Tau Echo Max

   delay(tECHOmax);
   _dseqoff(dec);
   obsunblank(); decunblank(); dec2unblank(); dec3unblank();
   obsblank(); // 10/11/23 debugging
}
