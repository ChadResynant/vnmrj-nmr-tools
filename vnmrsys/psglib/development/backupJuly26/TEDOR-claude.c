/*
	ZF_TEDOR_NUS_CT.c 

	History
	
	AJN 06/19/2010 started with jhscttedor2d.c
	MT  09/02/2011 add srate_tedor to avoid "spinset to -1" problem
	ALP 03/03/2022 Rajat reverted back to srate instead of srate_tedor
	[USER] 07/23/2025 Added NUS (Non-Uniform Sampling) and CT (Constant Time) support
		                                    
*/

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"
#include "biosolidnus.h"

// define values for phasetables

static int table1[4] =  {0,0,2,2};  	    			// phH90    	H90
static int table2[8]  = {3,3,3,3,1,1,1,1};			// phXhx        phi2 +270
static int table3[4] =  {3,3,3,3};           			// phHhx	Hmix
static int table4[4] =  {0,1,0,1};   	    			// ph1Yredor	XY4
static int table5[4] =  {0,1,0,1};           			// ph2Yredor
static int table6[4] =  {0,0,0,0};				// phX180p1     
static int table7[4] =  {3,1,3,1};				// phY90	phi3
static int table8[4] =  {0,0,0,0};				// phX90	0,0
static int table11[4] = {0,0,0,0};				// phY180p2   	90
static int table12[4] = {3,3,3,3};				// phXzf1  0.0
static int table13[4] = {3,3,3,3};				// phXzf2  phi4
static int table15[4] = {1,1,1,1};				// phXzf3  phi5 
static int table16[4]=  {1,1,1,1};				// phYzf1  phi8
static int table14[8]=  {0,2,2,0,2,0,0,2};			// phRec

#define phH90 t1
#define phXhx t2
#define phHhx t3
#define ph1Yredor t4
#define ph2Yredor t5
#define phX180p1 t6
#define phY90 t7
#define phX90 t8
#define phY180p2 t11
#define phXzf1 t12
#define phXzf2 t13
#define phRec t14
#define phXzf3 t15
#define phYzf1 t16

void pulsesequence() {

   check_array();

// Define Variables and Objects and Get Parameter Values* 

   double duty ,tRF, d2_;
   
   double aYredor = getval("aYredor");    		     	// REDOR pulse amplitude 
   double pwYredor = getval("pwYredor");  		     	// REDOR pulse width
   
   double aXredor = getval("aXredor");    		     	// REDOR pulse amplitude 
   double pwXredor = getval("pwXredor");  		     	// REDOR pulse width

   double pwXshp1 = getval("pwXshp1");				// X soft pulse

   double srate = getval("srate");  			     	// spin rate

   if (srate < 1) {
      printf("srate too small, set it > 1");
      psg_abort(1);
   }

   double taur = 1.0/srate;					// time of 1 rotor cycle
   double tauY0 =taur/2.0 - pwYredor/2.0;		     	// initial REDOR delay
   double tauY = taur/2.0 -  pwYredor;			     	// REDOR delay = half rotor cycle - 180 REDOR pulse
   double tauZ = taur - pwXredor/2.0 - pwYredor/2.0; 		// delay for refocusing in middle of REDOR ZF
   int n = floor((pwXshp1)/taur)+2;
   double tauB = (n*taur - pwXshp1 - pwYredor)/2.0;		// delay for refocusing in middle of REDOR BASE

// check to make sure delays are positive
   printf("Verifying REDOR timings (microseconds)\n");
   printf("taur:%f tauZ:%f tauB:%f\n",taur*1e6,tauZ*1e6,tauB*1e6);
   if( tauZ < 1e-7 ){ tauZ=tauZ + taur;}
   if( tauB < 1e-7 ){ tauB=tauB + taur;}

   int nredor = (int) (getval("nredor"));	     		// # of REDOR cycles bracketing 180 pulses 
   double tredor = taur * 4 * nredor;
   int nredor_max = (int) (getval("nredor_max"));
   double tredor_max = taur * 4 * nredor_max;
   double tredor_comp = tredor_max - tredor ;
   printf("Verifying TEDOR timings (milliseconds)\n");
   printf("tredor:%f tredor_max:%f tredor_comp:%f\n",tredor*1e3,tredor_max*1e3,tredor_comp*1e3);
   if(tredor_comp<0.0){tredor_comp=0;}
   nredor = nredor - 1;						// taking one out of the loop
   initval(nredor,v14);     			             	// v14 = max. # of REDOR cycles bracketing 180 pulses

   double rd = getval("rd");				     	// ring-down delay
 
   double pwX90,pwY90;
   pwX90 = getval("pwX90");
   pwY90 = getval("pwY90");

   // Constant Time (CT) parameters
   char CT_flag[MAXSTR];
   getstr("CT_flag",CT_flag);
   double tCT = getval("tCT");                                   // constant time period
   double d2_CT;                                                 // CT evolution time
   
   // Get ni for NUS support
   int ni = getval("ni");
        
   CP hx = getcp("HX",0.0,0.0,0.0,1); 
   strcpy(hx.fr,"dec");
   strcpy(hx.to,"obs");
   putCmd("frHX='dec'\n"); 
   putCmd("toHX='obs'\n");
     
   DSEQ dec = getdseq("H");
   DSEQ redor = getdseq("Hredor");

   PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);
   strcpy(shp1.ch,"obs");
   putCmd("chXshp1 ='obs'\n");

   char tedor_flag[MAXSTR];      
   getstr("tedor_flag",tedor_flag);

   char ZF_flag[MAXSTR];      
   getstr("ZF_flag",ZF_flag);

   tRF = getval("tRF");

   // NUS Implementation - following biosolidnus.h pattern
   if (NUS_ACTIVE()) {
        if (NUS_INIT(2) != 0) psg_abort(1);  // 2D experiment for ZF-TEDOR
        if (NUS_CALC_DELAYS() != 0) psg_abort(1);
        if (NUS_SAFETY_CHECK(tRF, CT_flag, tCT) != 0) psg_abort(1);
   }
  
