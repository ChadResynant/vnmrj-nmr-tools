#ifdef WE_INCLUDED_SOLID_DECSHAPES
#error "Trying to include DECSHAPES from both Solids and BioSolids"
#endif

#ifndef SOLIDDECSHAPES_H
#define SOLIDDECSHAPES_H

#define WE_INCLUDED_BIOSOLID_DECSHAPES

//Contents:

//Structures:
// DSEQ                             Router to the decoupler sequences
// TPPM                             TPPM Decoupling
// SPINAL                           SPINAL64 Decoupling
// WALTZ4                           WALTZ4 Decoupling
// WALTZ                            WALTZ Decoupling (WALTZ16?)
// TMREV                            TMREV added by EDW October 2013
// PARIS   added 05/10/10
// PIPS    added 06/01/16
// RCWAPA  added by RG in 2021:     10-17-23 BDZ commented out for now
// CORD    added by RG on 11/10/22: 10-17-23 BDZ commented out for now
// STPPM   added by RG in 2023

// BDZ NOTE 10-18-23
// CORD code commented out for now. Rajat had tested it on the
//   scope but never on a spectrometer. When we decide to turn
//   it on permanently we can remove commenting and test and fix
//   as needed

// BDZ NOTE 10-18-23
// RCWAPA code commented out for now. Rajat had written the code
//   but it has never been tested. When we decide to turn it on
//   permanently we can remove commenting and test and fix as
//   needed

//Implementation ("get") functions
// gettppm() - TPPM                 Build TPPM
// getstppm() - STPPM               Build STPPM
// getspinal() - SPINAL             Build SPINAL
// getwaltz4() - WALTZ4             Build WALTZ4
// getwaltz() - WALTZ               Build WALTZ
// gettmrev() - TMREV               Build TMREV
// getPARIS() - PARIS
// getPIPS() - PIPS
// getCORD() - CORD
// getrcwapa() - RCWAPA
// getdseq() - DSEQ                 Choose TPPM or SPINAL with Hseq
// setdseq() - DSEQ                 Choose TPPM or SPINAL with an

// Argument

//Calculation of .DEC files
// make_tppm() - TPPM               Calculate TPPM
// make_stppm() - STPPM             Calculate STPPM
// make_spinal() - SPINAL           Calculate SPINAL
// make_waltz4() - WALTZ4           Calculate WALTZ4
// make_waltz() - WALTZ             Calculate WALTZ
// make_tmrev() - TMREV             Calculate TMREV
// make_paris() - PARIS
// make_pips() - PIPS
// make_cord() - CORD
// make_rcwapa() - RCWAPA

//Delay-Adjust Functions

// adj_dseq() - DSEQ

//Run Time ("underscore") functions
// _dseqon() - void                 Turn On any of ten sequences
// _dseqoff() - void                Turn Off any of ten sequences
// _tppm() - void                   Turn On TPPM (Run By _dseqon())
// _stppm() - void                  Turn On STPPM (Run By _dseqon())
// _spinal - void                   Turn On SPINAL (Run By _dseqon())
// _waltz4 - void                   Turn On WALTZ4 (Run By _dseqon())
// _waltz - void                    Turn On WALTZ (Run By _dseqon())
// _tmrev() - void                  Turn On TMREV (Run By _dseqon())
// _paris() - void                  Turn on PARIS called by _dseqon()
// _pips() - void                   Turn on PIPS called by _dseqon()
// _cord() - void                   Turn on CORD called by _dseqon()
// _rcwapa() - void                 Turn on RCWAPA called by _dseqon()

//================================================
// Structures for Decoupling Elements
//================================================

//========
// TPPM
//========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double ph;
   double of;
   char   ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} TPPM;

//========
// SPINAL
//========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double ph;
#ifdef MATCH_MASTER

    // BDZ: OVJ master removed the following declaration and made a
    //  SPINAL2 sequence instead

#else

   double alp;

#endif

   double of;
   char   ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} SPINAL;

//========
// SPINAL2
//========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double ph;
   double alp;
   double of;
   char   ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} SPINAL2;

//========
// STPPM
// RPG July 17 ,2023
//========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double ph;
   double of;
   double so; // supercycling order
   double ru; // repeat units number
   char   ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} STPPM;

//=========
// WALTZ4
//=========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double ph;
   double of; 
   char ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} WALTZ4;

//=========
// WALTZ16
//=========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double ph;
   double of; 
   char ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} WALTZ;

//========
// TMREV
//========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double ph;
   double of;
   char   ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} TMREV;

//========
// PARIS
//========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double of;
   char   ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} PARIS;

//========
// CORD
//========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double of;
   char   ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} CORD;

//========
// rCWApA
//========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw180;
   double pwA;
   double of;
   char   ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} RCWAPA;

//========
// PIPS
//========

typedef struct {
   int preset1;
   int preset2;
   double strtdelay;  //wavegen start delay (INOVA)
   double offstdelay; //wavegen offset delay (INOVA)
   double apdelay;    //ap bus and wfg stop delay (INOVA)
   double t;
   double a;
   double pw;
   double ph;
   double of;
   int mtaur;
   char   ch[NCH];
   int    hasArray;
   AR     array;
   char pattern[NPATTERN];
} PIPS;

//================================================
// dseq manages the various decoupling structures
//================================================

// BDZ - TODO - 10-13-23
//   Should this ne improved by using a union?
//   Also let's rename these one letter (illogical) mappings to words!

typedef struct {
   TPPM t;
   SPINAL s;
   SPINAL2 r;
   WALTZ4 waltz4;    // was f for four
   WALTZ w;
   PARIS p;
   TMREV tmrev;      // was r for rev
   RCWAPA rcwapa;    // was c because r was taken
   PIPS pips;        // was i because p was taken
   CORD cord;        // was o because c was taken
   STPPM stppm;      // was x because t was taken RPG July 17, 2023
   char name[10];    // EDW 10/25/2013 change from 10 to 15, fix problem? No.
   char seq[14];
} DSEQ;

//===========================================
// Calculation of TPPM and SPINAL .DEC files
//===========================================

//=============
// Build TPPM
//=============

