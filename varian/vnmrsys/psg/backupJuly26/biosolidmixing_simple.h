/*
 * biosolidmixing.h - Header file for homonuclear mixing sequences (simplified version)
 * Consolidates common mixing elements used across hXX, hXXX, hYXX, and hXXXX sequences
 * For OpenVNMRJ seqgen compilation - contains complete implementations
 * Author: Consolidated from multiple sources
 * Date: July 2025
 */

#ifndef BIOSOLIDMIXING_H
#define BIOSOLIDMIXING_H

#include "standard.h"
#include "biosolidstandard.h"
#include "biosolidmpseqs.h"

// Mixing sequence types
typedef enum {
    MIX_RAD,
    MIX_SPC5,
    MIX_C6,
    MIX_C7,
    MIX_SPCN,
    MIX_RFDR,
    MIX_PAR,
    MIX_PARIS,
    MIX_NONE
} MixingType;

// Structure to hold mixing parameters
typedef struct {
    MixingType type;
    double tMix;           // mixing time
    double taur;           // rotor period
    char dqf_flag[MAXSTR]; // DQF flag ("1" or "2")
    
    // Pulse sequences for recoupling (using existing biosolids structures)
    MPSEQ seq;
    MPSEQ seq_ref;
    
    // Phase tables
    int phMix1;
    int phMix2;
    int phMix2dqf;
    int phSpc5;
    int phSpc5ref;
    
    // Power levels
    double aH90;
    double aHmix;
    double aHZF;
    double aX90;
    double aXseq;  // aXspc5, aXc7, etc.
    
    // Timing
    double tZF;
    double tPAR;
    
    // PAR specific
    double aHpar;
    double aXpar;
    
    // PARIS specific
    PARIS paris;
    
} MixingParams;

// Convert string to mixing type enum
static MixingType get_mixing_type(const char* mix_str) {
    if (!strcmp(mix_str, "rad")) return MIX_RAD;
    if (!strcmp(mix_str, "spc5")) return MIX_SPC5;
    if (!strcmp(mix_str, "c6")) return MIX_C6;
    if (!strcmp(mix_str, "c7")) return MIX_C7;
    if (!strcmp(mix_str, "spcn")) return MIX_SPCN;
    if (!strcmp(mix_str, "rfdr")) return MIX_RFDR;
    if (!strcmp(mix_str, "par")) return MIX_PAR;
    if (!strcmp(mix_str, "paris")) return MIX_PARIS;
    if (!strcmp(mix_str, "n")) return MIX_NONE;
    
    printf("Unknown mixing type: %s\n", mix_str);
    psg_abort(1);
    return MIX_NONE;
}

// Initialize mixing parameters structure
static MixingParams init_mixing_params(const char* mix_type, double mix_time) {
    MixingParams params = {};
    
    // Initialize basic parameters
    params.type = get_mixing_type(mix_type);
    params.tMix = mix_time;
    
    // Get common parameters
    double srate = getval("srate");
    if (srate >= 500.0) {
        params.taur = roundoff((1.0/srate), 0.125e-6);
    } else {
        printf("ABORT: Spin Rate (srate) must be greater than 500\n");
        psg_abort(1);
    }
    
    // Round mixing time to rotor periods
    params.tMix = roundoff(params.tMix, params.taur);
    
    // Get DQF flag
    getstr("dqf_flag", params.dqf_flag);
    
    // Get power levels (common to all)
    params.aH90 = getval("aH90");
    params.aHZF = getval("aHZF");
    params.aX90 = getval("aX90");
    params.tZF = getval("tZF");
    
    return params;
}