// Probe Protection -- limit to 40 ms high power decoupling
 
   duty = tredor_max + at;
   if (!strcmp(CT_flag, "y")) {
      duty += tCT;  // Add CT time to duty cycle calculation
   } else {
      duty += d2;   // Add maximum d2 time for regular evolution
   }

   printf("duty (total dec time (50 ms max)): %f \n", duty*1e3); 
    
   if (duty > 50.0e-3) {
      printf("Duration of sequence too long. Abort!\n");
      psg_abort(1);
   }

// Constant Time validation
   if (!strcmp(CT_flag, "y")) {
      if (tCT <= 0) {
         printf("Error: tCT must be positive when CT_flag='y'\n");
         psg_abort(1);
      }
      if (tCT < (ni-1)/getval("sw1")) {
         printf("Warning: tCT (%f ms) is shorter than maximum t1 (%f ms)\n", 
                tCT*1e3, ((ni-1)/getval("sw1"))*1e3);
      }
      printf("Constant Time enabled: tCT = %f ms\n", tCT*1e3);
   }

// Set Phase Tables
 
   settable(phH90,4,table1);
   settable(phXhx,8,table2);
   settable(phHhx,4,table3);
   settable(ph1Yredor,4,table4);
   settable(ph2Yredor,4,table5);
   settable(phX180p1,4,table6);
   settable(phY90,4,table7);
   settable(phX90,4,table8);
   settable(phXzf1,4,table12);
   settable(phXzf2,4,table13);
   settable(phXzf3,4,table15);
   settable(phYzf1,4,table16);
   settable(phRec,8,table14);
   setreceiver(phRec);

   // Phase cycling for indirect dimension
   int id2_ = (int) (d2 * getval("sw1") + 0.1);
   if (phase1 == 2) {            // States in F1
      tsadd(phY90,3,4);
   }
   if ((phase1 == 1) || (phase1 == 2)) {
      tsadd(phRec,2*id2_,4);     // FAD to displace axial peaks
      tsadd(phY90,2*id2_,4);
   }

// Begin Sequence

   txphase(phXhx); decphase(phH90);
   obspwrf(getval("aXhx")); decpwrf(getval("aH90"));  
   obsunblank(); decunblank(); dec2unblank();
   delay(d1);   
   sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);
   
// H to X Cross Polarization 
   
   decrgpulse(getval("pwH90"),phH90,0.0,0.0);
   decphase(phHhx);    
   _cp_(hx,phHhx,phXhx);
    
//------------------
// First REDOR period
//------------------

  _dseqon(redor);

// first half of redor sequence

   obspwrf(getval("aXredor"));
   txphase(phX180p1);       
   assign(v14,v8);
   if (nredor >= 1) {         
      dec2pwrf(aYredor);      
      initval(0,v11);                   
      delay(tauY0);
      getelem(ph1Yredor,v11,v12); dec2phase(v12); incr(v11);         
      dec2rgpulse(pwYredor,v12,0.0,0.0);
      getelem(ph1Yredor,v11,v12); dec2phase(v12); incr(v11);         
      delay(tauY);                  
      dec2rgpulse(pwYredor,v12,0.0,0.0);            
      loop(v8,v9);
         getelem(ph1Yredor,v11,v12); dec2phase(v12); incr(v11);
         delay(tauY); 
         dec2rgpulse(pwYredor,v12,0.0,0.0);
         getelem(ph1Yredor,v11,v12); dec2phase(v12); incr(v11);         
         delay(tauY);                  
         dec2rgpulse(pwYredor,v12,0.0,0.0);                          	   	            
      endloop(v9);
   }
   else
   {
      printf("nredor should be greater than 2!\n");
      psg_abort(1);
   }
     