TPPM gettppm(char *name)
{
   TPPM dec = {};
   char *var;
   extern void make_tppm(TPPM dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

// amplitude

   var = getname0("a","TPPM",name);
   dec.a = getval(var);

//pulse width

   var = getname0("pw","TPPM",name);
   dec.pw = getval(var);

//phase

   var = getname0("ph","TPPM",name);
   dec.ph = getval(var);
   dec.array = disarry(var, dec.array);

//channel

   var = getname0("ch","TPPM",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 64.0*dec.pw;

// create the shape

   int nRec = 0;
   char lpattern[NPATTERN];
   var = getname0("","TPPM",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","TPPM",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_tppm(dec);
   }
   return dec;
}

//===============
// Build SPINAL
//===============

SPINAL getspinal(char *name)
{
   SPINAL dec = {};
   char *var;
   extern void make_spinal(SPINAL dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

//amplitude

   var = getname0("a","SPINAL",name);
   dec.a = getval(var);

//pulse width

   var = getname0("pw","SPINAL",name); 
   dec.pw = getval(var);

//phase

   var = getname0("ph","SPINAL",name);
   dec.ph = getval(var);
   dec.array = disarry(var, dec.array);

#ifdef MATCH_MASTER

   // nothing: OVJ master removed this from SPINAL and put it in SPINAL2

#else

   var = getname0("alp","SPINAL",name);
   dec.alp = getval(var);
   dec.array = disarry(var, dec.array);

#endif

//channel

   var = getname0("ch","SPINAL",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 64.0*dec.pw;

// create the shape

   int nRec = 0;  
   char lpattern[NPATTERN];
   var = getname0("","SPINAL",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","SPINAL",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_spinal(dec);
   }
   return dec;
}

//=============================
// Build SPINAL2 with 2 angles
//=============================

SPINAL2 getspinal2(char *name)
{
   SPINAL2 dec = {};
   char *var;
   extern void make_spinal2(SPINAL2 dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

//amplitude

   var = getname0("a","SPINAL2",name);
   dec.a = getval(var);
//pulse width

   var = getname0("pw","SPINAL2",name);
   dec.pw = getval(var);

//phase

   var = getname0("ph","SPINAL2",name);
   dec.ph = getval(var);
   dec.array = disarry(var, dec.array);

//second phase

   var = getname0("alp","SPINAL2",name);
   dec.alp = getval(var);
   dec.array = disarry(var, dec.array);

//channel

   var = getname0("ch","SPINAL2",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 64.0*dec.pw;
// create the shape

   int nRec = 0;
   char lpattern[NPATTERN];
   var = getname0("","SPINAL2",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","SPINAL2",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_spinal2(dec);
   }
   return dec;
}


//==============================================
// Build SPINAL2 with 2 angles with name SPINAL
//==============================================

SPINAL2 getspinal2s(char *name)
{
   SPINAL2 dec = {};
   char *var;
   extern void make_spinal2(SPINAL2 dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

//amplitude

   var = getname0("a","SPINAL",name);
   dec.a = getval(var);
//pulse width

   var = getname0("pw","SPINAL",name);
   dec.pw = getval(var);

//phase

   var = getname0("ph","SPINAL",name);
   dec.ph = getval(var);
   dec.array = disarry(var, dec.array);

//second phase

   var = getname0("alp","SPINAL",name);
   dec.alp = getval(var);
   dec.array = disarry(var, dec.array);
//channel

   var = getname0("ch","SPINAL",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 64.0*dec.pw;
// create the shape

   int nRec = 0;
   char lpattern[NPATTERN];
   var = getname0("","SPINAL",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","SPINAL",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_spinal2(dec);
   }
   return dec;
}

//===============
// Build STPPM
// SPPM RPG July 17, 2023
//===============

STPPM getstppm(char *name)
{
   STPPM dec = {};
   char *var;
   extern void make_stppm(STPPM dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

//amplitude

   var = getname0("a","STPPM",name);
   dec.a = getval(var);

//pulse width

   var = getname0("pw","STPPM",name); 
   dec.pw = getval(var);

//phase

   var = getname0("ph","STPPM",name);
   dec.ph = getval(var);
   dec.array = disarry(var, dec.array);

//channel

   var = getname0("ch","STPPM",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//supercycling order 

   var = getname0("so","STPPM",name); 
   dec.so = getval(var);
   dec.array = disarry(var, dec.array);

//repeat units in block

   var = getname0("ru","STPPM",name); 
   dec.ru = getval(var);
   dec.array = disarry(var, dec.array);

//cycle time

   dec.t = 2.0*dec.so*dec.ru*dec.pw;
//     dec.t = 64.0*dec.pw ;

// create the shape

   int nRec = 0;  
   char lpattern[NPATTERN];
   var = getname0("","STPPM",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","STPPM",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_stppm(dec);
   }
   return dec;
}

//===============
// Build WALTZ4
//===============

WALTZ4 getwaltz4(char *name)
{
   WALTZ4 dec = {};
   char *var;
   extern void make_waltz4(WALTZ4 dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

//amplitude

   var = getname0("a","WALTZ4",name);
   dec.a = getval(var);

//pulse width

   var = getname0("pw","WALTZ4",name); 
   dec.pw = getval(var);

//channel

   var = getname0("ch","WALTZ4",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 64.0*dec.pw;

// create the shape

   int nRec = 0;  
   char lpattern[NPATTERN];
   var = getname0("","WALTZ4",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","WALTZ4",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_waltz4(dec);
   }
   return dec;
}   

//===============
// Build WALTZ-16
//===============

WALTZ getwaltz(char *name)
{
   WALTZ dec = {};
   char *var;
   extern void make_waltz(WALTZ dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

//amplitude

   var = getname0("a","WALTZ",name);
   dec.a = getval(var);

//pulse width

   var = getname0("pw","WALTZ",name); 
   dec.pw = getval(var);

//channel

   var = getname0("ch","WALTZ",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 64.0*dec.pw;

// create the shape

   int nRec = 0;  
   char lpattern[NPATTERN];
   var = getname0("","WALTZ",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","WALTZ",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_waltz(dec);
   }
   return dec;
}   

//=============
// Build TMREV
//=============

TMREV gettmrev(char *name)
{
   TMREV dec = {};
   char *var;
   extern void make_tmrev(TMREV dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

// amplitude

   var = getname0("a","TMREV",name);
   dec.a = getval(var);

//pulse width

   var = getname0("pw","TMREV",name);
   dec.pw = getval(var);

//phase

   var = getname0("ph","TMREV",name);
   dec.ph = getval(var);
   dec.array = disarry(var, dec.array);

//channel

   var = getname0("ch","TMREV",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 12.0*dec.pw;    

// create the shape

   int nRec = 0;
   char lpattern[NPATTERN];
   var = getname0("","TMREV",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","TMREV",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_tmrev(dec);
   }
   return dec;
}

//=============
// Build PARIS
//=============

PARIS getparis(char *name)
{
   PARIS dec = {};
   char *var;
   extern void make_paris(PARIS dec);


   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

// amplitude

   var = getname0("a","PARIS",name);
   dec.a = getval(var);
// this was not on OVJ MASTER   
//   dec.array = disarry(var, dec.array);

//pulse width

   var = getname0("pw","PARIS",name);
   dec.pw = getval(var);

//channel

   var = getname0("ch","PARIS",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 2.0*dec.pw;

// create the shape

   int nRec = 0;
   char lpattern[NPATTERN];
 
   var = getname0("","PARIS",name);
   sprintf(lpattern,"%s%d",var,nRec);   
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","PARIS",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_paris(dec);
   }
   return dec;
}

/*

//=============
// Build CORD
//=============

CORD getcord(char *name)
{
   CORD dec = {};
   char *var;
   extern void make_cord(CORD dec);


   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

// amplitude

   var = getname0("a","CORD",name);
   dec.a = getval(var);
//   dec.array = disarry(var, dec.array);

//pulse width

   var = getname0("pw","CORD",name);
   dec.pw = getval(var);

//channel

   var = getname0("ch","CORD",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 48.0*dec.pw;
   
// create the shape

   int nRec = 0;
   char lpattern[NPATTERN];
 
   var = getname0("","CORD",name);
   sprintf(lpattern,"%s%d",var,nRec);   
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","CORD",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_cord(dec);
   }
   return dec;
}

*/

/*

//===============
// Build RCWAPA
//===============

RCWAPA getrcwapa(char *name) {
   RCWAPA dec = {};
   char *var;
   extern void make_rcwapa(RCWAPA dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

//amplitude

   var = getname0("a","RCWAPA",name);
   dec.a = getval(var);

//pulse width

   var = getname0("pw180","RCWAPA",name); 
   dec.pw180 = getval(var);

   var = getname0("pwA","RCWAPA",name); 
   dec.pwA = getval(var);

//channel

   var = getname0("ch","RCWAPA",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   dec.t = 2*dec.pw180 + 4*dec.pwA;

// create the shape

   int nRec = 0;  
   char lpattern[NPATTERN];
   var = getname0("","RCWAPA",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","RCWAPA",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_rcwapa(dec);
   }
   return dec;
}
*/

//=============
// Build PIPS
//=============

PIPS getpips(char *name)
{
   PIPS dec = {};
   char *var;
   extern void make_pips(PIPS dec);

   dec.preset1 = 0;
   dec.preset2 = 0;
   dec.strtdelay = WFG_START_DELAY - WFG_OFFSET_DELAY;
   dec.offstdelay = WFG_OFFSET_DELAY;
   dec.apdelay = PWRF_DELAY;

   dec.array = parsearry(dec.array);

// amplitude

   var = getname0("a","PIPS",name);
   dec.a = getval(var);
   
//pulse width

   var = getname0("pw","PIPS",name);
   dec.pw = getval(var);
   dec.array = disarry(var, dec.array);

// mtaur
  
   var = getname0("mtaur","PIPS",name);
   dec.mtaur = getval(var);
   dec.array=disarry(var,dec.array);

//channel

   var = getname0("ch","PIPS",name);
   Getstr(var,dec.ch,sizeof(dec.ch));

//cycle time

   double taur = 1.0/getval("srate");
   int mrotor = dec.mtaur;
   
   if ((dec.mtaur*taur - dec.pw*2) <= 0) mrotor = ceil(dec.pw*2/taur);
   
   dec.t = mrotor * taur;

// create the shape

   int nRec = 0;
   char lpattern[NPATTERN];
   var = getname0("","PIPS",name);
   sprintf(lpattern,"%s%d",var,nRec);
   dec.hasArray = hasarry(dec.array, lpattern);
   int lix = arryindex(dec.array);
   var = getname0("","PIPS",name);
   sprintf(dec.pattern,"%s%d_%d",var,nRec,lix);
   if (dec.hasArray == 1) {
      make_pips(dec);
   }
   return dec;
}

//=================================================
// Make a TPPM waveform for DSEQ Decoupling
//=================================================

void make_tppm(TPPM dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   int i;
   extern char userdir[];
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_tppm(): can not create file %s!\n", str);
   }

   for (i = 0; i<8; i++) {
      fprintf(fp," 90.0 %10.3f\n", dec.ph);
      fprintf(fp," 90.0 %10.3f\n", -dec.ph);
      fprintf(fp," 90.0 %10.3f\n", dec.ph);
      fprintf(fp," 90.0 %10.3f\n", -dec.ph);
      fprintf(fp," 90.0 %10.3f\n", dec.ph);
      fprintf(fp," 90.0 %10.3f\n", -dec.ph);
      fprintf(fp," 90.0 %10.3f\n", dec.ph);
      fprintf(fp," 90.0 %10.3f\n", -dec.ph);
   }
   fclose(fp);
}

//==============================================
// Underscore Function to Start TPPM decoupling
//==============================================

void _tppm(TPPM dec)
{
   int chnl;
   double aTPPM;
   aTPPM = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_tppm() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;

   if (dec.preset1 == 0) p1 = 0;
   if (dec.preset1 == 1) p1 = 1;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1:
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aTPPM);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               obswfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aTPPM);                   
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               decwfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aTPPM);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) { 
               dec2wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aTPPM);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec3wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_tppm() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

//=============================================
// Make a SPINAL Waveform for DSEQ Decoupling
//=============================================

void make_spinal(SPINAL dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   extern char userdir[];
   int n,i;
   int sign[8] = {1,-1,-1,1,-1,1,1,-1};
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_spinal(): can not create file %s!\n", str);
   }

   for (i = 0; i<8; i++) {
      n = i%8;

#ifdef MATCH_MASTER

      // BDZ OVJ master's code. Notice alp is gone in SPINAL. People use SPINAL2 instead.

      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(1.5*dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-1.5*dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(2.0*dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-2.0*dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(1.5*dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-1.5*dec.ph));

#else

      // BDZ Our old code

      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph+dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph-dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph+2.0*dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph-2.0*dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph+dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph-dec.alp));

#endif

   }
   fclose(fp);
}

//================================================
// Underscore Function to Start SPINAL decoupling
//================================================

void _spinal(SPINAL dec)
{
   int chnl;
   double aSPINAL;
   aSPINAL = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_spinal() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;
   if (dec.preset1 == 0) p1 = 0;
   if (dec.preset1 == 1) p1 = 1;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1: 
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aSPINAL);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               obswfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aSPINAL);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               decwfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aSPINAL);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec2wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aSPINAL);                        
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec3wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_spinal() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

//=============================================
// Make a SPINAL2 Waveform for DSEQ Decoupling
//=============================================

void make_spinal2(SPINAL2 dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   extern char userdir[];
   int n,i;
   int sign[8] = {1,-1,-1,1,-1,1,1,-1};
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_spinal2(): can not create file %s!\n", str);
   }

   for (i = 0; i<8; i++) {
      n = i%8;
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph+dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph-dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph+2.0*dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph-2.0*dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph+dec.alp));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph-dec.alp));
   }
   fclose(fp);
}

//================================================
// Underscore Function to Start SPINAL2 decoupling
//================================================

void _spinal2(SPINAL2 dec)
{
   int chnl;
   double aSPINAL2;
   aSPINAL2 = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_spinal2() Error: Undefined Channel! < 0!\n");
   }

   int p1 = 0;
   if (dec.preset1 == 0) p1 = 0;
   if (dec.preset1 == 1) p1 = 1;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1:
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aSPINAL2);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               obswfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aSPINAL2);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               decwfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aSPINAL2);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec2wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aSPINAL2);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec3wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_spinal2() Error: Undefined Channel! < 0!\n");
            break;
      }
   }
}