// Setup mixing sequences based on type
static void setup_mixing_sequences(MixingParams *params, const char* nucleus) {
    char seq_name[MAXSTR];
    char ch_cmd[MAXSTR];
    
    switch(params->type) {
        case MIX_SPC5:
            sprintf(seq_name, "spc5%s", nucleus);
            params->seq = getspc5(seq_name, 0, 0.0, 0.0, 0, 1);
            params->seq_ref = getspc5(seq_name, params->seq.iSuper, 
                                     params->seq.phAccum, params->seq.phInt, 1, 1);
            strcpy(params->seq.ch, "obs");
            sprintf(ch_cmd, "ch%sspc5='obs'\n", nucleus);
            putCmd(ch_cmd);
            params->aXseq = getval("aXspc5");
            params->aHmix = getval("aHmixspc5");
            break;
            
        case MIX_C6:
            sprintf(seq_name, "c6%s", nucleus);
            params->seq = getpostc6(seq_name, 0, 0.0, 0.0, 0, 1);
            params->seq_ref = getpostc6(seq_name, params->seq.iSuper, 
                                       params->seq.phAccum, params->seq.phInt, 1, 1);
            strcpy(params->seq.ch, "obs");
            sprintf(ch_cmd, "ch%sc6='obs'\n", nucleus);
            putCmd(ch_cmd);
            params->aXseq = getval("aXc6");
            params->aHmix = getval("aHmixc6");
            break;
            
        case MIX_C7:
            sprintf(seq_name, "c7%s", nucleus);
            params->seq = getpostc7(seq_name, 0, 0.0, 0.0, 0, 1);
            params->seq_ref = getpostc7(seq_name, params->seq.iSuper, 
                                       params->seq.phAccum, params->seq.phInt, 1, 1);
            strcpy(params->seq.ch, "obs");
            sprintf(ch_cmd, "ch%sc7='obs'\n", nucleus);
            putCmd(ch_cmd);
            params->aXseq = getval("aXc7");
            params->aHmix = getval("aHmixc7");
            break;
            
        case MIX_SPCN:
            sprintf(seq_name, "spcn%s", nucleus);
            params->seq = getspcn(seq_name, 0, 0.0, 0.0, 0, 1);
            params->seq_ref = getspcn(seq_name, params->seq.iSuper, 
                                     params->seq.phAccum, params->seq.phInt, 1, 1);
            strcpy(params->seq.ch, "obs");
            sprintf(ch_cmd, "ch%sspcn='obs'\n", nucleus);
            putCmd(ch_cmd);
            params->aXseq = getval("aXspcn");
            params->aHmix = getval("aHmixspcn");
            break;
            
        case MIX_RFDR:
            sprintf(seq_name, "rfdr%s", nucleus);
            params->seq = getrfdrxy8(seq_name, 0, 0.0, 0.0, 0, 1);
            strcpy(params->seq.ch, "obs");
            sprintf(ch_cmd, "ch%srfdr='obs'\n", nucleus);
            putCmd(ch_cmd);
            params->aHmix = getval("aHrfdr");
            break;
            
        case MIX_RAD:
            params->aHmix = getval("aHmix");
            break;
            
        case MIX_PAR:
            params->tPAR = getval("tPAR");
            params->tPAR = roundoff(params->tPAR, 4.0*getval("pwX90")*4095/getval("aXpar"));
            params->aHpar = getval("aHpar");
            params->aXpar = getval("aXpar");
            break;
            
        case MIX_PARIS:
            params->paris = getparis("H");
            break;
            
        default:
            break;
    }
}

// Calculate duty cycle contribution from mixing
static double calculate_mixing_duty_cycle(MixingParams *params, double base_duty, double base_time) {
    double total_duty = base_duty;
    double total_time = base_time;
    
    switch(params->type) {
        case MIX_RAD:
            total_duty += 2.0*getval("pwX90") + 
                         (params->aHmix/4095) * (params->aHmix/4095) * params->tMix;
            total_time += params->tMix;
            break;
            
        case MIX_SPC5:
        case MIX_C6:
        case MIX_C7:
        case MIX_SPCN:
            total_duty += 2.0*getval("pwX90");
            if (!strcmp(params->dqf_flag, "2")) {
                total_duty += params->seq_ref.t;
            } else {
                total_duty += params->seq.t;
            }
            total_time += 2.0*params->tZF;
            break;
            
        case MIX_PAR:
            total_duty += 2.0*getval("pwX90") + params->tPAR;
            break;
            
        case MIX_RFDR:
            total_duty += 2.0*params->taur*getval("qXrfdr") + 2.0*getval("pwX90");
            break;
            
        default:
            break;
    }
    
    return total_duty / (total_time + getval("d1") + 4.0e-6);
}

// RAD/DARR mixing implementation
static void execute_rad_mixing(MixingParams *params) {
    decpwrf(params->aHmix);
    decunblank(); decon();
    obspwrf(params->aX90);
    rgpulse(getval("pwX90"), params->phMix1, 0.0, 0.0);
    delay(params->tMix);
    rgpulse(getval("pwX90"), params->phMix2, 0.0, 0.0);
    decoff();
}

