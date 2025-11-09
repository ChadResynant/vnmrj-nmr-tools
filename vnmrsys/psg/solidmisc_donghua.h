/*..........A collection of WFG pattern creating programs by..........
Donghua H. Zhou/Rienstra NMR Group, UIUC/ March 20 ,2004

.......................................................................*/

typedef struct {
  double a1;  /* aHhx fine power of the source channel; offset vale if ramped*/
  double a2;  /* aXhx fine power of the destination channel; offset if ramped*/
  double d;   /* dHX  delta parameter of tangent shape -d to +d*/
  double b;   /* bHX  beta parameter of tangent shape*/
  double t;   /* tHX  contact time in sec after initcp() or Getval() */
  int    n;   /* nHX  #steps */
  double of;  /* ofHX offset frequency */
  char  sh[2];   /* shHX shape of cp, "c", "l", "t" */
  char  ch[3];   /* chHX ramp channel "fr" or "to"*/
  char  fr[5];   /* frHX--the source channel "obs", "dec", "dec2", "dec3"*/
  char  to[5];   /* toHX--the destination channel "obs", "dec", "dec2", "dec3"*/
  char  m[7];  /* mHX--"normal" -d to 0 to d
                       "up"     |d| to 0 to |d|
                       "down"  -|d| to 0 to -|d|
                 */
  char  pattern[32]; /* shape file name */  
  char  device[5]; /* "wfg", "attn" (attenuation board) */
} CP;

typedef struct { 
  CP in; /* see getdream() below */
  CP drm;
  CP out;
  char ch[5]; /* "obs" etc */
  char  m[7]; /*see that in CP definition*/
} DREAM;

typedef struct { 
/*decide not to include mMIXtag='spc5','darr','rfdr'... in each module structure*/
  double t;  /* tMIX */
  double aH; /* aHmix*/
  double aX;  /* aXmix */
  int n;     /* five 720deg pulses occupy n rotor periods, usu. 2 or 3. BTW
                You may not use capital N in structure, this will not compile. */
  char ch[5]; /* "obs" etc */
  
  double pw; /* pw90, internal variable, will be calculated = n*Tr/40 */
  int cycles; /* internal */
  char pattern[32];
} SPC5;

typedef struct { 
/*decide not to include mMIXtag='tobsy','darr','rfdr'... in each module structure*/
  double t;  /* tMIX */
  double aH; /* aHmix*/
  double aX;  /* aXmix */
  int n;     /* five 720deg pulses occupy n rotor periods, usu. 2 or 3. BTW
                You may not use capital N in structure, this will not compile. */
  char ch[5]; /* "obs" etc */
  
  double pw; /* pw90, internal variable, will be calculated = n*Tr/40 */
  int cycles; /* internal */
  char pattern[32];
} TOBSY;


typedef struct {
  double a;  /* fine power, corresponding to nutation freq v1 ~75 kHz */
  double pw; /* = pw180/cos(ph) ~ 7u*/
  double ph; /* ~ 12deg. pattern (pw,ph) (pw, -ph) */
  
  char   m[4]; /* "raw" for original tppm; "am" for amplitude modulated
                          tppm. Takegoshi et al. CPL, 341 (2001) 540-544 */
  double aAM; /* ~ 0.03*a, 3 kHz */
  double fAM; /* = abs(2*v1*sin(ph))/pi  ~ 14kHz */
  
  char ch[5]; /* should set 'dec' by default */
  char pattern[32];
  
  int  nAMcyc; /* default to 3. (nAmcyc/fAM)/(2*pw) 
                  ~ 15 raw TPPM cycles; +/-ph 30 steps*/
} TPPM;

typedef struct {
  double a;  /* fine power, corresponding to nutation freq v1 ~75 kHz */
  double pw; /* = pw180/cos(ph) ~ 7u*/
  double ph; /*phi for SPINAL */
  double alpha; /*alpha for SPINAL */
  double beta; /*beta for SPINAL */
  char ch[5]; /* should set 'dec' by default */
  char pattern[32];

} SPINAL;

typedef struct {
  double a; /* fine power*/
  double pw; /*90deg pulsewidth*/
  
  char ch[5]; /*default to 'dec'*/
  char pattern[32];
} WALTZ16;

typedef struct {
  double a; /* aHdrp */
  double ph; /* phHdrp */
  double f; /* fHdrp */

  char m[5]; /* "raw" Paepe, et al, Jacs  125 (2003) 13938
                      Paepe, et al, CPL  376 (2003) 259
              or "am" DHZ 
                 "GT2" "GT4" "GT8" "GT16" Gerbaud et al  CPL 37 (2003) 1-5*/
  double aAM; /* aHamdrp */
  double fAM; /* fHamdrp */
 
  double res; /* time step, recommend 0.5e-6; must be multiple of 50e-9 */
  char ch[5]; /* "dec" if dn='H1' or "obs" if tn='H1', no corresponding vnmr parameter*/
  
  char pattern[32];
} DROOPY;

typedef struct {
 /* Use a train of pi pulses for decoupling on N15 channel during C13 acquisition.
    xy16 supercycle
 */
  double a;  /* aYdec, fine power */
  double pw; /* pwYdec, 180deg pulse width*/
  int m; /* nYdec, offset = m*Tr. For instance, in a C13 detected experiment with
            both N15 and H2 Pi-train decoupling during acquisition, we must offset
            the two series of pi-pulses. If a N15 pulse and a H2 pulse happen 
            simultaneously, their sum frequency is so close to the C13 frequency and
            no C13 filters could effectively filter this rf artefact, thus resulting
            periodic spikes in the FID.
          */
           /* The predelay dec.m*Tr is not implemented yet, you need to do it 
              explicitly in each .c file. I don't know how to set predelay 
              offered by the IB_STOP word in the pattern.*/ 
  int n; /* nYdec, period=n*Tr, one pulse each n rotor periods*/
  
  char ch[5]; /* chYdec, should set 'dec2' by default */
  char pattern[32]; /*internal variable */
  double res; /*internal variable, 0.5u by default*/
} PITRAIN;