//=============================================
// Make a STPPM Waveform for DSEQ Decoupling
// RPG July 17, 2023
//=============================================

void make_stppm(STPPM dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   extern char userdir[];
   int n,i,j;
   //int sign[8] = {1,-1,-1,1,-1,1,1,-1};
   // CMR 8/7/23 adjust to XY-16 type supercycling assuming XY element is 4pi pulses long
   int sign[8] = {1,-1,1,-1,1,-1,1,-1};
   int shift[8] = {0,0,180,180,180,180,0,0};
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_stppm(): can not create file %s!\n", str);
   }

   for (i = 0; i<dec.so; i++) {
      n = i%8;
      for (j = 0; j<dec.ru; j++) {
         fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph) + shift[n]);
         fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph) + shift[n]);
      }
   }
   fclose(fp);
}

//================================================
// Underscore Function to Start STPPM decoupling
// RPG July 17, 2023
//================================================

void _stppm(STPPM dec)
{
   int chnl;
   double aSTPPM;
   aSTPPM = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_stppm() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;
   if (dec.preset1 == 0) p1 = 0;
   if (dec.preset1 == 1) p1 = 1;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1: 
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aSTPPM);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               obswfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aSTPPM);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               decwfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aSTPPM);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec2wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aSTPPM);                        
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec3wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_stppm() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

//=============================================
// Make a WALTZ4 Waveform for DSEQ Decoupling
//=============================================

void make_waltz4(WALTZ4 dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   extern char userdir[];
   int i;
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_waltz4(): can not create file %s!\n", str);
   }

      fprintf(fp," 90.0      0.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp," 270.0     0.0\n");
      
      fprintf(fp," 90.0      0.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp," 270.0     0.0\n");
      
      fprintf(fp,"  90.0   180.0\n");
      fprintf(fp," 180.0     0.0\n");
      fprintf(fp," 270.0   180.0\n");
      
      fprintf(fp,"  90.0   180.0\n");
      fprintf(fp," 180.0     0.0\n");
      fprintf(fp," 270.0   180.0\n");

   fclose(fp);
}

//=============================================
// Make a WALTZ Waveform for DSEQ Decoupling
//=============================================

void make_waltz(WALTZ dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   extern char userdir[];
   int i;
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_waltz(): can not create file %s!\n", str);
   }


#ifdef MATCH_MASTER

   // BDZ new code from OVJ master that we haven't adopted yet
   
   for (i = 0; i<8; i++) {
      fprintf(fp,"  90.0   270.0\n");
      fprintf(fp," 180.0     0.0\n");
      fprintf(fp,"  90.0   180.0\n");
      fprintf(fp," 180.0   270.0\n");
      fprintf(fp,"  90.0    90.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp,"  90.0     0.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp,"  90.0   270.0\n");
   }

#else

   // BDZ our old code

      fprintf(fp," 270.0   180.0\n");
      fprintf(fp," 360.0     0.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp," 270.0     0.0\n");
      fprintf(fp,"  90.0   180.0\n");
      fprintf(fp," 180.0     0.0\n");
      fprintf(fp," 360.0   180.0\n");
      fprintf(fp," 180.0     0.0\n");
      fprintf(fp," 270.0   180.0\n");

      fprintf(fp," 270.0     0.0\n");
      fprintf(fp," 360.0   180.0\n");
      fprintf(fp," 180.0     0.0\n");
      fprintf(fp," 270.0   180.0\n");
      fprintf(fp,"  90.0     0.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp," 360.0     0.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp," 270.0     0.0\n");

      fprintf(fp," 270.0     0.0\n");
      fprintf(fp," 360.0   180.0\n");
      fprintf(fp," 180.0     0.0\n");
      fprintf(fp," 270.0   180.0\n");
      fprintf(fp,"  90.0     0.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp," 360.0     0.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp," 270.0     0.0\n");

      fprintf(fp," 270.0   180.0\n");
      fprintf(fp," 360.0     0.0\n");
      fprintf(fp," 180.0   180.0\n");
      fprintf(fp," 270.0     0.0\n");
      fprintf(fp,"  90.0   180.0\n");
      fprintf(fp," 180.0     0.0\n");
      fprintf(fp," 360.0   180.0\n");
      fprintf(fp," 180.0     0.0\n");
      fprintf(fp," 270.0   180.0\n");

#endif

   fclose(fp);
}

//=================================================
// Make a PARIS-xy waveform for Recoupling
//=================================================
// http://dx.doi.org/10.1016/j.cplett.2010.01.072

void make_paris(PARIS dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   
   extern char userdir[];
   int i;
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_paris(): can not create file %s!\n", str);
   }

   for (i = 0; i<8; i++) {
      fprintf(fp," 90.0   0.000 \n");
      fprintf(fp," 90.0 180.000 \n");
      fprintf(fp," 90.0   0.000 \n");
      fprintf(fp," 90.0 180.000 \n");
      fprintf(fp," 90.0   0.000 \n");
      fprintf(fp," 90.0 180.000 \n");
      fprintf(fp," 90.0   0.000 \n");
      fprintf(fp," 90.0 180.000 \n");
   }
   
   fclose(fp);
}

//==============================================
// Underscore Function to Start PARIS recoupling
//==============================================

