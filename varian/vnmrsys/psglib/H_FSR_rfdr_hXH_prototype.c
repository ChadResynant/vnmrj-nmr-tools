/*HhXH.c - JMC 20/02/2015
			CGB added X and Y redor dephasing  capabilities before
			rfdr mixing period with 32 step phase cycle */


#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidpboxpulse.h"

static int table1[4]   = {0,0,0,0};         // phH90
static int table10[16] = {0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2}; // ph1H90
static int table11[4]  = {0,0,0,0};         // phHrfdr
static int table12[16] = {2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0}; // ph2H90
static int table3[4]   = {1,1,1,1};         // phHhx
static int table2[4]   = {0,0,0,0};         // phXhx
static int table5[4]   = {3,3,3,3};         // ph1X90
//static int table7[4]   = {1,3,1,3};         // ph2X90
static int table7[8]   = {1,1,1,1,3,3,3,3};         // ph2X90_test

static int table6[4]   = {0,0,0,0};         // phHpxy
static int table9[4]   = {0,0,0,0};         // phXxh
static int table8[4]   = {1,1,1,1};         // phHxh
//static int table14[8]  = {0,2,0,2,0,2,0,2}; // phRec
static int table14[8]  = {0,2,0,2,2,0,2,0}; // phRec_test2

static int table18[16] = {0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1}; // phH180CT
static int table16[4]  = {0,0,1,1};         // phX180CT
static int table17[16] = {0,2,2,0,2,0,0,2,0,2,2,0,2,0,0,2}; // phRecCT
static int table15[4]  = {1,1,1,1};         // phHecho - solid echo cycle
//static int table29[4] = {1,1,1,1};  //  ,0,0,1,1,2,2,1,1,1,1,2,2,3,3,2,2,2,2,3,3,0,0,1,1,1,1,0,0};          // phH180-hard echo on 1H
static int table29[4] = {1,2,1,2}; // phH180-hard test

// For the REDOR part  	       
static int table13[32]=   {0,0,0,0,2,2,2,2,1,1,1,1,3,3,3,3,2,2,2,2,0,0,0,0,3,3,3,3,1,1,1,1};	    // phH1802_soft 
static int table22[16] =  {0,1,0,1,2,3,2,3,0,1,0,1,2,3,2,3};			                            // phX180  	
static int table23[32] =  {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};   	    // ph1Xredor	
static int table24[16] =  {0,1,0,1,2,3,2,3,0,1,0,1,2,3,2,3};			                            // phY180  	
static int table25[32] =  {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};   	    // ph1Yredor
static int table30[32]=   {0,0,0,0,2,2,2,2,1,1,1,1,3,3,3,3,2,2,2,2,0,0,0,0,3,3,3,3,1,1,1,1};	    // phH1802 soft 



#define phH90 t1
#define phXhx t2
#define phHhx t3
#define ph1X90 t5
#define phHpxy t6
#define ph2X90 t7
#define phHxh t8
#define phXxh t9
#define ph1H90 t10
#define phHrfdr t11
#define ph2H90 t12
#define phHecho t15
#define phRec t14
#define phX180CT t16
#define phRecCT t17
#define phH180CT t18


// For the REDOR part  	       
#define phH1802_soft t13
#define phX180 t22
#define ph1Xredor t23
#define phY180 t24
#define ph1Yredor t25
#define phH180 t29
#define phH1802 t30