typedef struct {
/* DARR: dipolar-assisted rotaional resonance for homonucear recoupling
   RAD: rf assisted diffusion  --another name for DARR
 (1) Takegoshi et al, CPL 344(2001) 631-637
 (2) Takegoshi et al, JCP 118 (2002) 2325-2341
   
   MORE: modulatory resonance recoupling of heteronuclear dipolar interactions
 (3) Takegoshi et al, CPL 260 (1996) 331-335

*/
  double t; /* mixing time in msec */
  double a; /* fine power. Set it so that v1=vR. */
                
  char   m[6]; /* 3 major categories in recoupling 1H-1H dipolar interaction, 
                  which helps to recouple 13C-13C through 1H-13C.
                  
R3                "raw" for traditional DARR. Constant amplitude a (vR).
                  "alt" alternate power between vR (a) and 2*vR (2a) in periods
                        of 0.25-0.5ms (controlled by t0) --- since 13C-13C 
                        coupling is a few kHz, see Ref 1. To be clear, amplitude
                        = a for t0 then amplitude = 2a for the next t0.
                    ____________________________________________________
MORE              "sin" for sinusoidal MORE modulated DARR.
                        Sine wave amplitude modulation, with modulation
                        freqequency of n*vR, n=1,2.
                        Supposed to be insensitive to amplitude, but may 
                        start with a ~ 2*vR.
                        
                  "sqw" for square wave modulated MORE+DARR; simplified version
                        of "sin".
                  "va"  VA-MORE + DARR 
                        power linearly vary from 0.5a to 2.5a with cycle time of
                        10*t0 diveided into 10 segments during each of which the
                        rf amplitude is kept constant. For vR = 10 to 30 kHz, Tr
                        = 33 to 100 usec, MORE period Tr/n = 33 to 100 us or 17 
                        to 50 usec. Need to round off t0 = [n*t0*vR+0.5]*Tr/n.
                     __________________________________________________
                   
REDOR             "redor" rotor-synchronized  pi-pulses appplied at each half-
                        rotation period. Will use aH90 & pwH90.*/
  double t0; /* 0.25-0.5 msec for 13C-13C recoupling. Just set it to 0.25 for current
                implementation. Will add vnmr parameter if necessary. */
                
  int n;     /* modulation frequency = n* vR */
  
  char ch[5]; /* should set 'dec' by default */
  char pattern[32];
  double res; /* for communication between darr() and _darr_() only */
  double pwr; /* for communication only */
} DARR;

/* Isarry is copied from Pbox_psg.h: isarry*/
int Isarry(arrpar)      /* check whether the parameter is arrayed */
char *arrpar;           /* works with double arrays */
{
  char array[MAXSTR];
  int i, j, k, m, n;

  getstr("array",array);

  if ((j = strlen(array)) < (n = strlen(arrpar))) return 0;
  else if (array[0] == '(') i=1, j--;
  else i=0;

  k=0; m=0;
  while(i < j)
  {
    while((i < j) && (arrpar[m] == array[i]))
      i++, m++;
    if((m == n) && ((i == j) || (array[i] == ','))) return 1;
    else
    {
      while((i < j) && (array[i] != ',')) i++;
    }
    m = 0; i++;

  }

  return 0;
}
/* One problem remains with Isarry() : in the case of double arrays,
   for instance, array = 'pw,ofsX' it will generate pw*ofsX shapes
   and not ofsX shapes, as would be required */

double roundoff(double par, double res)
{
 return( ((int) (par/res + 0.5)) * res);
}

void stepbreaker(FILE *fp,double duration, double ph,int amp,int gate,double res,double dres)
{ /* 
If the duration is larger than 255 counts, that duration will be  split into several instruction words by
stepbreaker(). c.f. pp188 "VNMR Pulse Sequences. See make_pitrain in solidshapes.h for an example.
     res: is the actual time resolution
     dres: is the resolution in terms of the so-called "flip-angle resolution"
     In the example of obsprgon(pattern,pw90,dres), res = pw90/dres
*/
 int i, countMax=255, count, nWord, countRemain;

 count = (int) (duration/res+0.5);
 nWord = (int) (count/countMax);
 countRemain = count - nWord * countMax;

 for (i=0; i<nWord; i++) {
   fprintf(fp, " %8.1f %8.1f %8d %8d\n",countMax*dres,ph,amp,gate);
 }
 fprintf(fp, " %8.1f %8.1f %8d %8d\n",countRemain*dres,ph,amp,gate); 
}

double Getval(parameter)
 char *parameter;
 /* always use this instead of getval() for ssnmr programs */
{
 double temp;

 temp = getval(parameter);
 switch (parameter[0]){
   case 't': temp = temp/1000.0; break; /* msec --> sec */
   default:  break;
 }
 return(temp);
}

void Getstr(char *parname, char *parval, int arrsize)
/* sizeof(parval) called in subroutines always return 4 for the pointer,
   instead of returning the character array size; so provide it
   as the 3rd argument. This Getstr() is supposed to be called only by getcp and
   other initilization functions in this head file. */
{
 char str[MAXSTR];  /* getstr() always returns an char array of size MAXSTR */
 getstr(parname,str); 
 strncpy(parval,str,arrsize);
 if (strlen(str) >= arrsize) *(parval+arrsize-1) = '\0';
}

