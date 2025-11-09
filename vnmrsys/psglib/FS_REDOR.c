/*
	FS_REDOR.c 

	History
	
	AJN 1/12/11 

	X detected, H decoupled, Y dephased REDOR, with soft pulse options 
	on X hard or soft pi pulse
	on Y hard, soft or no pulse
	Extended the phase cycle from 16 steps to 32. June 5th 2016.
			                                    
*/

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"

// define values for phasetables

static int table1[32] =  {0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,1,1,3,3,1,1,3,3,1,1,3,3,1,1,3,3};  	    			// phH90    	
static int table2[32]  = {0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,2,2,2,2,3,3,3,3};			// phXhx        
static int table3[32] =  {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};           			// phHhx
static int table6[32] =  {0,1,0,1,2,3,2,3,0,1,0,1,2,3,2,3,2,3,2,3,0,1,0,1,2,3,2,3,0,1,0,1};			// phX180 "normal" and  hard in soft/hard pair
static int table7[32]=   {0,0,0,0,2,2,2,2,1,1,1,1,3,3,3,3,2,2,2,2,0,0,0,0,3,3,3,3,1,1,1,1};	// phX1802 soft in soft/hard pair
static int table9[16] =  {0,1,0,1,2,3,2,3,0,1,0,1,2,3,2,3};			// phY180  	
static int table14[32]=  {0,2,2,0,3,1,1,3,0,2,2,0,3,1,1,3,2,0,0,2,1,3,3,1,2,0,0,2,1,3,3,1};			// phRec

static int table4[32] =  {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};   	    			// phredor	

#define phH90 t1
#define phXhx t2
#define phHhx t3
#define phredor t4
#define phX180 t6
#define phX1802 t7
#define phY180 t9
#define phRec t14

