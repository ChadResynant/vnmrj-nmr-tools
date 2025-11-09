// pboxpulse.h - getpboxpulse() makes a .RF file for a pulse shape in
//               the pBox wavelib
//
//PBOXPULSE getpboxpulse() - Implement a pBox Pulse with Explicit
//                           Amplitude and Scaler
//PBOXPULSE getsoftpulse() - Implement a pBox Pulse Based on a Reference
//                           Scaler
//
//void _pboxpulse() - Execute a pBox Pulse on any Channel Set Within N
//                    (even) Rotor Periods
//void _pboxsimpulse( ) - Execute pBox Pulses on any Two Channels (except
//                    dec3) Set Within N (even) Rotor Periods
//void _pboxsim3pulse() -  Execute pBox Pulses on any Three Channels
//                    (except dec3) Set With N (even) Rotor Periods

// =============================
// Structure for PBOXPULSE
// =============================

// BDZ copied this structure from 3.1A master code. We had been running 
//   an old and dangerous definition of the code before this.

typedef struct {
   char seqName[13]; // Xc7,Yspc5 etc...
   char ch[5];       // channel on which to execute the pulse
   char wv[64];      // type of softpulse, names in wavelib
   double db;        // scaler value - 16 to 63
   double a;         // amplitude 0 to 4095
   double pw;        // pulse width
   double t1;        // pre delay to fill n rotor periods
   double t2;        // post delay to fil n rotor periods
   double of;        // frequency offset, in Hz
   double st;        // 0 for inversion 1 for refocussing (or maybe 0.5)
   double ph;        // explicit phase
   double fla;       // flip angle 
   int nRec;         // copy number
   int calc;
   int hasArray;
   AR  array;
   char pattern[64]; // pattern name
   double B1max;      // field strength for 1023.0 (kHz)
} PBOXPULSE;

// BDZ note: there are 4 code lines below marked as "fishy".
//   I think they should be deleted. Either a call to
//   sim3shaped_pulse blanks everything (and then the code
//   is correct) or the three channel code was cut and paste
//   to make the two channel code and the devs forgot to
//   delete the extra unblank calls that come from a drop
//   in channel counts.

//============================================
// Dump the contents of a PBOXPULSE structure
//============================================

void dump_pboxpulse(PBOXPULSE a)
{
   printf("PBOXPULSE INFORMATION\n"); 
   printf("seqName = %10s : Parameter Group Name\n",a.seqName);
   printf("pattern = %10s : Filename for Shape\n",a.pattern); 
   printf("nRec    = %10d : Copy Number in Sequence\n",a.nRec);
   printf("ch      = %10s : Channel\n",a.ch); 
   printf("wv      = %10s : Wave\n",a.wv);
   printf("db      = %10.0f : Scaler\n",a.db);
   printf("a       = %10.0f : Scaled Amplitude\n",a.a); 
   printf("pw      = %10.2f : Pulse Width (microseconds)\n",a.pw*1.0e6); 
   printf("t1      = %10.2f : Predelay (microseconds)\n",a.t1*1.0e6); 
   printf("t2      = %10.2f : Postdelay (microseconds)\n",a.t2*1.0e6); 
   printf("of      = %10.1f : Offset (Hz)\n",a.of); 
   printf("st      = %10.1f : Status\n",a.st); 
   printf("ph      = %10.2f : Phase (degrees)\n",a.ph); 
   printf("fla     = %10.2f : Flip Angle (degrees)\n",a.fla); 
   printf("B1max   = %10.2f : Maximum Field Strength\n",a.B1max);
   printf("ARRAY CONTROL:\n");
   printf("calc    = %10d : = 1 for Calculation Complete\n",a.calc);
   printf("hasArray= %10d : = 1 for New Shapefile\n",a.hasArray); 
   printf("\n");
}

void dump_pboxpulse_array(PBOXPULSE a)
{
   int i,j,m,k,l;
   char temp[PAR_NAME_SZ]; 
   printf("PBOXPULSE WAVEFORM ARRAY INFORMATION\n"); 
   printf("pattern = %10s : Filename for Pulse\n",a.pattern); 
   printf("array.c = %10d : Number of Parameters\n", a.array.c);
   printf("     PARAM     COLUMN\n");
   m = strlen(a.array.a);
   for (i = 0; i < a.array.c; i++) {
      k = a.array.c - i - 1;
      m = m - a.array.b[k];
      l = 0;
      for (j = m; j < (m + a.array.b[k]); j++) {
         temp[l] = a.array.a[j];
         l++;
      }
      temp[l] = '\0'; 
      printf("%10s %10d\n",temp,a.array.f[k]);          
   }
   printf("array.d = %10d : Number of Array Columns\n", a.array.d);
   printf("    COLUMN   ARRAY_ON      INDEX       SIZE\n");
   for (i = 0; i < a.array.d; i++) {
      printf("%10d %10d %10d %10d\n",i,a.array.i[i],a.array.g[i],a.array.j[i]);     
   }
   printf("NEWSHAPE = %10d : = 1 For Shapefile Calculation\n",a.hasArray);
   printf("DURATION = %10.2f : Pulse Width (microseconds)\n",a.pw*1.0e6);
   printf("\n");
}


//================================================================
//  Update PBOXPULSE - Used to recall Pbox if the PBOX MODULE has 
//  been changed.
//================================================================