CP getcp(char *name) 
/* suppose name = "HXabcdefghijkl": the longest name have 14 letters (don't
count the trailing '\0'. The first two letters are for the channels involved
in cp, the rest up to 12 letters are for the suffix---if needed. The longest
vnmr parameters are such as dvHXabcdefghijkl, containing 16 letters. */
{
 int logic = 0;
 CP cp; 
 char up[3], low[3], suffix[13], var[17]; 
 double tmp_double, phase_res=15;
 extern void make_cp(CP cp); /* create the shape */


 if (strlen(name) >= 2 && strlen(name) <= 14) {
   strcpy(suffix,&(name[2]));

   up[2] = '\0'; low[2] = '\0';
   up[0]=(char) toupper(name[0]);   up[1]=(char) toupper(name[1]);
   low[0]=(char) tolower(name[0]); low[1]=(char) tolower(name[1]);
 } else {
   printf("Abort in getcp(): Bad name %s. At least two letters, but no more than 14.\n",name);
   exit(-1);
 }
 

/* logic = (Getval("arraydim") < 1.5 || ix==1); */
   logic = (ix==1); 

/* chHXsuffix */
   sprintf(var,"ch%s%s",up,suffix); Getstr(var,cp.ch,sizeof(cp.ch));
/* aHhxsuffix */
   sprintf(var,"a%c%s%s",up[0],low,suffix); cp.a1 = Getval(var);  
   logic = (logic || (!strcmp(cp.ch,"fr") && Isarry(var)));
/* aXhxsuffix */
   sprintf(var,"a%c%s%s",up[1],low,suffix); cp.a2 = Getval(var);
   logic = (logic || (!strcmp(cp.ch,"to") && Isarry(var)));
/* dHXsuffix */
   sprintf(var,"d%s%s",up,suffix);  cp.d = Getval(var);
   logic = (logic || Isarry(var)); 
/* bHXsuffix */
   sprintf(var,"b%s%s",up,suffix);  cp.b = Getval(var); 
   logic = (logic || Isarry(var));
/* tHXsuffix */
   sprintf(var,"t%s%s",up,suffix);  cp.t = Getval(var); 
   logic = (logic || Isarry(var));
/* nHXsuffix */
   sprintf(var,"n%s%s",up,suffix);  cp.n = (int) Getval(var); 
   logic = (logic || Isarry(var));
/* ofHXsuffix */
   sprintf(var,"of%s%s",up,suffix); cp.of = Getval(var);
   logic = (logic || Isarry(var));
/* shHXsuffix */
   sprintf(var,"sh%s%s",up,suffix); Getstr(var,cp.sh,sizeof(cp.sh));
   logic = (logic || Isarry(var));
/* frHXsuffix */ 
   sprintf(var,"fr%s%s",up,suffix); Getstr(var,cp.fr,sizeof(cp.fr));
/* toHXsuffix */
   sprintf(var,"to%s%s",up,suffix); Getstr(var,cp.to,sizeof(cp.to));
/* mHXsuffix */
   sprintf(var,"m%s%s",up,suffix); Getstr(var,cp.m,sizeof(cp.m));


   
/* devHXsuffix */
   sprintf(var,"dv%s%s",up,suffix); Getstr(var,cp.device,sizeof(cp.device));

 if (cp.of != 0.0) {
   /*if (cp.t/cp.n > 0.5/fabs(cp.of)) {*/
   if (cp.n < (int) (360.0/phase_res*cp.t*fabs(cp.of)+0.5)) { /* maximium 15deg per step */
     cp.n = (int) (360.0/phase_res*cp.t*fabs(cp.of) + 0.5);  
    if (logic) printf("FYI from getcp(): To faithfully represent frequency offset, the\n\
# of %s cp steps has been increased to %d [contact time %.3f ms].\n",name,cp.n,cp.t*1000.0);
    }
 }
 
 if (cp.t/cp.n < 200e-9) cp.n = (int) (cp.t/200e-9);

 tmp_double = roundoff(cp.t/cp.n,50e-9);
 cp.t = tmp_double * cp.n;


 if (!strcmp(cp.device,"wfg")) {
   if (logic) {sprintf(cp.pattern,"%s%s_%ld",up,suffix,ix); }
   else {sprintf(cp.pattern,"%s%s_1",up,suffix); }
   
   if (logic) {make_cp(cp);}
 }
   
 return cp;
}


TPPM gettppm(char *name) 
/* suppose name = "Habcdefghijkl": the longest name have 14 letters (don't
count the trailing '\0'. The first letter is for the channel involved
in cp, the rest up to 12 letters are for the suffix---if needed. The longest
vnmr parameters are such as fHamtppmabcdefghijkl, containing 20 letters. */
{
 int logic = 0;
 TPPM dec; 
 char up[2], suffix[13], var[21]; 
 extern void make_tppm(TPPM dec);


 if (strlen(name) >= 1 && strlen(name) <= 13) {
   strcpy(suffix,&(name[1]));

   up[1] = '\0'; up[0]=(char) toupper(name[0]);    
 } else {
   printf("Abort in gettppm(): Bad name %s. At least one letter, but no more than 13.\n",name);
   exit(-1);
 }
 

/* logic = (Getval("arraydim") < 1.5 || ix==1); */
   logic = (ix==1); 

/* mHtppmsuffix */
   sprintf(var,"m%stppm%s",up,suffix); Getstr(var,dec.m,sizeof(dec.m));
   logic = (logic || Isarry(var));


/* aHtppmsuffix */
   sprintf(var,"a%stppm%s",up,suffix); dec.a = Getval(var);
   logic = (logic || (!strcmp(dec.m,"am") && Isarry(var)) );    
/* pwHtppmsuffix */
   sprintf(var,"pw%stppm%s",up,suffix); dec.pw = Getval(var);  
   logic = (logic || Isarry(var)); 
/* phHtppmsuffix */
   sprintf(var,"ph%stppm%s",up,suffix); dec.ph = Getval(var);  
   logic = (logic || Isarry(var)); 
/* aHamtppmsuffix */
   sprintf(var,"a%samtppm%s",up,suffix); dec.aAM = Getval(var);  
   logic = (logic || Isarry(var));
/* fHamtppmsuffix */
   sprintf(var,"f%samtppm%s",up,suffix); dec.fAM = Getval(var);  
   logic = (logic || Isarry(var));
   


/* chHtppmsuffix */
   sprintf(var,"ch%stppm%s",up,suffix); Getstr(var,dec.ch,sizeof(dec.ch));

 if (logic) {sprintf(dec.pattern,"%stppm%s_%ld",up,suffix,ix); }
 else {sprintf(dec.pattern,"%stppm%s_1",up,suffix); }

    dec.nAMcyc = 3;
    
    make_tppm(dec); /* create the shape */
    
 return dec;
}