void _paris(PARIS dec)
{
   int chnl;
   double aPARIS;
   aPARIS = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_paris() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1:
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aPARIS);
            if (dec.pw > 0.5e-6) {
               obswfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aPARIS);                   
            if (dec.pw > 0.5e-6) {
               decwfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aPARIS);
            if (dec.pw > 0.5e-6) { 
               dec2wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aPARIS);
            if (dec.pw > 0.5e-6) {
               dec3wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_paris() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

/*

//=================================================
// Make a CORD waveform for Recoupling
//=================================================

void make_cord(CORD dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   
   extern char userdir[];
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_cord(): can not create file %s!\n", str);
   }

 // based on original article by Polenova Group
 // phase cycling xy8 used as described in the article 
 // https://doi.org/10.1016/j.jmr.2013.04.009
      fprintf(fp," 90.0  90.000 %10.3f\n", dec.a); // 0 
      fprintf(fp," 90.0 270.000 %10.3f\n", dec.a); 
      fprintf(fp," 90.0 180.000 %10.3f\n", dec.a);
      fprintf(fp," 90.0 180.000 %10.3f\n", dec.a); 
      fprintf(fp," 180.0  90.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 270.000 %10.3f\n", 0.5*dec.a); 
      fprintf(fp," 180.0 180.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 180.000 %10.3f\n", 0.5*dec.a); 
      
      fprintf(fp," 90.0 180.000 %10.3f\n", dec.a); // 90 
      fprintf(fp," 90.0 0.000 %10.3f\n", dec.a); 
      fprintf(fp," 90.0 270.000 %10.3f\n", dec.a);
      fprintf(fp," 90.0 270.000 %10.3f\n", dec.a); 
      fprintf(fp," 180.0 180.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 0.000 %10.3f\n", 0.5*dec.a); 
      fprintf(fp," 180.0 270.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 270.000 %10.3f\n", 0.5*dec.a); 
      
      fprintf(fp," 90.0 270.000 %10.3f\n", dec.a); //  180 
      fprintf(fp," 90.0 90.000 %10.3f\n", dec.a); 
      fprintf(fp," 90.0 0.000 %10.3f\n", dec.a);
      fprintf(fp," 90.0 0.000 %10.3f\n", dec.a); 
      fprintf(fp," 180.0 270.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 90.000 %10.3f\n", 0.5*dec.a); 
      fprintf(fp," 180.0 0.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 0.000 %10.3f\n", 0.5*dec.a); 
      
      fprintf(fp," 90.0  0.000 %10.3f\n", dec.a); //  270 
      fprintf(fp," 90.0 180.000 %10.3f\n", dec.a); 
      fprintf(fp," 90.0 90.000 %10.3f\n", dec.a);
      fprintf(fp," 90.0 90.000 %10.3f\n", dec.a); 
      fprintf(fp," 180.0 0.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 180.000 %10.3f\n", 0.5*dec.a); 
      fprintf(fp," 180.0 90.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 90.000 %10.3f\n", 0.5*dec.a); 
      
      fprintf(fp," 90.0  90.000 %10.3f\n", dec.a); //  0 
      fprintf(fp," 90.0 270.000 %10.3f\n", dec.a); 
      fprintf(fp," 90.0 180.000 %10.3f\n", dec.a);
      fprintf(fp," 90.0 180.000 %10.3f\n", dec.a); 
      fprintf(fp," 180.0  90.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 270.000 %10.3f\n", 0.5*dec.a); 
      fprintf(fp," 180.0 180.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 180.000 %10.3f\n", 0.5*dec.a); 
      
      fprintf(fp," 90.0  0.000 %10.3f\n", dec.a); //  270 
      fprintf(fp," 90.0 180.000 %10.3f\n", dec.a); 
      fprintf(fp," 90.0 90.000 %10.3f\n", dec.a);
      fprintf(fp," 90.0 90.000 %10.3f\n", dec.a); 
      fprintf(fp," 180.0 0.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 180.000 %10.3f\n", 0.5*dec.a); 
      fprintf(fp," 180.0 90.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 90.000 %10.3f\n", 0.5*dec.a); 
      
      fprintf(fp," 90.0 270.000 %10.3f\n", dec.a); //  180 
      fprintf(fp," 90.0 90.000 %10.3f\n", dec.a); 
      fprintf(fp," 90.0 0.000 %10.3f\n", dec.a);
      fprintf(fp," 90.0 0.000 %10.3f\n", dec.a); 
      fprintf(fp," 180.0 270.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 90.000 %10.3f\n", 0.5*dec.a); 
      fprintf(fp," 180.0 0.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 0.000 %10.3f\n", 0.5*dec.a); 
      
      fprintf(fp," 90.0 180.000 %10.3f\n", dec.a); //  90 
      fprintf(fp," 90.0 0.000 %10.3f\n", dec.a); 
      fprintf(fp," 90.0 270.000 %10.3f\n", dec.a);
      fprintf(fp," 90.0 270.000 %10.3f\n", dec.a); 
      fprintf(fp," 180.0 180.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 0.000 %10.3f\n", 0.5*dec.a); 
      fprintf(fp," 180.0 270.000 %10.3f\n", 0.5*dec.a);
      fprintf(fp," 180.0 270.000 %10.3f\n", 0.5*dec.a); 
       
   fclose(fp);
}

//==============================================
// Underscore Function to Start CORD recoupling
//==============================================

void _cord(CORD dec)
{
   int chnl;
   double aCORD;
   aCORD = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_cord() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1:
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aCORD);
            if (dec.pw > 0.5e-6) {
               obswfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aCORD);                   
            if (dec.pw > 0.5e-6) {
               decwfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aCORD);
            if (dec.pw > 0.5e-6) { 
               dec2wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aCORD);
            if (dec.pw > 0.5e-6) {
               dec3wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_cord() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

*/

//==============================================
// Underscore Function to Start WALTZ4 decoupling
//==============================================

void _waltz4(WALTZ4 dec)
{
   int chnl;
   double aWALTZ4;
   aWALTZ4 = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_waltz4() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;

   if (dec.preset1 == 0) p1 = 0;
   if (dec.preset1 == 1) p1 = 1;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1:
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aWALTZ4);
            if (dec.pw > 0.5e-6) {
               printf("dec.pattern = %s\n",dec.pattern);
               obswfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aWALTZ4);                   
            if (dec.pw > 0.5e-6) {
               decwfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aWALTZ4);
            if (dec.pw > 0.5e-6) { 
               dec2wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aWALTZ4);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec3wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_waltz4() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

//==============================================
// Underscore Function to Start WALTZ decoupling
//==============================================

void _waltz(WALTZ dec)
{
   int chnl;
   double aWALTZ;
   aWALTZ = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_waltz() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;

   if (dec.preset1 == 0) p1 = 0;
   if (dec.preset1 == 1) p1 = 1;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1:
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aWALTZ);
            if (dec.pw > 0.5e-6) {
               obswfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aWALTZ);                   
            if (dec.pw > 0.5e-6) {
               decwfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aWALTZ);
            if (dec.pw > 0.5e-6) { 
               dec2wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aWALTZ);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec3wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_waltz() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

//=================================================
// Make a TMREV waveform for DSEQ Decoupling
//=================================================

void make_tmrev(TMREV dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   int i;
   extern char userdir[];
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_tmrev(): can not create file %s!\n", str);
   }

// only valid for bigN_tmrev=4 case CMR/EDW 11/4/2013
      fprintf(fp," 90.0 270.0 1023.0\n");
      fprintf(fp," 90.0 270.0 0.0\n");
      fprintf(fp," 90.0 270.0 1023.0\n");
      fprintf(fp," 90.0 0.0 1023.0\n");
      fprintf(fp," 90.0 0.0 0.0\n");
      fprintf(fp," 90.0 0.0 1023.0\n");
      fprintf(fp," 90.0 180.0 1023.0\n");
      fprintf(fp," 90.0 180.0 0.0\n");
      fprintf(fp," 90.0 180.0 1023.0\n");
      fprintf(fp," 90.0 90.0 1023.0\n");
      fprintf(fp," 90.0 90.0 0.0\n");
      fprintf(fp," 90.0 90.0 1023.0\n");
 
      fprintf(fp," 90.0 0.0 1023.0\n");
      fprintf(fp," 90.0 0.0 0.0\n");
      fprintf(fp," 90.0 0.0 1023.0\n");
      fprintf(fp," 90.0 90.0 1023.0\n");
      fprintf(fp," 90.0 90.0 0.0\n");
      fprintf(fp," 90.0 90.0 1023.0\n");
      fprintf(fp," 90.0 270.0 1023.0\n");
      fprintf(fp," 90.0 270.0 0.0\n");
      fprintf(fp," 90.0 270.0 1023.0\n");
      fprintf(fp," 90.0 180.0 1023.0\n");
      fprintf(fp," 90.0 180.0 0.0\n");
      fprintf(fp," 90.0 180.0 1023.0\n");
      
      fprintf(fp," 90.0 90.0 1023.0\n");
      fprintf(fp," 90.0 90.0 0.0\n");
      fprintf(fp," 90.0 90.0 1023.0\n");
      fprintf(fp," 90.0 180.0 1023.0\n");
      fprintf(fp," 90.0 180.0 0.0\n");
      fprintf(fp," 90.0 180.0 1023.0\n");
      fprintf(fp," 90.0 0.0 1023.0\n");
      fprintf(fp," 90.0 0.0 0.0\n");
      fprintf(fp," 90.0 0.0 1023.0\n");
      fprintf(fp," 90.0 270.0 1023.0\n");
      fprintf(fp," 90.0 270.0 0.0\n");
      fprintf(fp," 90.0 270.0 1023.0\n");
      
      fprintf(fp," 90.0 180.0 1023.0\n");
      fprintf(fp," 90.0 180.0 0.0\n");
      fprintf(fp," 90.0 180.0 1023.0\n");
      fprintf(fp," 90.0 270.0 1023.0\n");
      fprintf(fp," 90.0 270.0 0.0\n");
      fprintf(fp," 90.0 270.0 1023.0\n");
      fprintf(fp," 90.0 90.0 1023.0\n");
      fprintf(fp," 90.0 90.0 0.0\n");
      fprintf(fp," 90.0 90.0 1023.0\n");
      fprintf(fp," 90.0 0.0 1023.0\n");
      fprintf(fp," 90.0 0.0 0.0\n");
      fprintf(fp," 90.0 0.0 1023.0"); 
   
   fclose(fp);
}

//==============================================
// Underscore Function to Start TMREV decoupling
//==============================================