PBOXPULSE update_PBOXPULSE(PBOXPULSE shp, int iRec)
{
   shape pboxshp;
   char *var;
   char cmd[MAXSTR];
   shp.nRec = iRec;
   shp.calc = 1; 

   char lpattern[NPATTERN];
   var = getname0("",shp.seqName,"");
   sprintf(lpattern,"%s%d",var,shp.nRec);
   shp.hasArray = hasarry(shp.array, lpattern);
   int lix = arryindex(shp.array);
   if (shp.calc > 0) {
      var = getname0("",shp.seqName,"");
      sprintf(shp.pattern,"%s%d_%d",var,shp.nRec,lix);
      if (shp.hasArray == 1) {
         int ret __attribute__((unused));
         sprintf(cmd,"Pbox %s.RF -w \"%s /%.7f %.2f %.2f %.2f %.2f\" -stepsize 0.2\n",
                 shp.pattern,shp.wv,shp.pw,shp.of,shp.st,shp.ph,shp.fla);
         ret = system(cmd);
         pboxshp = getRsh(shp.pattern); 
         shp.B1max = pboxshp.B1max; 
      }
   }
   return shp;
}

//================================================================
//  Combine two PBOXPULSE types - Used to add two shaped pulses for 
//  multiple-frequency excitation. 
//================================================================

PBOXPULSE combine_PBOXPULSE(PBOXPULSE shp1, PBOXPULSE shp2, int iRec, int calc)
{
   shape pboxshp;
   PBOXPULSE shp = {};
   shp.calc = calc;
//   shp.array = parsearry(shp.array);
   shp.nRec = iRec;
   char *var1, *var2;
   char bar1[256];
   char bar2[256];
   char cmd[2*MAXSTR];

// The two Shapes must be on the same channel

   if (strcmp(shp1.ch,shp2.ch) == 0) { 
      strcpy(shp.ch,shp1.ch); 

// Set the larger power for shp

      shp.db = shp1.db;
      if (shp2.db > shp1.db) shp.db = shp2.db; 

// Weight shp1 and shp2 in the new shape using shp1.a and shp2.a

      double shp1lvl = 2.8952*(shp1.a/4095.0)*exp(2.303*shp1.db/20.0); 
      double shp2lvl = 2.8952*(shp2.a/4095.0)*exp(2.303*shp2.db/20.0);
      double shplvl = shp1lvl + shp2lvl;
      double n1 = shp1.a/(shp1.a + shp2.a);
      double n2 = shp2.a/(shp1.a + shp2.a);
      double m1 = shp1.B1max/(shp1.B1max + shp2.B1max);
      double m2 = shp2.B1max/(shp1.B1max + shp2.B1max);
      shp1.fla = 180.0*n1/m1;
      shp2.fla = 180.0*n2/m2;
      shp.a = shplvl*(4095.0/2.8952)/exp(2.303*shp.db/20.0);
      shp.a = roundamp(shp.a,1.0);

      if (shp.a > 4095) {
         printf("Error in combine_PBOXPULSE(), Combined amplitude of %f greater than 4095", shp.a);
         psg_abort(1);
      } 

// Use the timing of the longest shaped pulse for shp

      shp.pw = shp1.pw;
      shp.t1 = shp1.t1;
      shp.t2 = shp1.t2;
      if (shp2.pw > shp1.pw) {
         shp.pw = shp2.pw; 
         shp.t1 = shp2.t1;
         shp.t2 = shp2.t2;
      }

// Use the combined array behavior for shp

      shp.array = combine_array(shp1.array,shp2.array);

// Concatenate the wave names for shp (not used)

      strcpy(shp.wv, shp1.wv);           
      strcat(shp.wv,shp2.wv);

// Concatenate the seqName's for shp (not used)

      strcpy(shp.seqName, shp1.seqName);
      strcat(shp.seqName,shp2.seqName);

// Set other defaults for shp

      shp.fla = 180.0; 
      shp.of = 0.0;
      shp.ph = 0.0;
      shp.st = 0.5;
  
// Create a combined shapefile name

      char lpattern[10*NPATTERN];
      var1 = getname0("",shp1.seqName,"");
      strcpy(bar1,var1); 
      var2 = getname0("",shp2.seqName,"");
      strcpy(bar2,var2);
      sprintf(lpattern,"%s%d_%s%d",bar1,shp1.nRec,bar2,shp2.nRec);
      shp.hasArray = hasarry(shp.array, lpattern);
      int lix = arryindex(shp.array);
      if (shp.calc > 0) {
         var1 = getname0("",shp1.seqName,"");
         strcpy(bar1,var1); 
         var2 = getname0("",shp2.seqName,"");
         strcpy(bar2,var2);  
         sprintf(shp.pattern,"%s%d_%s%d_%d",bar1,shp1.nRec,bar2,shp2.nRec,lix);
         if (shp.hasArray == 1) {
            int ret __attribute__((unused));
            sprintf(cmd,"Pbox %s.RF -w \"%s /%.7f %.2f %.2f %.2f %.2f\" \"%s /%.7f %.2f %.2f %.2f %.2f\" -stepsize 0.2" ,
               shp.pattern,shp1.wv,shp1.pw,shp1.of,shp1.st,shp1.ph,shp1.fla, 
                           shp2.wv,shp2.pw,shp2.of,shp2.st,shp2.ph,shp2.fla);
            ret = system(cmd);
            pboxshp = getRsh(shp.pattern); 
            shp.B1max = pboxshp.B1max; 
         }
      }
      return shp;
   }
   else {
      printf("Error in combine_PBOXPULSE(), Pulses must use the same channel");
      psg_abort(1);
   }
}

// ========================================================
// Implement a PBOXPULSE set within N (even) rotor periods
// ========================================================

