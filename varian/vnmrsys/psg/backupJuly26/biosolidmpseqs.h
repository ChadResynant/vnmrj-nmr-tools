
#ifdef WE_INCLUDED_SOLID_MPSEQS
#error "Trying to include MPSEQS from both Solids and BioSolids"
#endif

#ifndef SOLIDMPSEQS_H
#define SOLIDMPSEQS_H

#define WE_INCLUDED_BIOSOLID_MPSEQS

// BDZ 2-23-24:
//   Digging into history I see this file was derived from
//   the OVJ file solidmpseqs.h. I do not know exactly why.
//   I surmise that we wanted to be able to change things
//   and not get messed up when the OVJ master code changed.
//   Anyhow I did a line by line comparison between the two
//   files and they are 95% the same. I will mark with BDZ
//   below on what things are present only here or missing
//   from the other file or are slightly different.
//   I have also marked as MATCH_MASTER probably needed
//   updates for the future from the OVJ MASTER branch.

// Contents:

// getramp() - RAMP         Build a Tangent Ramp

// getms() - MPSEQ          Build a Magic Sandwich

// getpuls() - MPSEQ        Build a Pulse with offset

// getpasl() - MPSEQ        Build a Spinlock : needs compiler name GETPASL defined

// getr1235() - MPSEQ       Build R12-sub3-sup5 with Supercycle

// getr1462() - MPSEQ       Build R14-sub6-sup2

// getr1825() - MPSEQ       Build R18-sub2-sup5

// getspc5() - MPSEQ        Build SPC5

// getspc7() - MPSEQ        Build SPC7

// getspcn() - MPSEQ        Build SPCN

// getpostc6() - MPSEQ      Build POST C6

// getpostc7() - MPSEQ      Build POST C7

// getfslg() - MPSEQ        Build FSLG
// getpmlg() - MPSEQ        Build PMLG

// getblew12() - MPSEQ      Build BLEW12

// getdumbo() - MPSEQ       Build DUMBO

// getbaba() - MPSEQ        Build BABA

// getxy8() - MPSEQ         Build XY8

// getcp() - CP             Build CP with one offset

// getdream() - DREAM       Build DREAM

// getdraws() - MPSEQ       Build DRAWS

// getrapt() - MPSEQ        Build RAPT as alternating phase pulses

// getgrapt() - MPSEQ       Build Gaussian RAPT

// getpipsxy() - MPSEQ      Output a Pi pulse at N rotor period
// getsr4() - MPSEQ         Build SR4 - R4-sub1-sup2]3-sup1

// getsammyd() - MPSEQ      Build SAMMY for H

// getsammyo() - MPSEQ      Build SAMMY for X

// getlg() - MPSEQ          Build Lee-Goldburg (simple offset) Decoupling

// getrfdrxy8() - MPSEQ     Build RFDR with an XY8 Phase Cycle

// getseac7() - MPSEQ       Build SEASHORE, Shift Evolution Assisted POSTC7

// getsc14() - MPSEQ        Build SC14

// getptrfdr() - MPSEQ      Build "Pi-Thirds for constant time RFDR

// getfprfdr() - MPSEQ      Build "Finite Pulse" RFDR

// getspnl() - MPSEQ        Build SPINAL 16,32,64

// getr1817() - MPSEQ       Build R18-sub1-sup7

// gettmrev5 - MPSEQ        Build TMREV with a 5- member supercycle

// getpxy() - MPSEQ         Build Windowlwss XY decoupling
// getsamn() - MPSEQ        Build a SAM waveform 
// getpmlgxmx - MPSEQ       Build PMLG with Z-rotation
// getsuper() - MPSEQ       Build SUPER
// getdumboxmx() - MPSEQ    Build DUMBO with Z-rotation
// getsat() - MPSEQ         Build a windowed saturation pulse


// BDZ this one is not in solidmpseqs

void print_mpseq(MPSEQ *m) {
    int i = 0;
    printf("MPSEQ %s.seqName {\n", m->seqName);
    printf("\t%s.seqName = '%s'\n", m->seqName, m->seqName);
    printf("\t%s.ch = '%s'\n", m->seqName, m->ch);              //output channel
    printf("\t%s.pattern = '%s'\n", m->seqName, m->pattern);    //shape file name
    printf("\t%s.nelem = %d\n", m->seqName, m->nelem);                //number of base elements to output
    printf("\t%s.telem = %f\n", m->seqName, m->telem);                //duration of base element
    printf("\t%s.t = %f\n", m->seqName, m->t);                    //Output duration for _mpseq()
    printf("\t%s.a = %f\n", m->seqName, m->a);                    //Output amplitude for _mpseq()
    printf("\t%s.phInt = %f\n", m->seqName, m->phInt);                //extra phase accumulation
    printf("\t%s.phAccum = %f\n", m->seqName, m->phAccum);              //phase accumulated due to offset
    printf("\t%s.iSuper = %d\n", m->seqName, m->iSuper);               //supercycle number
    printf("\t%s.nRec = %d\n", m->seqName, m->nRec);                 //copy number
    printf("\t%s.nphBase = %d\n", m->seqName, m->nphBase);              //number of elements in the base list
    printf("\t%s.nphSuper = %d\n", m->seqName, m->nphSuper);             //number of elements in the supercycle list
    printf("\t%s.npw = %d\n", m->seqName, m->npw);                  //number of pulse widths
    printf("\t%s.nph = %d\n", m->seqName, m->nph);                  //number of phases
    printf("\t%s.no = %d\n", m->seqName, m->no);                   //number of offsets
    printf("\t%s.na = %d\n", m->seqName, m->na);                   //number of amplitudes
    printf("\t%s.ng = %d\n", m->seqName, m->ng);                   //number of gate states

    printf("\t%s.n = {", m->seqName);
    for(i = 0; i < m->nphBase; i++) {
       printf("%d", m->n[i]);
       if(i < m->nphBase-1) {
           printf(", ");
       }
    }
    printf("}\n");

    printf("\t%s.phSuper = {", m->seqName);
    for(i = 0; i < m->nphSuper; i++) {
       printf("%f", m->phSuper[i]);
       if(i < m->nphSuper-1) {
           printf(", ");
       }
    }
    printf("}\n");

    printf("\t%s.pw = {", m->seqName);
    for(i = 0; i < m->npw; i++) {
       printf("%f", m->pw[i]);
       if(i < m->npw-1) {
           printf(", ");
       }
    }
    printf("}\n");

    printf("\t%s.phBase = {", m->seqName);
    for(i = 0; i < m->nphBase; i++) {
       printf("%f", m->phBase[i]);
       if(i < m->nphBase-1) {
           printf(", ");
       }
    }
    printf("}\n");

    printf("\t%s.of = {", m->seqName);
    for(i = 0; i < m->no; i++) {
       printf("%f", m->of[i]);
       if(i < m->no-1) {
           printf(", ");
       }
    }
    printf("}\n");

    printf("\t%s.aBase = {", m->seqName);
    for(i = 0; i < m->na; i++) {
       printf("%f", m->aBase[i]);
       if(i < m->na-1) {
           printf(", ");
       }
    }
    printf("}\n");

    printf("\t%s.gateBase = {", m->seqName);
    for(i = 0; i < m->ng; i++) {
       printf("%f", m->gateBase[i]);
       if(i < m->ng-1) {
           printf(", ");
       }
    }
    printf("}\n");

    printf("%s.calc = %d\n", m->seqName, m->calc);
    printf("%s.hasArray = %d\n", m->seqName, m->hasArray);
//   AR      m->array;
    printf("}\n");
}

//===========================================================

//Implement MPSEQ and Misc Waveforms with "get" Functions

//===========================================================



//===============

// Build a RAMP

//===============



RAMP getramp(char *seqName, double p, double phint, int iRec, int calc)

{

   RAMP r = {};

   extern RAMP make_ramp(RAMP r);

   char *var;

   r.phAccum = p;

   r.phInt = phint;

   r.nRec = iRec;



// Supply INOVA Start Delays


   r.preset1 = 0;

   r.preset2 = 0;

   r.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;

   r.offstdelay = WFG_OFFSET_DELAY;

   r.apdelay = PWRF_DELAY;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 2) {

      printf("Error in getramp(). The sequence name %s is invalid!\n",seqName);

      psg_abort(-1);

   }

   sprintf(r.seqName,"%s",seqName);

   r.calc = calc;

   r.array = parsearry(r.array);

#ifdef MATCH_MASTER

// Set the Step Sizes

   r.n90 = VNMRSN90;
   r.n90m = VNMRSN90M;
   r.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      r.n90 = INOVAN90;
      r.n90m = INOVAN90M;
      r.trap = INOVATRAP;
   }

#else

   if (PWRF_DELAY > 0.0) r.n90 = 40; // CMR 11/17/23 change from 8 to 40
   else r.n90 = 40; // CMR 11/17/23 change from 8 to 40

#endif


//channel


   var = getname0("ch",r.seqName,"");

   Getstr(var,r.ch,sizeof(r.ch));



//amplitude



   var = getname0("a",r.seqName,"");

   r.a = getval(var);

   r.array = disarry(var, r.array);



//offset



   var = getname0("of",r.seqName,"");

   r.of = getval(var);

   r.array = disarry(var, r.array);



//delta



   var = getname0("d",r.seqName,"");

   r.d = getval(var);

   r.array = disarry(var, r.array);



//beta



   var = getname0("b",r.seqName,"");

   r.b = getval(var);

   r.array = disarry(var, r.array);



//time



   var = getname0("t",r.seqName,"");

   r.t = getval(var);

   r.array = disarry(var, r.array);



//shape



   var = getname0("sh",r.seqName,"");

   Getstr(var,r.sh,sizeof(r.sh));

   r.array = disarry(var, r.array);



//polarity



   var = getname0("p",r.seqName,"");

   Getstr(var,r.pol,sizeof(r.pol));

   r.array = disarry(var, r.array);



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",r.seqName,"");

   sprintf(lpattern,"%s%d",var,r.nRec);

   r.hasArray = hasarry(r.array,lpattern);

   int lix = arryindex(r.array);

   if (r.calc > 0) {
      var = getname0("",r.seqName,"");

      sprintf(r.pattern,"%s%d_%d",var,r.nRec,lix);

      if (r.hasArray == 1) {

         r = make_ramp(r); 

      }
      r.t = gett(lix, lpattern);

   }

   return r;

}



//========================

// Build Magic Sandwich

//========================



MPSEQ getms(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ m = {};

   extern MPSEQ MPchopper(MPSEQ m);

   char *var;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 2) {

      printf("Error in getms(). The sequence name %s is invalid!\n",seqName);

      psg_abort(-1);

   }

   sprintf(m.seqName,"%s",seqName);

   m.calc = calc;

   m.array = parsearry(m.array);



// Obtain Phase Arguments



   m.phAccum = p;

   m.iSuper = iph;

   m.nRec = iRec;

   m.phInt = phint;



// Allocate Arrays



   int nphBase = 4;

   int nphSuper = 1;

   int npw = 4;

   int nph = 4;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&m,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   m.n90 = VNMRSN90;
   m.n90m = VNMRSN90M;
   m.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      m.n90 = INOVAN90;
      m.n90m = INOVAN90M;
      m.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   m.phBase[0] = 90;

   m.phBase[1] = 0;

   m.phBase[2] = 180.0;

   m.phBase[3] = 90.0;



// Set the Supercycle Phase List



   m.phSuper[0] = 0.0;



//Set the Delay List



//pwXms



   var = getname0("pw",m.seqName,"");

   m.pw[0] = getval(var);

   m.pw[3] = 0.25*m.pw[0];

   m.array = disarry(var, m.array);



//tauXms



   var = getname0("tau",m.seqName,"");

   m.pw[1] = getval(var);

   m.pw[2] = m.pw[1];

   m.array = disarry(var, m.array);



// Set the Offset List



//ofXms



   var = getname0("of",m.seqName,"");

   m.of[0] = getval(var);

   m.array = disarry(var, m.array);



// Set the Amp and Gate Lists



   m.aBase[0] = 1023.0;

   m.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXms



   var = getname0("a",m.seqName,"");

   m.a = getval(var);

   m.array = disarry(var, m.array);



//nXms



   var = getname0("n",m.seqName,"");

   m.nelem = getval(var);

   m.array = disarry(var, m.array);