// X Refocussing Pulse

   if(!strcmp(tedor_flag, "ZF")) {    
      delay(tauZ);
      txphase(phX180p1);  obspwrf(aXredor);
      rgpulse(pwXredor,phX180p1,0.0,0.0);
      delay(tauZ);
   }
   if(!strcmp(tedor_flag, "BASE")) {   
      txphase(phX180p1); obspwrf(getval("aXshp1"));
      delay(tauB); 
      _pboxpulse(shp1,phX180p1); 
      delay(tauB); 
   } 
      
// second half of redor sequence

   assign(v14,v8);
   if (nredor >= 1) {                                                
      loop(v8,v9);
         getelem(ph1Yredor,v11,v12); dec2phase(v12); incr(v11);
         dec2rgpulse(pwYredor,v12,0.0,0.0);
         delay(tauY);
         getelem(ph1Yredor,v11,v12); dec2phase(v12); incr(v11);                           
         dec2rgpulse(pwYredor,v12,0.0,0.0);
         delay(tauY);                             	   	            
      endloop(v9);
      getelem(ph1Yredor,v11,v12); dec2phase(v12); incr(v11);
      dec2rgpulse(pwYredor,v12,0.0,0.0);
      delay(tauY);
      getelem(ph1Yredor,v11,v12); dec2phase(v12); incr(v11);                           
      dec2rgpulse(pwYredor,v12,0.0,0.0);
      delay(tauY0);
   }

   _dseqoff(redor);

// z-filter 

   if (getval("aHzf") > 0) {
      decpwrf(getval("aHzf"));
      decon();
   }

   obspwrf(getval("aX90"));
   txphase(phXzf1);
   rgpulse(pwX90,phXzf1,0.0,0.0);

   if(!strcmp(ZF_flag, "grad")) {   
      rgradient('z',getval("gzlvl1"));
      delay(getval("tZF2"));
      rgradient('z',0.0);
   }
  
   delay(getval("tZF"));
  
   dec2pwrf(getval("aY90"));
   dec2phase(phYzf1);
   dec2rgpulse(getval("pwY90"),phYzf1,0.0,0.0);

   if (getval("aHzf") > 0) {
      decoff();
   }

// t1 indirect Y dimension with CT support

   _dseqon(dec);

   // Handle evolution time based on CT flag
   if (!strcmp(CT_flag, "y")) {
      // Constant Time evolution
      d2_CT = d2;  // actual evolution time from NUS or linear sampling
      double t1_delay_pre = tCT/2.0 - d2_CT/2.0;
      double t1_delay_post = tCT/2.0 - d2_CT/2.0;
      
      if (t1_delay_pre < 0) {
         printf("Warning: d2 (%f us) exceeds tCT/2 (%f us), adjusting\n", d2*1e6, (tCT/2.0)*1e6);
         t1_delay_pre = 0;
         t1_delay_post = tCT - d2_CT;
         if (t1_delay_post < 0) t1_delay_post = 0;
      }
      
      delay(t1_delay_pre);
      
      // Echo refocusing during evolution if d2 > 4*pwX90
      if (d2_CT > 4.0*pwX90) {
         delay(d2_CT/2.0-2.0*pwX90);
         rgpulse(pwX90,0,0.0,0.0);
         rgpulse(2.0*pwX90,90,0.0,0.0);
         rgpulse(pwX90,0, 0.0,0.0);
         delay(d2_CT/2.0-2.0*pwX90);
      }
      else {
         delay(d2_CT);
      }
      
      delay(t1_delay_post);
   }
   else {
      // Standard evolution (with potential NUS)
      if (d2 > 4.0*pwX90) {
         delay(d2/2.0-2.0*pwX90);
         rgpulse(pwX90,0,0.0,0.0);
         rgpulse(2.0*pwX90,90,0.0,0.0);
         rgpulse(pwX90,0, 0.0,0.0);
         delay(d2/2.0-2.0*pwX90);
      }
      else {
         delay(d2);
      }
   }

   _dseqoff(dec);

   _dseqon(redor);