// Recoupling sequences (SPC5, C6, C7, SPCN)
static void execute_recoupling_mixing(MixingParams *params) {
    decpwrf(params->aHZF);
    decon();
    obspwrf(params->aX90);
    rgpulse(getval("pwX90"), params->phMix1, 0.0, 0.0);
    obspwrf(params->aXseq);
    xmtrphase(v1); txphase(params->phMix1);
    
    delay(params->tZF);
    decoff();
    
    decpwrf(params->aHmix);
    decon();
    _mpseq(params->seq, params->phSpc5);
    
    if (!strcmp(params->dqf_flag, "2")) {
        xmtrphase(v2); txphase(params->phMix2);
        _mpseq(params->seq_ref, params->phSpc5ref);
    }
    
    decoff();
    decpwrf(params->aHZF);
    decon();
    obspwrf(params->aX90);
    xmtrphase(zero); txphase(params->phMix2);
    delay(params->tZF);
    
    if (!strcmp(params->dqf_flag, "2")) {
        rgpulse(getval("pwX90"), params->phMix2dqf, 0.0, 0.0);
    } else {
        rgpulse(getval("pwX90"), params->phMix2, 0.0, 0.0);
    }
    decoff();
}

// RFDR mixing implementation
static void execute_rfdr_mixing(MixingParams *params) {
    decpwrf(params->aH90);
    obspwrf(params->aX90);
    decon();
    rgpulse(getval("pwX90"), params->phMix1, 0.0, 0.0);
    decpwrf(params->aHZF);
    delay(params->tZF);
    decpwrf(params->aHmix);
    _mpseq(params->seq, 0.0);
    decpwrf(params->aHZF);
    delay(params->tZF);
    obspwrf(params->aX90);
    decpwrf(params->aH90);
    rgpulse(getval("pwX90"), params->phMix2, 0.0, 0.0);
    decoff();
}

// PAR mixing implementation
static void execute_par_mixing(MixingParams *params) {
    decpwrf(params->aH90);
    obspwrf(params->aX90);
    decon();
    rgpulse(getval("pwX90"), params->phMix1, 0.0, 0.0);
    decoff();
    
    decpwrf(params->aHZF);
    decon();
    delay(params->tZF);
    decoff();
    
    decpwrf(params->aH90);
    obspwrf(params->aX90);
    decon();
    rgpulse(getval("pwX90"), getval("ph90Xzf1"), 0.0, 0.0);
    decoff();
    
    decpwrf(params->aHpar);
    obspwrf(params->aXpar);
    decon();
    rgpulse(params->tPAR, getval("phXpar"), 0.0, 0.0);
    decoff();
    
    decpwrf(params->aH90);
    obspwrf(params->aX90);
    decon();
    rgpulse(getval("pwX90"), getval("ph90Xzf2"), 0.0, 0.0);
    decoff();
    
    decpwrf(params->aHZF);
    decon();
    delay(params->tZF);
    decoff();
    
    decpwrf(params->aH90);
    obspwrf(params->aX90);
    decon();
    rgpulse(getval("pwX90"), params->phMix2, 0.0, 0.0);
    decoff();
}

// PARIS mixing implementation
static void execute_paris_mixing(MixingParams *params) {
    decpwrf(params->aH90);
    decunblank(); decon();
    rgpulse(getval("pwX90"), params->phMix1, 0.0, 0.0);
    decpwrf(getval("aHparis"));
    _paris(params->paris);
    delay(params->tMix);
    decprgoff();
    decpwrf(params->aH90);
    decunblank(); decon();
    rgpulse(getval("pwX90"), params->phMix2, 0.0, 0.0);
    decoff();
}

// Execute the appropriate mixing sequence
static void execute_mixing_sequence(MixingParams *params) {
    if (params->type == MIX_NONE) return;
    
    switch(params->type) {
        case MIX_RAD:
            execute_rad_mixing(params);
            break;
            
        case MIX_SPC5:
        case MIX_C6:
        case MIX_C7:
        case MIX_SPCN:
            execute_recoupling_mixing(params);
            break;
            
        case MIX_RFDR:
            execute_rfdr_mixing(params);
            break;
            
        case MIX_PAR:
            execute_par_mixing(params);
            break;
            
        case MIX_PARIS:
            execute_paris_mixing(params);
            break;
            
        default:
            printf("Mixing type not implemented\n");
            break;
    }
}

// Validate mixing parameters
static void validate_mixing_parameters(MixingParams *params) {
    if (params->type == MIX_SPCN) {
        int NXspcn = (int)getval("NXspcn");
        int qXspcn = (int)getval("qXspcn");
        
        if (qXspcn % NXspcn != 0) {
            printf("ABORT: qXspcn should be a multiple of NXspcn\n");
            psg_abort(1);
        }
    }
}

#endif /* BIOSOLIDMIXING_H */