void pulsesequence() {

/*Define Variables and Objects and Get Parameter Values*/

   double duty ,tRF, d2_;
   
   int decmode = getval("decmode");
   int decmodem = getval("decmodem");
   
   int i=0;
   
   //parameters for REDOR part
   
    int flag_redor=getval("flag_redor");                 // S0, S' or S
   	int redor_type=getval("redor_type");                 // REDOR pi pulses on X or Y or both
   	int echo_type=getval("echo_type");                   // type of refocussing pulses
   
   	double aXredor = getval("aXredor");                  // REDOR X pulse amplitude 
   	double pwXredor = getval("pwXredor");                // REDOR X pulse width
    
   	double aYredor = getval("aYredor");    		     	// REDOR Y pulse amplitude 
   	double pwYredor = getval("pwYredor");  		     	// REDOR Y pulse width
   
   	double aX180 = getval("aX180");    		     		// Hard X Pi pulse amplitude 
   	double pwX180 = getval("pwX180");  		     		// Hard X Pi pulse width

   	double aY180 = getval("aY180");    		     		// Hard Y pi pulse amplitude 
   	double pwY180 = getval("pwY180");  		     		// Hard Y Pi pulse width

   	double pwXshp1 = getval("pwXshp1");				    // X soft pulse
   	double pwYshp1 = getval("pwYshp1");				    // Y soft pulse

   	double srate = getval("srate");  			     	// spin rate

   	double taur = 1.0/srate;					 		// time of 1 rotor cycle
   
   	double tauY = taur/2.0 -  pwYredor;			     	// REDOR delay = half rotor cycle - 180 REDOR Y pulse
   	double tauX = taur/2.0 -  pwXredor;                 // REDOR delay = half rotor cycle - 180 REDOR X pulse
   	
   	double tauY0=0.0; double tauY1=0.0; 
   	double tauHN=0.0; double tauSN=0.0; double tauHH=0.0;
   	double tauSH=0.0; double tauHS=0.0; double tauSS=0.0;
   	
   	tauY0=(pwYredor - pwXredor)/2.0;                       //For extra time during redor_type=1
   	tauY1=tauX-tauY0;                                   
   	
   	if (redor_type==0) 
   	{
   		tauHN = taur - pwX180/2.0 - pwYredor/2.0;     // delay for refocusing in middle of REDOR Hard None (flag_redor==0)
   
   		int n=floor((pwXshp1)/taur);
   		tauSN = (n*taur - pwXshp1 - pwYredor)/2.0 ;	// delay for refocusing in middle of REDOR Soft none (flag_redor==0)
   						            
   		if(pwX180>pwY180){tauHH = taur - pwX180/2.0 - pwYredor/2.0;}  // delay for refocusing for Hard Hard (echo_type==0)
   		else{tauHH = taur - pwY180/2.0 - pwYredor/2.0;}
   						               
		if(pwXshp1>pwYshp1)                  // delay for refocusing Soft Soft (echo_type==1)
    	{
   			n = floor((pwXshp1)/taur);
   			tauSS = (n*taur - pwXshp1- pwYredor)/2.0;	
   		}
		else if(pwYshp1>pwXshp1)
   		{
   			n = floor((pwYshp1)/taur);
   			tauSS = (n*taur - pwYshp1 - pwYredor)/2.0;	
   		}        
		
		n = floor((pwXshp1)/taur);                   // delay for refocusing in middle of REDOR soft hard (echo_type==2)
		tauSH = (n*taur - pwXshp1- pwYredor)/2.0;	                                     
   
   			
   		n = floor((pwYshp1)/taur);                  // delay for refocusing in middle of REDOR hard soft (echo_type==3)
   		tauHS = (n*taur - pwYshp1 - pwYredor)/2.0;	
   	}
   	
   	else if (redor_type > 0)
   	{ 
   		tauHN = taur - pwX180/2.0 - pwXredor/2.0;     // delay for refocusing in middle of REDOR Hard None (flag_redor==0)
   
   		int n=floor((pwXshp1)/taur);
   		tauSN = (n*taur - pwXshp1 - pwXredor)/2.0 ;	// delay for refocusing in middle of REDOR Soft none (flag_redor==0)
   				            
   		if(pwX180>pwY180){tauHH = taur - pwX180/2.0 - pwXredor/2.0;}   // delay for refocusing for Hard Hard (echo_type==0)
   		else {tauHH = taur - pwY180/2.0 - pwXredor/2.0;}
 					               
		if(pwXshp1>pwYshp1)                        // delay for refocusing Soft Soft (echo_type==1)
    	{
   			n = floor((pwXshp1)/taur);
   			tauSS = (n*taur - pwXshp1- pwXredor)/2.0;	
   		}
		else if(pwYshp1>pwXshp1)
   		{
   			n = floor((pwYshp1)/taur);
   			tauSS = (n*taur - pwYshp1 - pwXredor)/2.0;	
   		}        
   			
		n = floor((pwXshp1)/taur);                 // delay for refocusing in middle of REDOR soft hard (echo_type==2)
		tauSH = (n*taur - pwXshp1- pwXredor)/2.0;	                                     
   		
   		n = floor((pwYshp1)/taur);                // delay for refocusing in middle of REDOR hard soft (echo_type==3)
   		tauHS = (n*taur - pwYshp1 - pwXredor)/2.0;	
   	}
   // check to make sure delays are positive

   printf("tauHH:%f tauHN:%f tauHS:%f\n",tauHH,tauHN,tauHS);
   printf("tauSN:%f tauSH:%f tauSS:%f\n",tauSN,tauSH,tauSS);
   printf("tauY:%f tauY0:%f tauY1:%f \n",tauY,tauY0,tauY1);

   if( tauHH < 1e-7 ){ tauHH=tauHH + taur;}
   if( tauHN < 1e-7 ){ tauHN=tauHN + taur;}
   if( tauHS < 1e-7 ){ tauHS=tauHS + taur;}
   if( tauSH < 1e-7 ){ tauSH=tauSH + taur;}
   if( tauSS < 1e-7 ){ tauSS=tauSS + taur;}
   if( tauSN < 1e-7 ){ tauSN=tauSN + taur;}

   printf("after neg check,taunSN:%f tauSH:%f tauSS:%f\n",tauSN,tauSH,tauSS);
   printf("after neg check, tauHH:%f tauHN:%f tauHS:%f\n",tauHH,tauHN,tauHS);

    // USING XY-4! Change this if the XY phase cycle changes size
	initval(4,v15);
   	initval(0,v11);

	
   	int nredor = (int) (getval("nredor"));	     		// # of REDOR cycles bracketing 180 pulses 
   	double tredor = taur * 2 * nredor;
   	int nredor_max = (int) (getval("nredor_max"));
   	initval(2*nredor_max,v13);
   	double tredor_max = taur * 2 * nredor_max;
   	double tredor_comp = tredor_max - tredor ;
   	printf("tredor:%f tredor_max:%f tredor_comp:%f\n",tredor,tredor_max,tredor_comp);
   	if(tredor_comp<0.0){tredor_comp=0;}
   	initval(nredor,v14);     			             	// v14 = max. # of REDOR cycles bracketing 180 pulses
   	int nredor2=nredor-1; 
   	initval(nredor2,v1);                               // for running the redor loop for redor_type=1
   	int  mredor_comp = (int) (tredor_comp/taur);
   	int mredor_comp2=(int) (mredor_comp-1);
   	initval(mredor_comp2, v3);
   	initval(mredor_comp,v4);
   	initval(nredor_max*2-1,v6);   			             	// v14 = max. # of REDOR cycles bracketing 180 pulses

   double rd = getval("rd");				     	// ring-down delay
   
   //Definitions for REDOR soft pulses part
   
   PBOXPULSE shp1 = getpboxpulse("shp1X",0,1);
   PBOXPULSE shp2 = getpboxpulse("shp1Y",0,1);

   strcpy(shp1.ch,"obs");
   putCmd("chXshp1 ='obs'\n");
    
   strcpy(shp2.ch,"dec2");
   putCmd("chYshp1 ='dec2'\n");
   
   //CP part of the code
     
   CP hx = getcp("HX",0.0,0.0,0.0,1); 
   strcpy(hx.fr,"dec");
   strcpy(hx.to,"obs");
   putCmd("frHX='dec'\n"); 
   putCmd("toHX='obs'\n");
   
   //Decoupling part of the code
     
   DSEQ dec = getdseq("H");
   DSEQ redor = getdseq("Hredor");
   DSEQ dec2 = getdseq("Y");
   DSEQ dec3 = getdseq("Z");

// Probe Protection -- limit to 5% total duty CMR 5/18/2019
 
   duty = (getval("tHX") + 2*tredor_max + at) / (d1 + getval("tHX") + 2*tredor_max + at); //this includes both 1H high power decoupling and Y rf compensation for REDOR S0

   printf("duty %f \n", duty); 
    
   if (duty > 0.05) {
      printf("Duration of sequence too long. Abort!\n");
      psg_abort(1);
   }
   
//Error message if nredor_max set incorrectly

	if (nredor_max == 0) {
		printf("nredor_max cannot be zero. Set it to greater than nredor.Abort!\n");
		psg_abort(1);
	}
	
	if (nredor_max < nredor) {
		printf("nredor_max has to be greater than nredor.Abort!\n");
		psg_abort(1);
	}

// Set Phase Tables
 
   settable(phH90,32,table1);
   settable(phXhx,32,table2);
   settable(phHhx,32,table3);
   settable(phredor,32,table4);
   settable(phX180,32,table6);
   settable(phX1802,32,table7);
   settable(phY180,16,table9);
   settable(phRec,32,table14);
   setreceiver(phRec);

   
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
// REDOR
//------------------

  _dseqon(redor);

// first half of redor sequence

  	obspwrf(getval("aX180"));
  	txphase(phX180);
      
	delay(taur);
    
    if ((flag_redor == 0) && (redor_type==0)) // S0, i.e. no pi pulse trains and no refocussing pulse on Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
      	{
    		dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwYredor/2.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);
    			delay(tauY);
    			delay(pwYredor);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11); 
    			delay(tauY);
    			delay(pwYredor);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwYredor/2.0);}
    }
    
    if ((flag_redor == 0) && (redor_type == 1)) // S0, i.e. no pi pulse trains and no refocussing pulse on Y
    {
    	assign(v1,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aXredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwXredor/2.0);
    		modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    		delay(tauX);
    		delay(pwXredor);
    		modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);
    		delay(tauY1);
    		delay(pwYredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauY1);
    			delay(pwXredor);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY1);
    			delay(pwYredor);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwXredor/2.0);}
    }
    
    if ((flag_redor == 0) && (redor_type == 2)) // S0, i.e. no pi pulse trains and no refocussing pulse on Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aXredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwXredor/2.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauX);
    			delay(pwXredor);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);  
    			delay(tauX);
    			delay(pwXredor);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwXredor/2.0);}
    }
    
    if ((flag_redor >0) && (redor_type == 0)) // S or S', with two pi pulses per rotor period on Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
    	{
    		dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwYredor/2.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);
    			delay(tauY);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwYredor/2.0);}
    }
    
    if ((flag_redor >0) && (redor_type==1)) // S or S', with two alternative pi pulses on H and Y per rotor period
    {
    	assign(v1,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aXredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwXredor/2.0);
    		modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    		delay(tauX);
    		rgpulse(pwXredor,v12,0.0,0.0);
    		modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);
    		delay(tauY1);
    		dec2rgpulse(pwYredor,v12,0.0,0.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauY1);
    			rgpulse(pwXredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY1);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwXredor/2.0);}
    }		
    
    if ((flag_redor> 0) && (redor_type==2)) // S or S', with two pi pulses on H per rotor period
    {
    	assign(v14,v8);
    	if (nredor>=1)
    	{
    		obspwrf(aXredor);dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwXredor/2.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauX); 
    			rgpulse(pwXredor,v12,0.0,0.0);
    			getelem(phredor,v11,v12); txphase(v12); incr(v11);  
    			delay(tauX);
    			rgpulse(pwXredor,v12,0.0,0.0);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwXredor/2.0);}
    }
     