// 90 Y pulse

   dec2pwrf(getval("aY90"));
   dec2phase(phY90);
   dec2rgpulse(getval("pwY90"),phY90,0.0,0.0);

// Delay to get rotor synchronized 

   double tcomp_base = getval("pwY90")*2+pwX90*2+getval("tZF");
   double tcomp;
   
   if (!strcmp(CT_flag, "y")) {
      tcomp = ceil((tCT + tcomp_base)/taur);
      tcomp = (tcomp*taur) - (tCT + tcomp_base);
   }
   else {
      tcomp = ceil((d2 + tcomp_base)/taur);
      tcomp = (tcomp*taur) - (d2 + tcomp_base);
   }
   
   printf("tcomp (us):%f\n",tcomp*1e6);
   delay(tcomp);

   obspwrf(getval("aX90"));
   txphase(phXzf1);
   rgpulse(pwX90,phX90,0.0,0.0);
  
//-------------------
//Second REDOR period
//-------------------

   obspwrf(aXredor);
   txphase(phX180p1);     
   assign(v14,v8);
   if (nredor >= 1) {         
      dec2pwrf(aYredor);      
      initval(0,v11);                   
      delay(tauY0);
      getelem(ph2Yredor,v11,v12); dec2phase(v12); incr(v11);
      dec2rgpulse(pwYredor,v12,0.0,0.0);
      getelem(ph2Yredor,v11,v12); dec2phase(v12); incr(v11);         
      delay(tauY);                  
      dec2rgpulse(pwYredor,v12,0.0,0.0);            
      loop(v8,v9);
         delay(tauY);
         getelem(ph2Yredor,v11,v12); dec2phase(v12); incr(v11);
         dec2rgpulse(pwYredor,v12,0.0,0.0);
         getelem(ph2Yredor,v11,v12); dec2phase(v12); incr(v11);         
         delay(tauY);                  
         dec2rgpulse(pwYredor,v12,0.0,0.0);                             	   	            
      endloop(v9);
   }
     
// X Refocussing Pulse

   if(!strcmp(tedor_flag, "ZF")) {    
      delay(tauZ);
      txphase(phX180p1);  obspwrf(aXredor);
      rgpulse(pwXredor,phX180p1,0.0,0.0);
      delay(tauZ);
   }

   if(!strcmp(tedor_flag, "BASE")) {   
      txphase(phX180p1); obspwrf(getval("aXshp1"));
      delay(tauB); 
      _pboxpulse(shp1,phX180p1); 
      delay(tauB); 
   } 

// 2nd Half of REDOR
   assign(v14,v8);
   if (nredor >= 1) {                                                
      loop(v8,v9);
         getelem(ph2Yredor,v11,v12); dec2phase(v12); incr(v11);
         dec2rgpulse(pwYredor,v12,0.0,0.0);
         delay(tauY);
         getelem(ph2Yredor,v11,v12); dec2phase(v12); incr(v11);                           
         dec2rgpulse(pwYredor,v12,0.0,0.0);
         delay(tauY);                             	   	            
      endloop(v9);
      getelem(ph2Yredor,v11,v12); dec2phase(v12); incr(v11);
      dec2rgpulse(pwYredor,v12,0.0,0.0);
      delay(tauY);
      getelem(ph2Yredor,v11,v12); dec2phase(v12); incr(v11);                           
      dec2rgpulse(pwYredor,v12,0.0,0.0);
      delay(tauY0);
   }

  _dseqoff(redor);

// z-filter 

   if (getval("aHzf") > 0) {
      decpwrf(getval("aHzf"));
      decon();
   }

   obspwrf(getval("aX90"));
   txphase(phXzf1);
   rgpulse(getval("pwX90"),phXzf1,0.0,0.0);
  
   delay(getval("tZF"));
  
   obspwrf(getval("aX90"));
   txphase(phXzf2);
   rgpulse(getval("pwX90"),phXzf2,0.0,0.0);

   if (getval("aHzf") > 0) {
      decoff();
   }

// Begin Acquisition 
   
   _dseqon(dec);

   obsblank(); dec2blank();
   delay(getval("rd"));   
   startacq(getval("ad"));
   acquire(np, 1/sw);
   endacq();

   // Adjust relaxation delay for CT and NUS
   if (!strcmp(CT_flag, "y")) {
      if (tRF > tCT) {
         delay(tRF - tCT);
      }
   }
   else {
      if (tRF > d2) {
         delay(tRF - d2);
      }
   }

   _dseqoff(dec);
   _dseqon(redor);
   delay(tredor_comp);
   _dseqoff(redor);
   obsunblank(); dec2unblank();  
}
