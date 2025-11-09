/* include this file after solidsmiscs.h */
/*..........A collection of WFG pattern creating programs by..........
Donghua H. Zhou/Rienstra NMR Group, UIUC/ March 20 ,2004


Will rewrite introduction.
......................................................................*/


/*~~~~~~~~~~~~~~~~~~~~~~~~~~tppm~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void make_tppm(TPPM dec) /* create the shapefile */
{
 char shapepath[MAXSTR],str[MAXSTR];
 FILE *fp;
 extern char userdir[]; 
 
 int i, n; double norm, amp1, amp2;

 sprintf(shapepath,"%s/shapelib/",userdir);
 sprintf(str,"%s%s.DEC",shapepath,dec.pattern);
 

if((fp = fopen(str,"w"))==NULL){
   printf("Error in make_tppm(): can not create file %s!\n", str);
   exit(-1);
}

 
if (!strcmp(dec.m,"raw")) {
 
 fprintf(fp,"#Donghua H. Zhou/ Rienstra NMR Group, UIUC/ March 29, 2004\n\
#      length       phase   amplitude   gate \n\
# ------------------------------------------------- \n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
",dec.ph,360.0-dec.ph);

} /* end "raw" */


else if (!strcmp(dec.m,"am")) {
  if (dec.a+dec.aAM > 4095) {
     printf("Warning from make_tppm(): a + aAM > 4095! Watch for deformed shape.\n");
  }
  else if (dec.a-dec.aAM < 0) {
     printf("Waring from make_tppm(): a - aAM < 0! Watch for deformed shape.\n");
  }
  
  n = (int) ((dec.nAMcyc/dec.fAM)/(2*dec.pw)+0.5);
  norm = 1023.0/(dec.a+dec.aAM);

 fprintf(fp,"#Donghua H. Zhou/ Rienstra NMR Group, UIUC/ March 29, 2004\n\
#      length       phase   amplitude   gate \n\
# ------------------------------------------------- \n\
");

  for (i=0; i<n; i++) {
    amp1 = norm*(dec.a+dec.aAM*sin(2*M_PI*dec.fAM*(2*i)*dec.pw)); 
    amp2 = norm*(dec.a+dec.aAM*sin(2*M_PI*dec.fAM*(2*i+1)*dec.pw));
    amp1 = (double) ( (int) (amp1 + 0.5) );
    amp2 = (double) ( (int) (amp2 + 0.5) );
    if (amp1 < 0.) amp1 = 0.; else if (amp1 > 1023.) amp1 = 1023.;
    if (amp2 < 0.) amp2 = 0.; else if (amp2 > 1023.) amp2 = 1023.;
 
    fprintf(fp,"     90.0    %6.1f  %6.1f    1\n\
     90.0    %6.1f  %6.1f    1\n", dec.ph, amp1, 360.0-dec.ph, amp2); 
   } /*in normal operation condition of TPPM, amp is always above zero, so
       we can set all gate value to 1 */
 } /*else if */ 
fclose(fp);
}

void _tppm(TPPM dec)
{
  int chnl; double aTPPM=dec.a;

  if (!strcmp(dec.m,"raw")) aTPPM = dec.a;
  else if (!strcmp(dec.m,"am")) aTPPM = dec.a + dec.aAM;
  
  if (!strcmp(dec.ch,"obs")) chnl = 1;
  else if (!strcmp(dec.ch,"dec")) chnl = 2;
  else if (!strcmp(dec.ch,"dec2")) chnl = 3;
  else if (!strcmp(dec.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
     switch (chnl) {
       case 1: obspwrf(aTPPM); obsprgon(dec.pattern,dec.pw,90.0); break;
       case 2: decpwrf(aTPPM); decprgon(dec.pattern,dec.pw,90.0); break;
       case 3: dec2pwrf(aTPPM); dec2prgon(dec.pattern,dec.pw,90.0); break;
       case 4: dec3pwrf(aTPPM); dec3prgon(dec.pattern,dec.pw,90.0); break;
       default: decpwrf(aTPPM); decprgon(dec.pattern,dec.pw,90.0); break;
     }    
}

 
void _tppm__(TPPM dec, double tDEC, double aPost)
{

/* tppm should give exact tDEC, unless tDEC<0.5u, in which case the total 
   duration would be 0.5u.
*/

  int ncyc, chnl;
  double dPost, dPM, aTPPM=dec.a;
  double dAP =0.5e-6, dSum = 3.2e-6; 

/* decpwrf(aTPPM) 0.5u, one AP bus word takes 0.5u
   decprgon() 0.5, wfg onboard delay 0.75, 0.45 offset delay (sum=1.7u)
   decprgoff() 0.5 
   decpwrf(aPost) 0.5
   total 3.2u
*/
   dSum = dSum - 0.45e-6;
  /* Identify that we shall not count offset delay in this. Jun 24, 2005*/


  if (!strcmp(dec.m,"raw")) aTPPM = dec.a;
  else if (!strcmp(dec.m,"am")) aTPPM = dec.a + dec.aAM;
 
  if (!strcmp(dec.ch,"obs")) chnl = 1;
  else if (!strcmp(dec.ch,"dec")) chnl = 2;
  else if (!strcmp(dec.ch,"dec2")) chnl = 3;
  else if (!strcmp(dec.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
  if (tDEC < 2*dAP) {
         dPost=tDEC-dAP; 
         if (dPost > 0) delay(dPost);    
         switch (chnl) {
         case 1: obspwrf(aPost);   break;
         case 2: decpwrf(aPost);   break;
         case 3: dec2pwrf(aPost);  break;
         case 4: dec3pwrf(aPost);  break;
         default: decpwrf(aPost);  break;
   } 
 
    return;
 }

    
  if (tDEC > 2*dec.pw + dSum) {
       ncyc = (int)((tDEC-dSum )/(2*dec.pw));
       dPM=ncyc*2*dec.pw; 
       dPost = tDEC-dPM-dSum; 
   } 
   else {ncyc = 0; dPM=0; dPost=tDEC-2*dAP;} 
   /*only decpwrf(aTPPM) and decpwrf(aPost)*/
   
     switch (chnl) {
       case 1: obspwrf(aTPPM);  break;
       case 2: decpwrf(aTPPM);  break;
       case 3: dec2pwrf(aTPPM); break;
       case 4: dec3pwrf(aTPPM); break;
       default: decpwrf(aTPPM); break;
     }

   if (ncyc > 0) {
     switch (chnl) {
       case 1: obsprgon(dec.pattern,dec.pw,90.0);   break;
       case 2: decprgon(dec.pattern,dec.pw,90.0);   break;
       case 3: dec2prgon(dec.pattern,dec.pw,90.0);  break;
       case 4: dec3prgon(dec.pattern,dec.pw,90.0);  break;
       default:  decprgon(dec.pattern,dec.pw,90.0); break;
     }
     delay(dPM);
    }
     
   if (ncyc > 0) {
     switch (chnl) {
       case 1: obsprgoff();   break;
       case 2: decprgoff();   break;
       case 3: dec2prgoff();  break;
       case 4: dec3prgoff();  break;
       default: decprgoff();  break;
     }
    }
    
       delay(dPost);
       switch (chnl) {
         case 1: obspwrf(aPost);   break;
         case 2: decpwrf(aPost);   break;
         case 3: dec2pwrf(aPost);  break;
         case 4: dec3pwrf(aPost);  break;
         default: decpwrf(aPost);  break;
       }
}

#define _tppm_(dec, tDEC) _tppm__(dec,tDEC,dec.a)
   
 /*~~~~~~~~~~~~~~~~~~~~~~~end of tppm~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
 
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~SPINAL64~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 /* edited by AJN LJS 12/02/08, using loops to create phase files from
 Dave Rice's soliddecshapes.h */
 
void make_spinal(SPINAL dec) /* create the shapefile */
{
 char shapepath[MAXSTR],str[MAXSTR];
 FILE *fp;
 extern char userdir[]; 
 
 /* new variables for loops */
 int n,i ;
 int sign[8] = {1,-1,-1,1,-1,1,1,-1};
 

 sprintf(shapepath,"%s/shapelib/",userdir);
 sprintf(str,"%s%s.DEC",shapepath,dec.pattern);
 

if((fp = fopen(str,"w"))==NULL){
   printf("Error in make_spinal(): can not create file %s!\n", str);
   exit(-1);
}

 
/* Donghua's old "loop" removed to make easier to use alpha and beta
AJN LJS, 12/02/08 
 
 fprintf(fp,"#Donghua H. Zhou/ Rienstra NMR Group, UIUC/ March 29, 2004\n\
#      length       phase   amplitude   gate \n\
# ------------------------------------------------- \n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
",10.0,350.0,15.0,345.0,20.0,340.0,15.0,345.0,
  350.0,10.0,345.0,15.0,340.0,20.0,345.0,15.0);
fprintf(fp,
"        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
",350.0,10.0,345.0,15.0,340.0,20.0,345.0,15.0,
  10.0,350.0,15.0,345.0,20.0,340.0,15.0,345.0 );
fprintf(fp,
"        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
",350.0,10.0,345.0,15.0,340.0,20.0,345.0,15.0,
  10.0,350.0,15.0,345.0,20.0,340.0,15.0,345.0);
fprintf(fp,
"        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
        90.0       %6.1f    1023.000        1\n\
",10.0,350.0,15.0,345.0,20.0,340.0,15.0,345.0,
  350.0,10.0,345.0,15.0,340.0,20.0,345.0,15.0);    
end of Donghua's "loop" */

/* New sexier loop */

   for (i = 0; i<8; i++) {
      n = i%8;
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph+dec.alpha));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph-dec.alpha));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph+dec.beta));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph-dec.beta));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(dec.ph+dec.alpha));
      fprintf(fp," 90.0 %10.3f\n", sign[n]*(-dec.ph-dec.alpha));
   }