PBOXPULSE getpboxpulse(char *seqName, int iRec, int calc)
{
   shape pboxshp;
   PBOXPULSE shp = {};
   char *var;
   char cmd[MAXSTR];
   if (strlen(seqName) >= sizeof(shp.seqName)  || strlen(seqName) < 1) {
      abort_message("Error in getpboxpulse(). The type name (%s) has a bad size!\n",seqName);
   }
   sprintf(shp.seqName,"%s",seqName);
   shp.calc = calc;
   shp.array = parsearry(shp.array);
   shp.nRec = iRec;

//chXsuffix

   Getstr(getname0("ch",shp.seqName,""),shp.ch,sizeof(shp.ch));

//wvXsuffix

   var = getname0("wv",shp.seqName,"");
   Getstr(var,shp.wv,sizeof(shp.wv));
   var = getname0("wv",shp.seqName,"");
   shp.array = disarry(var, shp.array);

//dbXsuffix

   var = getname0("db",shp.seqName,"");
   shp.db =  getval(var);

//aXsuffix

   var = getname0("a",shp.seqName,"");
   shp.a =  getval(var);

   var = getname0("pw",shp.seqName,"");
   shp.pw = getval(var);
   shp.array = disarry(var, shp.array);

   double srate = getval("srate");
   double taur = 2.0e-4;
   if (srate >= 500.0)
      taur = roundoff((1.0/srate), DTCK);
   else {
      abort_message("ABORT: Spin Rate (srate) must be greater than 500");
   }

// master has this:
/*
   //Center the pulse in N rotor periods - pwXsuffix

   int n = (int) ((shp.pw + 4.95e-6)/taur);
   if (n%2 != 0) n = n + 1;
   shp.t1 = 0.5*((double) (n*taur - shp.pw - 5.0e-6));
   while (shp.t1 < 0.0) {
     n = n + 2;
      shp.t1 = 0.5*((double) (n*taur - shp.pw - 5.0e-6));
   }
   shp.t2 = shp.t2 + 2.5e-6;
   shp.t1 = shp.t1 - 2.5e-6;
*/
// while we have this:

   //DO NOT Center the pulse in N rotor periods - pwXsuffix
   //USE DEFINED ROTOR SYNCHRONIZATION FROM THE SOFT PULSE
  
   shp.t1 = taur - 5e-6;
   shp.t2 = taur;

// end

//ofXsuffix

   var = getname0("of",shp.seqName,"");
   shp.of = getval(var);
   shp.array = disarry(var, shp.array);

// stXsuffix

   var = getname0("st",shp.seqName,"");

// master has this
/*
   shp.st = getval(var);
*/
// while we have this

   shp.st = (int) getval(var);

// end

   shp.array = disarry(var, shp.array);

// phXsuffix

   var = getname0("ph",shp.seqName,"");
   shp.ph = getval(var);
   shp.array = disarry(var, shp.array);

// Fix the flip angle at 180.0 degrees

   shp.fla = 180.0; 

// Create the Shapefile Name

   char lpattern[NPATTERN];
   var = getname0("",shp.seqName,"");
   sprintf(lpattern,"%s%d",var,shp.nRec);
   shp.hasArray = hasarry(shp.array, lpattern);
   int lix = arryindex(shp.array);
   if (shp.calc > 0) {
      var = getname0("",shp.seqName,"");
      sprintf(shp.pattern,"%s%d_%d",var,shp.nRec,lix);
      if (shp.hasArray == 1) {
		  
// master has this
/*
         sprintf(cmd,"Pbox %s.RF -w \"%s /%.7f %.2f %.2f %.2f %.2f\" -stepsize 0.2\n",
                 shp.pattern,shp.wv,shp.pw,shp.of,shp.st,shp.ph,shp.fla);
*/
// while we have this

         sprintf(cmd,"Pbox %s.RF -w \"%s /%.7f %.2f %.0f %.2f\" -s 0.5\n",
                 shp.pattern,shp.wv,shp.pw,shp.of,shp.st,shp.ph);  // CMR 11/17/23 changed 0.2 to 0.5 at end of the previous line

// end

         system(cmd);
         pboxshp = getRsh(shp.pattern); 
         shp.B1max = pboxshp.B1max;
      }
   }
   return shp;
}

// ========================================================
// Implement a PBOXPULSE set within N (even) rotor periods
// based on a reference db and pw.
// ========================================================

