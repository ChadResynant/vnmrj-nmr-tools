#ifndef SOLIDSTANDARD_H
#define SOLIDSTANDARD_H

#include "master_switch.h"

#include <standard.h>
#include <Pbox_psg.h> 
#include "soliddefs.h"      //Structures and Definitions of Constant
#include "solidelements.h"  //Miscellaneous Programs Including getname()

#ifdef MATCH_MASTER
// BDZ this is not in our old standard code. We purposely don't use
// solidchoppers for some reason unknown to BDZ.
// So commenting it out for now.
//#ifdef NVPSG
//#include "solidchoppers.h"  // Replacement choppers use userDECShape
//#endif
#endif

#include "solidshapegen.h"  //Functions to Calculate and Run .DEC files 
#include "biosoliddecshapes.h" //All Software for DSEQ, SPINAL and TPPM //AP for debugging
#include "solidobjects.h"   //Non-waveform pulse sequence functions.
#include "biosolidmpseqs.h"    //"get" Functions for Multiple Pulse Sequences
#include "solidstates.h"    //Functions to Output STATE's for Shaped Pulses
#include "solidpulses.h"    //"get" Functions for Shaped Pulses

#ifdef MATCH_MASTER
// BDZ this is not in our old standard code.
#include "solidwshapes.h"   //Functions to run shapes with acquisition windows
#endif

#include "soliddutycycle.h" //Duty cycle protection, missing from ayrshire files for some reason

#ifdef MATCH_MASTER
// BDZ this is not in our old standard code.
#include "solidhhdec.h"     //Functions to support Ames contribution.
#endif

#endif