SPINAL getspinal(char *name) 
/* suppose name = "Habcdefghijkl": the longest name have 14 letters (don't
count the trailing '\0'. The first letter is for the channel involved
in cp, the rest up to 12 letters are for the suffix---if needed. The longest
vnmr parameters are such as fHamtppmabcdefghijkl, containing 20 letters. */
{
 int logic = 0;
 SPINAL dec; 
 char up[2], suffix[13], var[21]; 
 extern void make_spinal(SPINAL dec);


 if (strlen(name) >= 1 && strlen(name) <= 13) {
   strcpy(suffix,&(name[1]));

   up[1] = '\0'; up[0]=(char) toupper(name[0]);    
 } else {
   printf("Abort in gettppm(): Bad name %s. At least one letter, but no more than 13.\n",name);
   exit(-1);
 }
 

/* logic = (Getval("arraydim") < 1.5 || ix==1); */
   logic = (ix==1); 

/* aHtppmsuffix */
   sprintf(var,"a%sspinal%s",up,suffix); dec.a = Getval(var);
   logic = (logic || Isarry(var));    
/* pwHtppmsuffix */
   sprintf(var,"pw%sspinal%s",up,suffix); dec.pw = Getval(var);
   logic = (logic || Isarry(var)); 
/* phHtppmsuffix */
   sprintf(var,"ph%sspinal%s",up,suffix); dec.ph = Getval(var);  
   logic = (logic || Isarry(var)); 
/* alphaHtppmsuffix */
   sprintf(var,"alpha%sspinal%s",up,suffix); dec.alpha = Getval(var);  
   logic = (logic || Isarry(var));  
/* betaHtppmsuffix */
   sprintf(var,"beta%sspinal%s",up,suffix); dec.beta = Getval(var);  
   logic = (logic || Isarry(var));     
/* chHtppmsuffix */
   sprintf(var,"ch%sspinal%s",up,suffix); Getstr(var,dec.ch,sizeof(dec.ch));

 if (logic) {sprintf(dec.pattern,"%sspinal%s_%ld",up,suffix,ix); }
 else {sprintf(dec.pattern,"%sspinal%s_1",up,suffix); }
    
    make_spinal(dec); /* create the shape */
    
 return dec;
}

WALTZ16 getwaltz16(char *name) 
/* suppose name = "Nabcdefghijkl": the longest name have 14 letters (don't
count the trailing '\0'. The first letter is for the channel involved
in cp, the rest up to 12 letters are for the suffix---if needed. The longest
vnmr parameters are such as fHamtppmabcdefghijkl, containing 20 letters. */
{
 int logic = 0;
 WALTZ16 dec; 
 char up[2], suffix[13], var[21], cmd[MAXSTR];

 if (strlen(name) >= 1 && strlen(name) <= 13) {
   strcpy(suffix,&(name[1]));

   up[1] = '\0'; up[0]=(char) toupper(name[0]);    
 } else {
   printf("Abort in gettppm(): Bad name %s. At least one letter, but no more than 13.\n",name);
   exit(-1);
 }
 

/* logic = (Getval("arraydim") < 1.5 || ix==1); */
   logic = (ix==1); 

/* aNsuffix */
   sprintf(var,"a%s%s",up,suffix); dec.a = Getval(var);
   logic = (logic || Isarry(var));    
/* pwNsuffix */
   sprintf(var,"pw%s%s",up,suffix); dec.pw = Getval(var);  
   logic = (logic || Isarry(var)); 
   
/* chNsuffix */
   sprintf(var,"ch%s%s",up,suffix); Getstr(var,dec.ch,sizeof(dec.ch));

 if (logic) {sprintf(dec.pattern,"%swaltz16%s_%ld",up,suffix,ix); }
 else {sprintf(dec.pattern,"%swaltz16%s_1",up,suffix); }
 
 if (logic) { /*add this logic here on Mar 25, 2005, to be tested*/
   sprintf(cmd,"Pbox %s.DEC -w \"WALTZ16 %f\" \n",dec.pattern,dec.pw*24);
   system(cmd);
 }
 return dec;
}