PBOXPULSE getrefpboxpulse(char *seqName, int iRec, int calc) 
{
   PBOXPULSE shp = {};
   char *var;
   char cmd[MAXSTR];
   if (strlen(seqName) >= sizeof(shp.seqName)  || strlen(seqName) < 1) {
      abort_message("Error in getrefpboxpulse(). The type name (%s) is a bad size!", seqName);
   }
   sprintf(shp.seqName,"%s",seqName);
   shp.calc = calc;
   shp.array = parsearry(shp.array);
   shp.nRec = iRec;

//chXsuffix

   var = getname0("ch",shp.seqName,"");
   Getstr(var,shp.ch,sizeof(shp.ch));

//wvXsuffix

   var = getname0("wv",shp.seqName,"");
   Getstr(var,shp.wv,sizeof(shp.wv));
   var = getname0("wv",shp.seqName,"");
   shp.array = disarry(var, shp.array);

//ref_dbXsuffix

   var = getname0("ref_db",shp.seqName,"");
   double ref_db =  getval(var);
   
//ref_pwXsuffix

   var = getname0("ref_pw",shp.seqName,"");
   double ref_pw =  getval(var);

//Center the pulse in N rotor periods - pwXsuffix

   var = getname0("pw",shp.seqName,"");
   shp.pw = getval(var);    
   shp.array = disarry(var, shp.array);

   double srate = getval("srate");
   double taur = 2.0e-4;
   if (srate >= 500.0)
      taur = roundoff((1.0/srate), DTCK);
   else {
      abort_message("ABORT: Spin Rate (srate) must be greater than 500\n");
   }
   int n = (int) ((shp.pw + 4.95e-6)/taur);
   if (n%2 != 0) n = n + 1;
   shp.t1 = 0.5*((double) (n*taur - shp.pw - 5.0e-6));
   while (shp.t1 < 0.0) {
      n = n + 2;
      shp.t1 = 0.5*((double) (n*taur - shp.pw - 5.0e-6));
   }
   shp.t2 = shp.t1 + 2.5e-6;
   shp.t1 = shp.t1 - 2.5e-6;

//ofXsuffix

   var = getname0("of",shp.seqName,"");
   shp.of = getval(var);
   shp.array = disarry(var, shp.array);
   
// stXsuffix

   var = getname0("st",shp.seqName,"");

// master has this
/*
   shp.st = getval(var);
*/
// while we have this

   shp.st = (int) getval(var);

   shp.array = disarry(var, shp.array);

// phXsuffix

   var = getname0("ph",shp.seqName,"");
   shp.ph = getval(var);
   shp.array = disarry(var, shp.array);
   
// Create the Shapefile Name

   char lpattern[NPATTERN];
   var = getname0("",shp.seqName,"");
   sprintf(lpattern,"%s%d",var,shp.nRec);
   shp.hasArray = hasarry(shp.array, lpattern);
   int lix = arryindex(shp.array);
   if (shp.calc > 0) {
      var = getname0("",shp.seqName,"");
      sprintf(shp.pattern,"%s%d_%d",var,shp.nRec,lix);
      if (shp.hasArray == 1) {

// master has this
/*
         sprintf(cmd,"Pbox %s.RF -w \"%s /%.7f %.2f %.2f %.2f\" -stepsize 0.2 -p %2.0f -l %f -attn e\n",
                shp.pattern,shp.wv,shp.pw,shp.of,shp.st,shp.ph,ref_db,ref_pw);

*/
// while we have this:

         sprintf(cmd,"Pbox %s.RF -w \"%s /%.7f %.2f %.0f %.2f\" -maxincr 10 -p %2.0f -l %f -attn e\n",
                shp.pattern,shp.wv,shp.pw,shp.of,shp.st,shp.ph,ref_db,ref_pw);

// end

         system(cmd);
      }
   }

   shape sh = getRsh(shp.pattern);
   shp.db = sh.pwr;
   shp.a = sh.pwrf;
   return shp;
}

// ===================================================================
// Underscore Function for PBOXPULSE set within N (even) rotor periods
// ===================================================================

void _pboxpulse(PBOXPULSE shp, codeint phase)
{
   // NMRFAM CODE
   double tpwr,decpwr,dec2pwr,dec3pwr; //to reset them later
   // end

   int chnl;
   chnl = 0;
   if (!strcmp(shp.ch,"obs")) chnl = 1;
   else if (!strcmp(shp.ch,"dec")) chnl = 2;
   else if (!strcmp(shp.ch,"dec2")) chnl = 3;
   else if (!strcmp(shp.ch,"dec3")) chnl = 4;
   else {
      abort_message("_pboxpulse() Error: Undefined Channel! (%s)\n", shp.ch);
   }

   switch (chnl) {
      case 1:
         
         // NMRFAM CODE
         tpwr=getval("tpwr");
         // end

         obsblank();

         obspower(shp.db);

         obspwrf(shp.a);

         txphase(phase);

         delay(3.0e-6);

         obsunblank();

         delay(shp.t1);
         delay(2.0e-6);

         shaped_pulse(shp.pattern,shp.pw,phase,0.0,0.0);

/* ORIG_BEHAVIOR but not consistent cases cases 2, 3, and 4 */

         // orig code: different than the other 3 cases TODO BDZ
         
         delay(shp.t2);

         // NMRFAM CODE
         obspower(tpwr);
         // end

         obsunblank();

/* BETTER BEHAVIOR?

         // proposed code to match other cases
         
         // NMRFAM CODE different than OVJ
         obspower(tpwr);
         // end

         delay(shp.t2);

         obsunblank();

  OR
  
         // proposed code that matches routines later in the file
         
         unblank();
         power();
         delay();

*/         
         
         break;
      case 2:
      
         // NMRFAM CODE
         decpwr=getval("dpwr");
         // end

         decblank();

         decpower(shp.db);

         decpwrf(shp.a);

         decphase(phase);

         delay(3.0e-6);

         decunblank();

         delay(shp.t1);
         delay(2.0e-6);

         decshaped_pulse(shp.pattern,shp.pw,phase,0.0,0.0);

         // NMRFAM CODE
         decpower(decpwr);
         // end

         delay(shp.t2);

         decunblank();

/* TODO BDZ

         // proposed code that matches routines later in the file
         
         unblank();
         power();
         delay();

*/         
         break;
      case 3:

         // NMRFAM CODE
         dec2pwr=getval("dpwr2");
         // end

         dec2blank();

         dec2power(shp.db);

         dec2pwrf(shp.a);

         dec2phase(phase);

         delay(3.0e-6);

         dec2unblank();

         delay(shp.t1);
         delay(2.0e-6);

         dec2shaped_pulse(shp.pattern,shp.pw,phase,0.0,0.0);

         // NMRFAM CODE
         dec2power(dec2pwr);
         // end

         delay(shp.t2);

         dec2unblank();

/* TODO BDZ

         // proposed code that matches routines later in the file
         
         unblank();
         power();
         delay();

*/         
         break;
      case 4:

         // NMRFAM CODE
         dec3pwr=getval("dpwr3");
         // end

         dec3blank();

         dec3power(shp.db);

         dec3pwrf(shp.a);

         dec3phase(phase);

         delay(3.0e-6);

         dec3unblank();

         delay(shp.t1);
         delay(2.0e-6);

         dec3shaped_pulse(shp.pattern,shp.pw,phase,0.0,0.0);

         // NMRFAM CODE
         dec3power(dec3pwr);
         // end

         delay(shp.t2);

         dec3unblank();

/* TODO BDZ

         // proposed code that matches routines later in the file
         
         unblank();
         power();
         delay();

*/         

         break;
      default:
         abort_message("_pboxpulse() Error: weird channel issue! Should be unreachable!\n");
         break;
   }
}