void pulsesequence() {

    check_array();

    // Define Variables and Objects and Get Parameter Values

    double duty;

    int decmode = getval("decmode");
    int decmodem = getval("decmodem");

    // parameters for REDOR part
    
    int flag_redor=getval("flag_redor");                 // S0, S' or S
   	int redor_type=getval("redor_type");                 // REDOR pi pulses on H or Y or X or XY
   	int echo_type=getval("echo_type");                   // type of refocussing pulses
   
   	double aHredor = getval("aHredor");                  // REDOR H pulse amplitude 
   	double pwHredor = getval("pwHredor");                // REDOR H pulse width

   	double aXredor = getval("aXredor");    		     	// REDOR X pulse amplitude 
   	double pwXredor = getval("pwXredor");  		     	// REDOR X pulse width

   	double aYredor = getval("aYredor");    		     	// REDOR Y pulse amplitude 
   	double pwYredor = getval("pwYredor");  		     	// REDOR Y pulse width
   
   	double aH180 = getval("aH180");    		     		// Hard H Pi pulse amplitude 
   	double pwH180 = getval("pwH180");  		     		// Hard H Pi pulse width

   	double aX180 = getval("aX180");    		     		// Hard X pi pulse amplitude 
   	double pwX180 = getval("pwX180");  		     		// Hard X Pi pulse width

   	double aY180 = getval("aY180");    		     		// Hard Y pi pulse amplitude 
   	double pwY180 = getval("pwY180");  		     		// Hard Y Pi pulse width

   	double pwHshp1 = getval("pwHshp1");				    // H soft pulse
   	double pwXshp1 = getval("pwXshp1");				    // X soft pulse
   	double pwYshp1 = getval("pwYshp1");				    // Y soft pulse

   	double srate = getval("srate");  			     	// spin rate

   	double taur = 1.0/srate;					        // time of 1 rotor cycle
   
   	double tauY = taur/2.0 -  pwYredor;			     	// REDOR delay = half rotor cycle - 180 REDOR Y pulse
   	double tauX = taur/2.0 -  pwXredor;			     	// REDOR delay = half rotor cycle - 180 REDOR X pulse
   	double tauH = taur/2.0 - pwHredor;                   // REDOR delay = half rotor cycle - 180 REDOR H pulse
   	
   	double tauY0=0.0; double tauY1=0.0; 
   	double tauX0=0.0; double tauX1=0.0; 
   	
   	double tauHN=0.0; double tauSN=0.0; double tauHH=0.0;
   	double tauSH=0.0; double tauHS=0.0; double tauSS=0.0;

    tauX0=(pwXredor - pwHredor)/2.0;                       //For extra time during redor_type=1, X   	
    tauY0=(pwYredor - pwHredor)/2.0;                       //For extra time during redor_type=1, Y
   	tauY1=tauH-tauY0;                                   
   	
   	/*
   	flag_redor 	= 0 : S0
 		   	= 1 : S'
   			= 2 : S

   	redor_type	= 0 : pi pulses on Y
			= 1 : pi pulses on H and Y
			= 2 : pi pulses on H
 			= 3 : pi pulses on X and Y
   	*/
   	
   	
   	if (redor_type==0) 
   	{
   		tauHN = taur - pwH180/2.0 - pwYredor/2.0;     // delay for refocusing in middle of REDOR Hard None (flag_redor==0)
   
   		int n=floor((pwHshp1)/taur);
   		tauSN = (n*taur - pwHshp1 - pwYredor)/2.0 ;	// delay for refocusing in middle of REDOR Soft none (flag_redor==0)
   						            
   		if(pwH180>pwY180){tauHH = taur - pwH180/2.0 - pwYredor/2.0;}  // delay for refocusing for Hard Hard (echo_type==0)
   		else{tauHH = taur - pwY180/2.0 - pwYredor/2.0;}
   						               
		if(pwHshp1>pwYshp1)                  // delay for refocusing Soft Soft (echo_type==1)
    	{
   			n = floor((pwHshp1)/taur);
   			tauSS = (n*taur - pwHshp1- pwYredor)/2.0;	
   		}
		else if(pwYshp1>pwHshp1)
   		{
   			n = floor((pwYshp1)/taur);
   			tauSS = (n*taur - pwYshp1 - pwYredor)/2.0;	
   		}        
		
		n = floor((pwHshp1)/taur);                   // delay for refocusing in middle of REDOR soft hard (echo_type==2)
		tauSH = (n*taur - pwHshp1- pwYredor)/2.0;	                                     
   
   			
   		n = floor((pwYshp1)/taur);                  // delay for refocusing in middle of REDOR hard soft (echo_type==3)
   		tauHS = (n*taur - pwYshp1 - pwYredor)/2.0;	
   	}
   	
   	else if (redor_type > 0)
   	{ 
   		tauHN = taur - pwH180/2.0 - pwHredor/2.0;     // delay for refocusing in middle of REDOR Hard None (flag_redor==0)
   
   		int n=floor((pwHshp1)/taur);
   		tauSN = (n*taur - pwHshp1 - pwHredor)/2.0 ;	// delay for refocusing in middle of REDOR Soft none (flag_redor==0)
   				            
   		if(pwH180>pwY180){tauHH = taur - pwH180/2.0 - pwHredor/2.0;}   // delay for refocusing for Hard Hard (echo_type==0)
   		else{tauHH = taur - pwY180/2.0 - pwHredor/2.0;}
 					               
		if(pwHshp1>pwYshp1)                        // delay for refocusing Soft Soft (echo_type==1)
    	{
   			n = floor((pwHshp1)/taur);
   			tauSS = (n*taur - pwHshp1- pwHredor)/2.0;	
   		}
		else if(pwYshp1>pwHshp1)
   		{
   			n = floor((pwYshp1)/taur);
   			tauSS = (n*taur - pwYshp1 - pwHredor)/2.0;	
   		}        
   			
		n = floor((pwHshp1)/taur);                 // delay for refocusing in middle of REDOR soft hard (echo_type==2)
		tauSH = (n*taur - pwHshp1- pwHredor)/2.0;	                                     
   		
   		n = floor((pwYshp1)/taur);                // delay for refocusing in middle of REDOR hard soft (echo_type==3)
   		tauHS = (n*taur - pwYshp1 - pwHredor)/2.0;	
   	}
   	
// check to make sure delays are positive

   	printf("tauHH:%f tauHN:%f tauHS:%f\n",tauHH,tauHN,tauHS);
   	printf("tauSN:%f tauSH:%f tauSS:%f\n",tauSN,tauSH,tauSS);
   	printf("tauY:%f tauY0:%f tauY1:%f \n",tauY,tauY0,tauY1);
   	printf("tauX:%f tauX0:%f tauX1:%f \n",tauX,tauX0,tauX1);

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
   	initval(nredor_max*2-1,v6);

   	double rd = getval("rd");				     	// ring-down delay
   	
// Definitions for REDOR soft pulses Part

    PBOXPULSE shp1 = getpboxpulse("shp1H",0,1);
    PBOXPULSE shp2 = getpboxpulse("shp1X",0,1);
    PBOXPULSE shp3 = getpboxpulse("shp1Y",0,1);
    
    strcpy(shp1.ch,"obs");
    putCmd("chHshp1 ='obs'\n");
    
    strcpy(shp2.ch,"dec2");
    putCmd("chXshp1 ='dec2'\n");
   
    strcpy(shp3.ch,"dec3");
    putCmd("chYshp1 ='dec3'\n");
   
   
//XH part of code
   

    double tmd2 = getval("tconst") - getval("d2") - getval("pwX90");
    if (tmd2 < 0.0) {
        tmd2 = 0.0;
    }

    double tmd3 = getval("tconst2") - getval("d3") - getval("pwH90");
    if (tmd3 < 0.0) {
        tmd3 = 0.0;
    }

    char CT[MAXSTR],HS[MAXSTR];
    getstr("CT",CT);
    getstr("HS",HS);

    double tHmix = getval("tHmix");
    CP hx = getcp("HX",0.0,0.0,0,1);

    strcpy(hx.fr,"obs");
    strcpy(hx.to,"dec");
    putCmd("frHX='obs'\n");
    putCmd("toHX='dec'\n");

    MPSEQ pxy = getpxy("pxyH",0,0.0,0.0,0,1);
    strcpy(pxy.ch,"obs");
    putCmd("chHpxy='obs'\n");

    MPSEQ pxy2 = getpxy("pxy2H",0,0.0,0.0,0,1);
    strcpy(pxy2.ch,"obs");
    putCmd("chHpxy2='obs'\n");

    MPSEQ rfdr = getrfdrxy8("rfdrH",0,0.0,0.0,0,1);
    strcpy(rfdr.ch,"obs");
    putCmd("chHrfdr='obs'\n");

    CP xh = getcp("XH",0.0,0.0,0,1);
    strcpy(xh.fr,"dec");
    strcpy(xh.to,"obs");
    putCmd("frXH='dec'\n");
    putCmd("toXH='obs'\n");
// 
//   	redor_types	= 0 : pi pulses on H
// 			= 1 : pi pulses on X
// 			= 2 : pi pulses on Y
//  			= 3 : pi pulses on X and Y
/*
    if  (redor_type==0) {
	DSEQ redor2 = getdseq("Xredor"); 
	DSEQ redor3 = getdseq("Yredor");
	DSEQ redor4 = getdseq("Zredor");
	}
    
    if  (redor_type==1) {
	DSEQ redor3 = getdseq("Yredor"); 
	DSEQ redor4 = getdseq("Zredor");
	}
	
    if  (redor_type==2) {
	DSEQ redor2 = getdseq("Xredor"); 
	DSEQ redor4 = getdseq("Zredor");
	}

    if  (redor_type==3) {
	DSEQ redor4 = getdseq("Zredor");
	}
*/
 	DSEQ redor2 = getdseq("Xredor");  // During REDOR: make sure the channel number is correct, i.e., redor2 for channel2
 	DSEQ redor3 = getdseq("Yredor");  // During REDOR: make sure the channel number is correct, i.e., redor3 for channel3
 	DSEQ redor4 = getdseq("Zredor"); // During REDOR

    DSEQ dec = getdseq("H");    //For t1 X evolution
    DSEQ dec2 = getdseq("X");   // For acq H evolution
    DSEQ dec3 = getdseq("Y");
    DSEQ dec4 = getdseq("Z");

    double t1Hechoinit = getval("tHecho")/2;
    double t2Hechoinit = getval("tHecho")/2;
    double t1Hecho  = t1Hechoinit - getval("pwH90");
    if (t1Hecho < 0.0) {
        t1Hecho = 0.0;
    }
    double t2Hecho  = t2Hechoinit - getval("pwH90") - getval("rd");
    if (t2Hecho < 0.0) {
        t2Hecho = 0.0;
    }

    // Dutycycle Protection

    duty = 4.0e-6 + 3*getval("pwH90") + getval("tHX") + getval("tconst") + getval("tXH") + t1Hecho + t2Hecho + getval("ad") + getval("rd") + at;

    duty = duty/(duty + d1 + 4.0e-6);
    if (duty > 0.1) {
        printf("Duty cycle %.1f%% >10%%. Abort!\n", duty*100);
        psg_abort(1);
    }

    double Gradlvl = (getval("qHpxy2")*2*getval("pwHpxy2")*getval("gzlvl1"));

    Gradlvl = abs(Gradlvl);
    printf("Grad Power %.1f%%.\n", (100*Gradlvl)/300);

    if (Gradlvl > 300) {
        printf("Grad Power %.1f%%. Abort!\n", (100*Gradlvl)/300);
        psg_abort(1);
    }

    // Set Phase Tables

    settable(phH90,4,table1);
    settable(phXhx,4,table2);
    settable(phHhx,4,table3);
    settable(ph1X90,4,table5);
    settable(phHpxy,4,table6);
    settable(ph2X90,8,table7);
    settable(phHxh,4,table8);
    settable(phXxh,4,table9);
    settable(ph1H90,16,table10);
    settable(phHrfdr,4,table11);
    settable(ph2H90,16,table12);
    settable(phRec,8,table14);
    settable(phHecho,4,table15);
    settable(phX180CT,4,table16);
    settable(phH180CT,16,table18);
    settable(phRecCT,16,table17);
    
    //For REDOR 
	settable(phH1802_soft,32,table13);
	settable(phX180,16,table22);
	settable(ph1Xredor,32,table23);
	settable(phY180,16,table24);
	settable(ph1Yredor,32,table25);
   	settable(phH180,4,table29);
   	settable(phH1802,32,table30);
    int id2_ = (int) (d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,2*id2_,4);  /* invert the phases of the CP and */
        tsadd(phRecCT,2*id2_,4);
        tsadd(phXhx,2*id2_,4); /* the receiver for FAD to displace the axial peaks */
    }
    if (phase1 == 2) {
        tsadd(ph1X90,3,4);
    } /* hypercomplex*/

    int id3_ = (int) (d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,2*id3_,4);  /* invert the phases of the CP and */
        tsadd(phRecCT,2*id3_,4);
        tsadd(phH90,2*id3_,4); /* the receiver for FAD to displace the axial peaks */
    }
    if (phase2 == 2) {
        tsadd(phH90,3,4);
    } /* hypercomplex*/

    if (!strcmp(CT,"y")) {
        setreceiver(phRecCT);
    }
    else {
        setreceiver(phRec);
    }
    
    // check for phase,phase2 array bug
    char arraycheck[MAXSTR];
    getstr("array",arraycheck);
    printf("array=%s\n",arraycheck);
    if(!strcmp(arraycheck,"phase,phase2")){
        abort_message("change array to phase2,phase\n");
    }

    // Begin Sequence

    // Initial Homospoil Pulse

    if(!strcmp(HS,"hs")) {
        status(B);
        hsdelay(getval("hst"));
        status(A);
        delay(getval("hstconst"));
    }

    if(!strcmp(HS,"z")) {
        rgradient('z',getval("gzlvl1"));
        delay(getval("tzgrad"));
        rgradient('z',0.0);
        delay(getval("tzgradconst"));
    }

    txphase(phH90); decphase(phXhx);
    obspwrf(getval("aH90")); decpwrf(getval("aXhx"));
    obsunblank(); decunblank(); _unblank34();
    delay(d1);
    sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);

    obspwrf(getval("aH90"));
    decpwrf(getval("aX90"));
    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));


	// initial H 90 degree pulse here:
    rgpulse(getval("pwH90"),phH90,0.0,0.0);


    /*REDOR on dec2 channel*/
    