DROOPY getdroopy(char *name) 
/* suppose name = "Habcdefghijkl": the longest name have 14 letters (don't
count the trailing '\0'. The first letter is for the channel involved
in cp, the rest up to 12 letters are for the suffix---if needed. The longest
vnmr parameters are such as fHamdrpabcdefghijkl, containing 19 letters. */
{
 int logic = 0;
 DROOPY dec; 
 char up[2], suffix[13], var[20],proton[5]; 
 extern void make_droopy(DROOPY dec);


 if (strlen(name) >= 1 && strlen(name) <= 13) {
   strcpy(suffix,&(name[1]));

   up[1] = '\0'; up[0]=(char) toupper(name[0]);    
 } else {
   printf("Abort in getdroopy(): Bad name %s. At least one letter, but no more than 13.\n",name);
   exit(-1);
 }
 
   dec.res = 0.2e-6; /* must be multiple of 50e-9, and no less than 200e-9 */


/* logic = (Getval("arraydim") < 1.5 || ix==1); */
   logic = (ix==1); 

/* mHdrpsuffix */
   sprintf(var,"m%sdrp%s",up,suffix); Getstr(var,dec.m,sizeof(dec.m));
   logic = (logic || Isarry(var));


/* aHdrpsuffix */
   sprintf(var,"a%sdrp%s",up,suffix); dec.a = Getval(var);
   logic = (logic || (!strcmp(dec.m,"am") && Isarry(var)) );    
/* fHdrpsuffix */
   sprintf(var,"f%sdrp%s",up,suffix); dec.f = Getval(var);  
   logic = (logic || Isarry(var)); 
/* phHdrpsuffix */
   sprintf(var,"ph%sdrp%s",up,suffix); dec.ph = Getval(var);  
   logic = (logic || Isarry(var)); 
/* aHamdrpsuffix */
   sprintf(var,"a%samdrp%s",up,suffix); dec.aAM = Getval(var);  
   logic = (logic || Isarry(var));
/* fHamdrpsuffix */
   sprintf(var,"f%samdrp%s",up,suffix); dec.fAM = Getval(var);  
   logic = (logic || Isarry(var));
   


/* chHtppmsuffix */
   sprintf(var,"dn"); Getstr(var,proton,sizeof(proton));
   if (!strcmp(proton,"H1")) strcpy(dec.ch,"dec");
   else {
     sprintf(var,"tn"); Getstr(var,proton,sizeof(proton));
     if (!strcmp(proton,"H1")) strcpy(dec.ch,"obs");
     else { printf("Error from getdroopy(): neither tn nor dn is 'H1'. Abort!");
            abort();
     }
   }

 if (logic) {sprintf(dec.pattern,"%sdrp%s_%ld",up,suffix,ix); }
 else {sprintf(dec.pattern,"%sdrp%s_1",up,suffix); }

  dec.a = fabs(dec.a); dec.aAM = fabs(dec.aAM);

  make_droopy(dec); /* create the shape */
    
 return dec;
}

PITRAIN getpitrain(char *name) 
/* suppose name = "Yabcdefghijklmn": the longest name have 14 letters (don't
count the trailing '\0'. The first letter is for the channel involved
in pi-train decoupling, the rest up to 14 letters are for the suffix---if needed. 
The longest vnmr parameters are such as chYdecabcdefghijkl, containing 20 letters. */
{
 int logic = 0;
 PITRAIN dec; 
 char up[2], suffix[15], var[21]; 
 extern void make_pitrain(PITRAIN dec);


 if (strlen(name) >= 1 && strlen(name) <= 15) {
   strcpy(suffix,&(name[1]));

   up[1] = '\0'; up[0]=(char) toupper(name[0]);    
 } else {
   printf("Abort in getpitrain(): Bad name %s. At least one letter, but no more than 15.\n",name);
   exit(-1);
 }
 

/* logic = (Getval("arraydim") < 1.5 || ix==1); */
   logic = (ix==1); 

/* aYdecsuffix */
   sprintf(var,"a%sdec%s",up,suffix); dec.a = Getval(var);
   logic = (logic || Isarry(var) );    
/* pwYdecsuffix */
   sprintf(var,"pw%sdec%s",up,suffix); dec.pw = Getval(var);  
   logic = (logic || Isarry(var)); 
/* mYdecsuffix */
   sprintf(var,"m%sdec%s",up,suffix); dec.m = Getval(var);  
   logic = (logic || Isarry(var));    
/* nYdecsuffix */
   sprintf(var,"n%sdec%s",up,suffix); dec.n = Getval(var);  
   logic = (logic || Isarry(var)); 
/* chYdecsuffix */
   sprintf(var,"ch%sdec%s",up,suffix); Getstr(var,dec.ch,sizeof(dec.ch)); 

   dec.res = 0.5e-6;

 if (logic) {sprintf(dec.pattern,"%spitrain%s_%ld",up,suffix,ix); }
 else {sprintf(dec.pattern,"%spitrain%s_1",up,suffix); }
   
    make_pitrain(dec); /* create the shape */
    
 return dec;
}

SPC5 getspc5(char *name) 
/* suppose name = "Xabcdefghijklmn": the longest name have 14 letters (don't
count the trailing '\0'. The first letter is for the channel of spc5, the rest up to 14 letters are for the suffix---if needed. 
The longest vnmr parameters are such as chYdecabcdefghijkl, containing 20 letters.
Currently, proton power during spc5 is controlled by vnmr parameter aHmix.
 */
{
 int logic = 0;
 SPC5 mix; 
 char up[2], suffix[15], var[21]; 
 double Tr, t_tmp;
 extern void make_spc5(SPC5 mix);


 if (strlen(name) >= 1 && strlen(name) <= 15) {
   strcpy(suffix,&(name[1]));

   up[1] = '\0'; up[0]=(char) toupper(name[0]);    
 } else {
   printf("Abort in getspc5(): Bad name %s. At least one letter, but no more than 15.\n",name);
   exit(-1);
 }
 
   logic = (ix==1); 

/* mMIXsuffix */
/*   sprintf(var,"mMIX%s",suffix); Getstr(var,mix.m,sizeof(mix.m)); */
/* tMIXsuffix */
   sprintf(var,"tMIX%s",suffix); mix.t = Getval(var);
   logic = (logic || Isarry(var) ); 
/* aHmixsuffix */
   sprintf(var,"aHmix%s",suffix); mix.aH = Getval(var);
/* aXmixsuffix */
   sprintf(var,"a%smix%s",up,suffix); mix.aX = Getval(var);
   logic = (logic || Isarry(var) );    
/* nXmixsuffix */
   sprintf(var,"n%smix%s",up,suffix); mix.n = Getval(var);
   logic = (logic || Isarry(var) );  
/* chXmixsuffix */
   sprintf(var,"ch%smix%s",up,suffix); Getstr(var,mix.ch,sizeof(mix.ch)); 

/*----*/
if (mix.t >10e-3 && ix == 1) {printf("tMIX too long for SPC5. exit!"); exit(0);}

 Tr = 1.0/Getval("srate"); 
 mix.cycles = (int) (mix.t/(2.0*mix.n*Tr)+0.5);

 t_tmp =  2.0*mix.cycles*mix.n*Tr;
 if (fabs(t_tmp-mix.t) >0.1e-3 && ix == 1) {
  printf("SPC5 mixing time %.2f ms has been modified to %.2f.\n",mix.t*1e3,t_tmp*1e3);
 } mix.t = t_tmp;

 mix.pw =mix.n*Tr/40.0;
 if (ix == 1) printf("Should set fine power so that pw90 = %.2fu during SPC5. \n",mix.pw*1e6);
 
/* mix.ZFdelay = Tr-1.7e-6; decide to delete this from structure*/
/*----*/
 if (logic) {sprintf(mix.pattern,"%sspc5%s_%ld",up,suffix,ix); }
 else {sprintf(mix.pattern,"%sspc5%s_1",up,suffix); }
   
    make_spc5(mix); /* create the shape */
    
 return mix;
}