void _tmrev(TMREV dec)
{
   int chnl;
   double aTMREV;
   aTMREV = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_tmrev() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;

   if (dec.preset1 == 0) p1 = 0;
   if (dec.preset1 == 1) p1 = 1;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1:
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aTMREV);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               obswfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aTMREV);                   
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               decwfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aTMREV);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) { 
               dec2wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aTMREV);
            if ((dec.pw > 0.5e-6) && (dec.ph > 0.0)) {
               dec3wfgon(dec.pattern,dec.pw,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_tmrev() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

/*

//=============================================
// Make a RCWAPA Waveform for DSEQ Decoupling
//=============================================

void make_rcwapa(RCWAPA dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   extern char userdir[];
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_rcwapa(): can not create file %s!\n", str);
   }

   double degA = 90.0*dec.pwA/dec.pw180;

   fprintf(fp," %6.1f %10.3f\n", degA, 0.0);
   fprintf(fp," 90.0 %10.3f\n", 90.0);
   fprintf(fp," %6.1f %10.3f\n", degA, 0.0);
   fprintf(fp," %6.1f %10.3f\n", degA, 180.0);
   fprintf(fp," 90.0 %10.3f\n", 90.0);
   fprintf(fp," %6.1f %10.3f\n", degA, 180.0);

   fclose(fp);
}

//================================================
// Underscore Function to Start RCWAPA decoupling
//================================================

void _rcwapa(RCWAPA dec) {
   int chnl;
   double aRCWAPA;
   aRCWAPA = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_rcwapa() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;
   if (dec.preset1 == 0) p1 = 0;
   if (dec.preset1 == 1) p1 = 1;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1: 
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aRCWAPA);
            if ((dec.pw180 > 0.5e-6) && (dec.pwA > 0.5e-6)) {
               obswfgon(dec.pattern,dec.pw180,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aRCWAPA);
            if ((dec.pw180 > 0.5e-6) && (dec.pwA > 0.5e-6)) {
               decwfgon(dec.pattern,dec.pw180,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aRCWAPA);
            if ((dec.pw180 > 0.5e-6) && (dec.pwA > 0.5e-6)) {
               dec2wfgon(dec.pattern,dec.pw180,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aRCWAPA);                        
            if ((dec.pw180 > 0.5e-6) && (dec.pwA > 0.5e-6)) {
               dec3wfgon(dec.pattern,dec.pw180,90.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_rcwapa() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

*/

//=================================================
// Make a PIPS waveform for DSEQ Decoupling
//=================================================

void make_pips(PIPS dec)
{
   char shapepath[MAXSTR+16],str[4*MAXSTR];
   FILE *fp;
   int i;
   extern char userdir[];
   sprintf(shapepath,"%s/shapelib/",userdir);
   sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

   if((fp = fopen(str,"w"))==NULL) {
      abort_message("Error in make_pips(): can not create file %s!\n", str);
   }

   double taur = 1.0/getval("srate");
   int mrotor = dec.mtaur;
   
   if ((dec.mtaur*taur - dec.pw*2) <= 0) mrotor = ceil(dec.pw*2/taur);
   
   //printf("m=%d",dec.mtaur);

   
   double degpipsd = 90.0*(mrotor * taur - dec.pw * 2)/dec.pw;

   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 0.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 90.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 0.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 90.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 90.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 0.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 90.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 0.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 180.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 270.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 180.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 270.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 270.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 180.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 270.0,1023.0);
   fprintf(fp," %6.1f %10.3f %10.3f\n", degpipsd, 0.0, 0.0);
   fprintf(fp," 180.0 %10.3f %10.3f\n", 180.0,1023.0);

   fclose(fp);
}

//==============================================
// Underscore Function to Start PIPS decoupling
//==============================================

void _pips(PIPS dec)
{
   int chnl;
   double aPIPS;
   aPIPS = dec.a;
   chnl = 0;
   if (!strcmp(dec.ch,"obs")) chnl = 1;
   else if (!strcmp(dec.ch,"dec")) chnl = 2;
   else if (!strcmp(dec.ch,"dec2")) chnl = 3;
   else if (!strcmp(dec.ch,"dec3")) chnl = 4;
   else {
      abort_message("_pips() Error: Undefined Channel! (%s)\n", dec.ch);
   }

   int p1 = 0;

   if (dec.preset1 == 0) p1 = 0;
   if (dec.preset1 == 1) p1 = 1;

   if (dec.a > 0.0) {
      switch (chnl) {
         case 1:
            obsunblank();
            if (p1 == 0) xmtron();
            obspwrf(aPIPS);
            if ((dec.pw > 0.5e-6)) {
               obswfgon(dec.pattern,dec.pw,1.0,0,dec.offstdelay);
            }
            if (p1 != 0) xmtron();
            break;
         case 2:
            decunblank();
            if (p1 == 0) decon();
            decpwrf(aPIPS);                   
            if ((dec.pw > 0.5e-6)) {
               decwfgon(dec.pattern,dec.pw,1.0,0,dec.offstdelay);
            }
            if (p1 != 0) decon();
            break;
         case 3:
            dec2unblank();
            if (p1 == 0) dec2on();
            dec2pwrf(aPIPS);
            if ((dec.pw > 0.5e-6)) { 
               dec2wfgon(dec.pattern,dec.pw,1.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec2on();
            break;
         case 4:
            dec3unblank();
            if (p1 == 0) dec3on();
            dec3pwrf(aPIPS);
            if ((dec.pw > 0.5e-6)) {
               dec3wfgon(dec.pattern,dec.pw,1.0,0,dec.offstdelay);
            }
            if (p1 != 0) dec3on();
            break;
         default:
            abort_message("_pips() Error: Undefined Channel! (%s)\n", dec.ch);
            break;
      }
   }
}

//==============================================
// DSEQ Decoupling Sequence Utilities
//==============================================

//==============================================
// Select a Decoupling Sequence From Panel
//==============================================

DSEQ getdseq(char *name)
{
   DSEQ d = {};
   char var[13];

   sprintf(var,"%s",name);
   strcat(var,"seq");
   Getstr(var,d.seq,sizeof(d.seq));
   printf("getdseq()%s \n",var);

//   if (!strcmp(d.seq,"cord")) {
//      d.cord = getcord(name);
//      return d;
//   }
   if (!strcmp(d.seq,"paris")) {
      d.p = getparis(name);
      return d;
   }
   if (!strcmp(d.seq,"pips")) {
      d.pips = getpips(name);
      return d;
   }
//   if (!strcmp(d.seq,"rcwapa")) {
//      d.rcwapa = getrcwapa(name);
//      return d;
//   }
   if (!strcmp(d.seq,"spinal")) {
      d.s = getspinal(name);
      return d;
   }
   if (!strcmp(d.seq,"spinal2")) {
      d.r = getspinal2(name);
      return d;
   }
   if (!strcmp(d.seq,"stppm")) {
      d.stppm = getstppm(name);
      return d;
   }
   if (!strcmp(d.seq,"tmrev")) {
      d.tmrev = gettmrev(name);
      return d;
   }
   if (!strcmp(d.seq,"tppm")) {
      d.t = gettppm(name);
      return d;
   }
   if (!strcmp(d.seq,"waltz")) {
      d.w = getwaltz(name);
      return d;
   }
   if (!strcmp(d.seq,"waltz4")) {
      d.waltz4 = getwaltz4(name);
      return d;
   }

   abort_message("getdseq() Error: Undefined Decoupling Sequence! %s \n",name);
}

//====================================================================
// Select a Decoupling Sequence From Panel with Default 2-Angle Spinal
//====================================================================

DSEQ getdseq2(char *name)
{
   printf("Sequence uses getdseq2 statement which will be obsolete soon. Please, use getdseq instead which provides the same functionality\n");
   DSEQ dec=getdseq(name);
   return dec;
}

//=========================================================
// Assign the Decoupling Sequence in the Pulse Sequence
//=========================================================

DSEQ setdseq(char *name, char *seq)
{
   DSEQ d = {};

   sprintf(d.seq,"%s",seq);
   
//   if (!strcmp(d.seq,"cord")) {
//      d.cord = getcord(name);
//      return d;
//   }   
   if (!strcmp(d.seq,"paris")) {
      d.p = getparis(name);
      return d;
   }   
   if (!strcmp(d.seq,"pips")) {
      d.pips = getpips(name);
      return d;
   }   
//   if (!strcmp(d.seq,"rcwapa")) {
//      d.rcwapa = getrcwapa(name);
//      return d;
//   }
   if (!strcmp(d.seq,"spinal")) {
      d.s = getspinal(name);
      return d;
   }
   if (!strcmp(d.seq,"spinal2")) {
      d.r = getspinal2(name);
      return d;
   }
   if (!strcmp(d.seq,"stppm")) {
      d.stppm = getstppm(name);
      return d;
   }
   if (!strcmp(d.seq,"tmrev")) {
      d.tmrev = gettmrev(name);
      return d;
   }
   if (!strcmp(d.seq,"tppm")) {
      d.t = gettppm(name);
      return d;
   }
   if (!strcmp(d.seq,"waltz")) {
      d.w = getwaltz(name);
      return d;
   }
   if (!strcmp(d.seq,"waltz4")) {
      d.waltz4 = getwaltz4(name);
      return d;
   }

   abort_message("setdseq() Error: Undefined Decoupling Sequence! %s \n",name);
}

//=================================================================================
// Assign the Decoupling Sequence in the Pulse Sequence with Default 2-Angle Spinal
//=================================================================================

DSEQ setdseq2(char *name, char *seq)
{
   printf("Sequence uses setdseq2 statement which will be obsolete soon. Please, use setdseq instead which provides the same functionality\n");
   DSEQ dec=setdseq(name,seq);
   return dec;
}

//==================================================================
// Underscore Function to adjust the delays before and after a DSEQ
//==================================================================

// BDZ: TODO: 10-13-23
//   Made code changes to add some "pips" clauses (but then commented
//     them out for now).
//   Cases that might be considered missing:
//     cord, rcwapa, paris, tmrev, waltz, waltz4
//   We need to think about those missing ones a bit.
//   Note it looks like in OVJ source only one stock pulse sequence
//     calls this method.

DSEQ adj_dseq(DSEQ a, double *b, double *c, int lp1, int lp2)      
{
   double strtdelay;
   double offstdelay;
   double apdelay;

   if (!strcmp(a.seq,"pips")) {
      strtdelay = a.pips.strtdelay;
      offstdelay = a.pips.offstdelay;
      apdelay = a.pips.apdelay;
   }
// BDZ more rcwapa commenting
//   else if (!strcmp(a.seq,"rcwapa")) {
//      strtdelay = a.rcwapa.strtdelay;
//      offstdelay = a.rcwapa.offstdelay;
//      apdelay = a.rcwapa.apdelay;
//   }
   else if (!strcmp(a.seq,"spinal")) {
      strtdelay = a.s.strtdelay;
      offstdelay = a.s.offstdelay;
      apdelay = a.s.apdelay;
   }
   else if (!strcmp(a.seq,"spinal2")) {
      strtdelay = a.r.strtdelay;
      offstdelay = a.r.offstdelay;
      apdelay = a.r.apdelay;
   }
   else if (!strcmp(a.seq,"stppm")) {
      strtdelay = a.stppm.strtdelay;
      offstdelay = a.stppm.offstdelay;
      apdelay = a.stppm.apdelay;
   }
   else if (!strcmp(a.seq,"tppm")) {
      strtdelay = a.t.strtdelay;
      offstdelay = a.t.offstdelay;
      apdelay = a.t.apdelay;
   }
   else {
      
      // BDZ 10-12-23
      // treat it as TPPM: this was how code always
      // behaved. this might be a bug so warn user.

      // only warn once for the same error in this call
      // other warnings commented out
      
      warn_message("Sequence (%s) is being treated as TPPM in part 1", a.seq);
      
      strtdelay = a.t.strtdelay;
      offstdelay = a.t.offstdelay;
      apdelay = a.t.apdelay;
   }

   if (!strcmp(a.seq,"pips")) {
      a.pips.preset1 = lp1;
      a.pips.preset2 = lp2;
   }
// BDZ more rcwapa commenting
//   else if (!strcmp(a.seq,"rcwapa")) {
//      a.rcwapa.preset1 = lp1;
//      a.rcwapa.preset2 = lp2;
//   }
   else if (!strcmp(a.seq,"spinal")) {
      a.s.preset1 = lp1;
      a.s.preset2 = lp2;
   }
   else if (!strcmp(a.seq,"spinal2")) {
      a.r.preset1 = lp1;
      a.r.preset2 = lp2;
   }
   else if (!strcmp(a.seq,"stppm")) {
      a.stppm.preset1 = lp1;
      a.stppm.preset2 = lp2;
   }
   else if (!strcmp(a.seq,"tppm")) {
      a.t.preset1 = lp1;
      a.t.preset2 = lp2;
   }
   else {

      // BDZ 10-12-23
      // treat it as TPPM: this was how code always
      // behaved. this might be a bug so warn user.

      //warn_message("Sequence (%s) is being treated as TPPM in part 2", a.seq);

      a.t.preset1 = lp1;
      a.t.preset2 = lp2;
   }

   if ((lp1 == 0) || (lp1 == 1)) {

      // BDZ 10-12-23 added this pips clause
      
      //if (!strcmp(a.seq,"pips")) {
      //   if (a.pips.a > 0.0) {
      //      if ((a.pips.pw > 0.5e-6) && (a.pips.ph > 0.0)) {
      //         *b = *b - strtdelay - apdelay - offstdelay;
      //      }
      //      else {
      //         *b = *b - apdelay;
      //      }
      //   }
      //}

// BDZ - more rcwapa commenting
//
//      if (!strcmp(a.seq,"rcwapa")) {
//         if (a.rcwapa.a > 0.0) {
//            if ((a.rcwapa.pw180 > 0.5e-6) && (a.rcwapa.pwA > 0.5e-6)) {
//               *b = *b - strtdelay - apdelay - offstdelay;
//            }
//            else {
//               *b = *b - apdelay;
//            }
//         }
//      }

      if (!strcmp(a.seq,"spinal")) {
         if (a.s.a > 0.0) {
            if ((a.s.pw > 0.5e-6) && (a.s.ph > 0.0)) {
               *b = *b - strtdelay - apdelay - offstdelay;
            }
            else {
               *b = *b - apdelay;
            }
         }
      }
      else if (!strcmp(a.seq,"spinal2")) {
         if (a.r.a > 0.0) {
            if ((a.r.pw > 0.5e-6) && (a.r.ph > 0.0)) {
               *b = *b - strtdelay - apdelay - offstdelay;
            }
            else {
               *b = *b - apdelay;
            }
         }
      }
      else if (!strcmp(a.seq,"stppm")) {
         if (a.stppm.a > 0.0) {
            if ((a.stppm.pw > 0.5e-6) && (a.stppm.ph > 0.0)) {
               *b = *b - strtdelay - apdelay - offstdelay;
            }
            else {
               *b = *b - apdelay;
            }
         }
      }
      else if (!strcmp(a.seq,"tppm")) {
         if (a.t.a > 0.0) {
            if ((a.t.pw > 0.5e-6) && (a.t.ph > 0.0)) {
               *b = *b - strtdelay - apdelay - offstdelay;
            }
            else {
               *b = *b - apdelay;
            }
         }
      }
      else {       
      
         // BDZ 10-12-23
         // treat it as TPPM: this was how code always
         // behaved. this might be a bug so warn user.

         //warn_message("Sequence (%s) is being treated as TPPM in part 3", a.seq);
      
         if (a.t.a > 0.0) {
            if ((a.t.pw > 0.5e-6) && (a.t.ph > 0.0)) {
               *b = *b - strtdelay - apdelay - offstdelay;
            }
            else {
               *b = *b - apdelay;
            }
         }
      }
   }

   if ((lp2 == 0) || (lp2 == 1)) {
       
      // BDZ 10-12-23 added this pips clause
      
      //if (!strcmp(a.seq,"pips")) {
      //   if (a.pips.a > 0.0) {
      //      *c = *c - apdelay;
      //   }
      //}
      
      // BDZ more rcwapa commenting
      //if (!strcmp(a.seq,"rcwapa")) {
      //   if (a.rcwapa.a > 0.0) {
      //      *c = *c - apdelay;
      //   }
      //}
      
      if (!strcmp(a.seq,"spinal")) {
         if (a.s.a > 0.0) {
            *c = *c - apdelay;
         }
      }
      else if (!strcmp(a.seq,"spinal2")) {
         if (a.r.a > 0.0) {
            *c = *c - apdelay;
         }
      }
      else if (!strcmp(a.seq,"stppm")) {
         if (a.stppm.a > 0.0) {
            *c = *c - apdelay;
         }
      }
      else if (!strcmp(a.seq,"tppm")) {
         if (a.t.a > 0.0) {
            *c = *c - apdelay;
         }
      }
      else {

         // BDZ 10-12-23
         // treat it as TPPM: this was how code always
         // behaved. this might be a bug so warn user.

         //warn_message("Sequence (%s) is being treated as TPPM in part 4", a.seq);
                  
         if (a.t.a > 0.0) {
            *c = *c - apdelay;
         }
      }
   }
   if (*b < 0.0) *b = 0.0;
   if (*c < 0.0) *c = 0.0;

   return a;
}

//==========================================================
// Underscore Function to Turn On and Off a DSEQ
//==========================================================

void dseqoff_emergency(char* message)
{
     xmtroff();
     decoff();
     dec2off();
     dec3off();
     obsprgoff();
     decprgoff();
     dec2prgoff();
     dec3prgoff();
     abort_message("%s",message);
}

void _dseqon(DSEQ dseq)
{
   //if (!strcmp(dseq.seq,"cord")) {
   //   _cord(dseq.cord);
   //   return;
   //}
   if (!strcmp(dseq.seq,"paris")) {
      _paris(dseq.p);
      return;
   }
   if (!strcmp(dseq.seq,"pips")) {
      _pips(dseq.pips);
      return;
   }
   //if (!strcmp(dseq.seq,"rcwapa")) {
   //   _rcwapa(dseq.rcwapa);
   //   return;
   //}
   if (!strcmp(dseq.seq,"spinal")) {
      _spinal(dseq.s);
      return;
   }
   if (!strcmp(dseq.seq,"spinal2")) {
      _spinal2(dseq.r);
      return;
   }
   if (!strcmp(dseq.seq,"stppm")) {
      _stppm(dseq.stppm);
      return;
   }
   if (!strcmp(dseq.seq,"tmrev")) {
      _tmrev(dseq.tmrev);
      return;
   }
   if (!strcmp(dseq.seq,"tppm")) {
      _tppm(dseq.t);
      return;
   }
   if (!strcmp(dseq.seq,"waltz")) {
      _waltz(dseq.w);
      return;
   }
   if (!strcmp(dseq.seq,"waltz4")) {
      _waltz4(dseq.waltz4);
      return;
   }

   abort_message("Error in _dseqon. Unrecognized Sequence! %s \n", dseq.seq);
}

void _dseqoff(DSEQ dseq) {

   int preset2;

   //if (!strcmp(dseq.seq,"cord")) {
   //   preset2 = dseq.cord.preset2;
   //}
   if (!strcmp(dseq.seq,"paris")) {
      preset2 = dseq.p.preset2;
   }
   else if (!strcmp(dseq.seq,"pips")) {
      preset2 = dseq.pips.preset2;
   }
   //else if (!strcmp(dseq.seq,"rcwapa")) {
   //   preset2 = dseq.rcwapa.preset2;
   //}
   else if (!strcmp(dseq.seq,"spinal")) {
      preset2 = dseq.s.preset2;
   }
   else if (!strcmp(dseq.seq,"spinal2")) {
      preset2 = dseq.r.preset2;
   }
   else if (!strcmp(dseq.seq,"stppm")) {
      preset2 = dseq.stppm.preset2;
   }
   else if (!strcmp(dseq.seq,"tmrev")) {
      preset2 = dseq.tmrev.preset2;
   }
   else if (!strcmp(dseq.seq,"tppm")) {
      preset2 = dseq.t.preset2;
   }
   else if (!strcmp(dseq.seq,"waltz")) {
      preset2 = dseq.w.preset2;
   }
   else if (!strcmp(dseq.seq,"waltz4")) {
      preset2 = dseq.waltz4.preset2;
   }
   else {
      warn_message("Unknown sequence: (%s). Treating as tppm.", dseq.seq);
      preset2 = dseq.t.preset2;
   }

   int p3 = 0;
   if (preset2 == 0) p3 = 0;
   if (preset2 == 1) p3 = 1;

   //if (!strcmp(dseq.seq,"cord")) {
   //   if (dseq.cord.a > 0.0) {
   //      if (!strcmp(dseq.cord.ch,"obs"))  {
   //         if (p3 != 0) xmtroff();
   //         if ((dseq.cord.pw > 0.5e-6)) {
   //            obsprgoff();
   //         }
   //         if (p3 == 0) xmtroff();
   //         obsunblank();
   //      }
   //      else if (!strcmp(dseq.cord.ch,"dec"))  {
   //         if (p3 != 0)  decoff();
   //         if ((dseq.cord.pw > 0.5e-6)) {
   //            decprgoff();
   //         }
   //         if (p3 == 0)  decoff();
   //         decunblank();
   //      }
   //      else if (!strcmp(dseq.cord.ch,"dec2")) {
   //         if (p3 != 0) dec2off();
   //         if ((dseq.cord.pw > 0.5e-6)) {
   //            dec2prgoff();
   //         }
   //         if (p3 == 0) dec2off();
   //         dec2unblank();
   //      }
   //      else if (!strcmp(dseq.cord.ch,"dec3")) {
   //         if (p3 == 0) dec3off();
   //         if ((dseq.cord.pw > 0.5e-6)) {
   //            dec3prgoff();
   //         }
   //         if (p3 == 0) dec3off();
   //         dec3unblank();
   //      }
   //      else dseqoff_emergency("dseqoff with cord: cannot recognize channel. Switching decoupling off\n");
   //   }
   //}
   
   if (!strcmp(dseq.seq,"paris")) {
      if (dseq.p.a > 0.0) {
         if (!strcmp(dseq.p.ch,"obs"))  {
            if (p3 != 0) xmtroff();
            if ((dseq.p.pw > 0.5e-6)) {
               obsprgoff();
            }
            if (p3 == 0) xmtroff();
            obsunblank();
         }
         else if (!strcmp(dseq.p.ch,"dec"))  {
            if (p3 != 0)  decoff();
            if ((dseq.p.pw > 0.5e-6)) {
               decprgoff();
            }
            if (p3 == 0)  decoff();
            decunblank();
         }
         else if (!strcmp(dseq.p.ch,"dec2")) {
            if (p3 != 0) dec2off();
            if ((dseq.p.pw > 0.5e-6)) {
               dec2prgoff();
            }
            if (p3 == 0) dec2off();
            dec2unblank();
         }
         else if (!strcmp(dseq.p.ch,"dec3")) {
            if (p3 == 0) dec3off();
            if ((dseq.p.pw > 0.5e-6)) {
               dec3prgoff();
            }
            if (p3 == 0) dec3off();
            dec3unblank();
         }
         else dseqoff_emergency("dseqoff with paris: cannot recognize channel. Switching decoupling off\n");
      }
   }
   
   else if (!strcmp(dseq.seq,"pips")) {
      if (dseq.pips.a > 0.0) {
         if (!strcmp(dseq.pips.ch,"obs")) {
            if (p3 != 0) xmtroff();
            if ((dseq.pips.pw > 0.5e-6)) {
               obsprgoff();
            }
            if (p3 == 0) xmtroff();
            obsunblank();
         }
         else if (!strcmp(dseq.pips.ch,"dec")) {
            if (p3 != 0) decoff();
            if ((dseq.pips.pw > 0.5e-6)) {
               decprgoff();
            }
            if (p3 == 0) decoff();
            decunblank();
         }
         else if (!strcmp(dseq.pips.ch,"dec2")) {
            if (p3 != 0) dec2off();
            if ((dseq.pips.pw > 0.5e-6)) {
               dec2prgoff();
            }
            if (p3 == 0) dec2off();
            dec2unblank();
         }
         else if (!strcmp(dseq.pips.ch,"dec3")) {
            if (p3 != 0) dec3off();
            if ((dseq.pips.pw > 0.5e-6)) {
               dec3prgoff();
            }
            if (p3 == 0) dec3off();
            dec3unblank();
         }
         else dseqoff_emergency("dseqoff with pips: cannot recognize channel. Switching decoupling off\n");
      }
   }  
   
   //else if (!strcmp(dseq.seq,"rcwapa")) {
   //   if (dseq.rcwapa.a > 0.0) {
   //      if (!strcmp(dseq.rcwapa.ch,"obs"))  {
   //         if (p3 != 0) xmtroff();
   //         if ((dseq.rcwapa.pw180 > 0.5e-6) && (dseq.rcwapa.pwA > 0.5e-6)) {
   //            obsprgoff();
   //         }
   //         if (p3 == 0) xmtroff();
   //         obsunblank();
   //      }
   //      else if (!strcmp(dseq.rcwapa.ch,"dec"))  {
   //         if (p3 != 0)  decoff();
   //         if ((dseq.rcwapa.pw180 > 0.5e-6) && (dseq.rcwapa.pwA > 0.5e-6)) {
   //            decprgoff();
   //         }
   //         if (p3 == 0)  decoff();
   //         decunblank();
   //      }
   //      else if (!strcmp(dseq.rcwapa.ch,"dec2")) {
   //         if (p3 != 0) dec2off();
   //         if ((dseq.rcwapa.pw180 > 0.5e-6) && (dseq.rcwapa.pwA > 0.5e-6)) {
   //            dec2prgoff();
   //         }
   //         if (p3 == 0) dec2off();
   //         dec2unblank();
   //      }
   //      else if (!strcmp(dseq.rcwapa.ch,"dec3")) {
   //         if (p3 == 0) dec3off();
   //         if ((dseq.rcwapa.pw180 > 0.5e-6) && (dseq.rcwapa.pwA > 0.5e-6)) {
   //            dec3prgoff();
   //         }
   //         if (p3 == 0) dec3off();
   //         dec3unblank();
   //      }
   //      else dseqoff_emergency("dseqoff with rcwapa: cannot recognize channel. Switching decoupling off\n");
   //   }
   //}
   
   else if (!strcmp(dseq.seq,"spinal")) {
      if (dseq.s.a > 0.0) {
         if (!strcmp(dseq.s.ch,"obs"))  {
            if (p3 != 0) xmtroff();
            if ((dseq.s.pw > 0.5e-6) && (dseq.s.ph > 0.0)) {
               obsprgoff();
            }
            if (p3 == 0) xmtroff();
            obsunblank();
         }
         else if (!strcmp(dseq.s.ch,"dec"))  {
            if (p3 != 0)  decoff();
            if ((dseq.s.pw > 0.5e-6) && (dseq.s.ph > 0.0)) {
               decprgoff();
            }
            if (p3 == 0)  decoff();
            decunblank();
         }
         else if (!strcmp(dseq.s.ch,"dec2")) {
            if (p3 != 0) dec2off();
            if ((dseq.s.pw > 0.5e-6) && (dseq.s.ph > 0.0)) {
               dec2prgoff();
            }
            if (p3 == 0) dec2off();
            dec2unblank();
         }
         else if (!strcmp(dseq.s.ch,"dec3")) {
            if (p3 == 0) dec3off();
            if ((dseq.s.pw > 0.5e-6) && (dseq.s.ph > 0.0)) {
               dec3prgoff();
            }
            if (p3 == 0) dec3off();
            dec3unblank();
         }
         else dseqoff_emergency("dseqoff with spinal: cannot recognize channel. Switching decoupling off\n");
      }
   }
   
   else if (!strcmp(dseq.seq,"spinal2")) {
      if (dseq.r.a > 0.0) {
         if (!strcmp(dseq.r.ch,"obs"))  {
            if (p3 != 0) xmtroff();
            if ((dseq.r.pw > 0.5e-6) && (dseq.r.ph > 0.0)) {
               obsprgoff();
            }
            if (p3 == 0) xmtroff();
            obsunblank();
         }
         else if (!strcmp(dseq.r.ch,"dec"))  {
            if (p3 != 0)  decoff();
            if ((dseq.r.pw > 0.5e-6) && (dseq.r.ph > 0.0)) {
               decprgoff();
            }
            if (p3 == 0)  decoff();
            decunblank();
         }
         else if (!strcmp(dseq.r.ch,"dec2")) {
            if (p3 != 0) dec2off();
            if ((dseq.r.pw > 0.5e-6) && (dseq.r.ph > 0.0)) {
               dec2prgoff();
            }
            if (p3 == 0) dec2off();
            dec2unblank();
         }
         else if (!strcmp(dseq.r.ch,"dec3")) {
            if (p3 == 0) dec3off();
            if ((dseq.r.pw > 0.5e-6) && (dseq.r.ph > 0.0)) {
               dec3prgoff();
            }
            if (p3 == 0) dec3off();
            dec3unblank();
         }
         else dseqoff_emergency("dseqoff with spinal2: cannot recognize channel. Switching decoupling off\n");
      }
   }

   else if (!strcmp(dseq.seq,"stppm")) {
      if (dseq.stppm.a > 0.0) {
         if (!strcmp(dseq.stppm.ch,"obs"))  {
            if (p3 != 0) xmtroff();
            if ((dseq.stppm.pw > 0.5e-6) && (dseq.stppm.ph > 0.0)) {
               obsprgoff();
            }
            if (p3 == 0) xmtroff();
            obsunblank();
         }
         else if (!strcmp(dseq.stppm.ch,"dec"))  {
            if (p3 != 0)  decoff();
            if ((dseq.stppm.pw > 0.5e-6) && (dseq.stppm.ph > 0.0)) {
               decprgoff();
            }
            if (p3 == 0)  decoff();
            decunblank();
         }
         else if (!strcmp(dseq.stppm.ch,"dec2")) {
            if (p3 != 0) dec2off();
            if ((dseq.stppm.pw > 0.5e-6) && (dseq.stppm.ph > 0.0)) {
               dec2prgoff();
            }
            if (p3 == 0) dec2off();
            dec2unblank();
         }
         else if (!strcmp(dseq.stppm.ch,"dec3")) {
            if (p3 == 0) dec3off();
            if ((dseq.stppm.pw > 0.5e-6) && (dseq.stppm.ph > 0.0)) {
               dec3prgoff();
            }
            if (p3 == 0) dec3off();
            dec3unblank();
         }
         else dseqoff_emergency("dseqoff with stppm: cannot recognize channel. Switching decoupling off\n");
      }
   }
   
   else if (!strcmp(dseq.seq,"tmrev")) {
      if (dseq.tmrev.a > 0.0) {
         if (!strcmp(dseq.tmrev.ch,"obs")) {
            if (p3 != 0) xmtroff();
            if ((dseq.tmrev.pw > 0.5e-6) && (dseq.tmrev.ph > 0.0)) {
               obsprgoff();
            }
            if (p3 == 0) xmtroff();
            obsunblank();
         }
         else if (!strcmp(dseq.tmrev.ch,"dec")) {
            if (p3 != 0) decoff();
            if ((dseq.tmrev.pw > 0.5e-6) && (dseq.tmrev.ph > 0.0)) {
               decprgoff();
            }
            if (p3 == 0) decoff();
            decunblank();
         }
         else if (!strcmp(dseq.tmrev.ch,"dec2")) {
            if (p3 != 0) dec2off();
            if ((dseq.tmrev.pw > 0.5e-6) && (dseq.tmrev.ph > 0.0)) {
               dec2prgoff();
            }
            if (p3 == 0) dec2off();
            dec2unblank();
         }
         else if (!strcmp(dseq.tmrev.ch,"dec3")) {
            if (p3 != 0) dec3off();
            if ((dseq.tmrev.pw > 0.5e-6) && (dseq.tmrev.ph > 0.0)) {
               dec3prgoff();
            }
            if (p3 == 0) dec3off();
            dec3unblank();
         }
         else dseqoff_emergency("dseqoff with tmrev: cannot recognize channel. Switching decoupling off\n");
      }
   }
   
   else if (!strcmp(dseq.seq,"tppm")) {
      if (dseq.t.a > 0.0) {
         if (!strcmp(dseq.t.ch,"obs")) {
            if (p3 != 0) xmtroff();
            if ((dseq.t.pw > 0.5e-6) && (dseq.t.ph > 0.0)) {
               obsprgoff();
            }
            if (p3 == 0) xmtroff();
            obsunblank();
         }
         else if (!strcmp(dseq.t.ch,"dec")) {
            if (p3 != 0) decoff();
            if ((dseq.t.pw > 0.5e-6) && (dseq.t.ph > 0.0)) {
               decprgoff();
            }
            if (p3 == 0) decoff();
            decunblank();
         }
         else if (!strcmp(dseq.t.ch,"dec2")) {
            if (p3 != 0) dec2off();
            if ((dseq.t.pw > 0.5e-6) && (dseq.t.ph > 0.0)) {
               dec2prgoff();
            }
            if (p3 == 0) dec2off();
            dec2unblank();
         }
         else if (!strcmp(dseq.t.ch,"dec3")) {
            if (p3 != 0) dec3off();
            if ((dseq.t.pw > 0.5e-6) && (dseq.t.ph > 0.0)) {
               dec3prgoff();
            }
            if (p3 == 0) dec3off();
            dec3unblank();
         }
         else dseqoff_emergency("dseqoff with tppm: cannot recognize channel. Switching decoupling off\n");
      }
   }
   
   else if (!strcmp(dseq.seq,"waltz")) {
      if (dseq.w.a > 0.0) {
         if (!strcmp(dseq.w.ch,"obs"))  {
            if (p3 != 0) xmtroff();
            if (dseq.w.pw > 0.5e-6) {
               obsprgoff();
            }
            if (p3 == 0) xmtroff();
            obsunblank();
         }
         else if (!strcmp(dseq.w.ch,"dec"))  {
            if (p3 != 0)  decoff();
            if (dseq.w.pw > 0.5e-6) {
               decprgoff();
            }
            if (p3 == 0)  decoff();
            decunblank();
         }
         else if (!strcmp(dseq.w.ch,"dec2")) {
            if (p3 != 0) dec2off();
            if (dseq.w.pw > 0.5e-6) {
               dec2prgoff();
            }
            if (p3 == 0) dec2off();
            dec2unblank();
         }
         else if (!strcmp(dseq.w.ch,"dec3")) {
            if (p3 == 0) dec3off();
            if (dseq.w.pw > 0.5e-6) {
               dec3prgoff();
            }
            if (p3 == 0) dec3off();
            dec3unblank();
         }
         else dseqoff_emergency("dseqoff with waltz: cannot recognize channel. Switching decoupling off\n");
      }
   }
   
   else if (!strcmp(dseq.seq,"waltz4")) {
      if (dseq.waltz4.a > 0.0) {
         if (!strcmp(dseq.waltz4.ch,"obs"))  {
            if (p3 != 0) xmtroff();
            if (dseq.waltz4.pw > 0.5e-6) {
               obsprgoff();
            }
            if (p3 == 0) xmtroff();
            obsunblank();
         }
         else if (!strcmp(dseq.waltz4.ch,"dec"))  {
            if (p3 != 0)  decoff();
            if (dseq.waltz4.pw > 0.5e-6) {
               decprgoff();
            }
            if (p3 == 0)  decoff();
            decunblank();
         }
         else if (!strcmp(dseq.waltz4.ch,"dec2")) {
            if (p3 != 0) dec2off();
            if (dseq.waltz4.pw > 0.5e-6) {
               dec2prgoff();
            }
            if (p3 == 0) dec2off();
            dec2unblank();
         }
         else if (!strcmp(dseq.waltz4.ch,"dec3")) {
            if (p3 == 0) dec3off();
            if (dseq.waltz4.pw > 0.5e-6) {
               dec3prgoff();
            }
            if (p3 == 0) dec3off();
            dec3unblank();
         }
         else dseqoff_emergency("dseqoff with waltz4: cannot recognize channel. Switching decoupling off\n");
      }
   }
}

//==================================================================
// Underscore Functions to Turn On and Off a DSEQ with 2-angle SPINAL
//==================================================================

void _dseqon2(DSEQ dseq)
{
  printf("Sequence uses _dseqon2 statement which will be obsolete soon. Please, use _dseqon instead which provides the same functionality\n");
  _dseqon(dseq);
}

void _dseqoff2(DSEQ dseq)
{
  printf("Sequence uses _dseqoff2 statement which will be obsolete soon. Please, use _dseqoff instead which provides the same functionality\n");
  _dseqoff(dseq);
}

#endif