fclose(fp);
}

void _spinal(SPINAL dec)
{
  int chnl; double aSPINAL=dec.a;

   aSPINAL = dec.a;
   
  if (!strcmp(dec.ch,"obs")) chnl = 1;
  else if (!strcmp(dec.ch,"dec")) chnl = 2;
  else if (!strcmp(dec.ch,"dec2")) chnl = 3;
  else if (!strcmp(dec.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
     switch (chnl) {
       case 1: obspwrf(aSPINAL); obsprgon(dec.pattern,dec.pw,90.0); break;
       case 2: decpwrf(aSPINAL); decprgon(dec.pattern,dec.pw,90.0); break;
       case 3: dec2pwrf(aSPINAL); dec2prgon(dec.pattern,dec.pw,90.0); break;
       case 4: dec3pwrf(aSPINAL); dec3prgon(dec.pattern,dec.pw,90.0); break;
       default: decpwrf(aSPINAL); decprgon(dec.pattern,dec.pw,90.0); break;
     }    
}

 
void _spinal__(SPINAL dec, double tDEC, double aPost)
{

/* tppm should give exact tDEC, unless tDEC<0.5u, in which case the total 
   duration would be 0.5u.
*/

  int ncyc, chnl;
  double dPost, dPM, aSPINAL=dec.a;
  double dAP =0.5e-6, dSum = 3.2e-6; 

/* decpwrf(aSPINAL) 0.5u, one AP bus word takes 0.5u
   decprgon() 0.5, wfg onboard delay 0.75, 0.45 offset delay (sum=1.7u)
   decprgoff() 0.5 
   decpwrf(aPost) 0.5
   total 3.2u
*/
   dSum = dSum - 0.45e-6;
  /* Identify that we shall not count offset delay in this. Jun 24, 2005*/


   aSPINAL = dec.a;
  
  if (!strcmp(dec.ch,"obs")) chnl = 1;
  else if (!strcmp(dec.ch,"dec")) chnl = 2;
  else if (!strcmp(dec.ch,"dec2")) chnl = 3;
  else if (!strcmp(dec.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
  if (tDEC < 2*dAP) {
         dPost=tDEC-dAP; 
         if (dPost > 0) delay(dPost);    
         switch (chnl) {
         case 1: obspwrf(aPost);   break;
         case 2: decpwrf(aPost);   break;
         case 3: dec2pwrf(aPost);  break;
         case 4: dec3pwrf(aPost);  break;
         default: decpwrf(aPost);  break;
   } 
 
    return;
 }

    
  if (tDEC > 64*dec.pw + dSum) {
       ncyc = (int)((tDEC-dSum )/(64*dec.pw));
       dPM=ncyc*64*dec.pw; 
       dPost = tDEC-dPM-dSum; 
   } 
   else {ncyc = 0; dPM=0; dPost=tDEC-2*dAP;} 
   /*only decpwrf(aSPINAL) and decpwrf(aPost)*/
   
     switch (chnl) {
       case 1: obspwrf(aSPINAL);  break;
       case 2: decpwrf(aSPINAL);  break;
       case 3: dec2pwrf(aSPINAL); break;
       case 4: dec3pwrf(aSPINAL); break;
       default: decpwrf(aSPINAL); break;
     }

   if (ncyc > 0) {
     switch (chnl) {
       case 1: obsprgon(dec.pattern,dec.pw,90.0);   break;
       case 2: decprgon(dec.pattern,dec.pw,90.0);   break;
       case 3: dec2prgon(dec.pattern,dec.pw,90.0);  break;
       case 4: dec3prgon(dec.pattern,dec.pw,90.0);  break;
       default:  decprgon(dec.pattern,dec.pw,90.0); break;
     }
     delay(dPM);
    }
     
   if (ncyc > 0) {
     switch (chnl) {
       case 1: obsprgoff();   break;
       case 2: decprgoff();   break;
       case 3: dec2prgoff();  break;
       case 4: dec3prgoff();  break;
       default: decprgoff();  break;
     }
    }
    
       delay(dPost);
       switch (chnl) {
         case 1: obspwrf(aPost);   break;
         case 2: decpwrf(aPost);   break;
         case 3: dec2pwrf(aPost);  break;
         case 4: dec3pwrf(aPost);  break;
         default: decpwrf(aPost);  break;
       }
}

#define _spinal_(dec, tDEC) _spinal__(dec,tDEC,dec.a)
   
 /*~~~~~~~~~~~~~~~~~~~~~~~end of SPINAL~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void _waltz16(WALTZ16 dec)
{
  int chnl; 

  if (!strcmp(dec.ch,"obs")) chnl = 1;
  else if (!strcmp(dec.ch,"dec")) chnl = 2;
  else if (!strcmp(dec.ch,"dec2")) chnl = 3;
  else if (!strcmp(dec.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
     switch (chnl) {
       case 1: obspwrf(dec.a); obsprgon(dec.pattern,dec.pw,90.0); break;
       case 2: decpwrf(dec.a); decprgon(dec.pattern,dec.pw,90.0); break;
       case 3: dec2pwrf(dec.a); dec2prgon(dec.pattern,dec.pw,90.0); break;
       case 4: dec3pwrf(dec.a); dec3prgon(dec.pattern,dec.pw,90.0); break;
       default: decpwrf(dec.a); decprgon(dec.pattern,dec.pw,90.0); break;
     }    
}

 
void _waltz16__(WALTZ16 dec, double tDEC, double aPost)
{
  int chnl;
  double duration;

/* decpwrf() 0.5u, one AP bus word takes 0.5u
   decprgon() 0.5, wfg onboard delay 0.75, 0.45 offset delay (sum=1.7u)
   decprgoff() 0.5 --execute this 0.5u ahead of the to compensate, 
                  
*/
  duration = tDEC-2.0e-6; /*approximate*/

  if (!strcmp(dec.ch,"obs")) chnl = 1;
  else if (!strcmp(dec.ch,"dec")) chnl = 2;
  else if (!strcmp(dec.ch,"dec2")) chnl = 3;
  else if (!strcmp(dec.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
  if (tDEC < 1e-6) {
    if (dec.a != aPost) {     
         switch (chnl) {
         case 1: obspwrf(aPost);   break;
         case 2: decpwrf(aPost);   break;
         case 3: dec2pwrf(aPost);  break;
         case 4: dec3pwrf(aPost);  break;
         default: decpwrf(aPost);  break;
       }
    }
    return;
  }
  
   
     switch (chnl) {
       case 1: obspwrf(dec.a);  break;
       case 2: decpwrf(dec.a);  break;
       case 3: dec2pwrf(dec.a); break;
       case 4: dec3pwrf(dec.a); break;
       default: decpwrf(dec.a); break;
     }

     switch (chnl) {
       case 1: obsprgon(dec.pattern,dec.pw,90.0);   break;
       case 2: decprgon(dec.pattern,dec.pw,90.0);   break;
       case 3: dec2prgon(dec.pattern,dec.pw,90.0);  break;
       case 4: dec3prgon(dec.pattern,dec.pw,90.0);  break;
       default:  decprgon(dec.pattern,dec.pw,90.0); break;
     }
     delay(duration);
     switch (chnl) {
       case 1: obsprgoff();   break;
       case 2: decprgoff();   break;
       case 3: dec2prgoff();  break;
       case 4: dec3prgoff();  break;
       default: decprgoff();  break;
     }


       switch (chnl) {
         case 1: obspwrf(aPost);   break;
         case 2: decpwrf(aPost);   break;
         case 3: dec2pwrf(aPost);  break;
         case 4: dec3pwrf(aPost);  break;
         default: decpwrf(aPost);  break;
       }
}

#define _waltz16_(dec, tDEC) _waltz16__(dec,tDEC,dec.a)
   
 /*~~~~~~~~~~~~~~~~~~~~~~~end of waltz16~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~ Droopy ~~~~~~~*/
void make_droopy(DROOPY dec) 
{
 char shapepath[MAXSTR];
 double amp0=-1000, amp=-1000, norm, ph0=-1000, ph=-1000;
 FILE *fp;
 extern char userdir[];
 int i, n, count=0, GTn=4;

 int  n0, i0=0; 
 double k1=5,k2=10,PH[100],max=-2000;

 if (strcmp(dec.m,"am") != 0) dec.aAM = 0.0;

 norm = 1023.0/(dec.a + dec.aAM);

  sprintf(shapepath,"%s/shapelib/%s.DEC",userdir,dec.pattern);
  if((fp = fopen(shapepath,"w"))==NULL){
     printf("Error in make_dec(): can not create file %s!\n", shapepath);
     exit(-1);
  } 

 fprintf(fp,"#Donghua H. Zhou/ Rienstra NMR Group, UIUC/ Apr 27, 2004\n\
#      length       phase   amplitude   gate \n\
# ------------------------------------------------- \n\
");

    if (dec.a+dec.aAM > 4095.0)
     printf("Warning from make_droopy(): Watch for deformed shape.\n");

    n = (int)( 1.0/(dec.f*dec.res)+0.5 ); dec.f = 1.0/(n*dec.res);
    n0 =n;
    if (!strcmp(dec.m,"GT2")) {GTn=2; n=n*2;}
    if (!strcmp(dec.m,"GT4")) {GTn=4; n=n*4;}
    if (!strcmp(dec.m,"GT8")) {GTn=8; n=n*8;}
    if (!strcmp(dec.m,"GT16")) {GTn=8; n=n*16;}

    if (!strcmp(dec.m,"am")) {
      n = (int) (1.0/(dec.fAM*dec.res) + 0.5 ); dec.fAM = 1.0/(n*dec.res);
    } /* f ~ 100kHz, fAM ~ 10 kHz */
 
/* for damped tanh */    
    for (i=0; i<n0;i++) {
      ph = ((double)i)/n0 - 0.5;
      ph = -tanh( k1*ph ) * exp(-k2*ph*ph);
      PH[i] = ph; 
      /*area = area + fabs(ph);*/
      if (max < ph) max = ph;
    }
    for (i=0; i< n0; i++) {
      PH[i] = dec.ph * PH[i]/max;
     /*PH[i] = dec.ph * PH[i ]* n0/area; */
   }
/* .... */

    for (i = 0; i < n; i++) {
      amp = norm*( dec.a + dec.aAM*sin(2*M_PI*dec.fAM*i*dec.res) );
      amp = (double) ( (int) (amp + 0.5) );
      if (amp < 0.0) amp = 0.0; else if (amp > 1023.0) amp = 1023.0;
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ___(1) cos  the original droopy */
     /*ph = dec.ph*cos(2*M_PI*dec.f*i*dec.res);  */

/*___ (2) cos^2 */
   /* ph = cos(2*M_PI*dec.f*i*dec.res); ph = dec.ph*ph*ph; */

/*___ (3) linear, saw, corresponding to constant frequcency sweep */
       i0 = i - ((int) (i/n0) ) * n0;
     /*  ph = -dec.ph + fabs(n0/2.0 -i0)*(2.0*dec.ph)/(n0/2.0); */  /*linear down up */
    /*   ph = dec.ph - (2.0*dec.ph/n0)*i0;  half of the linear version*/

    /* ph = -dec.ph + pow(n0/2.0 -i0,2.0)*(8.0*dec.ph)/(n0*n0); */ /*quad*/
   /*  ph = -dec.ph + fabs(pow(n0/2.0-i0,3.0))*(16.0*dec.ph)/(n0*n0*n0);*/ /*cubic*/

      /*ph = dec.ph * tanh( k1*(0.5-((double)i0)/n0) )/tanh(0.5*k1);*/      /* tanh, most similar to tppm */

      ph = PH[i0]; /* damped tanh */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      if (dec.m[0] == 'G' && dec.m[1] == 'T') { 
        ph = ph*exp(4.0/(GTn*GTn)*log(0.1)*dec.f*dec.f * (i*dec.res-0.5*GTn/dec.f)*(i*dec.res-0.5*GTn/dec.f));}

      ph = ph - ((int) (ph/360.0)) * 360.0;
      if (ph < 0.0) ph = 360.0 + ph;  ph = ((int) (ph*10.0 + 0.5))/10.0;

      if (i == 0) {amp0 = amp;  ph0=ph;  count = 0;}

         
      /*printf("%d %d %f %f\n",i,count,ph,amp);*/
      if ( amp != amp0 || ph != ph0) { 
        fprintf(fp,"     %6.1f    %6.1f  %6.1f    1\n", count*9.0, ph0, amp0); 
        amp0 = amp; ph0 = ph;  count = 0;
      } 
      count++;

    } /*end for loop */      
    if (count > 0) {fprintf(fp,"     %6.1f    %6.1f  %6.1f    1\n", count*9.0, ph, amp); }

  fclose(fp);

}

void _droopy(DROOPY dec)
{
  int chnl; double aDRP=dec.a;

  if(strcmp(dec.m,"am") != 0)  aDRP = dec.a; /* "raw" or "GT4" */
  else aDRP = dec.a + dec.aAM; /* "am" */
  
  if (!strcmp(dec.ch,"obs")) chnl = 1;
  else if (!strcmp(dec.ch,"dec")) chnl = 2;
  else if (!strcmp(dec.ch,"dec2")) chnl = 3;
  else if (!strcmp(dec.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
     switch (chnl) {
       case 1: obspwrf(aDRP); obsprgon(dec.pattern,dec.res*10.0,9.0); break;
       case 2: decpwrf(aDRP); decprgon(dec.pattern,dec.res*10.0,9.0); break;
       case 3: dec2pwrf(aDRP); dec2prgon(dec.pattern,dec.res*10.0,9.0); break;
       case 4: dec3pwrf(aDRP); dec3prgon(dec.pattern,dec.res*10.0,9.0); break;
       default: decpwrf(aDRP); decprgon(dec.pattern,dec.res*10.0,9.0); break;
     }    
}

void _droopy__(DROOPY dec, double tDEC, double aPost)
{
  int ncyc, chnl;
  double dPre, dPost, dDRP, aDRP=dec.a;
  double dAP =0.5e-6; 

/* decpwrf() 0.5u, one AP bus word takes 0.5u
   decprgon() 0.5, wfg onboard delay 0.75, 0.45 offset delay (sum=1.7u)
   decprgoff() 0.5 --execute this 0.5u ahead of the to compensate, 
                  dPM=ncyc*2*pwPM-dAP;
*/

  if(strcmp(dec.m,"am") != 0)  aDRP = dec.a; /* "raw" or "GT4" */
  else aDRP = dec.a + dec.aAM; /* "am" */
 
  if (!strcmp(dec.ch,"obs")) chnl = 1;
  else if (!strcmp(dec.ch,"dec")) chnl = 2;
  else if (!strcmp(dec.ch,"dec2")) chnl = 3;
  else if (!strcmp(dec.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
  if (tDEC < 1e-6) {
    if (dec.a != aPost) {     
         switch (chnl) {
         case 1: obspwrf(aPost);   break;
         case 2: decpwrf(aPost);   break;
         case 3: dec2pwrf(aPost);  break;
         case 4: dec3pwrf(aPost);  break;
         default: decpwrf(aPost);  break;
       }
    }
    return;
  }
  if (tDEC > 1.0/dec.f + 2.2e-6) {
     if (tDEC > 1.0/dec.f + 4.4e-6) {
       ncyc = (int)((tDEC-4.4e-6)*dec.f);
       dPost = (tDEC-ncyc/dec.f)/2.0; 
       dPre = dPost-2.2e-6; 
       dDRP=ncyc/dec.f - dAP; 
     } else {
       ncyc=1;   dDRP=ncyc/dec.f - dAP;
       dPre=0.0; dPost=tDEC-1.0/dec.f-2.2e-6;
     }
   } else {ncyc = 0;dPre=0.5e-6; dPost=0.0; dDRP=tDEC-2*dAP;}
    if (dPre < 0) dPre = 0; if (dDRP < 0)  dDRP = 0; 
   
     switch (chnl) {
       case 1: obspwrf(aDRP);  break;
       case 2: decpwrf(aDRP);  break;
       case 3: dec2pwrf(aDRP); break;
       case 4: dec3pwrf(aDRP); break;
       default: decpwrf(aDRP); break;
     }

     dps_skip(); delay(dPre);
     switch (chnl) {
       case 1: obsprgon(dec.pattern,dec.res*10.0,9.0);   break;
       case 2: decprgon(dec.pattern,dec.res*10.0,9.0);   break;
       case 3: dec2prgon(dec.pattern,dec.res*10.0,9.0);  break;
       case 4: dec3prgon(dec.pattern,dec.res*10.0,9.0);  break;
       default:  decprgon(dec.pattern,dec.res*10.0,9.0); break;
     }
     delay(dDRP);
     switch (chnl) {
       case 1: obsprgoff();   break;
       case 2: decprgoff();   break;
       case 3: dec2prgoff();  break;
       case 4: dec3prgoff();  break;
       default: decprgoff();  break;
     }

     if (dec.a == aPost) {dps_skip(); delay(dPost);}
     else {
       dPost = dPost - dAP;
       if (dPost < 0) dPost = 0;
       dps_skip(); delay(dPost);
       switch (chnl) {
         case 1: obspwrf(aPost);   break;
         case 2: decpwrf(aPost);   break;
         case 3: dec2pwrf(aPost);  break;
         case 4: dec3pwrf(aPost);  break;
         default: decpwrf(aPost);  break;
       }
     }
}

#define _droopy_(dec, tDEC) _droopy__(dec,tDEC,dec.a)

 /*~~~~~~~~~~~~~~~~~~~~~~ cp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void make_cp(CP cp)
{
 char shapepath[MAXSTR];
 double mean, amp0=-2000, amp=-1000, norm, ph0=-1000, ph=-1000;
 FILE *fp;
 extern char userdir[];
 int i, count=0, mode;

 if (!strcmp(cp.device,"attn")) return;

 if (!strcmp(cp.ch,"fr") ) {mean = cp.a1;}
 else {mean=cp.a2;}


 if (!strcmp(cp.m,"normal")) mode = 0;
 else if (!strcmp(cp.m,"down")) mode = -1;
 else if (!strcmp(cp.m,"up")) mode = 1;
 else {printf("Error from make_cp(): unknown tangent shape style %s, abort!\n",cp.m);
   abort();}
 
 if (strcmp(cp.sh,"c") == 0) {cp.b = 0.0; cp.d=0;}
 else if (strcmp(cp.sh,"l") == 0) cp.b = 1e8; /* only local copy is changed*/

 norm = 1023.0/(mean+fabs(cp.d));
 if (mode == -1) norm = 1023.0/mean;

  sprintf(shapepath,"%s/shapelib/%s.DEC",userdir,cp.pattern);
  if((fp = fopen(shapepath,"w"))==NULL){
     printf("Error in make_cp(): can not create file %s!\n", shapepath);
     exit(-1);
  } 

 fprintf(fp,"#Donghua H. Zhou/ Rienstra NMR Group, UIUC/ March 29, 2004\n\
#      length       phase   amplitude   gate \n\
# ------------------------------------------------- \n\
");

  if (cp.b == 0.0 || cp.d == 0.0) {
    fprintf(fp,"     9.0    0.0  1023.0    1\n");
  } 
  else {
    if (mode != -1 && mean+fabs(cp.d) > 4095.0)
     printf("Warning from make_cp(): mean + delta > 4095! Watch for deformed shape.\n");
    if (mode != 1 && mean - fabs(cp.d) < 0)
     printf("Warning from make_cp(): mean - delta < 0! Watch for deformed shape.\n");     

    for (i = 0; i < cp.n; i++) {
      switch (mode) {
       case 0:
         amp = norm*(mean - cp.b*tan( atan(cp.d/cp.b)*(1.0-(2.0*i+1.0)/cp.n) )); break;
       case -1:
         amp = norm*(mean - fabs(cp.b*tan( atan(cp.d/cp.b)*(1.0-(2.0*i+1.0)/cp.n) ))); break;
       case 1:
         amp = norm*(mean + fabs(cp.b*tan( atan(cp.d/cp.b)*(1.0-(2.0*i+1.0)/cp.n) ))); break;
       default: 
         amp = norm*(mean - cp.b*tan( atan(cp.d/cp.b)*(1.0-(2.0*i+1.0)/cp.n) ));  break;
      }

      amp = (double) ( (int) (amp + 0.5) );
      if (amp < 0.0) amp = 0.0; else if (amp > 1023.0) amp = 1023.0;

/* 	This phase was modified and tested for R2T only. We should
      rewrite a dedicated code for R2T or Dream.  
      ph = 360.0*cp.of*(i-(cp.n/2.0))*cp.t/cp.n; */

/* this is need for regular cp!!! DHZ 3/20/2007 */
ph = 360.0*cp.of*i*cp.t/cp.n; 
      
      ph = ph - ((int) (ph/360.0)) * 360.0;
      if (ph < 0.0) ph = 360.0 + ph;  
      if (ph > 360.0) ph = ph - 360.0;
      ph = ((int) (ph*10.0 + 0.5))/10.0;


/*    Note: This was the previous method for a CP off set which gave a greater CP efficiency but
      created a phase shift at the end of the CP period.  
     
      ph = 360.0*cp.of*i*cp.t/cp.n; ph = ph - ((int) (ph/360.0)) * 360.0;
      if (ph < 0.0) ph = 360.0 + ph;  ph = ((int) (ph*10.0 + 0.5))/10.0;
*/     

      if (i == 0) {amp0 = amp; ph0 = ph; count = 0;}
         
      if ( amp != amp0 || ph != ph0) { 
        fprintf(fp,"     %6.1f    %6.1f  %6.1f    1\n", count*9.0, ph0, amp0); 
        amp0 = amp; ph0 = ph;  count = 0;
      } 
      count++;  
    } /*end for loop */
    if (count > 0) {fprintf(fp,"     %6.1f    %6.1f  %6.1f    1\n", count*9.0, ph, amp); }
   
  } /* end else */

  
  fclose(fp);
 }

void cp_attn(CP cp)
/* to create and execute tangent shape by attenuation board --- only called by _cp_( ) */
{
  double minDur = 1e-6, dDur, aR, aC;
  double *Amp, *Dur, amp=-1000, amp0=-1000, dAP = 0.5e-6;
  int i, count=0, n=0, maxN = 1000, chR, chC;
  char  *chRamp, *chConst;

   if (cp.of != 0.0) {printf("Error from cp_attn(): to use frequency offset, \n\
you need to choose 'wfg' instead of 'attn' for device.\n");  abort();}
   if (!strcmp(cp.m,"up") || !strcmp(cp.m,"down")) {printf("Error from cp_attn(): to use\n\
tangent shape mode other than 'normal', you need to choose 'wfg' instead of 'attn' for device.\n");
   abort(); }
 if (!strcmp(cp.ch,"fr") ) {aR = cp.a1; aC = cp.a2; chRamp = cp.fr; chConst = cp.to;}
 else {aR = cp.a2; aC = cp.a1; chRamp = cp.to; chConst = cp.fr;}
 
    if (strcmp(chRamp,"obs") == 0) chR = 1;
    else if (strcmp(chRamp,"dec") == 0) chR = 2;
    else if (strcmp(chRamp,"dec2") == 0) chR = 3;
    else if (strcmp(chRamp,"dec3") == 0) chR = 4;
    else chR = 1;
    
    if (strcmp(chConst,"obs") == 0) chC = 1;
    else if (strcmp(chConst,"dec") == 0) chC = 2;
    else if (strcmp(chConst,"dec2") == 0) chC = 3;
    else if (strcmp(chConst,"dec3") == 0) chC = 4;
    else chC = 2; 

 if (strcmp(cp.sh,"c") == 0) {cp.b = 0.0; cp.d=0;}
 else if (strcmp(cp.sh,"l") == 0) cp.b = 1e8; /* only local copy is changed*/

 if (cp.n > maxN) cp.n = maxN; 
 if (cp.t/cp.n < minDur) {cp.n = (int) (cp.t/minDur + 0.5);}
 dDur = cp.t/cp.n;

 Amp = (double *) malloc(cp.n * sizeof(double));
 Dur = (double *) malloc(cp.n * sizeof(double));

 if (cp.b == 0 || cp.d == 0) {
    n = 1; Amp[0] = aR; Dur[0] = cp.t; }
 else {
    for (i = 0; i < cp.n; i++) {
      amp = aR - cp.b*tan( atan(cp.d/cp.b)*(1.0-(2.0*i+1.0)/cp.n) );
      amp = (double) ( (int) (amp + 0.5) );
      if (amp < 0.0) amp = 0.0; else if (amp > 4095.0) amp = 4095.0;

      if (i == 0) {amp0=amp; count=0;}
        
      if ( amp != amp0) { 
        Amp[n] = amp0; Dur[n] = dDur * count - dAP; n++;
        amp0 = amp; count = 0;
      }       count++;
    } /*end for loop */
    if (count>0) {Amp[n]=amp; Dur[n] = dDur*count - dAP;}
 }

    switch(chC) {
       case 1: obspwrf(aC);  xmtron(); break;
       case 2: decpwrf(aC);  decon();  break;
       case 3: dec2pwrf(aC); dec2on(); break;
       case 4: dec3pwrf(aC); dec3on(); break;
       default: decpwrf(aC);  decon();  break;
    }


        
    switch(chR) {
       case 1: xmtron(); for (i=0; i<n; i++) {
               obspwrf(Amp[i]); delay(Dur[i]); }
               xmtroff(); break;
       case 2: decon();  for (i=0; i<n; i++) {
               decpwrf(Amp[i]); delay(Dur[i]); }
               decoff(); break;
       case 3: dec2on();  for (i=0; i<n; i++) {
               dec2pwrf(Amp[i]); delay(Dur[i]); }
               dec2off(); break;
       case 4: dec3on();  for (i=0; i<n; i++) {
               dec3pwrf(Amp[i]); delay(Dur[i]); }
               dec3off(); break;
       default: decon();  for (i=0; i<n; i++) {
               decpwrf(Amp[i]); delay(Dur[i]); }
               decoff(); break;
    }
 

    switch(chC) {
       case 1:  xmtroff(); break;
       case 2:  decoff();  break;
       case 3:  dec2off(); break;
       case 4:  dec3off(); break;
       default: decoff(); break;
    }

 free(Amp); free(Dur);
}

void _cp_(CP cp)
{ 
    double aR,aC;
    char  *chRamp, *chConst;
    int steps, chR, chC;
    
    if (cp.t < 1e-6) {
      if (ix == 1) printf("Warning from _cp_(): CP contact time < 1 microsec, skip!\n");
      return;
    }

if (!strcmp(cp.device,"attn")) { cp_attn(cp); return;} /* jump out */    

    if (cp.t/cp.n < 50e-9) {
      printf("Error from _cp_(): CP stepsize < 50 nsec, aborted! Please use less steps.\n");
      abort();
    }   
      
    if (strcmp(cp.ch,"fr") == 0) {
      aR = cp.a1;  aC = cp.a2;
      chRamp = cp.fr; chConst = cp.to;
    }
    else { 
      aR = cp.a2;  aC = cp.a1;
      chRamp = cp.to; chConst = cp.fr;
    }  


    if (cp.sh[0] == 'c' || cp.b == 0.0 || cp.d == 0) {steps = 1;}
    else {steps = cp.n; aR = aR + fabs(cp.d); 
          if (!strcmp(cp.m,"down")) aR = aR -fabs(cp.d); }

    if (strcmp(chRamp,"obs") == 0) chR = 1;
    else if (strcmp(chRamp,"dec") == 0) chR = 2;
    else if (strcmp(chRamp,"dec2") == 0) chR = 3;
    else if (strcmp(chRamp,"dec3") == 0) chR = 4;
    else chR = 1;
    
    if (strcmp(chConst,"obs") == 0) chC = 1;
    else if (strcmp(chConst,"dec") == 0) chC = 2;
    else if (strcmp(chConst,"dec2") == 0) chC = 3;
    else if (strcmp(chConst,"dec3") == 0) chC = 4;
    else chC = 2;   

/* simply use delay(cp.t), the sevral microsecond hardware delays can be ignore in the usual CP conditions */

    switch(chC) {
       case 1: xmtron(); obspwrf(aC);   break;  /* turn on transmitter right away, before obspwrf, which*/
       case 2: decon();  decpwrf(aC);    break; /* takes 0.5u to execute and may result in loss of polarization*/
       case 3: dec2on(); dec2pwrf(aC);  break;
       case 4: dec3on(); dec3pwrf(aC);  break;
       default: decon(); decpwrf(aC);    break;
    }
    
    switch(chR) {
       case 1: obspwrf(aR); xmtron(); obsprgon(cp.pattern,cp.t/steps*10.0,9.0); 
               delay(cp.t); obsprgoff(); xmtroff(); break;
       case 2: decpwrf(aR); decon(); decprgon(cp.pattern,cp.t/steps*10.0,9.0);
               delay(cp.t); decprgoff(); decoff(); break;
       case 3: dec2pwrf(aR); dec2on(); dec2prgon(cp.pattern,cp.t/steps*10.0,9.0);
               delay(cp.t); dec2prgoff(); dec2off(); break;
       case 4: dec3pwrf(aR); dec3on(); dec3prgon(cp.pattern,cp.t/steps*10.0,9.0);
               delay(cp.t); dec3prgoff(); dec3off(); break;
       default: obspwrf(aR); xmtron(); obsprgon(cp.pattern,cp.t/steps*10.0,9.0);
                delay(cp.t); obsprgoff(); xmtroff(); break;
    }

    switch(chC) {
       case 1:  xmtroff(); break;
       case 2:  decoff();  break;
       case 3:  dec2off(); break;
       case 4:  dec3off(); break;
       default: decoff(); break;
    } 
} 
 /*~~~~~~~~~~~~~~~~~~~~~~~end of cp  ~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


void _dream_(DREAM dream)
{
    if (dream.drm.t < 1e-6) {
      if (ix == 1) printf("Warning from _dream_(): mixing time < 1 microsec, skip!\n");
      return;
    }  
   /* leave this contro to individual _cp_() call*/

 if (!strcmp(dream.in.device,"wfg"))  xmtron(); /*turn on before obspwrf and obsprgon to avoid 0.5u no rf delay */
 _cp_(dream.in);
 if (!strcmp(dream.drm.device,"wfg"))  xmtron();
 _cp_(dream.drm);
 if (!strcmp(dream.out.device,"wfg"))  xmtron();
 _cp_(dream.out);
}
 
 /*~~~~~~~~~~~~~~~~~~~~~~~~darr ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void make_darr(DARR mix, double *pwr, double *res)
 /* the second and third argument should be &(mix.pwr) and &(mix.res) when calling darr(). This
    function should only be called by getdarr() */
{ 
 char shapepath[MAXSTR];
 FILE *fp;
 extern char userdir[];
 double amp, phase;
 int i,j, steps=1, Steps = 11, gate=1; /* Steps used in "va" */
 
 sprintf(shapepath,"%s/shapelib/%s.DEC",userdir,mix.pattern);

 
if((fp = fopen(shapepath,"w"))==NULL){
   printf("Error in darr(): can not create file %s!\n", shapepath);
   exit(-1);
}

 fprintf(fp,"#Donghua H. Zhou/ Rienstra NMR Group, UIUC/ Apr 1, 2004\n\
#      length       phase   amplitude   gate \n\
# ------------------------------------------------- \n");

 if (!strcmp(mix.m,"raw")) { 
   mix.pwr = mix.a; mix.res = mix.t;
   fprintf(fp,"      90.0      0.0     1023.0      1\n"); } 
 else if (!strcmp(mix.m,"alt")) {
   mix.pwr = 2.0 * mix.a; mix.res = mix.t0;
   fprintf(fp,"      90.0      0.0     1023.0      1\n\
      90.0      0.0      511.5      1 \n"); }
 else if (!strcmp(mix.m,"sin")) {
   mix.pwr = mix.a; mix.res = 2e-6;
   steps = (int) ( 1.0/Getval("srate")/mix.n/mix.res + 0.5 );
     /* don't add 1 to steps, since we usually will repeat the pattern many
        times */
   for (i=0; i< steps; i++) {
   
     amp = 1023.0*sin(2*M_PI*i/steps);
     if (amp >= 0.0) phase = 0.0; else {phase = 180.0; amp = -amp;}
     amp = (double) ( (int) (amp + 0.5) );
     if (amp == 0.) gate = 0; else gate = 1;
     if (amp > 1023.) amp = 1023.;
     fprintf(fp,"     90.0    %6.1f  %6.1f    %d\n",phase,amp,gate);
   } }     
 else if (!strcmp(mix.m,"sqw")) {
   mix.pwr = mix.a; mix.res = 1.0/Getval("srate")/mix.n/2.0;
   fprintf(fp,"      90.0     0.0     1023.0     1\n\
      90.0     180.0   1023.0     1\n"); }
 else if (!strcmp(mix.m,"va")) {
   mix.pwr = 2.5 * mix.a; mix.res = 1.0/Getval("srate")/mix.n/2.0;
   steps = (int) ( mix.n*mix.t0*Getval("srate")+0.5 );
   for (j = 0; j < Steps; j++) {
      amp = 1023.0/2.5 * (0.5 + 2.0/(Steps-1.0)*j); 
      /* vary from 0.5*a to 2.5*a */
      amp = (double) ( (int) (amp + 0.5) );
      for (i = 0; i < steps; i++) {
        fprintf(fp,"      90.0     0.0     %6.1f     1\n\
      90.0     180.0   %6.1f     1\n",amp,amp); 
      }
   }   }
 else if (!strcmp(mix.m,"redor"))  { 
  /* _darr_() takes care of redor mode */
  /*    mix.pwr = Getval("aH90"); mix.res = 0.2e-6;
        fprintf(fp,"      %10.1f     0.0     1023.0     1\n\
      %10.1f     0.0     1023.0     0\n", 
      (double) ( (int)(2.0*Getval("pwH90")/mix.res) ), 
      (double) (  (int)( (0.5/Getval("srate") - 2.0*Getval("pwH90"))/mix.res ) )  );  */
  }

 
 fclose(fp);  

 *res = mix.res; /* reset the res parameter for the calling argument */
 *pwr = mix.pwr;
}

void _darr_(DARR mix)
{
 int ch, CH;
 double dAP = 0.5e-6, dres = 90.0, pwH180,Tr;

    if (mix.t < 1e-6) {
      if (ix == 1) printf("Warning from _darr_(): mixing time < 1 microsec, skip!\n");
      return;
    }

 if (!strcmp(mix.ch,"obs")) {ch = 1; CH = OBSch;}
 else if (!strcmp(mix.ch,"dec2")) {ch = 3; CH = DEC2ch;}
 else if (!strcmp(mix.ch,"dec3")) {ch = 4; CH = DEC3ch;}
 else {ch = 2; CH = DECch;}

 if (!strcmp(mix.m,"redor")) { /* dres = 1.0; mix.res = mix.res * 90.0; */

    switch(ch) {
      case 1: xmtroff(); obspwrf(Getval("aH90")); break;
      case 2: decoff();  decpwrf(Getval("aH90")); break;
      case 3: dec2off(); dec2pwrf(Getval("aH90")); break;
      case 4: dec3off(); dec3pwrf(Getval("aH90")); break;
      default: break;
    }
    Tr = 1.0/Getval("srate");
    initval((int)( (mix.t-dAP)*2.0/Tr + 0.5 ), v14);
    pwH180 = 2.0*Getval("pwH90");
    if (Tr > 2.0*pwH180) {
    /*  loop(v14,v13); */ starthardloop(v14);
        delay(Tr/2 - pwH180); 
        genrgpulse(pwH180,zero,0.0,0.0,CH);
    /*  endloop(v13); */ endhardloop(); }
    else { /* wouldnt happen at currently available speed, but add it anyway */
      printf("Error: aborted from _darr_(): Tr/2 < 2* pwH90 !!!\n");
      abort();
    }
 }
 else {
      switch(ch) {
       case 1:  obspwrf(mix.pwr);  xmtron(); obsprgon(mix.pattern,mix.res,dres);                delay(mix.pwr); obsprgoff(); xmtroff(); break;
       case 2:  decpwrf(mix.pwr);  decon();  decprgon(mix.pattern,mix.res,dres);                delay(mix.t); decprgoff();   decoff();  break;
       case 3:  dec2pwrf(mix.pwr); dec2on(); dec2prgon(mix.pattern,mix.res,dres);
                delay(mix.t); decprgoff();   dec2off(); break;
       case 4:  dec3pwrf(mix.pwr); dec3on(); ;
                delay(mix.t); dec3prgoff();  dec3off(); break;
       default: break;
      }
 }

}


void _rfdr_()
{
 static int XY16[16] = {0,1,0,1,1,0,1,0,2,3,2,3,3,2,3,2};
 double aXmix,pwXmix,tMIX,aHmix,Tr,t_pad;

 settable(t60,16,XY16); setautoincrement(t60);
  
 aXmix=Getval("aXmix"); pwXmix=Getval("pwXmix"); 
 tMIX=Getval("tMIX"); aHmix=Getval("aHmix"); Tr = 1.0/Getval("srate");
 t_pad = Tr/2.0 - pwXmix/2.0;

    if (tMIX < 1e-6) {
      if (ix == 1) printf("Warning from _rfdr_(): mixing time < 1 microsec, skip!\n");
      return;
    }


 initval((int)(tMIX/Tr + 0.5), v14);
     if (Tr >= pwXmix) {
        obspwrf(aXmix);
        decpwrf(aHmix); decon();
        loop(v14,v13); 
        /*  delay(t_pad); rgpulse(pwXmix,t60,0.0,0.0); delay(t_pad);*/
            rgpulse(pwXmix,t60,t_pad,t_pad);
        endloop(v13); 
        decoff();}
    else { 
      printf("Error: aborted from _rfdr_(): Tr < pwXmix !!!\n");
      abort();
    }
 } 
 

void make_pitrain(PITRAIN dec) /* create the shapefile*/
{
 char shapepath[MAXSTR],str[MAXSTR];
 FILE *fp;
 extern char userdir[]; 
 double Tr, t_pad;

 sprintf(shapepath,"%s/shapelib/",userdir);
 sprintf(str,"%s%s.DEC",shapepath,dec.pattern);

if((fp = fopen(str,"w"))==NULL){
   printf("Error in make_pitrain(): can not create file %s!\n", str);
   exit(-1);
}

 /* The predelay dec.m*Tr is not implemented yet, you need to do it 
    explicitly in each .c file.
    I don't know how to set predelay offered by the IB_STOP 
    word in the pattern.*/ 

 Tr = 1.0/Getval("srate");
 t_pad = dec.n*Tr - dec.pw; 
        

 /* usu. pw/(n*Tr) ~ 1 or 2%. We'll use 0.5u resolution, assume this is dres=1deg
    and pw90=45e-6 for obsprgon(pattern,pw90,dres).  */
 fprintf(fp,"#Donghua H. Zhou/ Rienstra NMR Group, UIUC/ Sept 17, 2004\n\
#      length       phase   amplitude   gate \n\
# ------------------------------------------------- \n");
 stepbreaker(fp,t_pad,0,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,0,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,90,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,90,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,0,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,0,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,90,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,90,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,90,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,90,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,0,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,0,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,90,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,90,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,0,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,0,1023,1,dec.res,1.0);
  
 stepbreaker(fp,t_pad,180,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,180,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,270,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,270,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,180,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,180,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,270,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,270,1023,1,dec.res,1.0);
 stepbreaker(fp,t_pad,270,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,270,1023,1,dec.res,1.0);  
 stepbreaker(fp,t_pad,180,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,180,1023,1,dec.res,1.0); 
 stepbreaker(fp,t_pad,270,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,270,1023,1,dec.res,1.0);  
 stepbreaker(fp,t_pad,180,0,0,dec.res,1.0); stepbreaker(fp,dec.pw,180,1023,1,dec.res,1.0); 

fclose(fp);
}

void _pitrain(PITRAIN dec)
{
  int chnl; 
  double pw90 = 90.0*dec.res;

  if (!strcmp(dec.ch,"obs")) chnl = 1;
  else if (!strcmp(dec.ch,"dec")) chnl = 2;
  else if (!strcmp(dec.ch,"dec2")) chnl = 3;
  else if (!strcmp(dec.ch,"dec3")) chnl = 4;
  else chnl = 2;

  switch (chnl) {
       case 1: obspwrf(dec.a); obsprgon(dec.pattern,pw90,1.0); break;
       case 2: decpwrf(dec.a); decprgon(dec.pattern,pw90,1.0); break;
       case 3: dec2pwrf(dec.a); dec2prgon(dec.pattern,pw90,1.0); break;
       case 4: dec3pwrf(dec.a); dec3prgon(dec.pattern,pw90,1.0); break;
       default: decpwrf(dec.a); decprgon(dec.pattern,pw90,1.0); break;
     }    
}

void make_spc5(SPC5 mix) /* create the shapefile*/
{
 char shapepath[MAXSTR],str[MAXSTR];
 FILE *fp;
 extern char userdir[]; 
 int i,j;
 int phi[10] = {0, 72, 144, 216, 288, 180, 252, 324, 36, 108};

 sprintf(shapepath,"%s/shapelib/",userdir);
 sprintf(str,"%s%s.DEC",shapepath,mix.pattern);
 

if((fp = fopen(str,"w"))==NULL){
   printf("Error in make_spc5(): can not create file %s!\n", str);
   exit(-1);
}

 
 fprintf(fp,"#Donghua H. Zhou/ Rienstra NMR Group, UIUC/ Sept 17, 2004\n\
#      length       phase   amplitude   gate \n\
# ------------------------------------------------- \n");

 /*fprintf(fp," %6d   %5d  0  0\n",(int)(90*mix.ZFdelay/mix.pw),0);*/
 for (j=0; j<mix.cycles;j++) {
  for (i=0;i<10; i++) {
   fprintf(fp,"%8d  %8d  1023 1\n",90,phi[i]);
   fprintf(fp,"%8d  %8d  1023 1\n",360,phi[i]+180);
   fprintf(fp,"%8d  %8d  1023 1\n",270,phi[i]);           
  }
 }
  
fclose(fp);
}

void _spc5_(SPC5 mix)
{
  int chnl; 
  double Tr;

    if (mix.t < 1e-6) {
      if (ix == 1) printf("Warning from _spc5_(): mixing time too short, skip!\n");
      return;
    }

  Tr = 1.0/Getval("srate");

  if (!strcmp(mix.ch,"obs")) chnl = 1;
  else if (!strcmp(mix.ch,"dec")) chnl = 2;
  else if (!strcmp(mix.ch,"dec2")) chnl = 3;
  else if (!strcmp(mix.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
  decpwrf(mix.aH);
  decon();
     switch (chnl) {
       case 1: obspwrf(mix.aX); xmtron(); obsprgon(mix.pattern,mix.pw,90.0); delay(mix.t); obsprgoff(); xmtroff(); break;
       case 2: decpwrf(mix.aX); decon();  decprgon(mix.pattern,mix.pw,90.0); delay(mix.t); decprgoff(); decoff();  break;
       case 3: dec2pwrf(mix.aX); dec2on(); dec2prgon(mix.pattern,mix.pw,90.0); delay(mix.t); dec2prgoff(); dec2off(); break;
       case 4: dec3pwrf(mix.aX); dec3on(); dec3prgon(mix.pattern,mix.pw,90.0); delay(mix.t); dec3prgoff(); dec3off(); break;
       default: obspwrf(mix.aX); xmtron(); obsprgon(mix.pattern,mix.pw,90.0); delay(mix.t); obsprgoff(); xmtroff(); break;
     }   
   decoff();
}

void make_tobsy(TOBSY mix) /* create the shapefile for TOBSY P9(1,n), n corresponds to parameter */
{
 char shapepath[MAXSTR],str[MAXSTR];
 FILE *fp;
 extern char userdir[]; 
 int i,j;
 int phi[9] = {0,  40, 80,  120, 160, 200, 240, 280, 320}; 
  

 sprintf(shapepath,"%s/shapelib/",userdir);
 sprintf(str,"%s%s.DEC",shapepath,mix.pattern);
 

if((fp = fopen(str,"w"))==NULL){
   printf("Error in make_tobsy(): can not create file %s!\n", str);
   exit(-1);
}

 
 fprintf(fp,"#Donghua H. Zhou/ Rienstra NMR Group, UIUC/ Sept 17, 2004\n\
#      length       phase   amplitude   gate \n\
# ------------------------------------------------- \n");

 /*fprintf(fp," %6d   %5d  0  0\n",(int)(90*mix.ZFdelay/mix.pw),0);*/
 /* 6 rotor periods per cycle */
 for (j=0; j<mix.cycles;j++) {
  for (i=0;i<9; i++) {
   fprintf(fp,"%8d  %8d  1023 1\n",90,phi[i]);
   fprintf(fp,"%8d  %8d  1023 1\n",360,phi[i]+180);
   fprintf(fp,"%8d  %8d  1023 1\n",270,phi[i]);           
  }
 }
  
fclose(fp);
}

void _tobsy_(TOBSY mix)
{
  int chnl; 
  double Tr;

    if (mix.t < 1e-6) {
      if (ix == 1) printf("Warning from _spc5_(): mixing time too short, skip!\n");
      return;
    }

  Tr = 1.0/Getval("srate");

  if (!strcmp(mix.ch,"obs")) chnl = 1;
  else if (!strcmp(mix.ch,"dec")) chnl = 2;
  else if (!strcmp(mix.ch,"dec2")) chnl = 3;
  else if (!strcmp(mix.ch,"dec3")) chnl = 4;
  else chnl = 2;
  
  decpwrf(mix.aH);
  decon();
     switch (chnl) {
       case 1: obspwrf(mix.aX); xmtron();  obsprgon(mix.pattern,mix.pw,90.0); delay(mix.t); obsprgoff(); xmtroff(); break;
       case 2: decpwrf(mix.aX); decon();   decprgon(mix.pattern,mix.pw,90.0); delay(mix.t); decprgoff(); decoff();  break;
       case 3: dec2pwrf(mix.aX); dec2on(); dec2prgon(mix.pattern,mix.pw,90.0); delay(mix.t); dec2prgoff(); dec2off(); break;
       case 4: dec3pwrf(mix.aX); dec3on(); dec3prgon(mix.pattern,mix.pw,90.0); delay(mix.t); dec3prgoff(); dec3off(); break;
       default: obspwrf(mix.aX); xmtron(); obsprgon(mix.pattern,mix.pw,90.0); delay(mix.t); obsprgoff(); xmtroff(); break;
     }   
   decoff();
}