TOBSY gettobsy(char *name) 
/* suppose name = "Xabcdefghijklmn": the longest name have 14 letters (don't
count the trailing '\0'. The first letter is for the channel of tobsy, the rest up to 14 letters are for the suffix---if needed. 
The longest vnmr parameters are such as chYdecabcdefghijkl, containing 20 letters.
Currently, proton power during tobsy is controlled by vnmr parameter aHmix.
 */
{
 int logic = 0;
 TOBSY mix; 
 char up[2], suffix[15], var[21]; 
 double Tr, t_tmp;
 extern void make_tobsy(TOBSY mix);


 if (strlen(name) >= 1 && strlen(name) <= 15) {
   strcpy(suffix,&(name[1]));

   up[1] = '\0'; up[0]=(char) toupper(name[0]);    
 } else {
   printf("Abort in gettobsy(): Bad name %s. At least one letter, but no more than 15.\n",name);
   exit(-1);
 }
 
   logic = (ix==1); 

/* mMIXsuffix */
/*   sprintf(var,"mMIX%s",suffix); Getstr(var,mix.m,sizeof(mix.m)); */
/* tMIXsuffix */
   sprintf(var,"tMIX%s",suffix); mix.t = Getval(var);
   logic = (logic || Isarry(var) ); 
/* aHmixsuffix */
   sprintf(var,"aHmix%s",suffix); mix.aH = Getval(var);
/* aXmixsuffix */
   sprintf(var,"a%smix%s",up,suffix); mix.aX = Getval(var);
   logic = (logic || Isarry(var) );    
/* nXmixsuffix */
   sprintf(var,"n%smix%s",up,suffix); mix.n = Getval(var);
   logic = (logic || Isarry(var) );  
/* chXmixsuffix */
   sprintf(var,"ch%smix%s",up,suffix); Getstr(var,mix.ch,sizeof(mix.ch)); 

/*----*/
if (mix.t >10e-3 && ix == 1) {printf("tMIX too long for TOBSY. exit!"); exit(0);}

 Tr = 1.0/Getval("srate"); 
 mix.cycles = (int) (mix.t/(mix.n*Tr)+0.5);

 t_tmp =  mix.cycles*mix.n*Tr;
 if (fabs(t_tmp-mix.t) >0.1e-3 && ix == 1) {
  printf("TOBSY mixing time %.2f ms has been modified to %.2f.\n",mix.t*1e3,t_tmp*1e3);
 } mix.t = t_tmp;

 mix.pw =mix.n*Tr/72.0;
 if (ix == 1) printf("Should set fine power so that pw90 = %.2fu during TOBSY. \n",mix.pw*1e6);
 

 if (logic) {sprintf(mix.pattern,"%stobsy%s_%ld",up,suffix,ix); }
 else {sprintf(mix.pattern,"%stobsy%s_1",up,suffix); }
   
    make_tobsy(mix); /* create the shape */
    
 return mix;
}



DARR getdarr(char* name) 
/* supposed name is null or suffix. Maximum 12 letters (don't
count the trailing '\0'. The longest
vnmr parameter chDARRabcdefghijkl, containing 18 letters. */
{
 int logic = 0;
 DARR mix;
 char suffix[13], var[19]; 
 extern void make_darr(DARR, double *,double *); /*create the shape */


 if (strlen(name) <= 12) {
   strcpy(suffix,name);
 } else {
   printf("Abort in getdarr(): name %s is longer than 12 letters.\n",name);
   exit(-1);
 }
 
/* logic = (Getval("arraydim") < 1.5 || ix==1); */
   logic = (ix==1); 

/* tMIXsuffix */
   sprintf(var,"tMIX%s",suffix); mix.t = Getval(var);
   
/* mDARRsuffix */
   sprintf(var,"mDARR%s",suffix); Getstr(var,mix.m,sizeof(mix.m));
   logic = (logic || Isarry(var));
      
/* aHmixsuffix */
   sprintf(var,"aHmix%s",suffix); mix.a = Getval(var);

/* chDARRsuffix */
   sprintf(var,"chDARR%s",suffix); Getstr(var,mix.ch,sizeof(mix.ch));
   
/* nMOREsuffix */
   sprintf(var,"nMORE%s",suffix); mix.n = Getval(var);
   

 if (logic) {sprintf(mix.pattern,"DARR%s_%ld",suffix,ix); }
 else {sprintf(mix.pattern,"DARR%s_1",suffix); }

    mix.t0 = 0.25e-3;
    mix.res = mix.t;
    
 make_darr(mix,&(mix.pwr),&(mix.res));
    
 return mix;
}