//chXms


   var = getname0("ch",m.seqName,"");

   Getstr(var,m.ch,sizeof(m.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",m.seqName,"");

   sprintf(lpattern,"%s%d",var,m.nRec);

   m.hasArray = hasarry(m.array, lpattern);

   int lix = arryindex(m.array);

   if (m.calc > 0) {
      var = getname0("",m.seqName,"");

      sprintf(m.pattern,"%s%d_%d",var,m.nRec,lix);

      if (m.hasArray == 1) {

         m = MPchopper(m);

         m.iSuper = iph + m.nelem%m.nphSuper;

      }
      m.t = gett(lix, lpattern);

   }

   return m;

}



//==============================

// Build a Generic Offset Pulse

//==============================



MPSEQ getpul(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ pul = {};

   extern MPSEQ MPchopper(MPSEQ pul);

   char *var;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 2) {

      printf("Error in getpuls(). The sequence name %s is invalid!\n",seqName);

      psg_abort(-1);

   }

   sprintf(pul.seqName,"%s",seqName);

   pul.calc = calc;

   pul.array = parsearry(pul.array);



// Obtain Phase Arguments



   pul.phAccum = p;

   pul.iSuper = iph;

   pul.nRec = iRec;

   pul.phInt = phint;



// Allocate Arrays



   int nphBase = 1;

   int nphSuper = 1;

   int npw = 1;

   int nph = 1;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&pul,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   pul.n90 = VNMRSN90;
   pul.n90m = VNMRSN90M;
   pul.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pul.n90 = INOVAN90;
      pul.n90m = INOVAN90M;
      pul.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   pul.phBase[0] = 0.0;



// Set the Supercycle Phase List



   pul.phSuper[0] = 0.0;



//Set the Delay List



//pwXpul



   var = getname0("pw",pul.seqName,"");

   pul.pw[0] = getval(var);

   pul.array = disarry(var, pul.array);



// Set the Offset List



//ofXpul



   var = getname0("of",pul.seqName,"");

   pul.of[0] = getval(var);

   pul.array = disarry(var, pul.array);



// Set the Amp and Gate Lists



   pul.aBase[0] = 1023.0;

   pul.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXpul



   var = getname0("a",pul.seqName,"");

   pul.a = getval(var);

   pul.array = disarry(var, pul.array);



// Number of Elements



   pul.nelem = 1;



//chXpul


   var = getname0("ch",pul.seqName,"");

   Getstr(var,pul.ch,sizeof(pul.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",pul.seqName,"");

   sprintf(lpattern,"%s%d",var,pul.nRec);

   pul.hasArray = hasarry(pul.array, lpattern);

   int lix = arryindex(pul.array);

   if (pul.calc > 0) {
      var = getname0("",pul.seqName,"");

      sprintf(pul.pattern,"%s%d_%d",var,pul.nRec,lix);

      if (pul.hasArray == 1) {

         pul = MPchopper(pul); 

         pul.iSuper = iph + pul.nelem%pul.nphSuper;

      }
      pul.t = gett(lix, lpattern);

   }

   return pul;

}



//===============

// Build a PASL

//===============



#ifdef GETPASL

MPSEQ getpasl(char *seqName,int iph,double p, double phint, int iRec, int calc)

{ 

   MPSEQ f = {};

   extern MPSEQ MPchopper(MPSEQ f); 

   char *var;

   f.phAccum = p;

   f.iSuper = iph;

   f.nRec = iRec;

   f.phInt = phint;

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 2) {

      printf("Error in getpasl(). The sequence name %s is invalid!\n",seqName);

      psg_abort(-1);

   }

   sprintf(f.seqName,"%s",seqName);

   f.calc = calc;

   f.array = parsearry(f.array);



// Obtain Phase Arguments



   f.phAccum = p;

   f.iSuper = iph;

   f.nRec = iRec;

   f.phInt = phint;



// Allocate Arrays



   int nphBase = 2;

   int nphSuper = 1;

   int npw = 2;

   int nph = 2;

   int nof = 2;

   int na = 1; 

   int ng = 1; 

   MPinitializer(&f,npw,nph,nof,na,ng,nphBase,nphSuper);

// Set the Step Sizes

   f.n90 = VNMRSN90;
   f.n90m = VNMRSN90M;
   f.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      f.n90 = INOVAN90;
      f.n90m = INOVAN90M;
      f.trap = INOVATRAP;
   }

// Set the Step Sizes

// The following 9 lines of code may be problematic.
// Most functions use the above code to initialize
// n90, n90m, and trap
   f.n90 = 4;
   f.n90m = 5;
   f.trap = 0;

   if (PWRF_DELAY > 0.0) {
      f.n90 = 16;
      f.n90m = 1;
      f.trap = 0;
   }


// Set the Base Phase List



   f.phBase[0] = 0.0;

   f.phBase[1] = 180.0;



// Set the Supercycle Phase List



   f.phSuper[0] = 0.0;



//Set the Delay List



//pwXpasl



   var = getname0("pw",f.seqName,"");

   f.pw[0] = getval(var);

   f.pw[1] = f.pw[0];

   f.array = disarry(var, f.array);



// Set the Offset List



//ofXpasl



   var = getname0("of",f.seqName,"");

   f.of[0] = getval(var);

   f.array = disarry(var, f.array);



// Set the Amp and Gate Lists



   f.aBase[0] = 1023.0;

   f.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXpasl



   var = getname0("a",f.seqName,"");

   f.a = getval(var);

   f.array = disarry(var, f.array);



//nXpasl



   var = getname0("n",f.seqName,"");

   f.nelem = (int) getval(var);

   f.array = disarry(var, f.array);



//chXpasl


   var = getname0("ch",f.seqName,"");

   Getstr(var,f.ch,sizeof(f.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",f.seqName,"");

   sprintf(lpattern,"%s%d",var,f.nRec);

   f.hasArray = hasarry(f.array, lpattern);

   int lix = arryindex(f.array);

   if (f.calc > 0) {
      var = getname0("",f.seqName,"");

      sprintf(f.pattern,"%s%d_%d",var,f.nRec,lix);

      if (f.hasArray == 1) {

         f = MPchopper(f); 

         f.iSuper = iph + f.nelem%f.nphSuper;

      }
      f.t = gett(lix, lpattern);

   }

   return f;

}

#endif    // GETPASL

//========================================

//Build [R12-sub3-sup5]3-sup1

//========================================



MPSEQ getr1235(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ r = {};

   extern MPSEQ MPchopper(MPSEQ r);

   char *var;

   r.phAccum = p;

   r.iSuper = iph;

   r.nRec = iRec;

   r.phInt = phint;

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getr1235(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   } 

   sprintf(r.seqName,"%s",seqName);

   r.calc = calc;

   r.array = parsearry(r.array); 



// Obtain Phase Arguments



   r.phAccum = p;

   r.iSuper = iph;

   r.nRec = iRec;

   r.phInt = phint;



// Allocate Arrays



   int nphBase = 1;

   int nphSuper = 72;

   int npw = 1;

   int nph = 1;

   int nof = 1;

   int na = 1; 

   int ng = 1; 

   MPinitializer(&r,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   r.n90 = VNMRSN90;
   r.n90m = VNMRSN90M;
   r.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      r.n90 = INOVAN90;
      r.n90m = INOVAN90M;
      r.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   r.phBase[0] = 0.0;



// Set the Supercycle Phase List



// R12 phase



   double r12phase[2];

   r12phase[0] = 77.0;

   r12phase[1] =285.0;



// Supercycle Phase



   double scphase[6];

   scphase[0] = 0.0;

   scphase[1] = 180.0;

   scphase[2] = 120.0;

   scphase[3] = 300.0;

   scphase[4] = 240.0;

   scphase[5] =  60.0;



// Calculate Supercycle Phases (72)



   int n1,n2,index;

   double phase = 0.0;

   for (index = 0; index < 72; index++) {

      n1 = index%2;

      n2 = index/12;

      phase = r12phase[n1] + scphase[n2];

      while (phase >= 360.0) phase = phase - 360.0;

      r.phSuper[index] = phase;

   }



// Set the Delay List



//pwXr1235



   var = getname0("pw",r.seqName,"");

   r.pw[0] = getval(var);

   r.pw[0] = 0.5*r.pw[0];

   r.array = disarry(var, r.array);



// Set the Offset List



//ofXr1235



   var = getname0("of",r.seqName,"");

   r.of[0] = getval(var);

   r.array = disarry(var, r.array);



// Set the Amp and Gate Lists



   r.aBase[0] = 1023.0;

   r.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXr1235



   var = getname0("a",r.seqName,"");

   r.a = getval(var);



//qXr1235



   var = getname0("q",r.seqName,"");

   r.nelem = getval(var);

   r.array = disarry(var, r.array); 



//chXr1235


   var = getname0("ch",r.seqName,"");

   Getstr(var,r.ch,sizeof(r.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",r.seqName,"");

   sprintf(lpattern,"%s%d",var,r.nRec);

   r.hasArray = hasarry(r.array, lpattern);

   int lix = arryindex(r.array);

   if (r.calc > 0) {
      var = getname0("",r.seqName,"");

      sprintf(r.pattern,"%s%d_%d",var,r.nRec,lix);

      if (r.hasArray == 1) {

         r = MPchopper(r); 

         r.iSuper = iph + r.nelem%72;

      }
      r.t = gett(lix, lpattern);

   }

   return r;

}



//=======================

// Build R14-sub2-sup6

//=======================



MPSEQ getr1426(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ r = {};

   extern MPSEQ MPchopper(MPSEQ r);

   char *var;

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getr1426(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(r.seqName,"%s",seqName);

   r.calc = calc;

   r.array = parsearry(r.array); 



// Obtain Phase Arguments



   r.phAccum = p;

   r.iSuper = iph;

   r.nRec = iRec;

   r.phInt = phint;



// Allocate Arrays



   int nphBase = 2;

   int nphSuper = 2;

   int npw = 2;

   int nph = 2;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&r,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   r.n90 = VNMRSN90;
   r.n90m = VNMRSN90M;
   r.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      r.n90 = INOVAN90;
      r.n90m = INOVAN90M;
      r.trap = INOVATRAP;
   }

#endif


// Set the Base Phase List



   r.phBase[0] = 0.0;

   r.phBase[1] = 180.0;



// Set the Supercycle Phase List



   r.phSuper[0] = 77.143; // inexact float

   r.phSuper[1] = 282.857; // inexact float



// Set the Delay List



//pwXr1426 



   var = getname0("pw",r.seqName,"");

   r.pw[0] = getval(var); 

   // is this next left side supposed to be r.pw[1]?
   r.pw[0] = 0.25*r.pw[0];

   // is this next left side supposed to be r.pw[2]?
   // should the right side not be r.pw[0] but the orginal getval(var)?
   r.pw[1] = 3.0*r.pw[0];

   r.array = disarry(var, r.array);



// Set the Offset List



//ofXr1426



   var = getname0("of",r.seqName,"");

   r.of[0] = getval(var);

   r.array = disarry(var, r.array);



// Set the Amp and Gate Lists



   r.aBase[0] = 1023.0;

   r.gateBase[0] = 0.0; 



// Set the Overall Amplitude, Elements and Channel



//aXr1426



   var = getname0("a",r.seqName,"");

   r.a = getval(var);



//qXr1426



   var = getname0("q",r.seqName,"");

   r.nelem = getval(var);

   r.array = disarry(var, r.array);



//chXr1426


   var = getname0("ch",r.seqName,"");

   Getstr(var,r.ch,sizeof(r.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",r.seqName,"");

   sprintf(lpattern,"%s%d",var,r.nRec);

   r.hasArray = hasarry(r.array, lpattern);

   int lix = arryindex(r.array);

   if (r.calc > 0) {
      var = getname0("",r.seqName,"");

      sprintf(r.pattern,"%s%d_%d",var,r.nRec,lix);

      if (r.hasArray == 1) {

         r = MPchopper(r); 

         r.iSuper = iph + r.nelem%r.nphSuper;

      }
      r.t = gett(lix, lpattern);

   }

   return r;

}



//=======================

// Build R18-sub2-sup5

//=======================



MPSEQ getr1825(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ r = {};

   extern MPSEQ MPchopper(MPSEQ r);

   char *var;

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getr1825(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(r.seqName,"%s",seqName);

   r.calc = calc;

   r.array = parsearry(r.array); 



// Obtain Phase Arguments



   r.phAccum = p;

   r.iSuper = iph;

   r.nRec = iRec;

   r.phInt = phint;



// Allocate Arrays

   int nphBase = 1;

   int nphSuper = 2;

   int npw = 1;

   int nph = 1;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&r,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   r.n90 = VNMRSN90;
   r.n90m = VNMRSN90M;
   r.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      r.n90 = INOVAN90;
      r.n90m = INOVAN90M;
      r.trap = INOVATRAP;
   }

#endif


// Set the Base Phase List



   r.phBase[0] = 0.0;



// Set the Supercycle Phase List



   r.phSuper[0] = 50.0;

   r.phSuper[1] = 310.0;



// Set the Delay List                                             



//pwXr1825



   var = getname0("pw",r.seqName,"");

   r.pw[0] = getval(var);

   r.pw[0] = 0.5*r.pw[0];  // once again, pw[0] mentioned multiple times

   r.array = disarry(var, r.array);



// Set the Offset List



//ofXr1825



   var = getname0("of",r.seqName,"");

   r.of[0] = getval(var);

   r.array = disarry(var, r.array);



// Set the Amp and Gate Lists



   r.aBase[0] = 1023.0;

   r.gateBase[0] = 0.0; 



// Set the Overall Amplitude, Elements and Channel



//aXr1825



   var = getname0("a",r.seqName,"");

   r.a = getval(var);



//qXr1825



   var = getname0("q",r.seqName,"");

   r.nelem = getval(var);

   r.array = disarry(var, r.array); 



//chXr1825


   var = getname0("ch",r.seqName,"");

   Getstr(var,r.ch,sizeof(r.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",r.seqName,"");

   sprintf(lpattern,"%s%d",var,r.nRec);

   r.hasArray = hasarry(r.array, r.pattern);

   int lix = arryindex(r.array);

   if (r.calc > 0) {
      var = getname0("",r.seqName,""); 

      sprintf(r.pattern,"%s%d_%d",var,r.nRec,lix);

      if (r.hasArray == 1) {

         r = MPchopper(r); 

         r.iSuper = iph + r.nelem%r.nphSuper;

      }
      r.t = gett(lix, lpattern);

   }

   return r;

}



//=============

// Build SPC5

//=============



MPSEQ getspc5(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ spc5 = {}; 

   extern MPSEQ MPchopper(MPSEQ spc5);

   char *var;



    if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getspc5(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(spc5.seqName,"%s",seqName);

   spc5.calc = calc;

   spc5.array = parsearry(spc5.array);



// Obtain Phase Arguments



   spc5.phAccum = p;

   spc5.iSuper = iph;

   spc5.nRec = iRec;

   spc5.phInt = phint;



// Allocate Arrays



   int nphBase = 3;

   int nphSuper = 10;

   int npw = 3;

   int nph = 3;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&spc5,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   spc5.n90 = VNMRSN90;
   spc5.n90m = VNMRSN90M;
   spc5.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      spc5.n90 = INOVAN90;
      spc5.n90m = INOVAN90M;
      spc5.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   spc5.phBase[0] = 0.0;

   spc5.phBase[1] = 180.0;

   spc5.phBase[2] = 0.0;



// Set the Supercycle Phase List



   spc5.phSuper[0] = 0.0;

   spc5.phSuper[1] = 72.0;

   spc5.phSuper[2] = 144.0;

   spc5.phSuper[3] = 216.0;

   spc5.phSuper[4] = 288.0;

   spc5.phSuper[5] = 180.0;

   spc5.phSuper[6] = 252.0;

   spc5.phSuper[7] = 324.0;

   spc5.phSuper[8] = 36.0;

   spc5.phSuper[9] = 108.0;



// Set the Delay List



//pwXspc5



   var = getname0("pw",spc5.seqName,"");

   spc5.pw[1] = getval(var);

   spc5.array = disarry(var, spc5.array);

   spc5.pw[0] = 0.25*spc5.pw[1];

   spc5.pw[2] = 0.75*spc5.pw[1];



// Set the Offset List



//ofXspc5



   var = getname0("of",spc5.seqName,"");

   spc5.of[0] = getval(var);

   spc5.array = disarry(var, spc5.array);



// Set the Amp and Gate Lists



   spc5.aBase[0] = 1023.0;

   spc5.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXspc5



   var = getname0("a",spc5.seqName,"");

   spc5.a = getval(var);



//qXspc5



   var = getname0("q",spc5.seqName,"");

   spc5.nelem = getval(var);

   spc5.array = disarry(var, spc5.array);



//chXspc5


   var = getname0("ch",spc5.seqName,"");

   Getstr(var,spc5.ch,sizeof(spc5.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",spc5.seqName,"");

   sprintf(lpattern,"%s%d",var,spc5.nRec);

   spc5.hasArray = hasarry(spc5.array, lpattern);

   int lix = arryindex(spc5.array);

   if (spc5.calc > 0) {
      var = getname0("",spc5.seqName,"");

      sprintf(spc5.pattern,"%s%d_%d",var,spc5.nRec,lix);  

      if (spc5.hasArray == 1) {

         spc5 = MPchopper(spc5); 

         spc5.iSuper = iph + spc5.nelem%spc5.nphSuper;

      }
      spc5.t = gett(lix, lpattern);

   }

   return spc5;
}


//=================

// Build SPC7

//=================


// BDZ this one not in solidmpseqs.h : it looks like it is ours.
//   It very closely mirrors getpostc7() with maybe two places
//   where the code is different. I have documented them below.

MPSEQ getspc7(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ spc7 = {};

   char *var;

   extern MPSEQ MPchopper(MPSEQ spc7);

   int i;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getspc7(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(spc7.seqName,"%s",seqName);

   spc7.calc = calc;

   spc7.array = parsearry(spc7.array);



// Obtain Phase Arguments



   spc7.phAccum = p;

   spc7.iSuper = iph;

   spc7.nRec = iRec;

   spc7.phInt = phint;



// Allocate Arrays



   int nphBase = 3;

   int nphSuper = 14;  // BDZ: postc7() difference: is 7

   int npw = 3;

   int nph = 3;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&spc7,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   spc7.n90 = VNMRSN90;
   spc7.n90m = VNMRSN90M;
   spc7.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      spc7.n90 = INOVAN90;
      spc7.n90m = INOVAN90M;
      spc7.trap = INOVATRAP;
   }

#endif


// Set the Base Phase List



   spc7.phBase[0] = 0.0;

   spc7.phBase[1] = 180.0;

   spc7.phBase[2] = 0.0;



// Set the Supercycle Phase List

   // BDZ: postc7() difference: 
   // BDZ:   these two for loops in here are one for loop in postc7()
   
   double dph = 360.0/7.0;
   spc7.phSuper[0] = 0;
   for( i = 1; i< spc7.nphSuper/2; i++)
      spc7.phSuper[i] = spc7.phSuper[i-1] + dph;
   spc7.phSuper[7] = 180;
   for( i = spc7.nphSuper/2+1; i< spc7.nphSuper; i++)
      spc7.phSuper[i] = spc7.phSuper[i-1] + dph;


// Set the Delay List



// pwXspc7



   var = getname0("pw",spc7.seqName,"");

   spc7.pw[1] = getval(var);

   spc7.array = disarry(var, spc7.array);

   spc7.pw[0] = 0.25*spc7.pw[1];

   spc7.pw[2] = 0.75*spc7.pw[1];



// Set the Offset List



// ofXc7



   var = getname0("of",spc7.seqName,"");

   spc7.of[0] = getval(var);

   spc7.array = disarry(var, spc7.array); 



// Set the Amp and Gate Lists



   spc7.aBase[0] = 1023.0;

   spc7.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



// aXspc7



   var = getname0("a",spc7.seqName,"");

   spc7.a = getval(var);



// qXc7



   var = getname0("q", spc7.seqName,"");

   spc7.nelem = getval(var);

   spc7.array = disarry(var, spc7.array);



// chXc7 - Set the Channel


   var = getname0("ch",spc7.seqName,"");

   Getstr(var,spc7.ch,sizeof(spc7.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",spc7.seqName,"");

   sprintf(lpattern,"%s%d",var,spc7.nRec);

   spc7.hasArray = hasarry(spc7.array, lpattern);

   int lix = arryindex(spc7.array);

   if (spc7.calc > 0) {
      var = getname0("",spc7.seqName,"");
      sprintf(spc7.pattern,"%s%d_%d",var,spc7.nRec,lix);
      if (spc7.hasArray == 1) {
         spc7 = MPchopper(spc7); 
         spc7.iSuper = iph + spc7.nelem%spc7.nphSuper;
      }
      spc7.t = gett(lix, lpattern);
   }

   return spc7;

}

// BDZ this is not in solidmpseqs.h

//=================
// Build general SPCN mixing sequence
//=================
MPSEQ getspcn(char *seqName, int iph, double p, double phint, int iRec, int calc) {
   MPSEQ spcn = {};
   char *var;
   extern MPSEQ MPchopper(MPSEQ spcn);
   int i;
   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {
        printf("Error in getspcn(). The type name %s is invalid!\n",seqName);
        psg_abort(1);
   }

   sprintf(spcn.seqName,"%s",seqName);
   spcn.calc = calc;
   spcn.array = parsearry(spcn.array);

// Obtain Phase Arguments
   spcn.phAccum = p;
   spcn.iSuper = iph;
   spcn.nRec = iRec;
   spcn.phInt = phint;


   var = getname0("N",spcn.seqName,"");
   int Nspcn = (int)getval(var);

// Allocate Arrays
   int nphBase = 3;
   int nphSuper = 2*Nspcn;
   int npw = 3;
   int nph = 3;
   int nof = 1;
   int na = 1;
   int ng = 1;
   MPinitializer(&spcn,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   spcn.n90 = 8; //VNMRSN90;  This is set to 16 in soliddefs.h. Not working for SPCN 7/23/25 CMR. Try 4. crashes a lot. Try 8.
   spcn.n90m = VNMRSN90M;
   spcn.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      spcn.n90 = INOVAN90;
      spcn.n90m = INOVAN90M;
      spcn.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List
   spcn.phBase[0] = 0.0;
   spcn.phBase[1] = 180.0;
   spcn.phBase[2] = 0.0;

// Set the Supercycle Phase List
   double dph = 360.0/Nspcn;
   spcn.phSuper[0] = 0;
   for( i = 1; i < Nspcn; i++)
      spcn.phSuper[i] = fmod((spcn.phSuper[i-1] + dph), 360);
   spcn.phSuper[Nspcn] = 180;
   for( i = Nspcn+1; i< spcn.nphSuper; i++)
      spcn.phSuper[i] = fmod((spcn.phSuper[i-1] + dph), 360);

// Set the Delay List
// pwXspcn
   var = getname0("pw",spcn.seqName,"");
   spcn.pw[1] = getval(var);
   spcn.array = disarry(var, spcn.array);
   spcn.pw[0] = 0.25*spcn.pw[1];
   spcn.pw[2] = 0.75*spcn.pw[1];

// Set the Offset List
// ofXc7
   var = getname0("of",spcn.seqName,"");
   spcn.of[0] = getval(var);
   spcn.array = disarry(var, spcn.array); 

// Set the Amp and Gate Lists
   spcn.aBase[0] = 1023.0;
   spcn.gateBase[0] = 0.0;

// Set the Overall Amplitude, Elements and Channel
// aXspcn
   var = getname0("a",spcn.seqName,"");
   spcn.a = getval(var);

// qXc7
   var = getname0("q", spcn.seqName,"");
   spcn.nelem = getval(var);
   spcn.array = disarry(var, spcn.array);

// chXc7 - Set the Channel
   var = getname0("ch",spcn.seqName,"");
   Getstr(var,spcn.ch,sizeof(spcn.ch));

// Create the Shapefile Name
   char lpattern[NPATTERN];
   var = getname0("",spcn.seqName,"");
   sprintf(lpattern,"%s%d",var,spcn.nRec);
   spcn.hasArray = hasarry(spcn.array, lpattern);
   int lix = arryindex(spcn.array);
   if (spcn.calc > 0) {
      var = getname0("",spcn.seqName,"");
      sprintf(spcn.pattern,"%s%d_%d",var,spcn.nRec,lix);
      if (spcn.hasArray == 1) {
         spcn = MPchopper(spcn); 
         spcn.iSuper = iph + spcn.nelem%spcn.nphSuper;
      }
      spcn.t = gett(lix, lpattern);
   }
   return spcn;
}


//
//=================

// Build POSTC6

//=================

// BDZ solidmpseqs has this at the bottom of the file but code matches

MPSEQ getpostc6(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ c6 = {};

   char *var;

   extern MPSEQ MPchopper(MPSEQ c6);

   int i;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getpostc6(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(c6.seqName,"%s",seqName);

   c6.calc = calc;

   c6.array = parsearry(c6.array);



// Obtain Phase Arguments



   c6.phAccum = p;

   c6.iSuper = iph;

   c6.nRec = iRec;

   c6.phInt = phint;



// Allocate Arrays



   int nphBase = 3;

   int nphSuper = 6;

   int npw = 3;

   int nph = 3;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&c6,npw,nph,nof,na,ng,nphBase,nphSuper);

// TODO BDZ: missing INOVA and VNMRS code: also true on OVJ master

// Set the Base Phase List



   c6.phBase[0] = 0.0;

   c6.phBase[1] = 180.0;

   c6.phBase[2] = 0.0;



// Set the Supercycle Phase List



   double dph = 360.0/6.0;

   c6.phSuper[0] = 0;

   for( i = 1; i< c6.nphSuper; i++)

      c6.phSuper[i] = c6.phSuper[i-1] + dph;



// Set the Delay List



// pwXc6



   var = getname0("pw",c6.seqName,"");

   c6.pw[1] = getval(var);

   c6.array = disarry(var, c6.array);

   c6.pw[0] = 0.25*c6.pw[1];

   c6.pw[2] = 0.75*c6.pw[1];



// Set the Offset List



// ofXc6



   var = getname0("of",c6.seqName,"");

   c6.of[0] = getval(var);

   c6.array = disarry(var, c6.array); 



// Set the Amp and Gate Lists



   c6.aBase[0] = 1023.0;

   c6.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



// aXc6



   var = getname0("a",c6.seqName,"");

   c6.a = getval(var);



// qXc6



   var = getname0("q",c6.seqName,"");

   c6.nelem = getval(var);

   c6.array = disarry(var, c6.array);



// chXc6 - Set the Channel


   var = getname0("ch",c6.seqName,"");

   Getstr(var,c6.ch,sizeof(c6.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",c6.seqName,"");

   sprintf(lpattern,"%s%d",var,c6.nRec);

   c6.hasArray = hasarry(c6.array, lpattern);

   int lix = arryindex(c6.array);

   if (c6.calc > 0) {
      var = getname0("",c6.seqName,"");

      sprintf(c6.pattern,"%s%d_%d",var,c6.nRec,lix);

      if (c6.hasArray == 1) {

         c6 = MPchopper(c6); 

         c6.iSuper = iph + c6.nelem%c6.nphSuper;

      }
      c6.t = gett(lix, lpattern);

   }

   return c6;

}



//=================

// Build POSTC7

//=================



MPSEQ getpostc7(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ c7 = {};

   char *var;

   extern MPSEQ MPchopper(MPSEQ c7);

   int i;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getpostc7(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(c7.seqName,"%s",seqName);

   c7.calc = calc;

   c7.array = parsearry(c7.array);



// Obtain Phase Arguments



   c7.phAccum = p;

   c7.iSuper = iph;

   c7.nRec = iRec;

   c7.phInt = phint;



// Allocate Arrays



   int nphBase = 3;

   int nphSuper = 7;

   int npw = 3;

   int nph = 3;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&c7,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   c7.n90 = VNMRSN90;
   c7.n90m = VNMRSN90M;
   c7.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      c7.n90 = INOVAN90;
      c7.n90m = INOVAN90M;
      c7.trap = INOVATRAP;
   }

#endif


// Set the Base Phase List



   c7.phBase[0] = 0.0;

   c7.phBase[1] = 180.0;

   c7.phBase[2] = 0.0;



// Set the Supercycle Phase List



   double dph = 360.0/7.0;

   c7.phSuper[0] = 0;

   for( i = 1; i< c7.nphSuper; i++)

      c7.phSuper[i] = c7.phSuper[i-1] + dph;



// Set the Delay List



// pwXc7



   var = getname0("pw",c7.seqName,"");

   c7.pw[1] = getval(var);

   c7.array = disarry(var, c7.array);

   c7.pw[0] = 0.25*c7.pw[1];

   c7.pw[2] = 0.75*c7.pw[1];



// Set the Offset List



// ofXc7



   var = getname0("of",c7.seqName,"");

   c7.of[0] = getval(var);

   c7.array = disarry(var, c7.array); 



// Set the Amp and Gate Lists



   c7.aBase[0] = 1023.0;

   c7.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



// aXc7



   var = getname0("a",c7.seqName,"");

   c7.a = getval(var);



// qXc7



   var = getname0("q",c7.seqName,"");

   c7.nelem = getval(var);

   c7.array = disarry(var, c7.array);



// chXc7 - Set the Channel


   var = getname0("ch",c7.seqName,"");

   Getstr(var,c7.ch,sizeof(c7.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",c7.seqName,"");

   sprintf(lpattern,"%s%d",var,c7.nRec);

   c7.hasArray = hasarry(c7.array, lpattern);

   int lix = arryindex(c7.array);

   if (c7.calc > 0) {
      var = getname0("",c7.seqName,"");

      sprintf(c7.pattern,"%s%d_%d",var,c7.nRec,lix);

      if (c7.hasArray == 1) {

         c7 = MPchopper(c7); 

         c7.iSuper = iph + c7.nelem%c7.nphSuper;

      }
      c7.t = gett(lix, lpattern);

   }

   return c7;

}



//========

// FSLG

//========



MPSEQ getfslg(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ f = {};

   extern MPSEQ MPchopper(MPSEQ f);



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getfslg(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(f.seqName,"%s",seqName);

   f.calc = calc;

   f.array = parsearry(f.array);



// Obtain Phase Arguments



   f.phAccum = p;

   f.iSuper = iph;

   f.nRec = iRec;

   f.phInt = phint;



// Allocate Arrays



   int nphBase = 2;

   int nphSuper = 1;

   int npw = 2;

   int nph = 2;

   int nof = 2;

   int na = 1; 

   int ng = 1; 

   MPinitializer(&f,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   f.n90 = VNMRSN90;
   f.n90m = VNMRSN90M;
   f.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      f.n90 = INOVAN90;
      f.n90m = INOVAN90M;
      f.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   f.phBase[0] = 0.0;

   f.phBase[1] = 180.0;



// Set the Supercycle Phase List



   f.phSuper[0] = 0.0;



// Set the delay List



//pwXfslg



   var = getname0("pw",f.seqName,"");

   f.pw[0] = getval(var);

   f.pw[1] = f.pw[0];

   f.array = disarry(var, f.array);

// Set the Offset List



//ofXfslg



   var = getname0("of",f.seqName,"");

   double loffset = getval(var);

   f.array = disarry(var, f.array);



//ofdXfslg



   var = getname0("ofd",f.seqName,"");

   f.of[0] = getval(var);

   f.of[1] = - f.of[0];

   f.of[0] = f.of[0] + loffset;

   f.of[1] = f.of[1] + loffset;

   f.array = disarry(var, f.array);



// Set the Amp and Gate Lists



   f.aBase[0] = 1023.0;

   f.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXfslg



   var = getname0("a",f.seqName,"");

   f.a = getval(var);



//nXfslg



   var = getname0("n",f.seqName,"");

   f.nelem = (int) getval(var); 

   f.array = disarry(var, f.array);



//chXfslg

   var = getname0("ch",f.seqName,"");

   Getstr(var,f.ch,sizeof(f.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",f.seqName,""); 

   sprintf(lpattern,"%s%d",var,f.nRec);

   f.hasArray = hasarry(f.array, lpattern);

   int lix = arryindex(f.array);

   if (f.calc > 0) {
      var = getname0("",f.seqName,""); 

      sprintf(f.pattern,"%s%d_%d",var,f.nRec,lix);

      if (f.hasArray == 1) {

         f = MPchopper(f);

         f.iSuper = iph + f.nelem%f.nphSuper;

      }
      f.t = gett(lix, lpattern);
   }

   return f;

}



//=============

// Build PMLG

//=============



MPSEQ getpmlg(char *seqName, int iph ,double p, double phint, int iRec, int calc)

{

   MPSEQ pm = {};

   int i;

   char *var;

   extern MPSEQ MPchopper(MPSEQ pm);



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

      printf("Error in getpmlg(). The type name %s is invalid!\n",seqName);

      psg_abort(1);

   }

   sprintf(pm.seqName,"%s",seqName);

   pm.calc = calc;

   pm.array = parsearry(pm.array);



// Obtain Phase Arguments



   pm.phAccum = p;

   pm.iSuper = iph;

   pm.nRec = iRec;

   pm.phInt = phint;



// Determine nphBase form qXpmlg



//qXpmlg



   var = getname0("q",pm.seqName,"");

   int nphBase = (int) getval(var);

   pm.array = disarry(var, pm.array);



   double sign = 1;

   if (nphBase < 0.0) {

      nphBase = -nphBase;

      sign = - sign;

   }
   int nsteps = nphBase;


// Allocate Arrays



   nphBase = 2*nphBase;

   int nphSuper = 1;

   int npw = nphBase;

   int nph = nphBase;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&pm,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   pm.n90 = VNMRSN90;
   pm.n90m = VNMRSN90M;
   pm.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pm.n90 = INOVAN90;
      pm.n90m = INOVAN90M;
      pm.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   double obsstep = 360.0/8192;

   double delta = 360.0/(sqrt(3)*nsteps);

   for (i = 0; i < pm.nphBase/2; i++) {

      pm.phBase[i] = sign*(i*delta + delta/2.0);

      pm.phBase[i] = roundphase(pm.phBase[i],obsstep);

   }

   for (i = pm.nphBase/2; i < pm.nphBase; i++) {

      pm.phBase[i] = 180.0 + sign*((2*nsteps - i)*delta - delta/2.0);

      pm.phBase[i] = roundphase(pm.phBase[i],obsstep);

   }



// Set the Supercycle Phase List



   pm.phSuper[0] = 0.0;



// Set the Delay List



//pwXpmlg (360 pulse)



   var = getname0("pw",pm.seqName,"");

   double pw360 = getval(var);

   pw360 = roundoff(pw360,DTCK*pm.n90);

   pm.array = disarry(var, pm.array);



   double pwlast = 0.0;

   for (i = 0; i < pm.nphBase/2; i++) {

      pm.pw[i] = roundoff((i+1)*2.0*pw360/pm.nphBase,DTCK*pm.n90) - pwlast;

      pwlast = pwlast + pm.pw[i];

      pm.pw[i + nsteps] = pm.pw[i];

   }



// Set the Offset List



//ofXpmlg



   var = getname0("of",pm.seqName,"");

   pm.of[0] = getval(var);

   pm.array = disarry(var, pm.array);



// Set the Amp and Gate Lists



   pm.aBase[0] = 1023.0;

   pm.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXpmlg



   var = getname0("a",pm.seqName,"");

   pm.a = getval(var);



//nXpmlg 



   var = getname0("n",pm.seqName,"");

   pm.nelem = (int) getval(var);

   pm.array = disarry(var, pm.array);



//chXpmlg
   
   var = getname0("ch",pm.seqName,"");

   Getstr(var,pm.ch,sizeof(pm.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",pm.seqName,"");

   sprintf(lpattern,"%s%d",var,pm.nRec);

   pm.hasArray = hasarry(pm.array, lpattern);

   int lix = arryindex(pm.array);

   if (pm.calc > 0) {
      var = getname0("",pm.seqName,"");

      sprintf(pm.pattern,"%s%d_%d",var,pm.nRec,lix);

      if (pm.hasArray == 1) {

         pm = MPchopper(pm); 

         pm.iSuper = iph + pm.nelem%pm.nphSuper;

      }
      pm.t = gett(lix, lpattern);

   }

   return pm;

}



//================

// Build BLEW12

//================



MPSEQ getblew(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ pm = {};

   int i;

   char *var;

   extern MPSEQ MPchopper(MPSEQ pm);



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

      printf("Error in getblew(). The type name %s is invalid !\n",seqName);

      psg_abort(1);

   }

   sprintf(pm.seqName,"%s",seqName);

   pm.calc = calc;

   pm.array = parsearry(pm.array);



// Obtain Phase Arguments



   pm.phAccum = p;

   pm.iSuper = iph;

   pm.nRec = iRec;

   pm.phInt = phint;



// Allocate Arrays



   int nphBase = 12;

   int nphSuper = 1;

   int npw = 12;

   int nph = 12;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&pm,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   pm.n90 = VNMRSN90;
   pm.n90m = VNMRSN90M;
   pm.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pm.n90 = INOVAN90;
      pm.n90m = INOVAN90M;
      pm.trap = INOVATRAP;
   }

#endif


// Set the Base Phase List



   pm.nphBase = 12; 

   pm.phBase[0] = 0.0;

   pm.phBase[1] = 90.0;

   pm.phBase[2] = 180.0;

   pm.phBase[3] = 90.0;

   pm.phBase[4] = 0.0;

   pm.phBase[5] = 90.0;

   pm.phBase[6] = 270.0;

   pm.phBase[7] = 180.0;

   pm.phBase[8] = 270.0;

   pm.phBase[9] =  0.0;

   pm.phBase[10] = 270.0;

   pm.phBase[11] = 180.0;



// Set the Supercycle Phase List



   pm.phSuper[0] = 0.0;



// Set the Delay List



//pwXblew (360 pulse)



   var = getname0("pw",pm.seqName,"");

   double pw = getval(var);

   pw = roundoff(pw/4.0,DTCK*pm.n90);

   pm.array = disarry(var, pm.array);



   for (i = 0; i < pm.nphBase; i++) {

      pm.pw[i] = pw;

   }



// Set the Offset List



//ofXblew



   var = getname0("of",pm.seqName,"");

   pm.of[0] = getval(var);

   pm.array = disarry(var, pm.array);



// Set the Amp and Gate Lists



   pm.aBase[0] = 1023.0;

   pm.gateBase[0] = 0.0; 



// Set the Overall Amplitude, Elements and Channel



//aXblew



   var = getname0("a",pm.seqName,"");

   pm.a = getval(var);



//nXblew



   var = getname0("n",pm.seqName,"");

   pm.nelem = (int) getval(var);

   pm.array = disarry(var, pm.array);



//chXblew


   var = getname0("ch",pm.seqName,"");

   Getstr(var,pm.ch,sizeof(pm.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",pm.seqName,"");

   sprintf(lpattern,"%s%d",var,pm.nRec);

   pm.hasArray = hasarry(pm.array, lpattern);

   int lix = arryindex(pm.array);

   if (pm.calc > 0) {
      var = getname0("",pm.seqName,"");

      sprintf(pm.pattern,"%s%d_%d",var,pm.nRec,lix);

      if (pm.hasArray == 1) {

         pm = MPchopper(pm);

         pm.iSuper = iph + pm.nelem%pm.nphSuper;

      }
      pm.t = gett(lix, lpattern);

   }

   return pm;

}



//======================

// Build DUMBO

//======================



MPSEQ getdumbo(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ pm = {};

   int i;

   char *var;

   extern MPSEQ MPchopper(MPSEQ pm);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

      printf("Error in getdumbo(). The type name %s is invalid!\n",seqName);

      psg_abort(1);

   }

   sprintf(pm.seqName,"%s",seqName);

   pm.calc = calc;

   pm.array = parsearry(pm.array);



// Obtain Phase Arguments



   pm.phAccum = p;

   pm.iSuper = iph;

   pm.nRec = iRec;

   pm.phInt = phint;



// Allocate Arrays



   int nphBase = 64;

   int nphSuper = 1;

   int npw = 64;

   int nph = 64;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&pm,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   pm.n90 = VNMRSN90;
   pm.n90m = VNMRSN90M;
   pm.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pm.n90 = INOVAN90;
      pm.n90m = INOVAN90M;
      pm.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



//phXdumbo



   var = getname0("ph",pm.seqName,"");

   double ph = getval(var); 

   pm.array = disarry(var, pm.array); 



   double phBase[64] = {31.824000, 111.959215, 140.187768, 118.272283,

   86.096791,   73.451274,  71.082669,  53.716611,  21.276000,   2.170197,

   12.992428,   28.379246,   5.986118, 302.048261, 248.763031, 264.564610,

   348.552000,  74.378423,  94.391707,  45.146603, 343.815209, 323.373522,

   341.072484, 353.743630, 333.756000, 296.836742, 274.284097, 271.756242,

   268.693882, 255.782366, 257.225816, 304.420775, 124.420775,  77.225816,

    75.782366,  88.693882,  91.756242,  94.284097, 116.836742, 153.756000,

   173.743630, 161.072484, 143.373522, 163.815209, 225.146603, 274.391707,

   254.378423, 168.552000,  84.564610,  68.763031, 122.048261, 185.986118,

   208.379246, 192.992428, 182.170197, 201.276000, 233.716611, 251.082669,

   253.451274, 266.096791, 298.272283, 320.187768, 291.959215, 211.824000};



  for (i = 0; i < pm.nphBase; i++) {

     pm.phBase[i] = phBase[i] + ph;

  }



// Set the Supercycle Phase List



   pm.phSuper[0] = 0.0;



// Set the Delay List



//pwXdumbo (360 pulse)



   var = getname0("pw",pm.seqName,"");

   double pw = getval(var);

   pw = roundoff(pw,DTCK*pm.n90);

   pm.array = disarry(var, pm.array);



   double pwlast = 0.0;

   for (i = 0; i < pm.nphBase; i++) {

      pm.pw[i] = roundoff((i+1)*pw/pm.nphBase,DTCK*pm.n90) - pwlast;

      pwlast = pwlast + pm.pw[i];

   }



// Set the Offset List



//ofXdumbo



   var = getname0("of",pm.seqName,"");

   pm.of[0] = getval(var);

   pm.array = disarry(var, pm.array);



// Set the Amp and Gate Lists



   pm.aBase[0] = 1023.0;

   pm.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXdumbo



   var = getname0("a",pm.seqName,"");

   pm.a = getval(var);



//nXdumbo



   var = getname0("n",pm.seqName,"");

   pm.nelem = (int) getval(var);

   pm.array = disarry(var, pm.array);



//chXdumbo


   var = getname0("ch",pm.seqName,"");

   Getstr(var,pm.ch,sizeof(pm.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",pm.seqName,"");

   sprintf(lpattern,"%s%d",var,pm.nRec);

   pm.hasArray = hasarry(pm.array, lpattern);

   int lix = arryindex(pm.array);

   if (pm.calc > 0) {
      var = getname0("",pm.seqName,"");

      sprintf(pm.pattern,"%s%d_%d",var,pm.nRec,lix);

      if (pm.hasArray == 1) {

         pm = MPchopper(pm);

         pm.iSuper = iph + pm.nelem%pm.nphSuper;

      }
      pm.t = gett(lix, lpattern);

   }

   return pm;

}



//=============

// Build BABA

//=============



MPSEQ getbaba(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ baba = {};

   extern MPSEQ MPchopper(MPSEQ baba);



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getbaba(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(baba.seqName,"%s",seqName);

   baba.calc = calc;

   baba.array = parsearry(baba.array);



// Obtain Phase Arguments



   baba.phAccum = p;

   baba.iSuper = iph;

   baba.nRec = iRec;

   baba.phInt = phint;



// Allocate Arrays



   int nphBase = 12;

   int nphSuper = 2;

   int npw = 12;

   int nph = 12;

   int nof = 1;

   int na = 1;

   int ng = 12;

   MPinitializer(&baba,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   baba.n90 = VNMRSN90;
   baba.n90m = VNMRSN90M;
   baba.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      baba.n90 = INOVAN90;
      baba.n90m = INOVAN90M;
      baba.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   baba.phBase[0] = 0.0;

   baba.phBase[1] = 0.0;

   baba.phBase[2] = 0.0;

   baba.phBase[3] = 90.0;

   baba.phBase[4] = 270.0;

   baba.phBase[5] = 270.0;

   baba.phBase[6] = 0.0;

   baba.phBase[7] = 0.0;

   baba.phBase[8] = 0.0;

   baba.phBase[9] = 270.0;

   baba.phBase[10] = 90.0;

   baba.phBase[11] = 90.0;



// Set the Supercycle Phase List



   baba.phSuper[0] = 0.0;

   baba.phSuper[1] = 180.0;



// Set the Delay List



// Get the Standard System Parameter "srate" and Set tauR. 



   double tauR = 2.0*roundoff(1.0/(2.0*getval("srate")),DTCK*baba.n90);



//pwXbaba



   var = getname0("pw",baba.seqName,"");

   double lpw = roundoff(getval(var),DTCK*baba.n90);

   baba.array = disarry(var, baba.array);

   if (lpw >= tauR/8.0) lpw = tauR/8.0; // lpw limited to 50% duty cycle



   baba.pw[0] = lpw;

   baba.pw[1] = tauR/2.0 - 2.0*lpw;

   baba.pw[2] = lpw;

   baba.pw[3] = lpw;

   baba.pw[4] = tauR/2.0 - 2.0*lpw;

   baba.pw[5] = lpw;

   baba.pw[6] = lpw;

   baba.pw[7] = tauR/2.0- 2.0*lpw;

   baba.pw[8] = lpw;

   baba.pw[9] = lpw;

   baba.pw[10] = tauR/2.0 - 2.0*lpw;

   baba.pw[11] = lpw;



// Set the Offset List



//ofXbaba



   var = getname0("of",baba.seqName,"");

   baba.of[0] = getval(var);

   baba.array = disarry(var, baba.array);



// Set the Base Amplitude List



   baba.aBase[0] = 1023.0;



// Set the Base Gate List



   baba.gateBase[0] = 1.0;

   baba.gateBase[1] = 0.0;

   baba.gateBase[2] = 1.0;

   baba.gateBase[3] = 1.0;

   baba.gateBase[4] = 0.0;

   baba.gateBase[5] = 1.0;

   baba.gateBase[6] = 1.0;

   baba.gateBase[7] = 0.0;

   baba.gateBase[8] = 1.0;

   baba.gateBase[9] = 1.0;

   baba.gateBase[10] = 0.0;

   baba.gateBase[11] = 1.0;



// Set the Overall Amplitude, Elements and Channel



//aXbaba



   var = getname0("a",baba.seqName,"");

   baba.a = getval(var);



//qXbaba



   var = getname0("q",baba.seqName,"");

   baba.nelem = (int) getval(var);

   baba.array = disarry(var, baba.array);



//chXbaba


   var  = getname0("ch",baba.seqName,"");

   Getstr(var,baba.ch,sizeof(baba.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",baba.seqName,"");

   sprintf(lpattern,"%s%d",var,baba.nRec);

   baba.hasArray = hasarry(baba.array, lpattern);

   int lix = arryindex(baba.array);

   if (baba.calc > 0) {
      var = getname0("",baba.seqName,"");

      sprintf(baba.pattern,"%s%d_%d",var,baba.nRec,lix);

      if (baba.hasArray == 1) {

         baba = MPchopper(baba); 

         baba.iSuper = iph + baba.nelem%baba.nphSuper;

      }
      baba.t = gett(lix, lpattern);

   }

   return baba;

}


//==========

// Build XY8

//==========



MPSEQ getxy8(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ xy8 = {};

   extern MPSEQ MPchopper(MPSEQ xy8);

   

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getxy8(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(xy8.seqName,"%s",seqName);

   xy8.calc = calc;

   xy8.array = parsearry(xy8.array);



// Obtain Phase Arguments



   xy8.phAccum = p;

   xy8.iSuper = iph;

   xy8.nRec = iRec;

   xy8.phInt = phint;



// Allocate Arrays



   int nphBase = 4;

   int nphSuper = 4;

   int npw = 4;

   int nph = 4;

   int nof = 1;

   int na = 1;

   int ng = 4;

   MPinitializer(&xy8,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   xy8.n90 = VNMRSN90;
   xy8.n90m = VNMRSN90M;
   xy8.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      xy8.n90 = INOVAN90;
      xy8.n90m = INOVAN90M;
      xy8.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   xy8.phBase[0] = 0.0;

   xy8.phBase[1] = 0.0;

   xy8.phBase[2] = 90.0;

   xy8.phBase[3] = 90.0;



// Set the Supercycle Phase List



   xy8.phSuper[0] = 0.0;

   xy8.phSuper[1] = 0.0;

   xy8.phSuper[2] = 90.0;

   xy8.phSuper[3] = 90.0;

// Set the Delay List



//Get Standard System Parameter "srate" and Set tauR.



   double tauR = 2.0*roundoff(1.0/(2.0*getval("srate")),DTCK*xy8.n90);



//pwXxy8 (180 degree pulse)



   var = getname0("pw",xy8.seqName,"");

   double lpw = roundoff(getval(var),DTCK*xy8.n90);

   xy8.array = disarry(var, xy8.array);

   if (lpw >= tauR/4.0) lpw = tauR/4.0; // lpw limited to 50% duty cycle



   xy8.pw[0] = tauR/2.0 - lpw;

   xy8.pw[1] = lpw;

   xy8.pw[2] = tauR/2.0 - lpw;

   xy8.pw[3] = lpw;



// Set the Offset List



//ofXxy8



   var = getname0("of",xy8.seqName,"");

   xy8.of[0] = getval(var);

   xy8.array = disarry(var, xy8.array);



// Set the Base Amplitude List



   xy8.aBase[0] = 1023.0;



// Set the Base Gate List



   xy8.gateBase[0] = 0.0;

   xy8.gateBase[1] = 1.0;

   xy8.gateBase[2] = 0.0;

   xy8.gateBase[3] = 1.0;



// Set the Overall Amplitude, Elements and Channel



//aXxy8



   var = getname0("a",xy8.seqName,"");

   xy8.a = getval(var);

   xy8.array = disarry(var, xy8.array);



//qXxy8



   var = getname0("q",xy8.seqName,"");

   xy8.nelem = (int) getval(var);

   xy8.array = disarry(var, xy8.array);



//chXxy8


   var = getname0("ch",xy8.seqName,"");

   Getstr(var,xy8.ch,sizeof(xy8.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",xy8.seqName,"");

   sprintf(lpattern,"%s%d",var,xy8.nRec);

   xy8.hasArray = hasarry(xy8.array, lpattern);

   int lix = arryindex(xy8.array);

   if (xy8.calc > 0) {
      var = getname0("",xy8.seqName,"");

      sprintf(xy8.pattern,"%s%d_%d",var,xy8.nRec,lix);

      if (xy8.hasArray == 1) {

         xy8 = MPchopper(xy8); 

         xy8.iSuper = iph + xy8.nelem%xy8.nphSuper;

      }
      xy8.t = gett(lix, lpattern);

   }

   return xy8;

}



//======================================================

// Build CP

//======================================================



CP getcp(char *seqName, double p, double phint, int iRec, int calc)

{

   CP cp = {};

   char *var;

   extern CP make_cp(CP cp);



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 2) {

      printf("Error in getcp(). The type name %s is invalid!\n",seqName);

      psg_abort(1);

   }
   //printf("name %s\n",seqName);

   sprintf(cp.seqName,"%s",seqName);

   cp.calc = calc;

   cp.array = parsearry(cp.array);



// Obtain Phase Arguments



   cp.phAccum = p;

   cp.phInt = phint;

   cp.nRec = iRec;

#ifdef MATCH_MASTER

// Set the Step Sizes

   cp.n90 = VNMRSN90;
   cp.n90m = VNMRSN90M;
   cp.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      cp.n90 = INOVAN90;
      cp.n90m = INOVAN90M;
      cp.trap = INOVATRAP;
   }

#endif


// Supply INOVA Start Delays



   if (PWRF_DELAY > 0.0) cp.n90 = 40; // CMR 11/17/23 change from 16 to 40
   else cp.n90 = 40; // CMR 11/17/23 change from 8 to 40   // CMR 11/17/23 changing all n90s to 40 on Ribeye
   
   // VNMRS CMR 7/21/2017 this might be helpful... change from 8 to 16 to avoid fifo underflows
   // CMR 2/7/23 testing with cp.n90=64 hard coded in both instances
   // CMR 2/15/23 back to 16
   // CMR 2/28/23 change to 80 following successful testing on Taurus 
   // CMR 4/13/23 problems with frequency offset CP in hXYXX code; back to 16 temporarily
   
   cp.preset1 = 0;

   cp.preset2 = 0;

   cp.preset3 = 0; // BDZ until 9-11-23 this one was not initialized.
   
   cp.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;

   cp.offstdelay = WFG_OFFSET_DELAY;

   cp.apdelay = PWRF_DELAY;



// chHXsuffix


   var = getname0("ch",cp.seqName,"");
   printf("param = %s\n",var);

   Getstr(var,cp.ch,sizeof(cp.ch));



// aHhxsuffix



   var = getname1("a",cp.seqName,1);
   //printf("param = %s\n",var);

   cp.a1 = getval(var);

   if ((strcmp(cp.ch,"fr") == 0)) 
      cp.array = disarry(var, cp.array);



// aXhxsuffix



   var = getname1("a",cp.seqName,2);
   //printf("param = %s\n",var);

   cp.a2 = getval(var);

   if ((strcmp(cp.ch,"to") == 0))  // well previous to 9-13-23: rajat found and fixed a bug here.
      cp.array = disarry(var, cp.array);



// dHXsuffix



   var = getname0("d",cp.seqName,"");
   //printf("param = %s\n",var);

   cp.d = getval(var);

   cp.array = disarry(var, cp.array);



// bHXsuffix



   var = getname0("b",cp.seqName,"");
   //printf("param = %s\n",var);

   cp.b = getval(var);

   cp.array = disarry(var, cp.array);



// tHXsuffix



   var = getname0("t",cp.seqName,"");
   //printf("param = %s\n",var);

   cp.t = getval(var); 

   cp.array = disarry(var, cp.array);



// ofHXsuffix



   var = getname0("of",cp.seqName,"");
   //printf("param = %s\n",var);

   cp.of = getval(var);

   cp.array = disarry(var, cp.array);



// shHXsuffix



   var = getname0("sh",cp.seqName,"");
   //printf("param = %s\n",var);

   Getstr(var,cp.sh,sizeof(cp.sh));

   cp.array = disarry(var, cp.array);

// BDZ 2-22-24
// THIS CODE NOT INCLUDED. IT IS IN OUR SOLIDMPSEQS.
// IS NOT IN OUR BIOSOLIDMPSEQS. This might be an
// oversight.
  
#if 0
  
// BDZ : these are our fixes: not in OVJ MASTER

// Set constant, linear or tangent CP

   if (strcmp(cp.sh,"c") == 0) {
      cp.b = 1.0e12;
      cp.d = 0.0;
   }
   else if (strcmp(cp.sh,"l") == 0) {
      cp.b = 1.0e12;
   }

// BDZ end fixes

#endif

// frHXsuffix


   var = getname0("fr",cp.seqName,"");
   //printf("param = %s\n",var);

   Getstr(var,cp.fr,sizeof(cp.fr));



// toHXsuffix


   var = getname0("to",cp.seqName,"");
   //printf("param = %s\n",var);

   Getstr(var,cp.to,sizeof(cp.to));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",cp.seqName,"");

   sprintf(lpattern,"%s%d",var,cp.nRec);

   cp.hasArray = hasarry(cp.array, lpattern);

   int lix = arryindex(cp.array);

   if (cp.calc > 0) {
      var = getname0("",cp.seqName,"");

      sprintf(cp.pattern,"%s%d_%d",var,cp.nRec,lix);

      if (cp.hasArray == 1) {
         cp = make_cp(cp);
      }
      cp.t = gett(lix, lpattern);

   }

   return cp;

}





//=================

// Build DREAM

//=================



DREAM getdream(char *seqName, double p, double phint, int iRec, int calc) {
    DREAM d = {};
    char *var;
    extern DREAM make_dream(DREAM d);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

      printf("Error in getdream! The sequence name %s is invalid !\n",seqName);

      psg_abort(-1);

   }
    sprintf(d.seqName,"%s",seqName);
    d.calc = calc;
    d.array = parsearry(d.array);
    sprintf(d.Ruu.seqName,"%s",d.seqName);
    sprintf(d.Rud.seqName,"%s",d.seqName);
    sprintf(d.Rdu.seqName,"%s",d.seqName);
    sprintf(d.Rdd.seqName,"%s",d.seqName);
    
    // Obtain Phase Arguments
    d.phAccum = p;
    d.phInt = phint;
    d.nRec = iRec;
    d.Ruu.phAccum = d.phAccum;
    d.Rud.phAccum = d.phAccum;
    d.Rdu.phAccum = d.phAccum;
    d.Rdd.phAccum = d.phAccum;
    d.Ruu.nRec = d.Rud.nRec = d.Rdu.nRec = d.Rdd.nRec = d.nRec;
    d.Ruu.phInt = d.Rud.phInt = d.Rdu.phInt = d.Rdd.phInt = d.phInt;

#ifdef MATCH_MASTER

// Set the Step Sizes

   d.n90 = VNMRSN90;
   d.n90m = VNMRSN90M;
   d.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      d.n90 = INOVAN90;
      d.n90m = INOVAN90M;
      d.trap = INOVATRAP;
   }

#else

    d.n90 = 40; // BDZ and CMR: changed from 16 on 11-20-23

#endif

    // Supply INOVA Start Delays
    d.preset1 = 0;
    d.preset2 = 0;
    d.strtdelay = WFG_START_DELAY;
    d.offstdelay = WFG_OFFSET_DELAY;
    d.apdelay = PWRF_DELAY;
    d.Ruu.n90 = d.Rud.n90 = d.Rdu.n90 = d.Rdd.n90 = d.n90;
    d.Ruu.preset1 = d.Rud.preset1 = d.Rdu.preset1 = d.Rdd.preset1 = d.preset1;
    d.Ruu.preset2 = d.Rud.preset2 = d.Rdu.preset2 = d.Rdd.preset2 = d.preset2;
    d.Ruu.strtdelay = d.Rud.strtdelay = d.Rdu.strtdelay = d.Rdd.strtdelay = d.strtdelay;
    d.Ruu.offstdelay = d.Rud.offstdelay = d.Rdu.offstdelay = d.Rdd.offstdelay = d.offstdelay;
    d.Ruu.apdelay = d.Rud.apdelay = d.Rdu.apdelay = d.Rdd.apdelay = d.apdelay;

    //chXdream
    var = getname0("ch",d.seqName,"");
    Getstr(var,d.ch,sizeof(d.ch));
    sprintf(d.Ruu.ch,"%s",d.ch);
    sprintf(d.Rud.ch,"%s",d.ch);
    sprintf(d.Rdu.ch,"%s",d.ch);
    sprintf(d.Rdd.ch,"%s",d.ch);

    //aXdream
    var = getname0("a",d.seqName,"");
    d.a = getval(var);
    d.array = disarry(var, d.array);
    d.Ruu.a = d.Rud.a = d.Rdu.a = d.Rdd.a = d.a;

    //ofXdream
    var = getname0("of",d.seqName,"");
    d.of = getval(var);
    d.array = disarry(var, d.array);
    d.Ruu.of = d.Rud.of = d.Rdu.of = d.Rdd.of = d.of;

    //dXdream
    var = getname0("d",d.seqName,"");
    d.d = getval(var);
    d.array = disarry(var, d.array);
    d.Ruu.d = d.Rud.d = d.Rdu.d = d.Rdd.d = d.d;

    //bXdream
    var = getname0("b",d.seqName,"");
    d.b = getval(var); 
    d.array = disarry(var, d.array);
    d.Ruu.b = d.Rud.b = d.Rdu.b = d.Rdd.b = d.b;

    //tXdream
    var = getname0("t",d.seqName,"");
    d.t = getval(var); 
    d.array = disarry(var, d.array);
    d.Ruu.t = d.Rud.t = d.Rdu.t = d.Rdd.t = d.t;

    //Set a Tangent Shape Only
    sprintf(d.Ruu.sh,"%s","t");
    sprintf(d.Rud.sh,"%s","t");
    sprintf(d.Rdu.sh,"%s","t");
    sprintf(d.Rdd.sh,"%s","t");

    //Set the Polarity of Each Waveform
    sprintf(d.Ruu.pol,"%s","uu");
    sprintf(d.Rud.pol,"%s","ud");
    sprintf(d.Rdu.pol,"%s","du");
    sprintf(d.Rdd.pol,"%s","dd");

    //Set the Pattern Name for Each Waveform
    char lpattern[NPATTERN];
    var = getname0("",d.seqName,"");
    sprintf(lpattern,"%s%d",var,d.nRec);
    d.hasArray = hasarry(d.array, d.Ruu.pattern);
    int lix = arryindex(d.array);
    if (d.calc > 0) {
	    var = getname0("uu",d.seqName,"");
	    sprintf(d.Ruu.pattern,"%s%d_%d",var,d.Ruu.nRec,lix);
	    var = getname0("ud",d.seqName,"");
	    sprintf(d.Rud.pattern,"%s%d_%d",var,d.Rud.nRec,lix);
	    var = getname0("du",d.seqName,"");
	    sprintf(d.Rdu.pattern,"%s%d_%d",var,d.Rdu.nRec,lix);
	    var = getname0("dd",d.seqName,"");
	    sprintf(d.Rdd.pattern,"%s%d_%d",var,d.Rdd.nRec,lix);


	    if (d.hasArray == 1) {
		    d = make_dream(d);
	    }

	    d.Ruu.t = gett(lix, lpattern);
	    d.Rud.t = gett(lix, lpattern);
	    d.Rdu.t = gett(lix, lpattern);
	    d.Rdd.t = gett(lix, lpattern);
    }
    return d;
}


//=================

// Build R2T

//=================


R2T getr2t(char *seqName, double p, double phint, int iRec, int calc) {

   R2T r2t = {};

   char *var;
   double tilt;

   extern R2T make_r2t(R2T r2t);



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

      printf("Error in getr2t! The sequence name %s is invalid !\n",seqName);

      psg_abort(-1);

   }

   sprintf(r2t.seqName,"%s",seqName);

   r2t.calc = calc;

   r2t.array = parsearry(r2t.array);
   



   sprintf(r2t.in.seqName,"%s",r2t.seqName);

   sprintf(r2t.drm.seqName,"%s",r2t.seqName);

   sprintf(r2t.out.seqName,"%s",r2t.seqName);



// Obtain Phase Arguments



   r2t.phAccum = p;

   r2t.phInt = phint;

   r2t.nRec = iRec;

   // TODO BDZ: this is our routine. We don't set n90, n90m, and trap
   //   like most other methods do. Copying behavior for now.

#ifdef MATCH_MASTER

// Set the Step Sizes

   r2t.n90 = VNMRSN90;
   r2t.n90m = VNMRSN90M;
   r2t.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      r2t.n90 = INOVAN90;
      r2t.n90m = INOVAN90M;
      r2t.trap = INOVATRAP;
   }

#else

   r2t.n90 = 40; // BDZ and CMR: changed from 16 on 11-20-23

#endif

   r2t.in.phAccum = r2t.phAccum;

   r2t.drm.phAccum = r2t.phAccum;

   r2t.out.phAccum = r2t.phAccum;

   r2t.in.nRec = r2t.drm.nRec = r2t.out.nRec = r2t.nRec;

   r2t.in.phInt = r2t.drm.phInt = r2t.out.phInt = r2t.phInt;



// Supply INOVA Start Delays

   r2t.preset1 = 0;

   r2t.preset2 = 0;

   r2t.strtdelay = WFG_START_DELAY;

   r2t.offstdelay = WFG_OFFSET_DELAY;

   r2t.apdelay = PWRF_DELAY;



   r2t.in.n90 = r2t.drm.n90 = r2t.out.n90 = r2t.n90;

   r2t.in.preset1 = r2t.drm.preset1 = r2t.out.preset1 = r2t.preset1;

   r2t.in.preset2 = r2t.drm.preset2 = r2t.out.preset2 = r2t.preset2;

   r2t.in.strtdelay = r2t.drm.strtdelay = r2t.out.strtdelay = r2t.strtdelay;

   r2t.in.offstdelay = r2t.drm.offstdelay = r2t.out.offstdelay = r2t.offstdelay;

   r2t.in.apdelay = r2t.drm.apdelay = r2t.out.apdelay = r2t.apdelay;



//chXr2t
  
   var = getname0("ch",r2t.seqName,"");

   Getstr(var,r2t.ch,sizeof(r2t.ch));



   sprintf(r2t.in.ch,"%s",r2t.ch);

   sprintf(r2t.drm.ch,"%s",r2t.ch);

   sprintf(r2t.out.ch,"%s",r2t.ch);





//aXr2t



   var = getname0("a",r2t.seqName,"");

   r2t.a = getval(var);

   r2t.array = disarry(var, r2t.array);



   r2t.in.a = r2t.a + r2t.d;
   r2t.drm.a = r2t.a + r2t.d;
   r2t.out.a = r2t.a - r2t.d;



//ofXr2t



   var = getname0("of",r2t.seqName,"");

   r2t.of = getval(var);

   r2t.array = disarry(var, r2t.array);



   r2t.in.of = r2t.drm.of = r2t.out.of = r2t.of;



//dXr2t



   var = getname0("d",r2t.seqName,"");

   r2t.d = getval(var);

   r2t.array = disarry(var, r2t.array);



   r2t.in.d = r2t.a + r2t.d;
   r2t.drm.d = r2t.d*(-2.);
   r2t.out.d = r2t.d - r2t.a;



//bXr2t



   var = getname0("b",r2t.seqName,"");

   r2t.b = getval(var); 

   r2t.array = disarry(var, r2t.array);



   r2t.in.b = r2t.drm.b = r2t.out.b = r2t.b;



//tXr2t


   tilt = getval("tilt");

   var = getname0("t",r2t.seqName,"");

   r2t.t = getval(var); 

   r2t.array = disarry(var, r2t.array);



   r2t.in.t = r2t.out.t = tilt;
   r2t.drm.t = r2t.t - r2t.in.t - r2t.out.t;



//Set a Tangent Shape Only



   sprintf(r2t.in.sh,"%s","l");

   sprintf(r2t.drm.sh,"%s","l");

   sprintf(r2t.out.sh,"%s","l");



//Set the Polarity of Each Waveform



   sprintf(r2t.in.pol,"%s","n");

   sprintf(r2t.drm.pol,"%s","n");

   sprintf(r2t.out.pol,"%s","n");



//Set the Pattern Name for Each Waveform


   char lpattern[NPATTERN];
   var = getname0("",r2t.seqName,"");

   sprintf(lpattern,"%s%d",var,r2t.nRec);

   r2t.hasArray = hasarry(r2t.array, r2t.in.pattern);

   int lix = arryindex(r2t.array);

   if (r2t.calc > 0) {
      var = getname0("in",r2t.seqName,"");

      sprintf(r2t.in.pattern,"%s%d_%d",var,r2t.in.nRec,lix);
      var = getname0("drm",r2t.seqName,"");

      sprintf(r2t.drm.pattern,"%s%d_%d",var,r2t.drm.nRec,lix);
      var = getname0("out",r2t.seqName,"");

      sprintf(r2t.out.pattern,"%s%d_%d",var,r2t.out.nRec,lix);
      if (r2t.hasArray == 1) {

         r2t = make_r2t(r2t);

      }
      r2t.in.t = gett(lix, lpattern);
      r2t.drm.t = gett(lix, lpattern);
      r2t.out.t = gett(lix, lpattern);
   }

   return r2t;

}


//========================================

// Build DRAWS

//========================================



MPSEQ getdraws(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ r = {};

   extern MPSEQ MPchopper(MPSEQ r);

   char *var;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getdraws(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(r.seqName,"%s",seqName);

   r.calc = calc;

   r.array = parsearry(r.array); 



// Obtain Phase Arguments



   r.phAccum = p;

   r.iSuper = iph;

   r.nRec = iRec;

   r.phInt = phint;



// Allocate Arrays



   int nphBase = 10;

   int nphSuper = 4;

   int npw = 10;

   int nph = 10;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&r,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   r.n90 = VNMRSN90;
   r.n90m = VNMRSN90M;
   r.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      r.n90 = INOVAN90;
      r.n90m = INOVAN90M;
      r.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   r.phBase[0] = 0.0;

   r.phBase[1] = 180.0;

   r.phBase[2] = 90.0;

   r.phBase[3] = 0.0;

   r.phBase[4] = 180.0;

   r.phBase[5] = 180.0;

   r.phBase[6] = 0.0;

   r.phBase[7] = 90.0;

   r.phBase[8] = 180.0;

   r.phBase[9] = 0.0;



// Set the Supercycle Phase List



   r.phSuper[0] = 0.0;

   r.phSuper[1] = 180.0;

   r.phSuper[2] = 180.0;

   r.phSuper[3] = 0.0;



// Set the Delay List



//pw1Xdraws



   var = getname0("pw1",r.seqName,"");

   double pw360 = getval(var);

   r.array = disarry(var, r.array);



//pw2Xdraws



   var = getname0("pw2",r.seqName,"");

   double pw90 = getval(var);

   r.array = disarry(var, r.array);



   r.pw[0] = pw360;

   r.pw[1] = pw360;

   r.pw[2] = pw90;

   r.pw[3] = pw360;

   r.pw[4] = pw360;

   r.pw[5] = pw360;

   r.pw[6] = pw360;

   r.pw[7] = pw90;

   r.pw[8] = pw360;

   r.pw[9] = pw360;



// Set the Offset List



//ofXdraws



   var = getname0("of",r.seqName,"");

   r.of[0] = getval(var);

   r.array = disarry(var, r.array);



// Set the Amp and Gate Lists



   r.aBase[0] = 1023.0;

   r.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXdraws



   var = getname0("a",r.seqName,"");

   r.a = getval(var);



//qXdraws



   var = getname0("q",r.seqName,"");

   r.nelem = getval(var);

   r.array = disarry(var, r.array);



//chXdraws

   var = getname0("ch",r.seqName,"");

   Getstr(var,r.ch,sizeof(r.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",r.seqName,"");

   sprintf(lpattern,"%s%d",var,r.nRec);

   r.hasArray = hasarry(r.array,lpattern);

   int lix = arryindex(r.array);

   if (r.calc > 0) {
      var = getname0("",r.seqName,"");

      sprintf(r.pattern,"%s%d_%d",var,r.nRec,lix);

      if (r.hasArray == 1) {

         r = MPchopper(r); 

         r.iSuper = iph + r.nelem%r.nphSuper;

      }
      r.t = gett(lix, lpattern);

   }

   return r;

}



//==================

// Build RAPT X-X

//==================



MPSEQ getrapt(char *seqName, double p, double phint, int iRec, int calc)

{

   MPSEQ r = {};

   extern MPSEQ MPchopper(MPSEQ r);

   char *var;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getrapt(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(r.seqName,"%s",seqName);

   r.calc = calc;

   r.array = parsearry(r.array);



// Obtain Phase Arguments



   r.phAccum = p;

   r.nRec = iRec;

   r.phInt = phint;



// Allocate Arrays



   int nphBase = 2;

   int nphSuper = 1;

   int npw = 2;

   int nph = 2;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&r,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   r.n90 = VNMRSN90;
   r.n90m = VNMRSN90M;
   r.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      r.n90 = INOVAN90;
      r.n90m = INOVAN90M;
      r.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   r.phBase[0] = 0.0;

   r.phBase[1] = 180.0;



// Set the Supercycle Phase List



   r.phSuper[0] = 0.0;



// Set the Delay List



//pwXrapt



   var = getname0("pw",r.seqName,"");

   r.pw[0] = getval(var);

   r.pw[1] = r.pw[0];

   r.array = disarry(var, r.array);



//Set the Offset List



//ofXrapt



   var = getname0("of",r.seqName,"");

   double offset = getval(var);

   r.of[0] = offset;

   r.array = disarry(var, r.array);



// Set the Amp and Gate Lists



   r.aBase[0] = 1023.0;

   r.gateBase[0] = 0.0;



//Calculate the Elements From the Time



//tXrapt



   var = getname0("t",r.seqName,"");

   double ttime = getval(var);

   int nelem = (int) (ttime/(2.0*r.pw[0] +DTCK));

   nelem = nelem + 1;

   r.nelem = 1; 

   if ((fabs(offset)) > 1.0) {

      r.nelem = nelem;

   }

   r.array = disarry(var, r.array);



// Set the Overall Amplitude and Channel



//aXrapt



   var = getname0("a",r.seqName,"");

   r.a = getval(var);



//chXrapt


   var = getname0("ch",r.seqName,"");

   Getstr(var,r.ch,sizeof(r.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",r.seqName,"");

   sprintf(lpattern,"%s%d",var,r.nRec);

   r.hasArray = hasarry(r.array, lpattern);

   int lix = arryindex(r.array);

   if (r.calc > 0) {
      var = getname0("",r.seqName,"");

      sprintf(r.pattern,"%s%d_%d",var,r.nRec,lix);

      if (r.hasArray == 1) {

         r = MPchopper(r);

      }
      r.t = gett(lix, lpattern);

   }



// Override r.t from the Chopper



   r.t = ttime;

   return r;

}



//=====================

// Build Gaussian RAPT

//=====================



MPSEQ getgrapt(char *seqName, double p, double phint, int iRec, int calc)

{

   MPSEQ r = {};

   extern MPSEQ MPchopper(MPSEQ r);

   char *var;

   int i;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getgrapt(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(r.seqName,"%s",seqName);

   r.calc = calc;

   r.array = parsearry(r.array);



// Obtain Phase Arguments



   r.phAccum = p;

   r.nRec = iRec;

   r.phInt = phint;



// Determine nphBase from the Pulse Width



//pwXgrapt



   var = getname0("pw",r.seqName,"");

   double pw = getval(var);

   r.array = disarry(var, r.array);



#ifdef MATCH_MASTER

   double dstep;
   if (PWRF_DELAY > 0.0)
      dstep =  INOVAN90 * DTCK;
   else
      dstep =  VNMRSN90 * DTCK;

#else

   // BDZ OUR OLD WAY

   double dstep = r.n90*DTCK;

#endif

   double steps = pw/(2.0*dstep);

   int nsteps = 2*((int) (steps + 0.5));

   r.array = disarry(var, r.array);



// Allocate Arrays



   int nphBase = 2*nsteps;

   int nphSuper = 1;

   int npw = 1;

   int nph = 1;

   int nof = nphBase;

   int na = nphBase;

   int ng = 1;

   MPinitializer(&r,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   r.n90 = VNMRSN90;
   r.n90m = VNMRSN90M;
   r.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      r.n90 = INOVAN90;
      r.n90m = INOVAN90M;
      r.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   r.phBase[0] = 0.0;



// Set the Supercycle Phase List



   r.phSuper[0] = 0.0;



// Set the Delay List



   r.pw[0] = dstep;



// Set the Base Offset List



//ofdXgrapt



   var = getname0("ofd",r.seqName,"");

   double offsetd = getval(var);

   r.array = disarry(var, r.array);



//ofXgrapt



   var = getname0("of",r.seqName,"");

   double offset = getval(var);

   r.array = disarry(var, r.array);



   for (i = 0; i < nsteps; i++) {

      r.of[i] =  offset + offsetd;

   }

   for (i = nsteps; i < 2*nsteps; i++) {

      r.of[i] =  offset - offsetd;

   }



//Set the Base Amplitude List



   double a = -4.60517;

   double t0 = nsteps*NMIN*DTCK/2;

   double time = dstep/2.0;

   double amp = 0.0;

   for (i = 0; i < nsteps; i++) {

      amp = 1023*exp(a*((time-t0)*(time-t0)/(t0*t0)));

#ifdef MATCH_MASTER

      r.aBase[i] = roundamp(amp, 1.0/FSD);  // maybe was a bugfix?

#else

      r.aBase[i] = roundamp(amp, 1.0);

#endif

      time = time + dstep;

   }

   time = dstep/2.0;

   for (i = nsteps; i < 2*nsteps; i++) {

      amp = 1023.0*exp(a*((time-t0)*(time-t0)/(t0*t0)));

#ifdef MATCH_MASTER

      r.aBase[i] = roundamp(amp, 1.0/FSD);  // maybe was a bugfix?

#else

      r.aBase[i] = roundamp(amp, 1.0);

#endif

      time = time + dstep;

   }



//Set the Base Gate List



   r.gateBase[0] = 0.0;



//Calculate the Elements From the Time



//tXrapt



   var = getname0("t",r.seqName,"");

   double ttime = getval(var);

   int nelem = (int) (ttime/(2.0*nsteps*NMIN*DTCK) + DTCK);

   nelem = nelem + 1;

   r.nelem = 1;

   if ((fabs(offset)) > 1.0) {

      r.nelem = nelem;

   }

   r.array = disarry(var, r.array);



// Set the Overall Amplitude and Channel



//aXgrapt



   var = getname0("a",r.seqName,"");

   r.a = getval(var);



//chXgrapt


   var  = getname0("ch",r.seqName,"");

   Getstr(var,r.ch,sizeof(r.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",r.seqName,"");

   sprintf(lpattern,"%s%d",var,r.nRec);

   r.hasArray = hasarry(r.array, lpattern);

   int lix = arryindex(r.array);

   if (r.calc > 0) {
      var = getname0("",r.seqName,"");

      sprintf(r.pattern,"%s%d_%d",var,r.nRec,lix);

      if (r.hasArray == 1) {

         r = MPchopper(r);

      }
      r.t = gett(lix, lpattern);

   }



//Override r.t from the Chopper



   r.t = ttime;

   return r;

}



//=============

// Build PIPS

//=============



MPSEQ getpipsxy(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ pips = {};

   extern MPSEQ MPchopper(MPSEQ pips);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getpipsxy(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(pips.seqName,"%s",seqName);

   pips.calc = calc;

   pips.array = parsearry(pips.array);



// Obtain Phase Arguments



   pips.phAccum = p;

   pips.iSuper = iph;

   pips.nRec = iRec;

   pips.phInt = phint;



// Allocate Arrays



   int nphBase = 4;

   int nphSuper = 1;

   int npw = 4;

   int nph = 4;

   int nof = 1;

   int na = 1;

   int ng = 4;

   MPinitializer(&pips,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   pips.n90 = VNMRSN90;
   pips.n90m = VNMRSN90M;
   pips.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pips.n90 = INOVAN90;
      pips.n90m = INOVAN90M;
      pips.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   pips.phBase[0] = 0.0;

   pips.phBase[1] = 0.0;

   pips.phBase[2] = 90.0;

   pips.phBase[3] = 90.0;



// Set the Supercycle Phase List



   pips.phSuper[0] = 0.0;



// Get Standard System Parameter "srate" and Set tauR.



   double tauR = 2.0*roundoff(1.0/(2.0*getval("srate")),DTCK*pips.n90);



// Set the delay List



//nXpips



   var = getname0("n",pips.seqName,"");

   int n = (int) getval(var);

   pips.array = disarry(var, pips.array);



//pwXpips



   var = getname0("pw",pips.seqName,"");

   double lpw = roundoff(getval(var),DTCK*pips.n90);



   pips.pw[0] = n*tauR - lpw;

   pips.pw[1] = lpw;

   pips.pw[2] = n*tauR - lpw;

   pips.pw[3] = lpw;



   pips.array = disarry(var, pips.array);



// Set the Offset List



//ofXpips



   var = getname0("of",pips.seqName,"");

   pips.of[0] = getval(var);

   pips.array = disarry(var, pips.array);



// Set the Base Amplitude List



   pips.aBase[0] = 1023.0;



// Set the Base Gate List



   pips.gateBase[0] = 0.0;

   pips.gateBase[1] = 1.0;

   pips.gateBase[2] = 0.0;

   pips.gateBase[3] = 1.0;



// Set the Overall Amplitude, Elements and Channel



//aXpips



   var = getname0("a",pips.seqName,"");

   pips.a = getval(var);



//qXpips



   var = getname0("q",pips.seqName,"");

   pips.nelem = (int) getval(var);

   pips.array = disarry(var, pips.array);



//chXpips


   var = getname0("ch",pips.seqName,"");

   Getstr(var,pips.ch,sizeof(pips.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",pips.seqName,"");

   sprintf(lpattern,"%s%d",var,pips.nRec);

   pips.hasArray = hasarry(pips.array, lpattern);

   int lix = arryindex(pips.array);

   if (pips.calc > 0) {
      var = getname0("",pips.seqName,"");

      sprintf(pips.pattern,"%s%d_%d",var,pips.nRec,lix);

      if (pips.hasArray == 1) {

         pips = MPchopper(pips); 

         pips.iSuper = iph + pips.nelem%pips.nphSuper;

      }
      pips.t = gett(lix, lpattern);

   }

   return pips;

}



//========================================

//Build [R4-sub1-sup2]3-sup1 (SR4)

//========================================



MPSEQ getsr4(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ r = {};

   extern MPSEQ MPchopper(MPSEQ r);

   char *var;   

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getsr4(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(r.seqName,"%s",seqName);

   r.calc = calc;

   r.array = parsearry(r.array);



// Obtain Phase Arguments



   r.phAccum = p;

   r.iSuper = iph;

   r.nRec = iRec;

   r.phInt = phint;



// Allocate Arrays

#ifdef MATCH_MASTER

   int SIZE = 24;

#else

   int SIZE = 72;

#endif

   int nphBase = 1;

   int nphSuper = SIZE;

   int npw = 1;

   int nph = 1;

   int nof = 1;

   int na = 1; 

   int ng = 1;

   MPinitializer(&r,npw,nph,nof,na,ng,nphBase,nphSuper);

   MPinitializer(&r,1,1,1,1,1,1,SIZE);

#ifdef MATCH_MASTER

// Set the Step Sizes

   r.n90 = VNMRSN90;
   r.n90m = VNMRSN90M;
   r.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      r.n90 = INOVAN90;
      r.n90m = INOVAN90M;
      r.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   r.phBase[0] = 0.0;



// Set the Supercycle Phase List



   double r4phase[2];

   r4phase[0] = 90.0;

   r4phase[1] =270.0;



   double scphase[6];

   scphase[0] = 0.0;

   scphase[1] = 180.0;

   scphase[2] = 120.0;

   scphase[3] = 300.0;

   scphase[4] = 240.0;

   scphase[5] =  60.0;



   int n1,n2,index;

   double phase = 0.0;

   for (index = 0; index < 72; index++) {

      n1 = index%2;

      n2 = index/4;

      phase = r4phase[n1] + scphase[n2];

      while (phase >= 360.0) phase = phase - 360.0;

      r.phSuper[index] = phase;

   }



// Set the Delay List



//pwXsr4



   var = getname0("pw",r.seqName,"");

   r.pw[0] = getval(var);

   r.pw[0] = 0.5*r.pw[0];

   r.array = disarry(var, r.array);



// Set the Duration List



//ofXsr4



   var = getname0("of",r.seqName,"");

   r.of[0] = getval(var);

   r.array = disarry(var, r.array);



// Set the Amp and Gate Lists



   r.aBase[0] = 1023.0;

   r.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXsr4



   var = getname0("a",r.seqName,"");

   r.a = getval(var);



//qXsr4



   var = getname0("q",r.seqName,"");

   r.nelem = getval(var);

   r.array = disarry(var, r.array); 

 

//chXsr4


   var = getname0("ch",r.seqName,"");

   Getstr(var,r.ch,sizeof(r.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",r.seqName,"");

   sprintf(lpattern,"%s%d",var,r.nRec);

   r.hasArray = hasarry(r.array, lpattern);

   int lix = arryindex(r.array);

   if (r.calc > 0) {
      var = getname0("",r.seqName,"");

      sprintf(r.pattern,"%s%d_%d",var,r.nRec,lix);

      if (r.hasArray == 1) {

         r = MPchopper(r); 

         r.iSuper = iph + r.nelem/72;

      }
      r.t = gett(lix, lpattern);

   }

   return r;

}



//====================

// Build SAMMY for DEC

//====================



MPSEQ getsammyd(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ sd = {};

   extern MPSEQ MPchopper(MPSEQ sd);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getsammyd(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(sd.seqName,"%s",seqName);

   sd.calc = calc;

   sd.array = parsearry(sd.array);



// Obtain Phase Arguments



   sd.phAccum = p;

   sd.iSuper = iph;

   sd.nRec = iRec;

   sd.phInt = phint;



// Allocate Arrays



   int nphBase = 9;

   int nphSuper = 1;

   int npw = 9;

   int nph = 9;

   int nof = 1;

   int na = 1;

   int ng = 9;

   MPinitializer(&sd,npw,nph,nof,na,ng,nphBase,nphSuper); 

#ifdef MATCH_MASTER

// Set the Step Sizes

   sd.n90 = VNMRSN90;
   sd.n90m = VNMRSN90M;
   sd.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      sd.n90 = INOVAN90;
      sd.n90m = INOVAN90M;
      sd.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   sd.phBase[0] = 0.0;

   sd.phBase[1] = 90.0;

   sd.phBase[2] = 270.0;

   sd.phBase[3] = 270.0;

   sd.phBase[4] = 180.0;

   sd.phBase[5] = 270.0;

   sd.phBase[6] = 90.0;

   sd.phBase[7] = 90.0;

   sd.phBase[8] = 0.0;



// Set the Supercycle Phase List



   sd.phSuper[0] = 0.0;



// Set the Delay List



//pwHsmyd



   var = getname0("pw",sd.seqName,"");

   double pw90 = roundoff(getval(var),2.0*DTCK*sd.n90);
   sd.array = disarry(var, sd.array);



//t1Hsmyd



   var = getname0("t1",sd.seqName,"");

   double del1 = roundoff(getval(var),DTCK*sd.n90);
   sd.array = disarry(var, sd.array);



//t2Hsmyd



   var = getname0("t2",sd.seqName,"");

   double del2 = roundoff(getval(var),2.0*DTCK*sd.n90);
   sd.array = disarry(var, sd.array);



   double dur1 = 3.5*pw90 + del1;

   double dur2 = 1.5*pw90 + del2/2.0;



   sd.pw[0] = dur1;

   sd.pw[1] = pw90;

   sd.pw[2] = 2.0*dur2;

   sd.pw[3] = pw90;

   sd.pw[4] = 2.0*dur1;

   sd.pw[5] = pw90;

   sd.pw[6] = 2.0*dur2;

   sd.pw[7] = pw90;

   sd.pw[8] = dur1;



// Set the Offset List



//ofHsmyd



   var = getname0("of",sd.seqName,"");

   sd.of[0] = getval(var);

   sd.array = disarry(var, sd.array);



// Set the Amplitude List



   sd.aBase[0] = 1023.0;



// Base Gate Pattern



   sd.gateBase[0] = 1.0;

   sd.gateBase[1] = 1.0;

   sd.gateBase[2] = 0.0;

   sd.gateBase[3] = 1.0;

   sd.gateBase[4] = 1.0;

   sd.gateBase[5] = 1.0;

   sd.gateBase[6] = 0.0;

   sd.gateBase[7] = 1.0;

   sd.gateBase[8] = 1.0;



// Set the Overall Amplitude, Elements and Channel

 

//aHsmyd



   var = getname0("a",sd.seqName,"");

   sd.a = getval(var); 



//qHsmyd



   var = getname0("q",sd.seqName,"");

   sd.nelem = (int) getval(var);

   sd.array = disarry(var, sd.array);



//chHsmyd


   var = getname0("ch",sd.seqName,"");

   Getstr(var,sd.ch,sizeof(sd.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",sd.seqName,"");

   sprintf(lpattern,"%s%d",var,sd.nRec);


   sd.hasArray = hasarry(sd.array, lpattern);

   int lix = arryindex(sd.array);

   if (sd.calc > 0) {
      var = getname0("",sd.seqName,"");

      sprintf(sd.pattern,"%s%d_%d",var,sd.nRec,lix);

      if (sd.hasArray == 1) {

         sd = MPchopper(sd); 

         sd.iSuper = iph + sd.nelem%sd.nphSuper;

      }
      sd.t = gett(lix, lpattern);

   }

   return sd;

}



//====================

// Build SAMMY for OBS

//====================



MPSEQ getsammyo(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ so = {};

   extern MPSEQ MPchopper(MPSEQ so);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getsammyo(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(so.seqName,"%s",seqName);

   so.calc = calc;

   so.array = parsearry(so.array);



// Obtain Phase Arguments



   so.phAccum = p;

   so.iSuper = iph;

   so.nRec = iRec;

   so.phInt = phint;



// Allocate Arrays



   int nphBase = 3;

   int nphSuper = 1;

   int npw = 3;

   int nph = 3;

   int nof = 1;

   int na = 1; 

   int ng = 1; 

   MPinitializer(&so,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   so.n90 = VNMRSN90;
   so.n90m = VNMRSN90M;
   so.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      so.n90 = INOVAN90;
      so.n90m = INOVAN90M;
      so.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List 



   so.phBase[0] = 0.0;

   so.phBase[1] = 180.0;

   so.phBase[2] = 0.0;



// Set the Supercycle Phase List



   so.phSuper[0] = 0.0;

   

// Set the Delay List



//pwXsmyo



   var = getname0("pw",so.seqName,"");

   double pw90 = roundoff(getval(var),2.0*DTCK*so.n90);

   so.array = disarry(var, so.array);


//t1Xsmyo



   var = getname0("t1",so.seqName,"");

   double del1 = roundoff(getval(var),DTCK*so.n90);

   so.array = disarry(var, so.array);


//t2Xsmyo



   var = getname0("t2",so.seqName,"");

   double del2 = roundoff(getval(var),2.0*DTCK*so.n90);

   so.array = disarry(var, so.array);


   double dur1 = 4.0*pw90 + del1;

   double dur2 = 2.0*pw90 + del2/2.0;



   so.pw[0] = dur1 + dur2;

   so.pw[1] = 2.0*(dur1 + dur2);

   so.pw[2] = dur1 + dur2;



// Set the Offset List



//ofXsmyo



   var = getname0("of",so.seqName,"");

   so.of[0] = getval(var);

   so.array = disarry(var, so.array);



// Set the Amp and Gate Lists



   so.aBase[0] = 1023.0;

   so.gateBase[0] = 0.0; 



// Set the Overall Amplitude, Elements and Channel



//aXsmyo



   var = getname0("a",so.seqName,"");

   so.a = getval(var);



//qXsmyo



   var = getname0("q",so.seqName,"");

   so.nelem = (int) getval(var);

   so.array = disarry(var, so.array);


//chXsmyo


   var = getname0("ch",so.seqName,"");

   Getstr(var,so.ch,sizeof(so.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",so.seqName,"");

   sprintf(lpattern,"%s%d",var,so.nRec);

   so.hasArray = hasarry(so.array, lpattern);

   int lix = arryindex(so.array);

   if (so.calc > 0) {
      var = getname0("",so.seqName,"");

      sprintf(so.pattern,"%s%d_%d",var,so.nRec,lix);

      if (so.hasArray == 1) {

         so = MPchopper(so); 

         so.iSuper = iph + so.nelem%so.nphSuper;

      }
      so.t = gett(lix, lpattern);

   }

   return so;

}



//========================================

// Lee Goldburg (simple offset) Decoupling

//========================================



MPSEQ getlg(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ f = {};

   extern MPSEQ MPchopper(MPSEQ f);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getlg(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(f.seqName,"%s",seqName);

   f.calc = calc;

   f.array = parsearry(f.array);



// Obtain Phase Arguments



   f.phAccum = p;

   f.nRec = iRec;

   f.phInt = phint;



// Allocate Arrays



   int nphBase = 1;

   int nphSuper = 1;

   int npw = 1;

   int nph = 1;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&f,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   f.n90 = VNMRSN90;
   f.n90m = VNMRSN90M;
   f.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      f.n90 = INOVAN90;
      f.n90m = INOVAN90M;
      f.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   f.nphBase = 1;

   f.phBase[0] = 0.0;



// Set the Supercycle Phase List



   f.nphSuper = 1;

   f.phSuper[0] = 0.0;



// Set the Delay List



//tXlg



   var = getname0("t",f.seqName,"");

   f.pw[0] = getval(var);

   f.array = disarry(var, f.array);



// Set the Offset List



//ofXlg



   var = getname0("of",f.seqName,"");

   double loffset = getval(var);

   f.of[0] = loffset;

   f.array = disarry(var, f.array);



//chXlg
   
   var = getname0("ch",f.seqName,"");

   Getstr(var,f.ch,sizeof(f.ch));



// Set the Amp and Gate Lists



   f.aBase[0] = 1023.0;

   f.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXlg



   var = getname0("a",f.seqName,"");

   f.a = getval(var);



// Set the number of Elements



   f.nelem = 1;



//chXlg



   Getstr(getname0("ch",f.seqName,""),f.ch,sizeof(f.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",f.seqName,"");

   sprintf(lpattern,"%s%d",var,f.nRec);

   f.hasArray = hasarry(f.array, lpattern);

   int lix = arryindex(f.array);

   if (f.calc > 0) {
      var = getname0("",f.seqName,"");

      sprintf(f.pattern,"%s%d_%d",var,f.nRec,lix);

      if (f.hasArray == 1) {

         f = MPchopper(f);

         f.iSuper = iph + f.nelem%f.nphSuper;

      }
      f.t = gett(lix, lpattern);

   }

   return f;

}



//====================================

// Build RFDR with an XY8 Phase Cycle

//====================================



MPSEQ getrfdrxy8(char *seqName, int iph, double p, double phint, int iRec, int calc)
{

   char *var;

   MPSEQ xy8 = {};

   extern MPSEQ MPchopper(MPSEQ xy8);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getrfdrxy8(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(xy8.seqName,"%s",seqName);

   xy8.calc = calc;

   xy8.array = parsearry(xy8.array);



// Obtain Phase Arguments



   xy8.phAccum = p;

   xy8.iSuper = iph;

   xy8.nRec = iRec;

   xy8.phInt = phint;



// Allocate Arrays



   int nphBase = 24;  // 6 in solidmpseqs

   int nphSuper = 8;  // 4 in solidmpseqs

   int npw = 24;  // 6 in solidmpseqs

   int nph = 24;  // 6 in solidmpseqs

   int nof = 1;

   int na = 1;

   int ng = 24;  // 6 in solidmpseqs

   MPinitializer(&xy8,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   xy8.n90 = VNMRSN90;
   xy8.n90m = VNMRSN90M;
   xy8.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      xy8.n90 = INOVAN90;
      xy8.n90m = INOVAN90M;
      xy8.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List (XY-4 element)



   xy8.phBase[0] = 0.0;

   xy8.phBase[1] = 0.0;

   xy8.phBase[2] = 0.0;

   xy8.phBase[3] = 90.0;

   xy8.phBase[4] = 90.0;

   xy8.phBase[5] = 90.0;
   
   // BDZ the next 18 lines are not in solidmpseqs
   
   xy8.phBase[6] = 0.0;

   xy8.phBase[7] = 0.0;

   xy8.phBase[8] = 0.0;

   xy8.phBase[9] = 90.0;

   xy8.phBase[10] = 90.0;

   xy8.phBase[11] = 90.0;


   xy8.phBase[12] = 90.0;

   xy8.phBase[13] = 90.0;

   xy8.phBase[14] = 90.0;

   xy8.phBase[15] = 0.0;

   xy8.phBase[16] = 0.0;

   xy8.phBase[17] = 0.0;
   
   xy8.phBase[18] = 90.0;

   xy8.phBase[19] = 90.0;

   xy8.phBase[20] = 90.0;

   xy8.phBase[21] =  0.0;

   xy8.phBase[22] =  0.0;

   xy8.phBase[23] =  0.0;


// Set the Supercycle Phase List for XY-16 and XY-32



   xy8.phSuper[0] = 0.0;

   xy8.phSuper[1] = 180.0;  // BDZ 0.0 in solidmpseqs

   xy8.phSuper[2] = 180.0;  // BDZ 90.0 in solidmpseqs

   xy8.phSuper[3] = 0.0;  // BDZ 90.0 in solidmpseqs
   
   // BDZ the next 4 lines are not in solidmpseqs

   xy8.phSuper[4] = 0.0;

   xy8.phSuper[5] = 0.0;

   xy8.phSuper[6] = 180.0;

   xy8.phSuper[7] = 180.0;

// Get the Standard System Parameter "srate" and Set tauR


   // BDZ suspicious looking code
   // in most code 12.5e-9 is replaced with DTCK. We seem to be doing
   //   10*DTCK*xy8.n90.
   // notice those are different by a factor of 10. did we copy code badly?

   double tauR = 2.0*roundoff(1.0/(2.0*getval("srate")),125e-9*xy8.n90);



// Set the Delay List



//pwXxy8 (180 degree pulse)



   var = getname0("pw",xy8.seqName,"");

   double lpw = 2.0*roundoff(getval(var)/2.0,DTCK*xy8.n90);


   // in solidmpseqs we limit to 50% duty cycle by dividing by 2.0

   if (lpw >= tauR/1.5) lpw = tauR/1.5; // lpw limited to 67% duty cycle



   xy8.pw[0] = tauR/2.0 - lpw/2.0;

   xy8.pw[1] = lpw;

   xy8.pw[2] = tauR/2.0 - lpw/2.0;

   xy8.pw[3] = tauR/2.0 - lpw/2.0;

   xy8.pw[4] = lpw;

   xy8.pw[5] = tauR/2.0 - lpw/2.0;
   
   // BDZ in solidmpseqs the next 18 lines don't exist

   xy8.pw[6] = tauR/2.0 - lpw/2.0;

   xy8.pw[7] = lpw;

   xy8.pw[8] = tauR/2.0 - lpw/2.0;

   xy8.pw[9] = tauR/2.0 - lpw/2.0;

   xy8.pw[10] = lpw;

   xy8.pw[11] = tauR/2.0 - lpw/2.0;

   xy8.pw[12] = tauR/2.0 - lpw/2.0;

   xy8.pw[13] = lpw;

   xy8.pw[14] = tauR/2.0 - lpw/2.0;

   xy8.pw[15] = tauR/2.0 - lpw/2.0;

   xy8.pw[16] = lpw;

   xy8.pw[17] = tauR/2.0 - lpw/2.0;

   xy8.pw[18] = tauR/2.0 - lpw/2.0;

   xy8.pw[19] = lpw;

   xy8.pw[20] = tauR/2.0 - lpw/2.0;

   xy8.pw[21] = tauR/2.0 - lpw/2.0;

   xy8.pw[22] = lpw;

   xy8.pw[23] = tauR/2.0 - lpw/2.0;


   xy8.array = disarry(var, xy8.array);



// Set the Offset List



//ofXxy8



   var = getname0("of",xy8.seqName,""); 

   xy8.of[0] = getval(var);

   xy8.array = disarry(var, xy8.array);



// Set the Amplitude List



   xy8.aBase[0] = 1023.0;



// Set the Base Gate List



   xy8.gateBase[0] = 0.0;

   xy8.gateBase[1] = 1.0;

   xy8.gateBase[2] = 0.0;

   xy8.gateBase[3] = 0.0;

   xy8.gateBase[4] = 1.0;

   xy8.gateBase[5] = 0.0;
   
   // BDZ in solidmpseqs the next 18 lines don't exist
   
   xy8.gateBase[6] = 0.0;

   xy8.gateBase[7] = 1.0;

   xy8.gateBase[8] = 0.0;

   xy8.gateBase[9] = 0.0;

   xy8.gateBase[10] = 1.0;

   xy8.gateBase[11] = 0.0;


   xy8.gateBase[12] = 0.0;

   xy8.gateBase[13] = 1.0;

   xy8.gateBase[14] = 0.0;

   xy8.gateBase[15] = 0.0;

   xy8.gateBase[16] = 1.0;

   xy8.gateBase[17] = 0.0;
   
   xy8.gateBase[18] = 0.0;

   xy8.gateBase[19] = 1.0;

   xy8.gateBase[20] = 0.0;

   xy8.gateBase[21] = 0.0;

   xy8.gateBase[22] = 1.0;

   xy8.gateBase[23] = 0.0;

// Set the Overall Amplitude, Elements and Channel



//aXxy8



   var = getname0("a",xy8.seqName,"");

   xy8.a = getval(var);



//qXxy8



   var = getname0("q",xy8.seqName,"");

   xy8.nelem = (int) getval(var); 

   xy8.array = disarry(var, xy8.array);



//chXxy8


   var = getname0("ch",xy8.seqName,""); 

   Getstr(var,xy8.ch,sizeof(xy8.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",xy8.seqName,"");

   sprintf(lpattern,"%s%d",var,xy8.nRec);

   xy8.hasArray = hasarry(xy8.array, lpattern);

   int lix = arryindex(xy8.array);

   if (xy8.calc > 0) {
      var = getname0("",xy8.seqName,"");

      sprintf(xy8.pattern,"%s%d_%d",var,xy8.nRec,lix);

      if (xy8.hasArray == 1) {

         xy8 = MPchopper(xy8); 

         xy8.iSuper = iph + xy8.nelem%xy8.nphSuper;

      }
      xy8.t = gett(lix, lpattern);

   }

   return xy8;

}


//====================================

// Build Doubly Perfect Echo CPMG with an XXbar Phase Cycle

//====================================

// BDZ this one not in solidmpseqs

MPSEQ getdpecpmg(char *seqName, int iph, double p, double phint, int iRec, int calc) {
   char *var;
   MPSEQ dpecpmg = {};
   extern MPSEQ MPchopper(MPSEQ dpecpmg);
   
   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {
        printf("Error in getdpecpmg(). The type name %s is invalid!\n",seqName);
        psg_abort(1);
   }
   
   sprintf(dpecpmg.seqName,"%s",seqName);
   dpecpmg.calc = calc;
   dpecpmg.array = parsearry(dpecpmg.array);
   
// Obtain Phase Arguments
   dpecpmg.phAccum = p;
   dpecpmg.iSuper = iph;
   dpecpmg.nRec = iRec;
   dpecpmg.phInt = phint;
   
// Allocate Arrays
   int nphBase = 15;
   int nphSuper = 16;
   int npw = 15;
   int nph = 15;
   int nof = 1;
   int na = 1;
   int ng = 15;

   MPinitializer(&dpecpmg,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   dpecpmg.n90 = VNMRSN90;
   dpecpmg.n90m = VNMRSN90M;
   dpecpmg.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      dpecpmg.n90 = INOVAN90;
      dpecpmg.n90m = INOVAN90M;
      dpecpmg.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List
   dpecpmg.phBase[0] = 0.0;
   dpecpmg.phBase[1] = 0.0;
   dpecpmg.phBase[2] = 0.0;
   dpecpmg.phBase[3] = 0.0;
   dpecpmg.phBase[4] = 0.0;
   dpecpmg.phBase[5] = 0.0;
   dpecpmg.phBase[6] = 0.0;
   dpecpmg.phBase[7] = 0.0;
   dpecpmg.phBase[8] = 0.0;
   dpecpmg.phBase[9] = 0.0;
   dpecpmg.phBase[10] = 0.0;
   dpecpmg.phBase[11] = 0.0;
   dpecpmg.phBase[12] = 0.0;
   dpecpmg.phBase[13] = 0.0;
   dpecpmg.phBase[14] = 0.0;
   
// Set the Supercycle Phase List
   dpecpmg.phSuper[0] = 0.0;
   dpecpmg.phSuper[1] = 0.0;
   dpecpmg.phSuper[2] = 0.0;
   dpecpmg.phSuper[3] = 0.0;
   
   dpecpmg.phSuper[4] = 0.0;
   dpecpmg.phSuper[5] = 0.0;
   dpecpmg.phSuper[6] = 0.0;
   dpecpmg.phSuper[7] = 0.0;
   
   dpecpmg.phSuper[8] = 0.0;
   dpecpmg.phSuper[9] = 0.0;
   dpecpmg.phSuper[10] = 0.0;
   dpecpmg.phSuper[11] = 0.0;
   
   dpecpmg.phSuper[12] = 0.0;
   dpecpmg.phSuper[13] = 0.0;
   dpecpmg.phSuper[14] = 0.0;
   dpecpmg.phSuper[15] = 0.0;
   
// Get the tauXdpecpmg
	double tauXdpecpmg = getval("tauXdpecpmg");

// Set the Delay List
//pwXdpecpmg (180 degree pulse)
	var = getname0("pw",dpecpmg.seqName,"");
	double pwXdpecpmg = getval(var);


   dpecpmg.pw[0] = tauXdpecpmg/8.0-pwXdpecpmg/2.0;
   dpecpmg.pw[1] = pwXdpecpmg;
   dpecpmg.pw[2] = tauXdpecpmg/8.0-pwXdpecpmg/2.0;
   dpecpmg.pw[3] = pwXdpecpmg/2.0;
   dpecpmg.pw[4] = tauXdpecpmg/8.0-pwXdpecpmg/2.0;
   dpecpmg.pw[5] = pwXdpecpmg;
   dpecpmg.pw[6] = tauXdpecpmg/8.0-pwXdpecpmg;
   dpecpmg.pw[7] = pwXdpecpmg;
   dpecpmg.pw[8] = tauXdpecpmg/8.0-pwXdpecpmg;
   dpecpmg.pw[9] = pwXdpecpmg;
   dpecpmg.pw[10] = tauXdpecpmg/8.0-pwXdpecpmg/2.0;
   dpecpmg.pw[11] = pwXdpecpmg/2.0;
   dpecpmg.pw[12] = tauXdpecpmg/8.0-pwXdpecpmg/2.0;
   dpecpmg.pw[13] = pwXdpecpmg;
   dpecpmg.pw[14] = tauXdpecpmg/8.0-pwXdpecpmg/2.0;

   dpecpmg.array = disarry(var, dpecpmg.array);
// Set the Offset List

//ofXdpecpmg
   var = getname0("of",dpecpmg.seqName,""); 
   dpecpmg.of[0] = getval(var);
   dpecpmg.array = disarry(var, dpecpmg.array);

// Set the Amplitude List
   dpecpmg.aBase[0] = 1023.0;

// Set the Base Gate List
   dpecpmg.gateBase[0] = 0.0;
   dpecpmg.gateBase[1] = 1.0;
   dpecpmg.gateBase[2] = 0.0;
   dpecpmg.gateBase[3] = 1.0;
   dpecpmg.gateBase[4] = 0.0;
   dpecpmg.gateBase[5] = 1.0;
   dpecpmg.gateBase[6] = 0.0;
   dpecpmg.gateBase[7] = 1.0;
   dpecpmg.gateBase[8] = 0.0;
   dpecpmg.gateBase[9] = 1.0;
   dpecpmg.gateBase[10] = 0.0;
   dpecpmg.gateBase[11] = 1.0;
   dpecpmg.gateBase[12] = 0.0;
   dpecpmg.gateBase[13] = 1.0;
   dpecpmg.gateBase[14] = 0.0;
   
// Set the Overall Amplitude, Elements and Channel

//aXdpecpmg
   var = getname0("a",dpecpmg.seqName,"");
   dpecpmg.a = getval(var);

//qXdpecpmg
   var = getname0("q",dpecpmg.seqName,"");
   dpecpmg.nelem = (int) getval(var); 
   dpecpmg.array = disarry(var, dpecpmg.array);

//chXdpecpmg
   var = getname0("ch",dpecpmg.seqName,""); 
   Getstr(var,dpecpmg.ch,sizeof(dpecpmg.ch));

// Create the Shapefile Name
   char lpattern[NPATTERN];
   var = getname0("",dpecpmg.seqName,"");
   sprintf(lpattern,"%s%d",var,dpecpmg.nRec);
   dpecpmg.hasArray = hasarry(dpecpmg.array, lpattern);
   int lix = arryindex(dpecpmg.array);
   if (dpecpmg.calc > 0) {
      var = getname0("",dpecpmg.seqName,"");
      sprintf(dpecpmg.pattern,"%s%d_%d",var,dpecpmg.nRec,lix);
      if (dpecpmg.hasArray == 1) {
         dpecpmg = MPchopper(dpecpmg); 
         dpecpmg.iSuper = iph + dpecpmg.nelem%dpecpmg.nphSuper;
      }
      dpecpmg.t = gett(lix, lpattern);
   }
   return dpecpmg;
}


//====================================

// Build REDOR (1 PI pulse per half-rotor period) with an XY8 Phase Cycle

//====================================

// BDZ this one not in solidmpseqs

MPSEQ getredor(char *seqName, int iph, double p, double phint, int iRec, int calc) {
    char *var;
    MPSEQ redor = {};
    extern MPSEQ MPchopper(MPSEQ redor);

    if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {
        printf("Error in getredor(). The type name %s is invalid!\n",seqName);
        psg_abort(1);
    }

    sprintf(redor.seqName,"%s",seqName);
    redor.calc = calc;
    redor.array = parsearry(redor.array);

    // Obtain Phase Arguments
    redor.phAccum = p;
    redor.iSuper = iph;
    redor.nRec = iRec;
    redor.phInt = phint;

    // Allocate Arrays
    int nphBase = 24;
    int nphSuper = 8;
    int npw = 24;
    int nph = 24;
    int nof = 1;
    int na = 1;
    int ng = 24;

    MPinitializer(&redor,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   redor.n90 = VNMRSN90;
   redor.n90m = VNMRSN90M;
   redor.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      redor.n90 = INOVAN90;
      redor.n90m = INOVAN90M;
      redor.trap = INOVATRAP;
   }

#endif

    // Set the Base Phase List (XY-8 element)
    redor.phBase[0] = 0.0;
    redor.phBase[1] = 0.0;
    redor.phBase[2] = 0.0;
    redor.phBase[3] = 90.0;
    redor.phBase[4] = 90.0;
    redor.phBase[5] = 90.0;
    redor.phBase[6] = 0.0;
    redor.phBase[7] = 0.0;
    redor.phBase[8] = 0.0;
    redor.phBase[9] = 90.0;
    redor.phBase[10] = 90.0;
    redor.phBase[11] = 90.0;
    redor.phBase[12] = 90.0;
    redor.phBase[13] = 90.0;
    redor.phBase[14] = 90.0;
    redor.phBase[15] = 0.0;
    redor.phBase[16] = 0.0;
    redor.phBase[17] = 0.0;
    redor.phBase[18] = 90.0;
    redor.phBase[19] = 90.0;
    redor.phBase[20] = 90.0;
    redor.phBase[21] =  0.0;
    redor.phBase[22] =  0.0;
    redor.phBase[23] =  0.0;

    // Set the Supercycle Phase List for XY-16 and XY-32
    redor.phSuper[0] = 0.0;
    redor.phSuper[1] = 180.0;
    redor.phSuper[2] = 180.0;
    redor.phSuper[3] = 0.0;
    redor.phSuper[4] = 0.0;
    redor.phSuper[5] = 0.0;
    redor.phSuper[6] = 180.0;
    redor.phSuper[7] = 180.0;

    // BDZ suspicious looking code
    // in most code 12.5e-9 is replaced with DTCK. We seem to be doing
    //   10*DTCK*redor.n90.
    // notice those are different by a factor of 10. did we copy code badly?

    // Get the Standard System Parameter "srate" and Set tauR
    double tauR = 2.0*roundoff(1.0/(2.0*getval("srate")),125e-9*redor.n90);

    // Set the Delay List
    //pwXredor (180 degree pulse)
    var = getname0("pw",redor.seqName,"");
    double lpw = 2.0*roundoff(getval(var)/2.0,DTCK*redor.n90);
    redor.t = (tauR/4.0-lpw/2.0)*16 + lpw*8;

    redor.pw[0] = tauR/4.0 - lpw/2.0;
    redor.pw[1] = lpw;
    redor.pw[2] = tauR/4.0 - lpw/2.0;
    redor.pw[3] = tauR/4.0 - lpw/2.0;
    redor.pw[4] = lpw;
    redor.pw[5] = tauR/4.0 - lpw/2.0;
    redor.pw[6] = tauR/4.0 - lpw/2.0;
    redor.pw[7] = lpw;
    redor.pw[8] = tauR/4.0 - lpw/2.0;
    redor.pw[9] = tauR/4.0 - lpw/2.0;
    redor.pw[10] = lpw;
    redor.pw[11] = tauR/4.0 - lpw/2.0;
    redor.pw[12] = tauR/4.0 - lpw/2.0;
    redor.pw[13] = lpw;
    redor.pw[14] = tauR/4.0 - lpw/2.0;
    redor.pw[15] = tauR/4.0 - lpw/2.0;
    redor.pw[16] = lpw;
    redor.pw[17] = tauR/4.0 - lpw/2.0;
    redor.pw[18] = tauR/4.0 - lpw/2.0;
    redor.pw[19] = lpw;
    redor.pw[20] = tauR/4.0 - lpw/2.0;
    redor.pw[21] = tauR/4.0 - lpw/2.0;
    redor.pw[22] = lpw;
    redor.pw[23] = tauR/4.0 - lpw/2.0;

    redor.array = disarry(var, redor.array);

    // Set the Offset List
    //ofXredor
    var = getname0("of",redor.seqName,""); 
    redor.of[0] = getval(var);
    redor.array = disarry(var, redor.array);

    // Set the Amplitude List
    redor.aBase[0] = 1023.0;

    // Set the Base Gate List
    redor.gateBase[0] = 0.0;
    redor.gateBase[1] = 1.0;
    redor.gateBase[2] = 0.0;
    redor.gateBase[3] = 0.0;
    redor.gateBase[4] = 1.0;
    redor.gateBase[5] = 0.0;
    redor.gateBase[6] = 0.0;
    redor.gateBase[7] = 1.0;
    redor.gateBase[8] = 0.0;
    redor.gateBase[9] = 0.0;
    redor.gateBase[10] = 1.0;
    redor.gateBase[11] = 0.0;
    redor.gateBase[12] = 0.0;
    redor.gateBase[13] = 1.0;
    redor.gateBase[14] = 0.0;
    redor.gateBase[15] = 0.0;
    redor.gateBase[16] = 1.0;
    redor.gateBase[17] = 0.0;
    redor.gateBase[18] = 0.0;
    redor.gateBase[19] = 1.0;
    redor.gateBase[20] = 0.0;
    redor.gateBase[21] = 0.0;
    redor.gateBase[22] = 1.0;
    redor.gateBase[23] = 0.0;

    // Set the Overall Amplitude, Elements and Channel

    //aXredor
    var = getname0("a",redor.seqName,"");
    redor.a = getval(var);

    //qXredor
    var = getname0("q",redor.seqName,"");
    redor.nelem = (int) getval(var); 
    redor.array = disarry(var, redor.array);

    //chXredor
    var = getname0("ch",redor.seqName,""); 
    Getstr(var,redor.ch,sizeof(redor.ch));

    // Create the Shapefile Name
    char lpattern[NPATTERN];
    var = getname0("",redor.seqName,"");

    sprintf(lpattern,"%s%d",var,redor.nRec);
    redor.hasArray = hasarry(redor.array, lpattern);

    int lix = arryindex(redor.array);
    if (redor.calc > 0) {
        var = getname0("",redor.seqName,"");
        sprintf(redor.pattern,"%s%d_%d",var,redor.nRec,lix);
        if (redor.hasArray == 1) {
            redor = MPchopper(redor); 
            redor.iSuper = iph + redor.nelem%redor.nphSuper;
        }
        redor.t = gett(lix, lpattern);
    }

    return redor;
}



//==============================================

// Build Shift Evolution Assisted POSTC7 - SEAC7

//==============================================



MPSEQ getseac7(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ seac7 = {};

   char *var;

   extern MPSEQ MPchopper(MPSEQ seac7);

   int i,j,k;

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getseac7(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(seac7.seqName,"%s",seqName);

   seac7.calc = calc;

   seac7.array = parsearry(seac7.array);



// Obtain Phase Arguments



   seac7.phAccum = p;

   seac7.iSuper = iph;

   seac7.nRec = iRec;

   seac7.phInt = phint;



// Get the Values of "m" and "n" - mXseac7, nXseac7



   var = getname0("m",seac7.seqName,"");

   int m = getval(var);

   seac7.array = disarry(var, seac7.array);



   var = getname0("n",seac7.seqName,"");

   int n = getval(var);

   seac7.array = disarry(var, seac7.array);



   int nlphBase = 3;

   int nlphElem = 7*m/2;



// Allocate Arrays



   int nphBase = nlphBase*nlphElem + 1;

   int nphSuper = 1;

   int npw = nphBase;

   int nph = nphBase;

   int nof = 1;

   int na = 1;

#ifdef MATCH_MASTER
   int ng = nphBase;  // maybe was a bugfix?
#else
   int ng = 1;
#endif

   MPinitializer(&seac7,npw,nph,nof,na,ng,nphBase,nphSuper);


#ifdef MATCH_MASTER

// Set the Step Sizes

   seac7.n90 = VNMRSN90;
   seac7.n90m = VNMRSN90M;
   seac7.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      seac7.n90 = INOVAN90;
      seac7.n90m = INOVAN90M;
      seac7.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   double lphBase[3];

   lphBase[0] = 0.0;

   lphBase[1] = 180.0;

   lphBase[2] = 0.0;



   double dph = 360.0/7.0;

   seac7.nphBase = nlphBase*nlphElem + 1;

   k = 0;

   for( i = 0; i<nlphElem; i++) {

      for( j = 0; j<nlphBase; j++) {

         seac7.phBase[k] = lphBase[j] + i*dph;

         k = k + 1;

      }

   }

   seac7.phBase[nlphBase*nlphElem] = 0.0;



// Set the Supercycle Phase List



   seac7.phSuper[0] = 0.0;



// Set the Delay List



// pwXseac7



   var = getname0("pw",seac7.seqName,"");

   double pwX360 = roundoff(getval(var),4.0*DTCK*seac7.n90);

   seac7.array = disarry(var, seac7.array);



   double lpw[3];

   lpw[0] = pwX360/4.0;

   lpw[1] = pwX360;

   lpw[2] = 3.0*pwX360/4.0;



   k = 0; 

   for( i = 0; i<nlphElem; i++) {



      for( j = 0; j<nlphBase; j++) {

         seac7.pw[k] = lpw[j];

         k =  k + 1;

      }

   }

   seac7.pw[nlphBase*nlphElem] = 14*pwX360*n/2.0;



// Set the Offset List



//ofXseac7



   var = getname0("of",seac7.seqName,"");

   seac7.of[0] = getval(var);

   seac7.array = disarry(var, seac7.array);



// Set the Amplitude List



   seac7.aBase[0] = 1023.0;



// Set the Base Gate List



   k = 0;

   for( i = 0; i<nlphElem; i++) {

      for( j = 0; j<nlphBase; j++) {

         seac7.gateBase[k] = 1.0;

         k = k + 1;

      }

   }

   seac7.gateBase[nlphBase*nlphElem] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXseac7



   var = getname0("a",seac7.seqName,"");

   seac7.a = getval(var);



//qXc7 - nummber of reps "N" as seac7.nelem



   var = getname0("q",seac7.seqName,"");

   seac7.nelem = getval(var);

   seac7.array = disarry(var, seac7.array);



//chXseac7


   var = getname0("ch",seac7.seqName,"");

   Getstr(var,seac7.ch,sizeof(seac7.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",seac7.seqName,"");

   sprintf(lpattern,"%s%d",var,seac7.nRec);

   seac7.hasArray = hasarry(seac7.array, lpattern);

   int lix = arryindex(seac7.array);

   if (seac7.calc > 0) {
      var = getname0("",seac7.seqName,"");

      sprintf(seac7.pattern,"%s%d_%d",var,seac7.nRec,lix);

      if (seac7.hasArray == 1) {

         seac7 = MPchopper(seac7); 

         seac7.iSuper = iph + seac7.nelem%seac7.nphSuper;

      }
      seac7.t = gett(lix, lpattern);

   }

   return seac7;

}



//====================================================

// Build SC14 - C14 - sub4 - super5 with 2*pisub0 Base

//====================================================



MPSEQ getsc14(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ sc14 = {};

   extern MPSEQ MPchopper(MPSEQ sc14);

   char *var;

   int i;

    if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getsc14(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

    }

   sprintf(sc14.seqName,"%s",seqName);

   sc14.calc = calc;

   sc14.array = parsearry(sc14.array);



// Obtain Phase Arguments



   sc14.phAccum = p;

   sc14.iSuper = iph;

   sc14.nRec = iRec;

   sc14.phInt = phint;



// Allocate Arrays



   int nphBase = 1;

   int nphSuper = 14;

   int npw = 1;

   int nph = 1;

   int nof = 1;

   int na = 1; 

   int ng = 1; 

   MPinitializer(&sc14,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   sc14.n90 = VNMRSN90;
   sc14.n90m = VNMRSN90M;
   sc14.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      sc14.n90 = INOVAN90;
      sc14.n90m = INOVAN90M;
      sc14.trap = INOVATRAP;
   }

#endif


// Set the Base Phase List



   sc14.phBase[0] = 0.0;



// Set the Supercycle Phase List



   double dph = 5.0*360.0/14.0;

   sc14.nphSuper = 14;

   sc14.phSuper[0] = 0;

   for( i = 1; i< sc14.nphSuper; i++)

      sc14.phSuper[i] = sc14.phSuper[i-1] + dph;



// Set the Delay List



//pwXsc14



   var = getname0("pw",sc14.seqName,"");

   sc14.pw[0] = roundoff(getval(var),DTCK*sc14.n90);

   sc14.array = disarry(var, sc14.array);



// Set the Offset List



//ofXsc14



   var = getname0("of",sc14.seqName,"");

   sc14.of[0] = getval(var);

   sc14.array = disarry(var, sc14.array);



// Set the Overall Amplitude, Elements and Channel



//aXsc14



   var = getname0("a",sc14.seqName,"");

   sc14.a = getval(var);



//qXsc14



   var = getname0("q",sc14.seqName,"");

   sc14.nelem = getval(var);

   sc14.array = disarry(var, sc14.array);



//chXsc14


   var = getname0("ch",sc14.seqName,"");

   Getstr(var,sc14.ch,sizeof(sc14.ch)); 



// Create shapefile name


   char lpattern[NPATTERN];
   var = getname0("",sc14.seqName,"");

   sprintf(lpattern,"%s%d",var,sc14.nRec);

   sc14.hasArray = hasarry(sc14.array, sc14.pattern);

   int lix = arryindex(sc14.array);

   if (sc14.calc > 0) {
      var = getname0("",sc14.seqName,"");

      sprintf(sc14.pattern,"%s%d_%d",var,sc14.nRec,lix);

      if (sc14.hasArray == 1) {

         sc14 = MPchopper(sc14); 

         sc14.iSuper = iph + sc14.nelem%sc14.nphSuper;

      }
      sc14.t = gett(lix, lpattern);

   }

   return sc14;

}



//===========================================

// Build PiThird RFDR with an XY4 Phase Cycle

//===========================================



MPSEQ getptrfdr(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ pt = {};

   extern MPSEQ MPchopper(MPSEQ pt);

   int index;

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getptrfdr(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(pt.seqName,"%s",seqName);

   pt.calc = calc;

   pt.array = parsearry(pt.array);

   

// Obtain Phase Arguments



   pt.phAccum = p;

   pt.iSuper = iph;

   pt.nRec = iRec;

   pt.phInt = phint;



// Allocate Arrays



   int nphBase = 23;

   int nphSuper = 1;

   int npw = 23;

   int nph = 23;

   int nof = 1;

   int na = 1; 

#ifdef MATCH_MASTER
   int ng = 23;  // maybe was a bugfix?
#else
   int ng = 1;
#endif

   MPinitializer(&pt,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   pt.n90 = VNMRSN90;
   pt.n90m = VNMRSN90M;
   pt.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pt.n90 = INOVAN90;
      pt.n90m = INOVAN90M;
      pt.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   int lptphBase[23] = {0,1,1,0,0,1,1,0,0,1,1,0,

                        0,1,1,0,0,1,1,0,0,1,1};



   for (index = 0; index < 23; index++) {

      pt.phBase[index] = lptphBase[index]*90.0;

   }



// Set the Supercycle Phase List



   pt.phSuper[0] = 0.0;



// Set the Delay List



//pwXptrfdr (180 degree pulse)



   var = getname0("pw",pt.seqName,"");

   double lpw = roundoff(getval(var),DTCK*pt.n90);



   pt.array = disarry(var, pt.array);



   int lptpw[23] = {1,2,1,2,1,2,1,3,1,2,1,2,

                    1,2,1,3,1,2,1,2,1,2,1};



   for (index = 0; index < 23; index++) {

      pt.pw[index] = lptpw[index]*lpw;

   }



// Set the Offset List



//ofXptrfdr



   var = getname0("of",pt.seqName,"");

   pt.of[0] = getval(var);

   pt.array = disarry(var, pt.array);



// Set the Amplitude List



   pt.aBase[0] = 1023.0;



// Set the Base Gate List



   int lptphGate[23] = {1,0,1,0,1,0,1,0,1,0,1,0,

                        1,0,1,0,1,0,1,0,1,0,1};



   for (index = 0; index < 23; index++) {

      pt.gateBase[index] = lptphGate[index]*1.0;

   }



// Set the Overall Amplitude, Elements and Channel 



//aXptrfdr



   var = getname0("a",pt.seqName,"");

   pt.a = getval(var);



//qXptrfdr



   var = getname0("q",pt.seqName,"");

   pt.nelem = (int) getval(var);

   pt.array = disarry(var, pt.array);



//chXptrfdr


   var = getname0("ch",pt.seqName,"");

   Getstr(var,pt.ch,sizeof(pt.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",pt.seqName,"");

   sprintf(lpattern,"%s%d",var,pt.nRec);

   pt.hasArray = hasarry(pt.array, lpattern);

   int lix = arryindex(pt.array);

   if (pt.calc > 0) {
      var = getname0("",pt.seqName,"");

      sprintf(pt.pattern,"%s%d_%d",var,pt.nRec,lix);

      if (pt.hasArray == 1) {

         pt = MPchopper(pt); 

         pt.iSuper = iph + pt.nelem%pt.nphSuper;

      }
      pt.t = gett(lix, lpattern);

   }

   return pt;

}



//==========================================================

// Build Finite Pules (1/3)TauR RFDR with an XY4 Phase Cycle

//==========================================================



MPSEQ getfprfdr(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ fp = {};

   extern MPSEQ MPchopper(MPSEQ fp);

   int index;

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getfprfdr(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(fp.seqName,"%s",seqName);

   fp.calc = calc;

   fp.array = parsearry(fp.array);



// Obtain Phase Arguments



   fp.phAccum = p;

   fp.iSuper = iph;

   fp.nRec = iRec;

   fp.phInt = phint;



// Allocate Arrays



   int nphBase = 9;

   int nphSuper = 1;

   int npw = 9;

   int nph = 9;

   int nof = 1;

   int na = 1; 

#ifdef MATCH_MASTER
   int ng = 9;  // maybe was a bugfix?
#else
   int ng = 1;
#endif

   MPinitializer(&fp,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   fp.n90 = VNMRSN90;
   fp.n90m = VNMRSN90M;
   fp.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      fp.n90 = INOVAN90;
      fp.n90m = INOVAN90M;
      fp.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   int lfpphBase[9] = {0,0,1,1,0,0,1,1,0};



   for (index = 0; index < 9; index++) {

      fp.phBase[index] = lfpphBase[index]*90.0;

   }



// Set the Supercycle Phase List



   fp.phSuper[0] = 0.0;



// Set the Delay List



//pwXfprfdr (180 degree pulse)



   var = getname0("pw",fp.seqName,"");

   double lpw = roundoff(getval(var),DTCK*fp.n90);



   fp.array = disarry(var, fp.array);



   int lfppw[9] = {1,1,2,1,2,1,2,1,1};



   for (index = 0; index < 9; index++) {

      fp.pw[index] = lfppw[index]*lpw;

   }



// Set the Offset List



//ofXfprdfr



   var = getname0("of",fp.seqName,"");

   fp.of[0] = getval(var);

   fp.array = disarry(var, fp.array);



// Set the amplitude List



   fp.aBase[0] = 1023.0;

   

//Set the Base Gate List



   int lfpphGate[9] = {0,1,0,1,0,1,0,1,0};



   for (index = 0; index < 9; index++) {

      fp.gateBase[index] = lfpphGate[index]*1.0;

   }



// Set the Overall Amplitude, Elements and Channel



//aXfprfdr



   var = getname0("a",fp.seqName,"");

   fp.a = getval(var);



//qXfprfdr



   var = getname0("q",fp.seqName,"");

   fp.nelem = (int) getval(var);

   fp.array = disarry(var, fp.array);



//chXfprfdr


   var = getname0("ch",fp.seqName,"");

   Getstr(var,fp.ch,sizeof(fp.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",fp.seqName,"");

   sprintf(lpattern,"%s%d",var,fp.nRec);

   fp.hasArray = hasarry(fp.array, lpattern);

   int lix = arryindex(fp.array);

   if (fp.calc > 0) {
      var = getname0("",fp.seqName,"");

      sprintf(fp.pattern,"%s%d_%d",var,fp.nRec,lix);

      if (fp.hasArray == 1) {

         fp = MPchopper(fp);

         fp.iSuper = iph + fp.nelem%fp.nphSuper;

      }
      fp.t = gett(lix, lpattern);

   }

   return fp;

}



//======================================================================================

// Build (TPPM - 2 or SPINAL - 16, 32, 64 as an MPSEQ (note that getspinal is DSEQ type)

// Notation follows Brauinger et.al, Monatshefte fur Chemie 133, 1549-1554 (2002) 

// with beta = 2.0*alfa. 

//======================================================================================



MPSEQ getspnl(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ spnl = {};

   extern MPSEQ MPchopper(MPSEQ spnl);

   char *var;

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getspnl(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(spnl.seqName,"%s",seqName);

   spnl.calc = calc;

   spnl.array = parsearry(spnl.array); 

// Obtain Phase Arguments


   spnl.phAccum = p;

   spnl.iSuper = iph;

   spnl.nRec = iRec;

   spnl.phInt = phint;

// Allocate Arrays



   int nphBase = 1;

   int nphSuper = 64;

   int npw = 1;

   int nph = 1;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&spnl,npw,nph,nof,na,ng,nphBase,nphSuper);

// TODO BDZ: missing INOVA and VNMRS code: true on OVJ master too

// Set the Base Phase List


   spnl.phBase[0] = 0.0;

// Set the Supercycle Phase List

//ph1Xspnl



   var = getname0("ph1",spnl.seqName,"");

   double phi1 = getval(var);

   spnl.array = disarry(var, spnl.array);



//ph2Xspnl



   var = getname0("ph2",spnl.seqName,"");

   double phi2 = getval(var);

   spnl.array = disarry(var, spnl.array);

   double phase1[64] = {1.0,-1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0,

                        -1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0,1.0,

                        -1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0,1.0,

                        1.0,-1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0,

                        -1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0,1.0,

                        1.0,-1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0,

                        1.0,-1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0,

                        -1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0,1.0};



   double phase2[64] = {0.0,0.0,1.0,-1.0,2.0,-2.0,1.0,-1.0,

                        0.0,0.0,-1.0,1.0,-2.0,2.0,-1.0,1.0,

                        0.0,0.0,-1.0,1.0,-2.0,2.0,-1.0,1.0,

                        0.0,0.0,1.0,-1.0,2.0,-2.0,1.0,-1.0,

                        0.0,0.0,-1.0,1.0,-2.0,2.0,-1.0,1.0,

                        0.0,0.0,1.0,-1.0,2.0,-2.0,1.0,-1.0,

                        0.0,0.0,1.0,-1.0,2.0,-2.0,1.0,-1.0,

                        0.0,0.0,-1.0,1.0,-2.0,2.0,-1.0,1.0};


//nXspnl - n is the repeat number 2, 16, 32, 64



   var = getname0("n",spnl.seqName,"");

   int m = getval(var);

   int n = 2;

   if (m<3) n = 2;

   else if (m<17) n = 16;

   else if (m<33) n = 32;

   else n = 64;


   int i;

   spnl.nphSuper = n;

   for (i = 0; i < n; i++) {

      spnl.phSuper[i] = phi1*phase1[i] + phi2*phase2[i]; 

   }

// Set the Delay List


//pwXspnl



   var = getname0("pw",spnl.seqName,"");

   spnl.pw[1] = getval(var); 

   spnl.array = disarry(var, spnl.array);

// Set the Offset List


//ofXspnl



   var = getname0("of",spnl.seqName,"");

   spnl.of[0] = getval(var);

   spnl.array = disarry(var, spnl.array);


// Set the Amp and Gate Lists



   spnl.aBase[0] = 1023.0;

   spnl.gateBase[0] = 0.0; 

// Set the Overall Amplitude, Elements and Channel


//aXspnl



   var = getname0("a",spnl.seqName,"");       

   spnl.a = getval(var);


//qXspnl - is the number of pulses written in the waveform



   var = getname0("q",spnl.seqName,"");

   spnl.nelem = getval(var);

   spnl.array = disarry(var, spnl.array);



//chXspnl


   var = getname0("ch",spnl.seqName,"");

   Getstr(var,spnl.ch,sizeof(spnl.ch));



// Create the Shapefile Name

   char lpattern[NPATTERN];
   var = getname0("",spnl.seqName,"");

   sprintf(lpattern,"%s%d",var,spnl.nRec);

   spnl.hasArray = hasarry(spnl.array, lpattern);

   int lix = arryindex(spnl.array);

   if (spnl.calc > 0) {
      var = getname0("",spnl.seqName,"");

      sprintf(spnl.pattern,"%s%d_%d",var,spnl.nRec,lix);

      if (spnl.hasArray == 1) {

         spnl = MPchopper(spnl); 

         spnl.iSuper = iph + spnl.nelem%spnl.nphSuper;

      }
      spnl.t = gett(lix, lpattern);

   }

   return spnl;

}



//=======================

// Build R18-sub1-sup7

//=======================



MPSEQ getr1817(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ r = {};

   extern MPSEQ MPchopper(MPSEQ r);

   char *var;

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getr1817(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(r.seqName,"%s",seqName);

   r.calc = calc;

   r.array = parsearry(r.array); 

// Obtain Phase Arguments


   r.phAccum = p;

   r.iSuper = iph;

   r.nRec = iRec;

   r.phInt = phint;

// Allocate Arrays



   int nphBase = 1;

   int nphSuper = 2;

   int npw = 1;

   int nph = 1;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&r,npw,nph,nof,na,ng,nphBase,nphSuper);

// TODO BDZ: missing INOVA and VNMRS code: also true on OVJ master


// Set the Base Phase List 


   r.phBase[0] = 0.0;

// Set the Supercycle Phase List



   r.phSuper[0] = 70.0;

   r.phSuper[1] = 290.0;


// Set the Delay List

//pwXr1817



   var = getname0("pw",r.seqName,"");

   double lpw = getval(var); 
   r.array = disarry(var, r.array);


   lpw = 2.0*roundoff(lpw/2.0,DTCK*r.n90);

   r.pw[0] = lpw/2.0;


// Set the Offset List

//ofXr1817



   var = getname0("of",r.seqName,"");

   r.of[0] = getval(var);

   r.array = disarry(var, r.array);

// Set the Amp and Gate Lists



   r.aBase[0] = 1023.0;

   r.gateBase[0] = 0.0;

// Set the Overall Amplitude, Elements and Channel

//aXr1817



   var = getname0("a",r.seqName,"");

   r.a = getval(var);



//qXr1817



   var = getname0("q",r.seqName,"");

   r.nelem = getval(var);

   r.array = disarry(var, r.array);



//chXr1817


   var = getname0("ch",r.seqName,"");

   Getstr(var,r.ch,sizeof(r.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",r.seqName,"");

   sprintf(lpattern,"%s%d",var,r.nRec);

   r.hasArray = hasarry(r.array, lpattern);

   int lix = arryindex(r.array);

   if (r.calc > 0) {
      var = getname0("",r.seqName,"");

      sprintf(r.pattern,"%s%d_%d",var,r.nRec,lix);

      if (r.hasArray == 1) {

         r = MPchopper(r); 

         r.iSuper = iph + r.nelem%r.nphSuper;

      }
      r.t = gett(lix, lpattern);

   }

   return r;

}



//=======================

// Build TMREV

//=======================



MPSEQ gettmrev5(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ tm = {};

   extern MPSEQ MPchopper(MPSEQ tm);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in gettmrev5(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(tm.seqName,"%s",seqName);

   tm.calc = calc;

   tm.array = parsearry(tm.array); 

// Obtain Phase Arguments


   tm.phAccum = p;

   tm.iSuper = iph;

   tm.nRec = iRec;

   tm.phInt = phint;


// Allocate Arrays



   int nphBase = 12;

   int nphSuper = 5;

   int npw = 1;

   int nph = 12;

   int nof = 1;

   int na = 1;

   int ng = 12;

   MPinitializer(&tm,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   tm.n90 = VNMRSN90;
   tm.n90m = VNMRSN90M;
   tm.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      tm.n90 = INOVAN90;
      tm.n90m = INOVAN90M;
      tm.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List


   tm.phBase[0] = 270.0;

   tm.phBase[1] = 270.0;

   tm.phBase[2] = 270.0;

   tm.phBase[3] =  0.0;

   tm.phBase[4] =  0.0;

   tm.phBase[5] =  0.0;

   tm.phBase[6] = 180.0;

   tm.phBase[7] = 180.0;

   tm.phBase[8] = 180.0;

   tm.phBase[9] = 90.0;

   tm.phBase[10] = 90.0;

   tm.phBase[11] = 90.0;

// Set the Supercycle Phase List


   int i = 0;

   for (i = 0; i < 5; i++) {

#ifdef MATCH_MASTER

     tm.phSuper[i] = roundoff(360.0/5, 360.0/(PSD*8192));  // maybe was a bugfix?

#else

     tm.phSuper[i] = roundoff(360.0/5, 360.0/8192);

#endif

   }

// Set the Delay List


//pwXtmrev (360 degree pulse)



   var = getname0("pw",tm.seqName,"");

   double lpw = getval(var);
   tm.array = disarry(var, tm.array);


   lpw = 4.0*roundoff(lpw/4.0,DTCK*tm.n90);

   tm.pw[0] = lpw; 

// Set the Offset List



//ofXtmrev



   var = getname0("of",tm.seqName,"");

   tm.of[0] = getval(var);

   tm.array = disarry(var, tm.array);

// Set the Gate List

 

   tm.gateBase[0] = 1.0;

   tm.gateBase[1] = 0.0;

   tm.gateBase[2] = 1.0;

   tm.gateBase[3] = 1.0;

   tm.gateBase[4] = 0.0;

   tm.gateBase[5] = 1.0;

   tm.gateBase[6] = 1.0;

   tm.gateBase[7] = 0.0;

   tm.gateBase[8] = 1.0;

   tm.gateBase[9] = 1.0;

   tm.gateBase[10] = 0.0;

   tm.gateBase[11] = 1.0;

// Set the Amplitude List 

   tm.aBase[0] = 1023.0;

// Set the Overall Amplitude, Elements and Channel

//aXtmrev



   var = getname0("a",tm.seqName,"");

   tm.a = getval(var);


//qXtmrev



   var = getname0("q",tm.seqName,"");

   tm.nelem = (int) getval(var);

   tm.array = disarry(var, tm.array);


//chXtmrev


   var = getname0("ch",tm.seqName,"");

   Getstr(var,tm.ch,sizeof(tm.ch));



//write the shape file


   char lpattern[NPATTERN];
   var = getname0("",tm.seqName,"");

   sprintf(lpattern,"%s%d",var,tm.nRec);

   tm.hasArray = hasarry(tm.array, lpattern);

   int lix = arryindex(tm.array);

   if (tm.calc > 0) {
      var = getname0("",tm.seqName,"");

      sprintf(tm.pattern,"%s%d_%d",var,tm.nRec,lix);

      if (tm.hasArray == 1) {

         tm = MPchopper(tm); 

         tm.iSuper = iph + tm.nelem%tm.nphSuper;

      }
      tm.t = gett(lix, lpattern);

   }

   return tm;

}



//=======================================

// Build Presat Windowless XY Decoupling

//=======================================



MPSEQ getpxy(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ pxy = {};

   extern MPSEQ MPchopper(MPSEQ pxy);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getpxy(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(pxy.seqName,"%s",seqName);

   pxy.calc = calc;

   pxy.array = parsearry(pxy.array);


// Obtain Phase Arguments

   pxy.phAccum = p;

   pxy.iSuper = iph;

   pxy.nRec = iRec;

   pxy.phInt = phint;

// Allocate Arrays



   int nphBase = 2;

   int nphSuper = 1;

   int npw = 2;

   int nph = 2;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&pxy,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   pxy.n90 = VNMRSN90;
   pxy.n90m = VNMRSN90M;
   pxy.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pxy.n90 = INOVAN90;
      pxy.n90m = INOVAN90M;
      pxy.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List


   pxy.phBase[0] = 0.0;

   pxy.phBase[1] = 90.0;

// Set the Supercycle Phase List



   pxy.phSuper[0] = 0.0;



// Set the Delay List



//pwXpxy



   var = getname0("pw",pxy.seqName,"");

   double lpw = roundoff(getval(var),DTCK*pxy.n90); 

   pxy.array = disarry(var, pxy.array);


   pxy.pw[0] = lpw;

   pxy.pw[1] = lpw;


// Set the Offset List


//ofXpxy



   var = getname0("of",pxy.seqName,"");

   pxy.of[0] = getval(var);

   pxy.array = disarry(var, pxy.array);


// Set the Amp and Gate Lists



   pxy.aBase[0] = 1023.0;

   pxy.gateBase[0] = 0.0; 

// Set the Overall Amplitude, Elements and Channel



//aXpxy



   var = getname0("a",pxy.seqName,"");

   pxy.a = getval(var);

//qXpxy



   var = getname0("q",pxy.seqName,"");

   pxy.nelem = (int) getval(var);

   pxy.array = disarry(var, pxy.array);


//chXpxy


   var = getname0("ch",pxy.seqName,"");

   Getstr(var,pxy.ch,sizeof(pxy.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",pxy.seqName,"");

   sprintf(lpattern,"%s%d",var,pxy.nRec);

   pxy.hasArray = hasarry(pxy.array, lpattern);

   int lix = arryindex(pxy.array);

   if (pxy.calc > 0) {
      var = getname0("",pxy.seqName,"");

      sprintf(pxy.pattern,"%s%d_%d",var,pxy.nRec,lix);

      if (pxy.hasArray == 1) {

         pxy = MPchopper(pxy);

         pxy.iSuper = iph + pxy.nelem%pxy.nphSuper;

      }
      pxy.t = gett(lix, lpattern);

   }

   return pxy;

}

//================

// Build SAMn

//================



MPSEQ getsamn(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ pm = {};

   int i;

   char *var;

   extern MPSEQ MPchopper(MPSEQ pm);



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

      printf("Error in getsamn(). The type name %s is invalid !\n",seqName);

      psg_abort(1);

   }

   sprintf(pm.seqName,"%s",seqName);

   pm.calc = calc;

   pm.array = parsearry(pm.array);



// Obtain Phase Arguments



   pm.phAccum = p;

   pm.iSuper = iph;

   pm.nRec = iRec;

   pm.phInt = phint;

#ifdef MATCH_MASTER

// Set the Step Sizes

   pm.n90 = VNMRSN90;
   pm.n90m = VNMRSN90M;
   pm.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pm.n90 = INOVAN90;
      pm.n90m = INOVAN90M;
      pm.trap = INOVATRAP;
   }

#else

// BDZ - old way - not master ovj - leaving for now

// Calculate the Number of Steps per Cycle and the Total Steps in taur

   if (PWRF_DELAY > 0.0) pm.n90 = 16; // CMR 11/17/23 change from 16 to 40; back to 16 2/16/24
   else pm.n90 = 8;  // CMR 11/17/23 change from 8 to 40; back to 8 2/16/24

#endif


//qHsam - RF cycles per Rotor Cycle


   var = getname0("q",pm.seqName,"");

   int cycles = getval(var);

   pm.array = disarry(var, pm.array);

//pwHsam - The Time for One Rotor Cycle

   var = getname0("pw",pm.seqName,"");
   double taur = getval(var);

#ifdef MATCH_MASTER

   // maybe was a bugfix?
   taur = 2.0*cycles*roundoff(taur/(2.0*cycles),DTCK*pm.n90*pm.n90m);
   int totalsteps = (int) roundoff(taur/(DTCK*pm.n90*pm.n90m),1);

#else

   taur = 2.0*cycles*roundoff(taur/(2.0*cycles),DTCK*pm.n90);

   int totalsteps = (int) roundoff(taur/(DTCK*pm.n90),1);

#endif


// Allocate Arrays



   int nphBase = totalsteps;

   int nphSuper = 1;

   int npw = totalsteps;

   int nph = totalsteps;

   int nof = 1;

   int na = totalsteps;

   int ng = 1;


   MPinitializer(&pm,npw,nph,nof,na,ng,nphBase,nphSuper);

   // NOTE BDZ: INOVA and VNMRS code not missing: see above

// Set the Base Phase and Amplitude and Duration Lists

   

   for (i = 0; i < totalsteps; i++) {

      double a = 1023.0*cos(2.0*3.14159265358979323846*(i + 0.5)*cycles/totalsteps);

      if (a < 0) {
         pm.aBase[i] = -a;

         pm.phBase[i] = 180.0;
      }
      else {

         pm.aBase[i] = a;

         pm.phBase[i] = 0.0;

      }
#ifdef MATCH_MASTER

      pm.pw[i] = DTCK*pm.n90*pm.n90m;  // maybe was a bugfix?

#else

      pm.pw[i] = DTCK*pm.n90;

#endif

   }



// Set the Supercycle Phase List



   pm.phSuper[0] = 0.0;



// Set the Offset List



//ofXsam



   var = getname0("of",pm.seqName,"");

   pm.of[0] = getval(var);

   pm.array = disarry(var, pm.array);



// Set the Overall Amplitude, Elements and Channel



//aXsam



   var = getname0("a",pm.seqName,"");

   pm.a = getval(var); 

   pm.array = disarry(var, pm.array);



//nXsam



   var = getname0("n",pm.seqName,"");

   pm.nelem = (int) getval(var);

   pm.array = disarry(var, pm.array);



//chXsam



   var = getname0("ch",pm.seqName,"");

   Getstr(var,pm.ch,sizeof(pm.ch));



// Create the Shapefile Name



   char lpattern[NPATTERN];

   var = getname0("",pm.seqName,"");

   sprintf(lpattern,"%s%d",var,pm.nRec);

   pm.hasArray = hasarry(pm.array, lpattern);

   int lix = arryindex(pm.array);

   if (pm.calc > 0) {

      var = getname0("",pm.seqName,"");

      sprintf(pm.pattern,"%s%d_%d",var,pm.nRec,lix);

      if (pm.hasArray == 1) {

         pm = MPchopper(pm);

         pm.iSuper = iph + pm.nelem%pm.nphSuper;

      }

      pm.t = gett(lix, lpattern);

   }

   return pm;

}


MPSEQ getpmlgxmx(char *seqName, int iph ,double p, double phint, int iRec, int calc)

{

   MPSEQ pm = {};

   int i;

   char *var;

   extern MPSEQ MPchopper(MPSEQ pm);



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

      printf("Error in getpmlgxmx(). The type name %s is invalid!\n",seqName);

      psg_abort(1);

   }

   sprintf(pm.seqName,"%s",seqName);

   pm.calc = calc;

   pm.array = parsearry(pm.array);



// Obtain Phase Arguments



   pm.phAccum = p;

   pm.iSuper = iph;

   pm.nRec = iRec;

   pm.phInt = phint;



// Determine nphBase form qXpmlg



//qXpmlg



   var = getname0("q",pm.seqName,"");

   int nphBase = (int) getval(var);

   pm.array = disarry(var, pm.array);



   double sign = 1;

   if (nphBase < 0.0) {

      nphBase = -nphBase;

      sign = - sign;

   }
   int nsteps = nphBase;


// Allocate Arrays



   nphBase = 2*nphBase;

   int nphSuper = 2;

   int npw = nphBase;

   int nph = nphBase;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&pm,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   pm.n90 = VNMRSN90;
   pm.n90m = VNMRSN90M;
   pm.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pm.n90 = INOVAN90;
      pm.n90m = INOVAN90M;
      pm.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   double obsstep = 360.0/8192;

   double delta = 360.0/(sqrt(3)*nsteps);

   for (i = 0; i < pm.nphBase/2; i++) {

      pm.phBase[i] = sign*(i*delta + delta/2.0);

      pm.phBase[i] = roundphase(pm.phBase[i],obsstep);

   }

   for (i = pm.nphBase/2; i < pm.nphBase; i++) {

      pm.phBase[i] = 180.0 + sign*((2*nsteps - i)*delta - delta/2.0);

      pm.phBase[i] = roundphase(pm.phBase[i],obsstep);

   }



// Set the Supercycle Phase List



   pm.phSuper[0] = 0.0;
   pm.phSuper[1] = 180.0;



// Set the Delay List



//pwXpmlg (360 pulse)



   var = getname0("pw",pm.seqName,"");

   double pw360 = getval(var);

   pw360 = roundoff(pw360,DTCK*pm.n90);

   pm.array = disarry(var, pm.array);



   double pwlast = 0.0;

   for (i = 0; i < pm.nphBase/2; i++) {

      pm.pw[i] = roundoff((i+1)*2.0*pw360/pm.nphBase,DTCK*pm.n90) - pwlast;

      pwlast = pwlast + pm.pw[i];

      pm.pw[i + nsteps] = pm.pw[i];

   }



// Set the Offset List



//ofXpmlg



   var = getname0("of",pm.seqName,"");

   pm.of[0] = getval(var);

   pm.array = disarry(var, pm.array);



// Set the Amp and Gate Lists



   pm.aBase[0] = 1023.0;

   pm.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXpmlg



   var = getname0("a",pm.seqName,"");

   pm.a = getval(var);



//nXpmlg 



   var = getname0("n",pm.seqName,"");

   pm.nelem = (int) getval(var);

   pm.array = disarry(var, pm.array);



//chXpmlg
   
   var = getname0("ch",pm.seqName,"");

   Getstr(var,pm.ch,sizeof(pm.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",pm.seqName,"");

   sprintf(lpattern,"%s%d",var,pm.nRec);

   pm.hasArray = hasarry(pm.array, lpattern);

   int lix = arryindex(pm.array);

   if (pm.calc > 0) {
      var = getname0("",pm.seqName,"");

      sprintf(pm.pattern,"%s%d_%d",var,pm.nRec,lix);

      if (pm.hasArray == 1) {

         pm = MPchopper(pm); 

         pm.iSuper = iph + pm.nelem%pm.nphSuper;

      }
      pm.t = gett(lix, lpattern);

   }

   return pm;

}

//=============

// Build SUPER

//=============



MPSEQ getsuper(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   char *var;

   MPSEQ super = {};

   extern MPSEQ MPchopper(MPSEQ super);



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getsuper(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(super.seqName,"%s",seqName);

   super.calc = calc;

   super.array = parsearry(super.array);



// Obtain Phase Arguments



   super.phAccum = p;

   super.iSuper = iph;

   super.nRec = iRec;

   super.phInt = phint;



// Allocate Arrays



   int nphBase = 480;

   int nphSuper = 1;

   int npw = 15;

   int nph = 480;

   int nof = 1;

   int na = 1;

   int ng = 15;

   MPinitializer(&super,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   super.n90 = VNMRSN90;
   super.n90m = VNMRSN90M;
   super.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      super.n90 = INOVAN90;
      super.n90m = INOVAN90M;
      super.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List

// The 8 phase cycles for SUPER, from S.-F. Liu etal JMR 
// 155, 15-28, 2002 and the Web site of K. Schmdt-Rohr, 
// were translated into a single phase list. The labels 
// 12-19 are the phase tables in the Avance sequence of 
// K. Schmidt-Rohr. To compare, count forward for each 
// label at each "^". When two phases are on the same line
// they will end up always equal. Note that phi[65]=phi[0]
// and restarts the cycle. The gate elements and sign are
// shown for reference. The internal cycle (0 to 14) spans two 
// rotor periods. 

   int phi[65] = {0,2,1,3,2,0,3,1,0,0,1,1,2,2,3,3,
                  2,1,2,3,0,3,0,1,0,1,2,1,2,3,0,3,
                  3,2,1,2,3,0,3,0,1,0,1,2,1,2,3,0,
                  3,0,2,1,3,2,0,3,1,0,0,1,1,2,2,3,
                  0};

   int i; 
   for (i = 0; i < 32; i++) {                         //Gate Tables

      super.phBase[15*i] = phi[2*i]*90.0;              //0   12^

      super.phBase[15*i+1] = phi[2*i]*90.0;            //1   12,13^

      super.phBase[15*i+2] = (phi[2*i] + 2)*90.0;      //1M  14^,15^

      super.phBase[15*i+3] = (phi[2*i] + 2)*90.0;      //0M  16^ 

      super.phBase[15*i+4] = (phi[2*i] + 2)*90.0;      //1M  16^,17^ 

      super.phBase[15*i+5] = phi[2*i]*90.0;            //1   18^,19^


      super.phBase[15*i+6] = phi[2*i + 1]*90.0;        //0   12^

      super.phBase[15*i+7] = phi[2*i + 1]*90.0;        //1   12 

      super.phBase[15*i+8] = (phi[2*i + 1] + 2)*90.0;  //1M  13^,14^ 

      super.phBase[15*i+9] = phi[2*i + 1]*90.0;        //1   15^ 

      super.phBase[15*i+10] = (phi[2*i + 1] + 2)*90.0; //0M  16^
      super.phBase[15*i+11] = (phi[2*i + 1] + 2)*90.0; //1M  16

      super.phBase[15*i+12] = phi[2*i + 1]*90.0;       //1   17^,18^ 

      super.phBase[15*i+13] = (phi[2*i + 1] + 2)*90.0; //1M  19^

      super.phBase[15*i+14] = phi[2*i + 2]*90.0;       //0   12"^ at loop end only" 
   }


// Set the Supercycle Phase List



   super.phSuper[0] = 0.0;



// Set the Delay List

// Input tauR, taua and pw360 and calculate the second delay
// del = tauR - 2.0*taub.  



//pwXsuper (input pw360 to calculate pw180)



   var = getname0("pw",super.seqName,"");

   double lpw = 0.5*roundoff(getval(var),2.0*DTCK*super.n90);

   super.array = disarry(var, super.array);

//t1Xsuper (tauR) 



   var = getname0("t1",super.seqName,"");

   double taur = roundoff(getval(var),DTCK*super.n90);

   super.array = disarry(var, super.array);

//t2Xsuper (taua)



   var = getname0("t2",super.seqName,"");

   double taua = roundoff(getval(var),DTCK*super.n90);

   super.array = disarry(var, super.array);

   double del = taur - 2.0*taua - 8.0*lpw;
   if (del < 0.0) {
      printf("Error in getsuper(). tauR too small!");

      psg_abort(1);
   }


   super.pw[0] = taua;

   super.pw[1] = 2.0*lpw;

   super.pw[2] = 2.0*lpw;

   super.pw[3] = del;

   super.pw[4] = 2.0*lpw;

   super.pw[5] = 2.0*lpw;

   super.pw[6] = 2.0*taua;

   super.pw[7] = lpw;

   super.pw[8] = 2.0*lpw;

   super.pw[9] = lpw;

   super.pw[10] = del;
   super.pw[11] = lpw;

   super.pw[12] = 2.0*lpw;

   super.pw[13] = lpw;

   super.pw[14] = taua;


// Set the Offset List



//ofXsuper



   var = getname0("of",super.seqName,"");

   super.of[0] = getval(var);

   super.array = disarry(var, super.array);



// Set the Base Amplitude List



   super.aBase[0] = 1023.0;



// Set the Base Gate List



   super.gateBase[0] = 0.0;

   super.gateBase[1] = 1.0;

   super.gateBase[2] = 1.0;

   super.gateBase[3] = 0.0;

   super.gateBase[4] = 1.0;

   super.gateBase[5] = 1.0;

   super.gateBase[6] = 0.0;

   super.gateBase[7] = 1.0;

   super.gateBase[8] = 1.0;

   super.gateBase[9] = 1.0;

   super.gateBase[10] = 0.0;

   super.gateBase[11] = 1.0;

   super.gateBase[12] = 1.0;

   super.gateBase[13] = 1.0;
   super.gateBase[14] = 0.0;


// Set the Overall Amplitude, Elements and Channel



//aXsuper



   var = getname0("a",super.seqName,"");

   super.a = getval(var);



//qXsuper



   var = getname0("q",super.seqName,"");

   super.nelem = (int) getval(var);

   super.array = disarry(var, super.array);



//chXsuper


   var  = getname0("ch",super.seqName,"");

   Getstr(var,super.ch,sizeof(super.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",super.seqName,"");

   sprintf(lpattern,"%s%d",var,super.nRec);

   super.hasArray = hasarry(super.array, lpattern);

   int lix = arryindex(super.array);

   if (super.calc > 0) {
      var = getname0("",super.seqName,"");

      sprintf(super.pattern,"%s%d_%d",var,super.nRec,lix);

      if (super.hasArray == 1) {

         super = MPchopper(super); 

         super.iSuper = iph + super.nelem%super.nphSuper;

      }
      super.t = gett(lix, lpattern);

   }

   return super;

}


//======================

// Build DUMBO

//======================



MPSEQ getdumboxmx(char *seqName, int iph, double p, double phint, int iRec, int calc)

{

   MPSEQ pm = {};

   int i;

   char *var;

   extern MPSEQ MPchopper(MPSEQ pm);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

      printf("Error in getdumboxmx(). The type name %s is invalid!\n",seqName);

      psg_abort(1);

   }

   sprintf(pm.seqName,"%s",seqName);

   pm.calc = calc;

   pm.array = parsearry(pm.array);



// Obtain Phase Arguments



   pm.phAccum = p;

   pm.iSuper = iph;

   pm.nRec = iRec;

   pm.phInt = phint;



// Allocate Arrays



   int nphBase = 64;

   int nphSuper = 2;

   int npw = 64;

   int nph = 64;

   int nof = 1;

   int na = 1;

   int ng = 1;

   MPinitializer(&pm,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   pm.n90 = VNMRSN90;
   pm.n90m = VNMRSN90M;
   pm.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      pm.n90 = INOVAN90;
      pm.n90m = INOVAN90M;
      pm.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



//phXdumbo



   var = getname0("ph",pm.seqName,"");

   double ph = getval(var); 

   pm.array = disarry(var, pm.array); 



   double phBase[64] = {31.824000, 111.959215, 140.187768, 118.272283,

   86.096791,   73.451274,  71.082669,  53.716611,  21.276000,   2.170197,

   12.992428,   28.379246,   5.986118, 302.048261, 248.763031, 264.564610,

   348.552000,  74.378423,  94.391707,  45.146603, 343.815209, 323.373522,

   341.072484, 353.743630, 333.756000, 296.836742, 274.284097, 271.756242,

   268.693882, 255.782366, 257.225816, 304.420775, 124.420775,  77.225816,

    75.782366,  88.693882,  91.756242,  94.284097, 116.836742, 153.756000,

   173.743630, 161.072484, 143.373522, 163.815209, 225.146603, 274.391707,

   254.378423, 168.552000,  84.564610,  68.763031, 122.048261, 185.986118,

   208.379246, 192.992428, 182.170197, 201.276000, 233.716611, 251.082669,

   253.451274, 266.096791, 298.272283, 320.187768, 291.959215, 211.824000};



  for (i = 0; i < pm.nphBase; i++) {

     pm.phBase[i] = phBase[i] + ph;

  }



// Set the Supercycle Phase List



   pm.phSuper[0] = 0.0;
   pm.phSuper[1] = 180.0;


// Set the Delay List



//pwXdumbo (360 pulse)



   var = getname0("pw",pm.seqName,"");

   double pw = getval(var);

   pw = 64.0*roundoff(pw/64.0,DTCK*pm.n90);

   pm.array = disarry(var, pm.array);



   double pwlast = 0.0;

   for (i = 0; i < pm.nphBase; i++) {

      pm.pw[i] = roundoff((i+1)*pw/pm.nphBase,DTCK*pm.n90) - pwlast;

      pwlast = pwlast + pm.pw[i];

   }



// Set the Offset List



//ofXdumbo



   var = getname0("of",pm.seqName,"");

   pm.of[0] = getval(var);

   pm.array = disarry(var, pm.array);

   printf("pm.of = %f\n",pm.of[0]); 



// Set the Amp and Gate Lists



   pm.aBase[0] = 1023.0;

   pm.gateBase[0] = 0.0;



// Set the Overall Amplitude, Elements and Channel



//aXdumbo



   var = getname0("a",pm.seqName,"");

   pm.a = getval(var);



//nXdumbo



   var = getname0("n",pm.seqName,"");

   pm.nelem = (int) getval(var);

   pm.array = disarry(var, pm.array);



//chXdumbo


   var = getname0("ch",pm.seqName,"");

   Getstr(var,pm.ch,sizeof(pm.ch));



// Create the Shapefile Name


   char lpattern[NPATTERN];
   var = getname0("",pm.seqName,"");

   sprintf(lpattern,"%s%d",var,pm.nRec);

   pm.hasArray = hasarry(pm.array, lpattern);

   int lix = arryindex(pm.array);

   if (pm.calc > 0) {
      var = getname0("",pm.seqName,"");

      sprintf(pm.pattern,"%s%d_%d",var,pm.nRec,lix);

      if (pm.hasArray == 1) {

         pm = MPchopper(pm);

         pm.iSuper = iph + pm.nelem%pm.nphSuper;

      }
      pm.t = gett(lix, lpattern);

   }

   return pm;

}

//=======================================
// Build DUMBO using Fourier coefficients
//=======================================

MPSEQ getdumbogen(char *seqName, char *coeffName, int iph, double p, double phint, int iRec, int calc)

/*DUMBO-1 experimental description A.Lesage et al., JMR, 163 (2003) 105
  eDumbo method of optimisation G. de Paepe, Chem.Phys.Lett., 376 (2003) 259
  The Fourier coefficients are taken from http://www.ens-lyon.fr/CHEMIE/Fr/Groupes/NMR/Pages/library.html
  The supercycled version: M.Leskes, Chem. Phys. Lett., 466 (2008) 95 */

{
   MPSEQ dumbo = {};
   int i;
   char *var;
   extern MPSEQ MPchopper(MPSEQ dumbo);

   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {
      printf("Error in getdumbogen(). The type name %s is invalid!\n",seqName);
      psg_abort(1);
   }
   sprintf(dumbo.seqName,"%s",seqName);
   
   dumbo.calc = calc;
   dumbo.array = parsearry(dumbo.array);

// Obtain Phase Arguments

   dumbo.phAccum = p;
   dumbo.iSuper = iph;
   dumbo.nRec = iRec;
   dumbo.phInt = phint;

//chXdumbo
   
   var = getname0("ch",dumbo.seqName,"");
   Getstr(var,dumbo.ch,sizeof(dumbo.ch));
   printf("Channel: %s\n",dumbo.ch);

//set the flag for the supercycle

   int supercyc;
   var = getname0("sc",coeffName,"");
   supercyc=getval(var);
   dumbo.array = disarry(var, dumbo.array);

   //reset supercycle to normal if shape will be used for windowed acquisition
   if (!strcmp(dumbo.ch,"obs"))
	supercyc=0;

// Determine nphBase from qXdumbo

//qXdcf

   var = getname0("q",coeffName,"");
   int nphBase = (int) getval(var);
   if (nphBase%2) {
   	printf("The number of DUMBO steps should be even\n");
	psg_abort(1); 
   }
   if (supercyc)
   	nphBase*=2;//two times longer for cupercycled version
   dumbo.array = disarry(var, dumbo.array);


// Allocate Arrays

   int nphSuper = 1;
   int npw = nphBase;
   int nph = nphBase;
   int nof = 1;
   int na = 1;
   int ng = 1;
   MPinitializer(&dumbo,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   dumbo.n90 = VNMRSN90;
   dumbo.n90m = VNMRSN90M;
   dumbo.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      dumbo.n90 = INOVAN90;
      dumbo.n90m = INOVAN90M;
      dumbo.trap = INOVATRAP;
   }

#endif

//get Fourier coefficients
   char *coef[12]={"ca1","ca2","ca3","ca4","ca5","ca6","cb1","cb2","cb3","cb4","cb5","cb6"};
   double c[12];
   for (i=0; i<12; i++){
   	var = getname0(coef[i],coeffName,"");
   	c[i] = (double) getval(var);
	dumbo.array = disarry(var, dumbo.array);
   }
   
// Set the Base Phase List
   const double obsstep = 360.0/(PSD*8192);
   
   //phXdumbo

   var = getname0("ph",dumbo.seqName,"");
   double ph = (double) getval(var); 
   dumbo.array = disarry(var, dumbo.array);
   
   
   int bsteps=(supercyc)? nphBase/4 : nphBase/2; //steps in each block
   
//for the first half
   for (i=0; i<bsteps; i++) {
   	double dumboscale=2*M_PI*i/bsteps;
	dumbo.phBase[i]=ph;
	int j=0;
	for (j=0; j<6; j++){
		dumbo.phBase[i]+=360*c[j+6]*sin((j+1)*dumboscale)+360*c[j]*cos((j+1)*dumboscale);
	}
	dumbo.phBase[i] = roundphase(dumbo.phBase[i],obsstep);
   }
//invert for the second half
   int k;
   for (i=bsteps,k=bsteps-1; i<2*bsteps; i++,k--) {
   	dumbo.phBase[i]=dumbo.phBase[k]+180;
	dumbo.phBase[i] = roundphase(dumbo.phBase[i],obsstep);
   }

   if (supercyc) {
   	for (i=0,k=2*bsteps; i<2*bsteps; i++,k++) {
		dumbo.phBase[k]=roundphase(dumbo.phBase[i]+180.0,obsstep);
	}
   }

// Set the Supercycle Phase List (don't used for XiX)

   dumbo.phSuper[0] = 0.0;

// Set the Delay List

   var = getname0("pw",dumbo.seqName,"");
   double pw = (double) getval(var);
   pw = roundoff(pw,0.1e-6);
   dumbo.array = disarry(var, dumbo.array);
   
// Set DUMBO pulse width array   

   double pwlast = 0.0;
    
   for (i = 0; i < bsteps; i++) { 
      dumbo.pw[i] = roundoff((i+1)*pw/(2*bsteps),0.1e-6) - pwlast;
      pwlast = pwlast + dumbo.pw[i];   
      dumbo.pw[i + bsteps] = dumbo.pw[i];      
   }
   
   if (supercyc) {
   	for (i=0,k=2*bsteps; i<2*bsteps; i++,k++) {
		dumbo.pw[k]=dumbo.pw[i];
	}
   }

// Set the Offset List

//ofXdumbo

   var = getname0("of",dumbo.seqName,"");
   dumbo.of[0] = getval(var);
   dumbo.array = disarry(var, dumbo.array);

// Set the Amp and Gate Lists

   dumbo.aBase[0] = 1023.0;
   dumbo.gateBase[0] = 0.0;

// Set the Overall Amplitude, Elements and Channel

//aXdumbo

   var = getname0("a",dumbo.seqName,"");
   dumbo.a = getval(var);

//nXpmlg 

   var = getname0("n",dumbo.seqName,"");
   dumbo.nelem = (int) getval(var);
   if (!dumbo.nelem)
	dumbo.nelem=1;
   dumbo.array = disarry(var, dumbo.array);

// Create the Shapefile Name

   char lpattern[NPATTERN];
   var = getname0("",dumbo.seqName,"");
   sprintf(lpattern,"%s%d",var,dumbo.nRec);
   dumbo.hasArray = hasarry(dumbo.array, lpattern);
   int lix = arryindex(dumbo.array);
   if (dumbo.calc > 0) {
      var = getname0("",dumbo.seqName,"");
      sprintf(dumbo.pattern,"%s%d_%d",var,dumbo.nRec,lix);
      if (dumbo.hasArray == 1) {
         dumbo = MPchopper(dumbo); 
         dumbo.iSuper = iph + dumbo.nelem%dumbo.nphSuper;
      }
      dumbo.t = gett(lix, lpattern);
   }
   return dumbo;
}

//==================

// Build saturation X

//==================

MPSEQ getsat(char *seqName, double p, double phint, int iRec, int calc)

{

   MPSEQ s = {};

   char *var;



   if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {

        printf("Error in getsat(). The type name %s is invalid!\n",seqName);

        psg_abort(1);

   }

   sprintf(s.seqName,"%s",seqName);

   s.calc = calc;

   s.array = parsearry(s.array);

   extern MPSEQ MPchopper(MPSEQ s);

   

// Obtain Phase Arguments



   s.phAccum = p;

   s.nRec = iRec;

   s.phInt = phint;



// Allocate Arrays



   int nphBase = 2;

   int nphSuper = 1;

   int npw = 2;

   int nph = 2;

   int nof = 1;

   int na = 2;

   int ng = 2;

   MPinitializer(&s,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   s.n90 = VNMRSN90;
   s.n90m = VNMRSN90M;
   s.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      s.n90 = INOVAN90;
      s.n90m = INOVAN90M;
      s.trap = INOVATRAP;
   }

#endif

// Set the Base Phase List



   s.phBase[0] = 0.0;

   s.phBase[1] = 0.0;



// Set the Supercycle Phase List



   s.phSuper[0] = 0.0;



// Set the Delay List



//tXsat



   var = getname0("t",s.seqName,"");
   double cycle = roundoff(getval(var),DTCK*s.n90);

   s.array = disarry(var, s.array);



//pwXsat



   var = getname0("pw",s.seqName,"");
   s.pw[1] = roundoff(getval(var),DTCK*s.n90);

   s.array = disarry(var, s.array);
   s.pw[0] = cycle - s.pw[1];

   if (s.pw[0] <= 0.0) { 
      abort_message("Error in getsat(). No delay between pulses\n");
   }


//Set the Offset List



//ofXsat



   var = getname0("of",s.seqName,"");

   double offset = getval(var);

   s.of[0] = offset;

   s.array = disarry(var, s.array);



// Set the Amp and Gate Lists



   s.aBase[0] = 1023.0;

   s.aBase[1] = 1023.0;



   s.gateBase[0] = 0.0;

   s.gateBase[1] = 1.0;



// Set the Overall Amplitude, Elements and Channel



//aXsat



   var = getname0("a",s.seqName,"");

   s.a = getval(var);

//qXsat



   var = getname0("q",s.seqName,"");

   s.nelem = (int) getval(var);

   s.array = disarry(var, s.array);



//chXsat



   var = getname0("ch",s.seqName,"");

   Getstr(var,s.ch,sizeof(s.ch));



// Create the Shapefile Name



   char lpattern[NPATTERN];

   var = getname0("",s.seqName,"");

   sprintf(lpattern,"%s%d",var,s.nRec);

   s.hasArray = hasarry(s.array, lpattern);

   int lix = arryindex(s.array);

   if (s.calc > 0) {

      var = getname0("",s.seqName,"");

      sprintf(s.pattern,"%s%d_%d",var,s.nRec,lix);

      if (s.hasArray == 1) {

         s = MPchopper(s);

      }

      s.t = gett(lix, lpattern);

   }

   return s;

}

// BDZ solidmpseqs has getpostc6() here


//====================================
// Build RNnv
//====================================

// BDZ this one is not in solidmpseqs

MPSEQ getrseq(char *seqName, int iph, double p, double phint, int iRec, int calc) {
    char *var;
    int i = 0;
    MPSEQ rseq = {};
    extern MPSEQ MPchopper(MPSEQ rseq);
    if (strlen(seqName) >= NSUFFIX  || strlen(seqName) < 1) {
        printf("Error in getrseq(). The type name %s is invalid!\n",seqName);
        psg_abort(1);
    }
    sprintf(rseq.seqName,"%s",seqName);
    rseq.calc = calc;
    rseq.array = parsearry(rseq.array);

    // Obtain Phase Arguments
    rseq.phAccum = p;
    rseq.iSuper = iph;
    rseq.nRec = iRec;
    rseq.phInt = phint;

    var = getname0("bigN",rseq.seqName,"");
    int big_n = (int)getval(var);
    // Allocate Arrays
    int nphBase = 2*big_n;
    int nphSuper = 1;
    int npw = 2*big_n;
    int nph = 2*big_n;
    int nof = 1;
    int na = 1;
    int ng = 1;
    MPinitializer(&rseq,npw,nph,nof,na,ng,nphBase,nphSuper);

#ifdef MATCH_MASTER

// Set the Step Sizes

   rseq.n90 = VNMRSN90;
   rseq.n90m = VNMRSN90M;
   rseq.trap = VNMRSTRAP;

   if (PWRF_DELAY > 0.0) {
      rseq.n90 = INOVAN90;
      rseq.n90m = INOVAN90M;
      rseq.trap = INOVATRAP;
   }

#endif

    rseq.array = disarry(var, rseq.array);

    var = getname0("nu",rseq.seqName,"");
    int nu = (int)getval(var);
    // Set the Base Phase List (XY-4 element)
    double phi;
    for(i = 0; i < big_n; i++) {
        phi = (1-2*(i%2)) * ((double)nu) * 90.0 / big_n;
        rseq.phBase[2*i] = phi;
        rseq.phBase[2*i+1] = fmod(180+phi,360);
    }
    rseq.array = disarry(var, rseq.array);

    // Set the Supercycle Phase List for XY-16 and XY-32
    rseq.phSuper[0] = 0;

    // BDZ suspicious looking code
    // in most code 12.5e-9 is replaced with DTCK. We seem to be doing
    //   10*DTCK*rseq.n90.
    // notice those are different by a factor of 10. did we copy code badly?

    // Get the Standard System Parameter "srate" and Set tauR
    double tauR = 2.0*roundoff(1.0/(2.0*getval("srate")),125e-9*rseq.n90);

    // Set the Delay List
    var = getname0("n",rseq.seqName,"");
    int little_n = (int)getval(var);
    double lpw = little_n*tauR/(4*big_n);
    lpw = 2.0*roundoff(lpw/2.0,DTCK);
    for(i = 0; i < big_n; i++) {
        rseq.pw[2*i] = lpw;
        rseq.pw[2*i+1] = 3*lpw;
    }
    rseq.array = disarry(var, rseq.array);

    // Set the Offset List
    //ofXrseq
    var = getname0("of",rseq.seqName,""); 
    rseq.of[0] = getval(var);
    rseq.array = disarry(var, rseq.array);

    // Set the Amplitude List
    rseq.aBase[0] = 1023.0;

    // Set the Base Gate List
    rseq.gateBase[0] = 1;

    // Set the Overall Amplitude, Elements and Channel
    //aXrseq
    var = getname0("a",rseq.seqName,"");
    rseq.a = getval(var);
    rseq.array = disarry(var, rseq.array);

    //qXrseq
    var = getname0("q",rseq.seqName,"");
    rseq.nelem = (int) getval(var); 
    rseq.array = disarry(var, rseq.array);

    //chXrseq
    var = getname0("ch",rseq.seqName,""); 
    Getstr(var,rseq.ch,sizeof(rseq.ch));

    // Create the Shapefile Name
    char lpattern[NPATTERN];
    var = getname0("",rseq.seqName,"");

    sprintf(lpattern,"%s%d",var,rseq.nRec);
    rseq.hasArray = hasarry(rseq.array, lpattern);


    int lix = arryindex(rseq.array);
    if (rseq.calc > 0) {
        var = getname0("",rseq.seqName,"");
        sprintf(rseq.pattern,"%s%d_%d",var,rseq.nRec,lix);
        if (rseq.hasArray == 1) {
            rseq = MPchopper(rseq); 
            rseq.iSuper = iph + rseq.nelem%rseq.nphSuper;
        }
        rseq.t = gett(lix, lpattern);
    }

    return rseq;
}

#endif