//first half of REDOR sequence

 	_dseqon(redor2);
 	_dseqon(redor3); 
   	_dseqon(redor4);  
    
    obspwrf(aH180);  // the 180 echo pulse is on the 1H channel
    if ((echo_type==0) || (echo_type==3)) {txphase(phH180);}
    else {txphase(phH180);}
    
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
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    			delay(tauY);
    			delay(pwYredor);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11); 
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
    		obspwrf(aHredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor/2.0);
    		modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    		delay(tauH);
    		delay(pwHredor);
    		modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    		delay(tauY1);
    		delay(pwYredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauY1);
    			delay(pwHredor);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY1);
    			delay(pwYredor);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwHredor/2.0);}
    }
    
    if ((flag_redor == 0) && (redor_type == 2)) // S0, i.e. no pi pulse trains and no refocussing pulse on Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aHredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor/2.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauH);
    			delay(pwHredor);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);  
    			delay(tauH);
    			delay(pwHredor);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwHredor/2.0);}
    }
    
//CGB added
    if ((flag_redor == 0) && (redor_type == 3)) // S0, i.e. no pi pulse trains and no refocussing pulse on X and Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aHredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor/2.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauH);
    			delay(pwHredor);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);  
    			delay(tauH);
    			delay(pwHredor);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwHredor/2.0);}
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
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    			delay(tauY);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);  
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
    		obspwrf(aHredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor/2.0);
    		modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    		delay(tauH);
    		rgpulse(pwHredor,v12,0.0,0.0);
    		modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    		delay(tauY1);
    		dec2rgpulse(pwYredor,v12,0.0,0.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauY1);
    			rgpulse(pwHredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY1);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwHredor/2.0);}
    }		
    
    if ((flag_redor> 0) && (redor_type==2)) // S or S', with two pi pulses on H per rotor period
    {
    	assign(v14,v8);
    	if (nredor>=1)
    	{
    		obspwrf(aHredor);dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor/2.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauH); 
    			rgpulse(pwHredor,v12,0.0,0.0);
    			getelem(ph1Yredor,v11,v12); txphase(v12); incr(v11);  
    			delay(tauH);
    			rgpulse(pwHredor,v12,0.0,0.0);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwHredor/2.0);}
    }		
    
    // CGB added for X and Y pulses 
    if ((flag_redor >0) && (redor_type == 3)) // S or S', with two pi pulses per rotor period on X and Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
    	{
    		decpwrf(aXredor);dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwYredor/2.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    			modn(v11,v15,v16); getelem(ph1Xredor,v16,v12); decphase(v12); incr(v11);
    			if ((tauX>tauY) && (aXredor >0) && (aYredor >0)){
    			delay(tauY);
    			}
    			else if ((tauX<tauY) && (aXredor >0) && (aYredor >0)){
    			delay(tauX);
    			}
    			else if (aYredor == 0){
    			delay(tauX);
    			}
    			else if (aXredor == 0){
    			delay(tauY);
    			}
    			sim3pulse(0.0,pwXredor,pwYredor,v12,v12,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    			modn(v11,v15,v16); getelem(ph1Xredor,v16,v12); decphase(v12); incr(v11);  
    			if ((tauX>tauY) && (aXredor >0) && (aYredor >0)){
    			delay(tauY);
    			}
    			else if ((tauX<tauY) && (aXredor >0) && (aYredor >0)){
    			delay(tauX);
    			}
    			else if (aYredor == 0){
    			delay(tauX);
    			}
    			else if (aXredor == 0){
    			delay(tauY);
    			}
    			sim3pulse(0.0,pwXredor,pwYredor,v12,v12,v12,0.0,0.0);
//    			decrgpulse(pwXredor,v12,0.0,0.0);
//   			dec2rgpulse(pwYredor,v12,0.0,0.0);
    		endloop(v9);
    	}
    
    	else 
    	{delay(pwYredor/2.0);}
    }
       