// X or Y Refocussing Pulse

if (flag_redor < 2) // S0 or S': no refocussing pulse on Y
{ 
	if ((echo_type == 0) || (echo_type ==3))// hard refocussing pulse on X
	{
		if((redor_type == 1)&&(nredor >= 1)) {delay(tauHN-tauY0);}
		else {delay(tauHN);}
   		txphase(phX180);  obspwrf(aX180);
   		rgpulse(pwX180,phX180,0.0,0.0);
   		delay(tauHN);
   	}
   	else if ((echo_type == 1) || (echo_type==2))  // soft refocussing pulse on X
   	{
  		if((redor_type==1)&&(nredor>=1)) {delay(tauSN-tauY0);}
		else {delay(tauSN);}
  		txphase(phX180); 
  		_pboxpulse(shp1,phX180);
  		delay(tauSN);
  	}
}

if (flag_redor == 2)   // S: refocussing pulse on Y present too
{
	if (echo_type == 0) // hard refocussing pulse on both X and Y
	{
		if ((redor_type==1) && (nredor>=1)) {delay(tauHH-tauY0);}
		else {delay(tauHH);}
   		txphase(phX180);  obspwrf(aX180);
   		dec2phase(phY180);	dec2pwrf(aY180);
   		sim3pulse(pwX180,0.0,pwY180,phX180,0.0,phY180,0.0,0.0); // simultaneous 180 pulse on H and Y channel
   		delay(tauHH);
   	}
	
	else if (echo_type == 1)  // soft refocussing pulse on both H and Y
	{
		if ((redor_type==1) && (nredor>=1)) {delay(tauSS-tauY0);}
		else {delay(tauSS);}
 		_pboxsimpulse(shp1,shp2,phX180,phY180);
 		delay(tauSS);
 	}
	
	else if (echo_type == 2) //soft refocussing pulse on H and hard on Y
	{
		if ((redor_type==1) && (nredor>=1)) {delay(tauSH-tauY0);}
		else {delay(tauSH);}
  		txphase(phX180); dec2phase(phY180);
  		_pboxsimpulse(shp1,shp2,phX180,phY180);
  		delay(tauSH);
 	} 
 	
 	else if (echo_type == 3) //hard refocussing pulse on H and soft on Y
 	{
 		if ((redor_type==1) && (nredor>=1)) {delay(tauHS-tauY0);}
		else {delay(tauHS);}
   		txphase(phX180); dec2phase(phY180);
   		_pboxsimpulse(shp1,shp2,phX180,phY180);
		delay(tauHS);		
   	}
}	
    
