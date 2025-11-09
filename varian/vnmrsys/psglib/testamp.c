/* testamp.c
simple pulse sequence to test amplifiers with control over blanking logic
1. during pulse delay, blanking controlled by string "blank1" (y=blanked,n=unblanked)
2. during pulse, always unblanked
 blanking status is always unblanked (cannot be controlled; see ELN 10/7/23 9:30 am)
3. during delay after pulse, controlled by "blank2" (y=blanked,n=unblanked)
4. always blanked during acquisition time "at"
*/

#include "standard.h"

// Define Values for Phasetables

void pulsesequence() {

// Define Variables and Objects and Get Parameter Values

// blank1 is string to control blanking during pulse delay d1
// y = blanked (low voltage, 0 V), n = unblanked (high voltage, +5 V)
   char blank1[MAXSTR];
   getstr("blank1",blank1);
   
// blank2 is string to control blanking during pulse d2 
// y = blanked (low voltage, 0 V), n = unblanked (high voltage, +5 V)
   char blank2[MAXSTR];
   getstr("blank2",blank2);

// Begin Sequence

   if (!strcmp(blank1,"n")) 
   	{obsunblank(); }
   else if (!strcmp(blank1,"y"))
   	{obsblank();} 
     
   delay(d1);

 // External trigger on channel 1 number 1
   obsunblank();
   sp1on(); delay(2.0e-6); sp1off(); delay(2.0e-6);
   rcvroff();
 
// customized unblanking for CHN3 Tomco F19 Mode 10/9/23 CMR
   sp3on();
// (after determining that standard blanking output is not working properly
   delay(d3);	
   obspwrf(getval("aX90"));
   rgpulse(getval("pwX90"),0.0,0.0,0.0);
   
   if (!strcmp(blank2,"n")) 
   	{obsunblank(); sp3on();}
   else if (!strcmp(blank2,"y"))
   	{obsblank(); } 

   delay(d2);
   sp3off();
   
// Begin Acquisition

   obsblank(); 

   delay(getval("rd"));
   startacq(getval("ad"));
   acquire(np, 1/sw);
   endacq();

}