// ============================================================================
// Underscore Function for PBOXPULSE Simpulse set within N (even) rotor periods
// ============================================================================

void _pboxsimpulse(PBOXPULSE shp1, PBOXPULSE shp2, int phase1, int phase2)
{

   // NMRFAM CODE
   double tpwr,decpwr,dec2pwr,dec3pwr; //to reset them later
   // end

   int chnl1 = 0;
   int chnl2 = 0;
   if (!strcmp(shp1.ch,"obs")) chnl1 = 1;
   else if (!strcmp(shp1.ch,"dec")) chnl1 = 2;
   else if (!strcmp(shp1.ch,"dec2")) chnl1 = 3;
   else if (!strcmp(shp1.ch,"dec3")) {
      abort_message("_pboxsimpulse() Error: dec3 is not allowed!\n");
   }
   else {
      abort_message("_pboxsimpulse() Error: Undefined Channel 1 (%s)!\n", shp1.ch);
   }

   if (!strcmp(shp2.ch,"obs")) chnl2 = 1;
   else if (!strcmp(shp2.ch,"dec")) chnl2 = 2;
   else if (!strcmp(shp2.ch,"dec2")) chnl2 = 3;
   else if (!strcmp(shp2.ch,"dec3")) {
      abort_message("_pboxsimpulse() Error: dec3 is not allowed!\n");
   }
   else {
      abort_message("_pboxsimpulse() Error: Undefined Channel 2 (%s)!\n", shp2.ch);
   }

   if (chnl1 == chnl2) {
      abort_message("_pboxsimpulse() Error: channel 1 = channel 2!\n");
   }

// TODO BDZ the grabbing of power levels is happening at inconsistent times
//   for the many cases.
   
   if ((chnl1 == 1) && (chnl2 == 2)) {

      // TODO BDZ POWER SECOND for cases 1and2, 2and1. why?

      obsblank();
      decblank();
      
      // NMRFAM CODE
      tpwr=getval("tpwr");
      decpwr=getval("dpwr");
      // end
      
      obspower(shp1.db);
      decpower(shp2.db);

      obspwrf(shp1.a);
      decpwrf(shp2.a);

      txphase(phase1);
      decphase(phase2);

      delay(3.0e-6);

      obsunblank();
      decunblank();

      delay(shp1.t1);
      delay(2.0e-6);

      simshaped_pulse(shp1.pattern,shp2.pattern,shp1.pw,shp2.pw,
         phase1,phase2,0.0,0.0);

      obsunblank();
      decunblank();
      
      // NMRFAM CODE
      obspower(tpwr);
      decpower(decpwr);
      // end

      delay(shp1.t2);
   }
   else if ((chnl1 == 1) && (chnl2 == 3)) {

      // TODO BDZ POWER FIRST for cases 1and3, 2and3, 3and1, 3and2. why?
      
      // NMRFAM CODE
      tpwr=getval("tpwr");
      dec2pwr=getval("dpwr2");
      // end
      
      obsblank();
      if (NUMch > 2)
         dec2blank();

      obspower(shp1.db);
      if (NUMch > 2)
         dec2power(shp2.db);

      obspwrf(shp1.a);
      if (NUMch > 2)
         dec2pwrf(shp2.a);

      txphase(phase1);
      if (NUMch > 2)
         dec2phase(phase2);

      delay(3.0e-6);

      obsunblank();
      if (NUMch > 2)
         dec2unblank();

      delay(shp1.t1);
      delay(2.0e-6);

      if (NUMch > 2) {
      
         sim3shaped_pulse(shp1.pattern,"",shp2.pattern,shp1.pw,0.0,shp2.pw,
            phase1,zero,phase2,0.0,0.0);

         obsunblank();
         decunblank();  // TODO BDZ looks fishy! Nobody calls decblank() above.
         dec2unblank();

         // NMRFAM CODE
         obspower(tpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
      
         shaped_pulse(shp1.pattern,shp1.pw,phase1,0.0,0.0);

         obsunblank();
         
         // NMRFAM CODE
         obspower(tpwr);
         // end
         
         delay(shp1.t2);
      }
   }
   else if ((chnl1 == 2) && (chnl2 == 1)) {
      
      // TODO BDZ POWER SECOND for cases 1and2, 2and1. why?

      decblank();
      obsblank();
      
      // NMRFAM CODE
      tpwr=getval("tpwr");
      decpwr=getval("dpwr");
      // end
      
      decpower(shp1.db);
      obspower(shp2.db);

      decpwrf(shp1.a);
      obspwrf(shp2.a);

      decphase(phase1);
      txphase(phase2);

      delay(3.0e-6);

      decunblank();
      obsunblank();

      delay(shp1.t1);
      delay(2.0e-6);

      simshaped_pulse(shp2.pattern,shp1.pattern,shp2.pw,shp1.pw,
         phase2,phase1,0.0,0.0);

      decunblank();
      obsunblank();
      
      // NMRFAM CODE
      obspower(tpwr);
      decpower(decpwr);     
      // end
      
      delay(shp1.t2);
   }
   else if ((chnl1 == 2) && (chnl2 == 3)) {

      // TODO BDZ POWER FIRST for cases 1and3, 2and3, 3and1, 3and2. why?

      // NMRFAM CODE
      decpwr=getval("dpwr");
      dec2pwr=getval("dpwr2");   
      // end
      
      decblank();
      if (NUMch > 2)
         dec2blank();

      decpower(shp1.db);
      if (NUMch > 2)
         dec2power(shp2.db);

      decpwrf(shp1.a);
      if (NUMch > 2)
         dec2pwrf(shp2.a);

      decphase(phase1);
      if (NUMch > 2)
         dec2phase(phase2);

      delay(3.0e-6);

      decunblank();
      if (NUMch > 2)
         dec2unblank();

      delay(shp1.t1);
      delay(2.0e-6);

      if (NUMch > 2) {
      
         sim3shaped_pulse("",shp1.pattern,shp2.pattern,0.0,shp1.pw,shp2.pw,
            zero,phase1,phase2,0.0,0.0);

         obsunblank();  // TODO BDZ looks fishy! Nobody calls obsblank() above.
         decunblank();
         dec2unblank();

         // NMRFAM CODE
         decpower(decpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
         
         decshaped_pulse(shp1.pattern,shp1.pw,phase1,0.0,0.0);

         decunblank();

         // NMRFAM CODE
         decpower(decpwr);
         // end
         
         delay(shp1.t2);
      }
   }
   else if ((chnl1 == 3) && (chnl2 == 1)) {

      // TODO BDZ POWER FIRST for cases 1and3, 2and3, 3and1, 3and2. why?

      // NMRFAM CODE
      tpwr=getval("tpwr");
      dec2pwr=getval("dpwr2");
      // end
      
      if (NUMch > 2)
         dec2blank();
      obsblank();

      if (NUMch > 2)
         dec2power(shp1.db);
      obspower(shp2.db);

      if (NUMch > 2)
         dec2pwrf(shp1.a);
      obspwrf(shp2.a);

      if (NUMch > 2)
         dec2phase(phase1);
      txphase(phase2);

      delay(3.0e-6);

      if (NUMch > 2)
         dec2unblank();
      obsunblank();

      delay(shp1.t1);
      delay(2.0e-6);

      if (NUMch > 2) {
      
         sim3shaped_pulse(shp2.pattern,"",shp1.pattern,shp2.pw,0.0,shp1.pw,
            phase2,zero,phase1,0.0,0.0);

         obsunblank();
         decunblank();  // TODO BDZ looks fishy! Nobody calls decblank() above.
         dec2unblank();

         // NMRFAM CODE
         obspower(tpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
      
         shaped_pulse(shp2.pattern,shp2.pw,phase2,0.0,0.0);

         obsunblank();

         // NMRFAM CODE
         obspower(tpwr);
         //dec2power(dec2pwr);  // TODO BDZ looks fishy! If NUMch > 2 dec2power() was not set.
         // end
         
         delay(shp1.t2);
      }
   }
   else if ((chnl1 == 3) && (chnl2 == 2)) {

      // TODO BDZ POWER FIRST for cases 1and3, 2and3, 3and1, 3and2. why?

      // NMRFAM CODE
      decpwr=getval("dpwr");
      dec2pwr=getval("dpwr2");     
      // end
      
      if (NUMch > 2)
         dec2blank();
      decblank();

      if (NUMch > 2)
         dec2power(shp1.db);
      decpower(shp2.db);

      if (NUMch > 2)
         dec2pwrf(shp1.a);
      decpwrf(shp2.a);

      if (NUMch > 2)
         dec2phase(phase1);
      decphase(phase2);

      delay(3.0e-6);

      if (NUMch > 2)
         dec2unblank();
      decunblank();

      delay(shp1.t1);
      delay(2.0e-6);

      if (NUMch > 2) {
          
         sim3shaped_pulse("",shp2.pattern,shp1.pattern,0.0,shp2.pw,shp1.pw,
            zero,phase2,phase1,0.0,0.0);

         obsunblank();  // TODO BDZ looks fishy! Nobody called obsblank() above.
         decunblank();
         dec2unblank();
         
         // NMRFAM CODE
         decpower(decpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
      
         decshaped_pulse(shp2.pattern,shp2.pw,phase2,0.0,0.0);
         decunblank();

         // NMRFAM CODE
         decpower(decpwr);
         // end
         
         delay(shp1.t2);
      }
   }
   else {
      abort_message("_pboxsimpulse() Error: Undefined Channels!\n");
   }
}

// ==============================================================================
// Underscore Function for PBOXPULSE Sim3pulse set within N (even) rotor periods
// ==============================================================================

void _pboxsim3pulse(PBOXPULSE shp1, PBOXPULSE shp2, PBOXPULSE shp3,
                    int phase1, int phase2, int phase3)
{

   // NMRFAM CODE
   double tpwr,decpwr,dec2pwr,dec3pwr; //to reset them later
   // end

   int chnl1 = 0;
   int chnl2 = 0;
   int chnl3 = 0;
   
   if (!strcmp(shp1.ch,"obs")) chnl1 = 1;
   else if (!strcmp(shp1.ch,"dec")) chnl1 = 2;
   else if (!strcmp(shp1.ch,"dec2")) chnl1 = 3;
   else if (!strcmp(shp1.ch,"dec3")) {
      abort_message("_pboxsim3pulse() Error: dec3 is not allowed!\n");
   }
   else {
      abort_message("_pboxsim3pulse() Error: Undefined Channel 1 (%s)!\n",shp1.ch);
   }

   if (!strcmp(shp2.ch,"obs")) chnl2 = 1;
   else if (!strcmp(shp2.ch,"dec")) chnl2 = 2;
   else if (!strcmp(shp2.ch,"dec2")) chnl2 = 3;
   else if (!strcmp(shp2.ch,"dec3")) {
      abort_message("_pboxsim3pulse() Error: dec3 is not allowed!\n");
   }
   else {
      abort_message("_pboxsim3pulse() Error: Undefined Channel 2 (%s)!\n",shp2.ch);
   }

   if (!strcmp(shp3.ch,"obs")) chnl3 = 1;
   else if (!strcmp(shp3.ch,"dec")) chnl3 = 2;
   else if (!strcmp(shp3.ch,"dec2")) chnl3 = 3;
   else if (!strcmp(shp3.ch,"dec3")) {
      abort_message("_pboxsim3pulse() Error: dec3 is not allowed!\n");
   } 
   else {
      abort_message("_pboxsim3pulse() Error: Undefined Channel 3 (%s)!\n",shp3.ch);
   }

   if ((chnl1 == chnl2) || (chnl1 == chnl3)|| (chnl2 == chnl3)) {
      abort_message("_pboxsim3pulse() Error: Two Channels the Same!\n");
   }

   // TODO BDZ : unlike other routines this one has case clauses that are
   //   very consistent. Could this routine be a model for the order of
   //   operations of power, delay, blank, unblank, etc?
   
   if ((chnl1 == 1) && (chnl2 == 2) && (chnl3 == 3)) {
      
      obsblank();
      decblank();
      if (NUMch > 2)
         dec2blank();

      // NMRFAM CODE
      tpwr=getval("tpwr");
      decpwr=getval("dpwr");
      dec2pwr=getval("dpwr2");
      // end

      obspower(shp1.db);
      decpower(shp2.db);
      if (NUMch > 2)
         dec2power(shp3.db);

      obspwrf(shp1.a);
      decpwrf(shp2.a);
      if (NUMch > 2)
         dec2pwrf(shp3.a);

      txphase(phase1);
      decphase(phase2);
      if (NUMch > 2)
         dec2phase(phase3);

      delay(3.0e-6);

      obsunblank();
      decunblank();
      if (NUMch > 2)
         dec2unblank();

      delay(shp1.t1);
      delay(2.0e-6);

      // BDZ 6-14-24: after discussion with Dan we removed this one line
      //   as it appears to be a bug. Of the six subcases in this routine
      //   only this subcase had this line.
      //
      //delay(rof1);

      if (NUMch > 2) {
         
         sim3shaped_pulse(shp1.pattern,shp2.pattern,shp3.pattern,shp1.pw,
            shp2.pw,shp3.pw,phase1,phase2,phase3,0.0,0.0);

         obsunblank();
         decunblank();
         dec2unblank();

         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
         
         simshaped_pulse(shp1.pattern,shp2.pattern,shp1.pw,shp2.pw,
            phase1,phase2,0.0,0.0);

         obsunblank();
         decunblank();

         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         // end
         
         delay(shp1.t2);
      }
   }
   else if ((chnl1 == 1) && (chnl2 == 3) && (chnl3 == 2)) {
      
      obsblank();
      if (NUMch > 2)
         dec2blank();
      decblank();

      // NMRFAM CODE
      tpwr=getval("tpwr");
      decpwr=getval("dpwr");
      dec2pwr=getval("dpwr2");
      // end

      obspower(shp1.db);
      if (NUMch > 2)
         dec2power(shp2.db);
      decpower(shp3.db);

      obspwrf(shp1.a);
      if (NUMch > 2)
         dec2pwrf(shp2.a);
      decpwrf(shp3.a);

      txphase(phase1);
      if (NUMch > 2)
         dec2phase(phase2);
      decphase(phase3);

      delay(3.0e-6);

      obsunblank();
      if (NUMch > 2)
         dec2unblank();
      decunblank();

      delay(shp1.t1);
      delay(2.0e-6);

      if (NUMch > 2) {
         
         sim3shaped_pulse(shp1.pattern,shp3.pattern,shp2.pattern,shp1.pw,shp3.pw,
            shp2.pw,phase1,phase3,phase2,0.0,0.0);

         obsunblank();
         decunblank();
         dec2unblank();

         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
         
         simshaped_pulse(shp1.pattern,shp3.pattern,shp1.pw,shp3.pw,
            phase1,phase3,0.0,0.0);

         obsunblank();
         decunblank();
         
         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         // end
         
         delay(shp1.t2);
      }
   }
   else if ((chnl1 == 2) && (chnl2 == 1) && (chnl3 == 3)) {
      
      decblank();
      obsblank();
      if (NUMch > 2)
         dec2blank();

      // NMRFAM CODE
      tpwr=getval("tpwr");
      decpwr=getval("dpwr");
      dec2pwr=getval("dpwr2");
      // end

      decpower(shp1.db);
      obspower(shp2.db);
      if (NUMch > 2)
         dec2power(shp3.db);

      decpwrf(shp1.a);
      obspwrf(shp2.a);
      if (NUMch > 2)
         dec2pwrf(shp3.a);

      decphase(phase1);
      txphase(phase2);
      if (NUMch > 2)
         dec2phase(phase3);

      delay(3.0e-6);

      decunblank();
      obsunblank();
      if (NUMch > 2)
         dec2unblank();

      delay(shp1.t1);
      delay(2.0e-6);

      if (NUMch > 2) {
         
         sim3shaped_pulse(shp2.pattern,shp1.pattern,shp3.pattern,shp2.pw,
            shp1.pw,shp3.pw,phase2,phase1,phase3,0.0,0.0);

         obsunblank();
         decunblank();
         dec2unblank();
         
         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
         
         simshaped_pulse(shp2.pattern,shp1.pattern,shp2.pw,shp1.pw,
            phase2,phase1,0.0,0.0);

         decunblank();
         obsunblank();

         // NMRFAM CODE
         decpower(decpwr);
         obspower(tpwr);
         // end
         
         delay(shp1.t2);
      }
   }
   else if ((chnl1 == 2) && (chnl2 == 3) && (chnl3 == 1)) {
      
      decblank();
      if (NUMch > 2)
        dec2blank();
      obsblank();

      // NMRFAM CODE
      tpwr=getval("tpwr");
      decpwr=getval("dpwr");
      dec2pwr=getval("dpwr2");
      // end

      decpower(shp1.db);
      if (NUMch > 2)
         dec2power(shp2.db);
      obspower(shp3.db);

      decpwrf(shp1.a);
      if (NUMch > 2)
         dec2pwrf(shp2.a);
      obspwrf(shp3.a);

      decphase(phase1);
      if (NUMch > 2)
         dec2phase(phase2);
      txphase(phase3);

      delay(3.0e-6);

      decunblank();
      if (NUMch > 2)
         dec2unblank();
      obsunblank();

      delay(shp1.t1);
      delay(2.0e-6);

      if (NUMch > 2) {
         
         sim3shaped_pulse(shp3.pattern,shp1.pattern,shp2.pattern,shp3.pw,
            shp1.pw,shp2.pw,phase3,phase1,phase2,0.0,0.0);

         obsunblank();
         decunblank();
         dec2unblank();

         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
         
         simshaped_pulse(shp3.pattern,shp1.pattern,shp3.pw,shp1.pw,
            phase3,phase1,0.0,0.0);

         obsunblank();
         decunblank();

         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         // end
         
         delay(shp1.t2);
      }
   }
   else if ((chnl1 == 3) && (chnl2 == 1) && (chnl3 == 2)) {
      
      if (NUMch > 2)
         dec2blank();
      obsblank();
      decblank();

      // NMRFAM CODE
      tpwr=getval("tpwr");
      decpwr=getval("dpwr");
      dec2pwr=getval("dpwr2");
      // end

      if (NUMch > 2)
         dec2power(shp1.db);
      obspower(shp2.db);
      decpower(shp3.db);

      if (NUMch > 2)
         dec2pwrf(shp1.a);
      obspwrf(shp2.a);
      decpwrf(shp3.a);

      if (NUMch > 2)
         dec2phase(phase1);
      txphase(phase2);
      decphase(phase3);

      delay(3.0e-6);

      if (NUMch > 2)
         dec2unblank();
      obsunblank();
      decunblank();

      delay(shp1.t1);
      delay(2.0e-6);

      if (NUMch > 2) {
         
         sim3shaped_pulse(shp2.pattern,shp3.pattern,shp1.pattern,shp2.pw,
            shp3.pw,shp1.pw,phase2,phase3,phase1,0.0,0.0);
         obsunblank();
         decunblank();
         dec2unblank();

         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
         
         simshaped_pulse(shp2.pattern,shp3.pattern,shp2.pw,shp3.pw,
            phase2,phase3,0.0,0.0);

         obsunblank();
         decunblank();

         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         // end
         
         delay(shp1.t2);
      }
   }
   else if ((chnl1 == 3) && (chnl2 == 2) && (chnl3 == 1)) {
      
      if (NUMch > 2)
         dec2blank();
      decblank();
      obsblank();

      // NMRFAM CODE
      tpwr=getval("tpwr");
      decpwr=getval("dpwr");
      dec2pwr=getval("dpwr2");
      // end

      if (NUMch > 2)
         dec2power(shp1.db);
      decpower(shp2.db);
      obspower(shp3.db);

      if (NUMch > 2)
         dec2pwrf(shp1.a);
      decpwrf(shp2.a);
      obspwrf(shp3.a);

      if (NUMch > 2)
         dec2phase(phase1);
      decphase(phase2);
      txphase(phase3);

      delay(3.0e-6);

      if (NUMch > 2)
         dec2unblank();
      decunblank();
      obsunblank();

      delay(shp1.t1);
      delay(2.0e-6);

      if (NUMch > 2) {
         
         sim3shaped_pulse(shp3.pattern,shp2.pattern,shp1.pattern,shp3.pw,
            shp2.pw,shp1.pw,phase3,phase2,phase1,0.0,0.0);

         obsunblank();
         decunblank();
         dec2unblank();

         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         dec2power(dec2pwr);
         // end
         
         delay(shp1.t2);
      }
      else {
         
         simshaped_pulse(shp3.pattern,shp2.pattern,shp3.pw,shp2.pw,
            phase3,phase2,0.0,0.0);

         obsunblank();
         decunblank();

         // NMRFAM CODE
         obspower(tpwr);
         decpower(decpwr);
         // end
         
         delay(shp1.t2);
      }
   }
   else {
      abort_message("_pboxsimpulse() Error: Undefined Channels!\n");
   }
}