// second half of redor sequence

if ((flag_redor == 0) && (redor_type==0)) // S0, i.e. no pi pulse trains and no refocussing pulse on Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
      	{
    		dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwYredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);
    			delay(tauY);
    			delay(pwYredor);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY);
    			delay(pwYredor);
    		endloop(v9);
    		delay(taur - (pwYredor/2.0));
    	}
    
    	else 
    	{delay(pwYredor/2.0);
    	delay(taur);}
    	
    }
    
    if ((flag_redor == 0) && (redor_type == 1)) // S0, i.e. no pi pulse trains and no refocussing pulse on Y
    {
    	assign(v1,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aXredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwXredor);
    		modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    		delay(tauX);
    		delay(pwXredor);
    		modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);
    		delay(tauY1);
    		delay(pwYredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauY1);
    			delay(pwXredor);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY1);
    			delay(pwYredor);
    		endloop(v9);
    		delay(taur - (pwYredor/2.0));
    	}
    
    	else 
    	{delay(pwXredor/2.0);
    	delay(taur);}
    }
    
    if ((flag_redor == 0) && (redor_type == 2)) // S0, i.e. no pi pulse trains and no refocussing pulse on Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aXredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwXredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauX);
    			delay(pwXredor);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);  
    			delay(tauX);
    			delay(pwXredor);
    		endloop(v9);
    		delay(taur - (pwXredor/2.0));
    	}
    
    	else 
    	{delay(pwXredor/2.0);
    	delay(taur);}
    	
    }
    
    if ((flag_redor >0) && (redor_type == 0)) // S or S', with two pi pulses per rotor period on Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
    	{
    		dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwYredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);
    			delay(tauY);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    		endloop(v9);
    		delay(taur - (pwYredor/2.0));
    	}
    
    	else 
    	{delay(pwYredor/2.0);
    	delay(taur);}
    	
    }

    if ((flag_redor >0) && (redor_type==1)) // S or S', with two alternative pi pulses on H and Y per rotor period
    {
    	assign(v1,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aXredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwXredor);
    		modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    		delay(tauX);
    		rgpulse(pwXredor,v12,0.0,0.0);
    		modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);
    		delay(tauY1);
    		dec2rgpulse(pwYredor,v12,0.0,0.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauY1);
    			rgpulse(pwXredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY1);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    		endloop(v9);
    		delay(taur - (pwYredor/2.0));
    	}
    
    	else 
    	{delay(pwXredor/2.0);
    	delay(taur);}
    	
    }		
    
    if ((flag_redor> 0) && (redor_type==2)) // S or S', with two pi pulses on H per rotor period
    {
    	assign(v14,v8);
    	if (nredor>=1)
    	{
    		obspwrf(aXredor);dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwXredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauX); 
    			rgpulse(pwXredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(phredor,v16,v12); txphase(v12); incr(v11); 
    			delay(tauX);
    			rgpulse(pwXredor,v12,0.0,0.0); 
    		endloop(v9);
    		delay(taur - (pwXredor/2.0));
    	}
    
    	else 
    	{delay(pwXredor/2.0);
    	 delay(taur);}
    	
    }		
   else { delay(pwYredor/2.0);}

  _dseqoff(redor);