DREAM getdream(char *name)  /*Also works for R2T, simply set mode='normal'*/
{/*name = Xtag */ 

 int logic = 0;
 DREAM dream; 
 double tmp_double, aHdrm, aXread, aXdrm, dXdrm, bXdrm, pwREAD;
 double tDRM, ofDRM, ofIN, ofOUT, d_theta=30;
 int nDRM=100, nREAD=20;
 char up[2], suffix[15], var[21];
 extern void make_cp(CP cp);

 if (strlen(name) >= 1 && strlen(name) <= 15) {
   strcpy(suffix,&(name[1]));

   up[1] = '\0'; up[0]=(char) toupper(name[0]);    
 } else {
   printf("Abort in getdream(): Bad name %s. At least one letter, but no more than 15.\n",name);
   exit(-1);
 }
 

 
   logic = (ix==1); 

   sprintf(var,"ofXmix%s",suffix); ofDRM = Getval(var);  
   logic = (logic || Isarry(var));
   sprintf(var,"ofXmix%s",suffix); ofIN = Getval(var);  
   logic = (logic || Isarry(var));
   sprintf(var,"ofXmix%s",suffix); ofOUT = Getval(var);  
   logic = (logic || Isarry(var));

   sprintf(var,"aHmix%s",suffix); aHdrm = Getval(var);  
   logic = (logic || Isarry(var));

   sprintf(var,"a%sread%s",up,suffix); aXread = Getval(var);
   logic = (logic || Isarry(var));
   
   sprintf(var,"a%smix%s",up,suffix);   aXdrm = Getval(var);
   logic = (logic || Isarry(var));
   
   sprintf(var,"d%smix%s",up,suffix);    dXdrm = Getval(var);
   logic = (logic || Isarry(var));
   
   sprintf(var,"b%smix%s",up,suffix);   bXdrm = Getval(var);
   logic = (logic || Isarry(var));
   
   sprintf(var,"pwREAD%s",suffix);   pwREAD = Getval(var);
   logic = (logic || Isarry(var));
   
   sprintf(var,"tMIX%s",suffix);   tDRM = Getval(var);
   logic = (logic || Isarry(var));

   sprintf(var,"ch%smix%s",up,suffix);   
   Getstr(var,dream.ch,sizeof(dream.ch));

   sprintf(var,"m%smix%s",up,suffix);   
   Getstr(var,dream.m,sizeof(dream.m));
   
   
 if (tDRM/nDRM < 200e-9) nDRM = (int) (tDRM/200e-9);
if (pwREAD/nREAD < 200e-9) nREAD = (int) (pwREAD/200e-9); 

 tmp_double = roundoff(tDRM/nDRM,50e-9);
 tDRM = tmp_double * nDRM;
 
 tmp_double = roundoff(pwREAD/nREAD,50e-9);
 pwREAD = tmp_double * nREAD;

/* ____ Read in */
 dream.in.a1 = aHdrm; 
 if (!strcmp(dream.m,"normal")) tmp_double = aXdrm - dXdrm; 
 else if (!strcmp(dream.m,"up")) tmp_double = aXdrm + fabs(dXdrm);
 else if (!strcmp(dream.m,"down")) tmp_double = aXdrm - fabs(dXdrm); 
 dream.in.a2 = (aXread + tmp_double)/2.0;
 dream.in.d = -(aXread - tmp_double)/2.0;
 dream.in.b = 1e6;
 dream.in.t = pwREAD;
 dream.in.n = nREAD;
 dream.in.of = ofIN;
 sprintf(dream.in.sh,"%s","l");
 sprintf(dream.in.ch,"%s","to");
 sprintf(dream.in.fr,"%s","dec");
 sprintf(dream.in.to,"%s",dream.ch);
 sprintf(dream.in.m,"%s","normal");
 sprintf(dream.in.device,"%s","wfg");

 if (dream.in.of != 0.0) {
   if (dream.in.n < (int) (360.0/d_theta*dream.in.t*fabs(dream.in.of)+0.5)) { 
     dream.in.n = (int) (360.0/d_theta*dream.in.t*fabs(dream.in.of) + 0.5);  
    }
 }  if (dream.in.t/dream.in.n < 200e-9) dream.in.n = (int) (dream.in.t/200e-9);

 tmp_double = roundoff(dream.in.t/dream.in.n,50e-9);
 dream.in.t = tmp_double * dream.in.n;


 if (logic) {sprintf(dream.in.pattern,"%s_%ld","DREAMin",ix); }
 else {sprintf(dream.in.pattern,"%s_1","DREAMin"); }
 if (logic) {make_cp(dream.in);}

/* ____ Dream */
 dream.drm.a1 = aHdrm;
 dream.drm.a2 = aXdrm;
 dream.drm.d = dXdrm;
 dream.drm.b = bXdrm;
 dream.drm.t = tDRM;
 dream.drm.n = nDRM;
 dream.drm.of = ofDRM;
 sprintf(dream.drm.sh,"%s","t");
 sprintf(dream.drm.ch,"%s","to");
 sprintf(dream.drm.fr,"%s","dec");
 sprintf(dream.drm.to,"%s",dream.ch);
 sprintf(dream.drm.m,"%s",dream.m);
 sprintf(dream.drm.device,"%s","wfg");
 
  if (dream.drm.of != 0.0) {
   if (dream.drm.n < (int) (360.0/d_theta*dream.drm.t*fabs(dream.drm.of)+0.5)) { 
     dream.drm.n = (int) (360.0/d_theta*dream.drm.t*fabs(dream.drm.of) + 0.5);  
    }
 }  if (dream.drm.t/dream.drm.n < 200e-9) dream.drm.n = (int) (dream.drm.t/200e-9);

 tmp_double = roundoff(dream.drm.t/dream.drm.n,50e-9);
 dream.drm.t = tmp_double * dream.drm.n;

 if (logic) {sprintf(dream.drm.pattern,"%s_%ld","DREAM",ix); }
 else {sprintf(dream.drm.pattern,"%s_1","DREAM"); }
 if (logic) {make_cp(dream.drm);}

/* ____ Read out */
 dream.out.a1 = aHdrm; 
 if (!strcmp(dream.m,"normal")) tmp_double = aXdrm + dXdrm; 
 else if (!strcmp(dream.m,"up")) tmp_double = aXdrm + fabs(dXdrm);
 else if (!strcmp(dream.m,"down")) tmp_double = aXdrm - fabs(dXdrm); 
 dream.out.a2 = (aXread + tmp_double)/2.0;
 dream.out.d = (aXread - tmp_double)/2.0;
 dream.out.b = 1e6;
 dream.out.t = pwREAD;
 dream.out.n = nREAD;
 dream.out.of = ofOUT;
 sprintf(dream.out.sh,"%s","l");
 sprintf(dream.out.ch,"%s","to");
 sprintf(dream.out.fr,"%s","dec");
 sprintf(dream.out.to,"%s",dream.ch);
 sprintf(dream.out.m,"%s","normal");
 sprintf(dream.out.device,"%s","wfg");
 
  if (dream.out.of != 0.0) {
   if (dream.out.n < (int) (360.0/d_theta*dream.out.t*fabs(dream.out.of)+0.5)) { 
     dream.out.n = (int) (360.0/d_theta*dream.out.t*fabs(dream.out.of) + 0.5);  
    }
 }  if (dream.out.t/dream.out.n < 200e-9) dream.out.n = (int) (dream.out.t/200e-9);

 tmp_double = roundoff(dream.out.t/dream.out.n,50e-9);
 dream.out.t = tmp_double * dream.out.n;
 
 if (logic) {sprintf(dream.out.pattern,"%s_%ld","DREAMout",ix); }
 else {sprintf(dream.out.pattern,"%s_1","DREAMout"); }
 if (logic) {make_cp(dream.out);}
  
 return dream;
}