// H or Y Refocussing Pulse

if (flag_redor < 2) // S0 or S': no refocussing pulse on Y
{ 
	if ((echo_type == 0) || (echo_type ==3))// hard refocussing pulse on H 
	{
		if((redor_type==1)&&(nredor>=1)) {delay(tauHN-tauY0);}
		else {delay(tauHN);}
   		txphase(phH180);  obspwrf(aH180);
   		rgpulse(pwH180,phH180,0.0,0.0);
   		delay(tauHN);
   	}
   	else if ((echo_type == 1) || (echo_type==2))  // soft refocussing pulse on H
   	{
  		if((redor_type==1)&&(nredor>=1)) {delay(tauSN-tauY0);}
		else {delay(tauSN);}
  		txphase(phH1802); 
  		_pboxpulse(shp1,phH1802);
  		delay(tauSN);
  	}
}

if (flag_redor == 2)   // S: refocussing pulse on Y present too
{
	if (echo_type == 0) // hard refocussing pulse on both H and Y
	{
		if ((redor_type==1) && (nredor>=1)) {delay(tauHH-tauY0);}
		else {delay(tauHH);}
   		txphase(phH180);  obspwrf(aH180);
   		dec2phase(phY180);	dec2pwrf(aY180);
   		sim3pulse(pwH180,0.0,pwY180,phH180,0.0,phY180,0.0,0.0); // simultaneous 180 pulse on H and Y channel
   		delay(tauHH);
   	}
	
	else if (echo_type == 1)  // soft refocussing pulse on both H and Y
	{
		if ((redor_type==1) && (nredor>=1)) {delay(tauSS-tauY0);}
		else {delay(tauSS);}
 		_pboxsimpulse(shp1,shp2,phH1802,phY180);
 		delay(tauSS);
 	}
	
	else if (echo_type == 2) //soft refocussing pulse on H and hard on Y
	{
		if ((redor_type==1) && (nredor>=1)) {delay(tauSH-tauY0);}
		else {delay(tauSH);}
  		dec2phase(phY180);
  		_pboxsimpulse(shp1,shp2,phH1802,phY180);
  		delay(tauSH);
 	} 
 	
 	else if (echo_type == 3) //hard refocussing pulse on H and soft on Y
 	{
 		if ((redor_type==1) && (nredor>=1)) {delay(tauHS-tauY0);}
		else {delay(tauHS);}
   		txphase(phH180); dec2phase(phY180);
   		_pboxsimpulse(shp1,shp2,phH180,phY180);
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
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    			delay(tauY);
    			delay(pwYredor);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);  
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
    		obspwrf(aHredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor);
    		modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    		delay(tauH);
    		delay(pwHredor);
    		modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    		delay(tauY1);
    		delay(pwYredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauY1);
    			delay(pwHredor);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY1);
    			delay(pwYredor);
    		endloop(v9);
    		delay(taur - (pwYredor/2.0));
    	}
    
    	else 
    	{delay(pwHredor/2.0);
    	delay(taur);}
    }
    
    if ((flag_redor == 0) && (redor_type == 2)) // S0, i.e. no pi pulse trains and no refocussing pulse on Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aHredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauH);
    			delay(pwHredor);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);  
    			delay(tauH);
    			delay(pwHredor);
    		endloop(v9);
    		delay(taur - (pwHredor/2.0));
    	}
    
    	else 
    	{delay(pwHredor/2.0);
    	delay(taur);}
    }

    if ((flag_redor == 0) && (redor_type == 3)) // S0, i.e. no pi pulse trains and no refocussing pulse on X and Y
    {
    	assign(v14,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aHredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauH);
    			delay(pwHredor);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);  
    			delay(tauH);
    			delay(pwHredor);
    		endloop(v9);
    		delay(taur - (pwHredor/2.0));
    	}
    
    	else 
    	{delay(pwHredor/2.0);
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
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    			delay(tauY);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    		endloop(v9);
    		delay(taur - (pwYredor/2.0));
    	}
    
    	else {
    	delay(pwYredor/2.0);
    	delay(taur);
    	}
    	
    }

    if ((flag_redor >0) && (redor_type==1)) // S or S', with two alternative pi pulses on H and Y per rotor period
    {
    	assign(v1,v8);
    	if (nredor>=1)
      	{
    		obspwrf(aHredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor);
    		modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    		delay(tauH);
    		rgpulse(pwHredor,v12,0.0,0.0);
    		modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    		delay(tauY1);
    		dec2rgpulse(pwYredor,v12,0.0,0.0);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauY1);
    			rgpulse(pwHredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);  
    			delay(tauY1);
    			dec2rgpulse(pwYredor,v12,0.0,0.0);
    		endloop(v9);
    		delay(taur - (pwYredor/2.0));
    	}
    
    	else 
    	{delay(pwHredor/2.0);
    	delay(taur);}
    	
    }		
    
    if ((flag_redor> 0) && (redor_type==2)) // S or S', with two pi pulses on H per rotor period
    {
    	assign(v14,v8);
    	if (nredor>=1)
    	{
    		obspwrf(aHredor);dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwHredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11);
    			delay(tauH); 
    			rgpulse(pwHredor,v12,0.0,0.0);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); txphase(v12); incr(v11); 
    			delay(tauH);
    			rgpulse(pwHredor,v12,0.0,0.0); 
    		endloop(v9);
    		delay(taur - (pwHredor/2.0));
    	}
    
    	else 
    	{delay(pwHredor/2.0);
    	 delay(taur);}
        }	
        
        
        if ((flag_redor >0) && (redor_type == 3)) // S or S', with two pi pulses per rotor period on X and Y
 	{	
    	assign(v14,v8);
    	if (nredor>=1) 
    	{
    		decpwrf(aXredor); dec2pwrf(aYredor);
    		assign(0,v11);
    		delay(pwYredor);
    		loop(v8,v9);
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);
    			modn(v11,v15,v16); getelem(ph1Xredor,v16,v12); decphase(v12); incr(v11);
    			if ((tauX>tauY) && (aXredor >0) && (aYredor >0)){
    			delay(tauY);
    			}
    			else if ((tauX<tauY) && (aXredor >0) && (aYredor >0)){
    			delay(tauX);
    			}
    			else if (aYredor == 0){
    			delay(tauX);
    			}
    			else if (aXredor == 0){
    			delay(tauY);
    			}
//    			dec2rgpulse(pwYredor,v12,0.0,0.0); 
//    			decrgpulse(pwXredor,v12,0.0,0.0);
			sim3pulse(0.0,pwXredor,pwYredor,v12,v12,v12,0.0,0.0);    			
    			modn(v11,v15,v16); getelem(ph1Yredor,v16,v12); dec2phase(v12); incr(v11);  
    			if ((tauX>tauY) && (aXredor >0) && (aYredor >0)){
    			delay(tauY);
    			}
    			else if ((tauX<tauY) && (aXredor >0) && (aYredor >0)){
    			delay(tauX);
    			}
    			else if (aYredor == 0){
    			delay(tauX);
    			}
    			else if (aXredor == 0){
    			delay(tauY);
    			}
//			dec2rgpulse(pwYredor,v12,0.0,0.0); 
//			decrgpulse(pwXredor,v12,0.0,0.0);
			sim3pulse(0.0,pwXredor,pwYredor,v12,v12,v12,0.0,0.0);
    		endloop(v9);
    		delay(taur - (pwYredor/2.0));
    	}
    
    	else {
    	delay(pwYredor/2.0);
    	delay(taur);
    	}
   }
   _dseqoff(redor2); 
   _dseqoff(redor3);
   _dseqoff(redor4);  


    // H F1 Period
    if (!strcmp(CT,"n")) {
        if (d3>0) {
            if (d3>2*(getval("pwX90") + getval("pwY90") + getval("pwZ90"))) {
                delay(0.5*d3 - getval("pwX90") - getval("pwY90") - getval("pwZ90"));
                dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
                decrgpulse(2*getval("pwX90"),0,0.0,0.0);
                dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
                delay(0.5*d3 - getval("pwX90") - getval("pwY90") - getval("pwZ90"));
            }
            else {
                delay(d3);
            }
        }

    }
    else if (!strcmp(CT,"y")) {
        delay( (getval("tconst2")-d3)/2.0 - (getval("pwH90"))  );
        rgpulse(2*getval("pwH90"),phH180CT,0.0,0.0);
        delay((getval("tconst2")-d3)/2.0 - getval("pwH90") - 2*getval("pwX90") - 2*getval("pwY90") -2*getval("pwZ90") - getval("pwH90")*(2./3.14) );
        delay(d3/2.0);
        dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
        decrgpulse(2*getval("pwX90"),0,0.0,0.0);
        dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
        delay(d3/2.0);
    }


    // XY8 Mixing
    if (getval("qHrfdr")>0 || tHmix > 0) {
        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph1H90,0.0,0.0);
        if (getval("qHrfdr")>0 && getval("aHrfdr")>0) {
            _mpseq(rfdr,phHrfdr);
        }
        else if(tHmix>0) {
            delay(tHmix);
        }
        obspwrf(getval("aH90"));
        rgpulse(getval("pwH90"),ph2H90,0.0,0.0);
    }

    // H to X Cross Polarization
    txphase(phHhx);
    _cp_(hx,phHhx,phXhx);
    decphase(ph1X90);
    decpwrf(getval("aX90"));


    // X F2 Period with H XiX (via TPPM) or SPINAL Decoupling and Constant Time N Evolution

    _dseqon(dec);

    dec2pwrf(getval("aY90"));
    dec3pwrf(getval("aZ90"));
    if (!strcmp(CT,"n")) {
        if (d2>0) {
            if (d2>2*getval("pwY90")) {
                delay(0.5*d2-getval("pwY90")-getval("pwZ90"));
                dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
                dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
                delay(0.5*d2 - getval("pwY90")-getval("pwZ90"));
            }
        }
        else {
            delay(d2);
        }

        decrgpulse(getval("pwX90"),ph1X90,0.0,0.0);
        decphase(ph2X90);
        decunblank();
        delay(tmd2);
    }
    else if (!strcmp(CT,"y")) {
        if (getval("tconst")< getval("ni")/sw1) {
            printf("tconst< tmd2 evol!! abort.\nMay consider setting ni=0.");
            psg_abort(1);
        }
        if (getval("tconst")< d2) {
            printf("tconst< d2 evol!! abort. check that ni/sw<tconst\n");
            psg_abort(1);
        }
        delay( (getval("tconst")-d2)/2.0 - (getval("pwX90"))  );
        decrgpulse(2*getval("pwX90"),phX180CT,0.0,0.0);
        delay((getval("tconst")-d2)/2.0 - getval("pwX90") - 2*getval("pwY90") -2*getval("pwZ90") - getval("pwX90")*(2./3.14) );
        delay(d2/2.0);
        dec2rgpulse(2*getval("pwY90"),0,0.0,0.0);
        dec3rgpulse(2*getval("pwZ90"),0,0.0,0.0);
        delay(d2/2.0);
        decrgpulse(getval("pwX90"),ph1X90,0.0,0.0);
        decphase(ph1X90);
    }

    _dseqoff(dec);

    txphase(phHxh);
    obspwrf(getval("aHpxy"));

    // Homospoil Delay

    if(!strcmp(HS,"hs")) {
        status(B);
        hsdelay(getval("hst"));
        status(A);
        delay(getval("hstconst"));
    }



    // Saturation Pulses
    if(!strcmp(HS,"z")) {
        rgradient('z',getval("gzlvl1"));
        _mpseq(pxy2,phHpxy);
        rgradient('z',0.0);
    }

    _mpseq(pxy,phHpxy);
    txphase(phHxh);

    // X to H Cross Polarization
    obspwrf(getval("aH90"));
    obsunblank(); xmtron();
    decrgpulse(getval("pwX90"),ph2X90,0.0,0.0);
    xmtroff();
    decphase(phXxh);
    _cp_(xh,phXxh,phHxh);




    // Begin X Decoupling

    if (!strcmp(HS,"hs")) {
        status(C);
    }
    _dseqon(dec2); _dseqon(dec3); _dseqon(dec4);
    obsblank(); _blank34();

    // H Hahn Echo

    if(t1Hecho>0.0) {
        txphase(phHecho);
        obspwrf(getval("aH90"));
        delay(t1Hecho);
        rgpulse(2*getval("pwH90"),phHecho,0.0,0.0);
        delay(t2Hecho);
    }
    // Begin H Acquisition

    delay(getval("rd"));
    startacq(getval("ad"));
    acquire(np, 1/sw);
    endacq();
    _dseqoff(dec2); _dseqoff(dec3); _dseqoff(dec4);
    obsunblank(); decunblank(); _unblank34();
}