// Begin Acquisition 
   
   _dseqon(dec);
   _dseqon(dec2);
   _dseqon(dec3);

    //dec3blank();
    
   obsblank(); 
   delay(getval("rd"));   
   startacq(getval("ad"));
   acquire(np, 1/sw);
   endacq();

   _dseqoff(dec);
   _dseqoff(dec2);
   _dseqoff(dec3);
   
// CONSTANT DUTY CYCLE

	obsunblank(); decunblank(); _unblank34();
	
   _dseqon(redor);
   
 	delay(tredor_comp); 
 	
 	_dseqoff(redor);  // CMR 3/12/2019 keep Hduty the same regardless of nredor_max-nredor value
 	       
	if (((nredor==0)||(flag_redor==0)) && (redor_type==0))
	{
		assign(v13,v2);
    		dec2pwrf(aYredor);
    		delay(pwYredor/2.0);
    		loop(v2,v8);
    			delay(tauY);
    			dec2rgpulse(pwYredor,0,0.0,0.0);
    			delay(tauY);
    			dec2rgpulse(pwYredor,0,0.0,0.0);
    		endloop(v8);
    }
    
    else if(((nredor==0)||(flag_redor==0)) && (redor_type==1))
    {
    	assign(v13,v2);
    	obspwrf(aXredor); dec2pwrf(aYredor);
    	delay(pwXredor/2.0);
    	loop(v2,v8);
    		delay(tauY1);
    		rgpulse(pwXredor,0,0.0,0.0); 
    		delay(tauY1);
    		dec2rgpulse(pwYredor,0,0.0,0.0);
    	endloop(v8);
    }
    else if(((nredor==0)||(flag_redor==0)) && (redor_type==2))
    { 
    	assign(v13,v10);
    	obspwrf(aXredor); 
    	delay(pwXredor/2.0);
    	loop(v10,v12);
    		delay(tauX);
    		rgpulse(pwXredor,0,0.0,0.0);
    		delay(tauX);
    		rgpulse(pwXredor,0,0.0,0.0);
    	endloop(v12);
    }
    
   	assign(v4,v11);     
      	if ((flag_redor>0) && (redor_type==0))
		{
			assign(v11,v2);
    		if (nredor>=1)
      		{
    			dec2pwrf(aYredor);
    			delay(pwYredor/2.0);
    			loop(v2,v8);
    				delay(tauY);
    				dec2rgpulse(pwYredor,0,0.0,0.0);
    				delay(tauY);
    				dec2rgpulse(pwYredor,0,0.0,0.0);
    			endloop(v8);
    			delay(tredor_max-tredor_comp);
    		}
   		 }
    	else if((flag_redor>0)&&(redor_type==1))
    	{
    		assign(v11,v2);
    		if (nredor>=1)
      		{
    			obspwrf(aXredor); dec2pwrf(aYredor);
    			delay(pwXredor/2.0);
    			loop(v2,v8);
    				delay(tauY1);
    				rgpulse(pwXredor,0,0.0,0.0); 
    				delay(tauY1);
    				dec2rgpulse(pwYredor,0,0.0,0.0);
    			endloop(v8);
    			delay(tredor_max-tredor_comp);
    		}
    	}
    	else if((flag_redor>0)&&(redor_type==2))
    	{ 
    		assign(v11,v10);
        	if (nredor>=1)
      		{
    			obspwrf(aXredor); dec2pwrf(aYredor);
    			delay(pwXredor/2.0);
    			loop(v10,v12);
    				delay(tauX);
    				rgpulse(pwXredor,0,0.0,0.0);
    				delay(tauX);
    				rgpulse(pwXredor,0,0.0,0.0);
    			endloop(v12);
    			delay(tredor_max-tredor_comp);
    		}
    	} 
    	

    
   obsunblank(); decunblank(); _unblank34();
}