/*------------------------shape pulse-------------------------*/
/* rsnob
 usage: 
#include "solidstandard.h"
#include <Pbox_psg.h>    ---must after solidstandard.h    
    RSNOB shpX;                 ---declare
    shpX = getrsnob("Xsoft");   ---create
    pbox_pulse(getRsh(shpX.pattern),zero,0.0,0.0); ---use it, pbox_decpulse, pbox_dec2pulse, etc
*/

typedef struct {
  char wv[64]; /*type of softpulse, names in wavelib*/
  
  int ref_sc; /*reference coarse power (scalar, in Ip terms) in dB. e.g. 43, will use  
            -attn 43I, which means keep coarse power at 45dB, while internally 
            attenuate for shaping*/
  double ref_pw; /*reference 90deg pulsewidth at given by coarse power ref_sc and max fine 
               power 4095*/
  
  double pw; /*pulse width in us, bw*pw=1.85; if a 180 hard pulse follows the
              rsnob pulse, to keep polarization from decay, need to set rsnob
              pulse width to be n+0.5 rotor period, see Ying Li's paper */
  double of; /*frequency offset, in Hz*/
  
  int st;
  
  double ph; /*constant phase; soft pulse has additional phase which
               is not zero even ph is 0, and the value dependent on pulse
               width. In some application, you want to set ph to compensate it.
             */
   
  char pattern[32];
} SOFTPULSE;

SOFTPULSE getsoftpulse(char *name) 
/* suppose name = "Nabcdefghijkl": the longest name have 14 letters (don't
count the trailing '\0'. The first letter is for the channel involved
in cp, the rest up to 12 letters are for the suffix---if needed. The longest
vnmr parameters are such as fHamtppmabcdefghijkl, containing 20 letters. */
{
 int logic = 0;
 SOFTPULSE shp; 
 char up[2], suffix[13], var[21], cmd[MAXSTR];

 if (strlen(name) >= 1 && strlen(name) <= 13) {
   strcpy(suffix,&(name[1]));

   up[1] = '\0'; up[0]=(char) toupper(name[0]);    
 } else {
   printf("Abort in getrsnob(): Bad name %s. At least one letter, but no more than 13.\n",name);
   exit(-1);
 }
 

/* logic = (Getval("arraydim") < 1.5 || ix==1); */
   logic = (ix==1); 

/* wvCsuffix */
   sprintf(var,"wv%s%s",up,suffix); Getstr(var,shp.wv,sizeof(shp.wv));
   logic = (logic || Isarry(var));   
/* ref_scCsuffix */
   sprintf(var,"ref_sc%s%s",up,suffix); shp.ref_sc = (int) (Getval(var));
   logic = (logic || Isarry(var));    
/* ref_pwCsuffix */
   sprintf(var,"ref_pw%s%s",up,suffix); shp.ref_pw = Getval(var)*1e6;  
   logic = (logic || Isarry(var)); 
/* pwCsuffix */
   sprintf(var,"pw%s%s",up,suffix); shp.pw = Getval(var);  
   logic = (logic || Isarry(var)); 
/* ofCsuffix */
   sprintf(var,"of%s%s",up,suffix); shp.of = Getval(var);  
   logic = (logic || Isarry(var));   
/* stCsuffix */
   sprintf(var,"st%s%s",up,suffix); shp.st = Getval(var);  
   logic = (logic || Isarry(var));   
/* phCsuffix */
   sprintf(var,"ph%s%s",up,suffix); shp.ph = Getval(var);  
   logic = (logic || Isarry(var));        

 if (logic) {sprintf(shp.pattern,"%s%s%s_%ld",shp.wv,up,suffix,ix); }
 else {sprintf(shp.pattern,"%s%s%s_1",shp.wv,up,suffix); }
 
 if (logic) {
/*   sprintf(cmd,"Pbox %s.RF -w \"%s /%.7f %.2f %d %.2f\" -maxincr 10  -pl %d %f -attn %dI\n",shp.pattern,shp.wv,shp.pw,shp.of,shp.st,shp.ph,shp.ref_sc,shp.ref_pw,shp.ref_sc);*/
     sprintf(cmd,"Pbox %s.RF -w \"%s /%.7f %.2f %d %.2f\" -maxincr 10  -pl %d %f\n",shp.pattern,shp.wv,shp.pw,shp.of,shp.st,shp.ph,shp.ref_sc,shp.ref_pw);
   system(cmd);
   printf("bandwidth is %.0f\n",1.85/shp.pw);
 }
 return shp;
}


