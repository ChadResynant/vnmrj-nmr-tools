/*
 * biosolidevolution.h - Consolidated Evolution Period System
 * 
 * Provides unified functions for handling evolution periods with automatic
 * switching between regular and constant time modes, composite pulse
 * decoupling, and parameter validation.
 * 
 * Key Features:
 * - Automatic regular vs constant time selection
 * - Composite pulse refocusing during evolution
 * - Multi-nucleus decoupling management
 * - Parameter validation and safety checks
 * 
 * Usage:
 *   EvolutionParams params = init_evolution_params("H", d2, "CT_flag", tconst);
 *   execute_evolution_period(&params);
 */

#ifndef BIOSOLIDEVOLUTION_H
#define BIOSOLIDEVOLUTION_H

#include "standard.h"

/* Evolution parameter structure */
typedef struct {
    char nucleus[8];           // Primary evolving nucleus ("H", "X", "Y", "Z")
    char mode[8];              // "regular" or "ct" (constant time)
    double d_time;             // Evolution time (d2, d3, d4)
    double t_const;            // Constant time period (for CT mode)
    double sw;                 // Spectral width for validation
    int ni;                    // Number of increments for validation
    
    // Pulse parameters
    double pw90;               // 90-degree pulse width for primary nucleus
    double pw180;              // 180-degree pulse width for refocusing
    int ph_comp1;              // Composite pulse phase 1
    int ph_comp2;              // Composite pulse phase 2
    int ph_refocus;            // Refocusing pulse phase
    
    // Decoupling nuclei (up to 3 additional nuclei)
    char decouple[3][8];       // Nuclei to decouple during evolution
    double pw_dec[3];          // 180-degree pulse widths for decoupled nuclei
    int n_decouple;            // Number of nuclei to decouple
    
    // Calculated parameters (filled by init function)
    double delay_pre;          // Pre-evolution delay
    double delay_post;         // Post-evolution delay
    double delay_refocus_pre;  // Pre-refocusing delay
    double delay_refocus_post; // Post-refocusing delay
    
} EvolutionParams;

/* Initialize evolution parameters */
EvolutionParams init_evolution_params(const char* nucleus, double d_time, 
                                    const char* ct_flag, double t_const) {
    EvolutionParams params = {0};
    
    // Copy nucleus name
    strncpy(params.nucleus, nucleus, sizeof(params.nucleus) - 1);
    
    // Set mode based on CT flag
    if (!strcmp(ct_flag, "y")) {
        strcpy(params.mode, "ct");
        params.t_const = t_const;
    } else {
        strcpy(params.mode, "regular");
        params.t_const = 0.0;
    }
    
    params.d_time = d_time;
    
    // Set default phases (can be overridden)
    params.ph_comp1 = 0;
    params.ph_comp2 = 1;
    params.ph_refocus = 0;
    
    // Initialize decoupling array
    params.n_decouple = 0;
    
    return params;
}

/* Add decoupling nucleus */
void add_decoupling_nucleus(EvolutionParams* params, const char* nucleus, double pw180) {
    if (params->n_decouple < 3) {
        strncpy(params->decouple[params->n_decouple], nucleus, 7);
        params->pw_dec[params->n_decouple] = pw180;
        params->n_decouple++;
    } else {
        printf("WARNING: Maximum 3 decoupling nuclei supported\n");
    }
}

/* Set pulse parameters */
void set_evolution_pulses(EvolutionParams* params, double pw90, double pw180, 
                         int ph_comp1, int ph_comp2, int ph_refocus) {
    params->pw90 = pw90;
    params->pw180 = pw180;
    params->ph_comp1 = ph_comp1;
    params->ph_comp2 = ph_comp2;
    params->ph_refocus = ph_refocus;
}

/* Set spectral parameters for validation */
void set_evolution_spectral_params(EvolutionParams* params, double sw, int ni) {
    params->sw = sw;
    params->ni = ni;
}

/* Calculate timing parameters */
void calculate_evolution_timing(EvolutionParams* params) {
    if (!strcmp(params->mode, "ct")) {
        // Constant time calculations
        if (params->t_const < params->d_time) {
            printf("ABORT: tconst (%.3f ms) < d_time (%.3f ms)! Check CT setup.\n",
                   params->t_const * 1e3, params->d_time * 1e3);
            psg_abort(1);
        }
        
        if (params->ni > 0 && params->sw > 0) {
            double max_evolution = (params->ni - 1) / params->sw;
            if (params->t_const < max_evolution) {
                printf("WARNING: tconst (%.3f ms) < maximum evolution (%.3f ms)\n",
                       params->t_const * 1e3, max_evolution * 1e3);
            }
        }
        
        // Calculate CT delays
        params->delay_pre = (params->t_const - params->d_time) / 2.0 - params->pw90;
        params->delay_post = (params->t_const - params->d_time) / 2.0 - params->pw90;
        
        // Account for decoupling pulses in CT mode
        double total_dec_time = 0.0;
        for (int i = 0; i < params->n_decouple; i++) {
            total_dec_time += 2.0 * params->pw_dec[i];
        }
        
        params->delay_refocus_pre = params->d_time / 2.0;
        params->delay_refocus_post = params->d_time / 2.0;
        
        // Adjust for pulse compensation in CT
        if (params->delay_pre < 0) {
            printf("WARNING: Negative pre-delay in CT mode, adjusting\n");
            params->delay_pre = 0.0;
            params->delay_post = params->t_const - params->d_time - params->pw90;
            if (params->delay_post < 0) params->delay_post = 0.0;
        }
        
    } else {
        // Regular evolution calculations
        double min_refocus_time = 4.0 * params->pw90;
        for (int i = 0; i < params->n_decouple; i++) {
            min_refocus_time += 4.0 * params->pw_dec[i];
        }
        
        if (params->d_time > min_refocus_time) {
            // Use composite pulse refocusing
            double total_pulse_time = 2.0 * params->pw90;
            for (int i = 0; i < params->n_decouple; i++) {
                total_pulse_time += 2.0 * params->pw_dec[i];
            }
            
            params->delay_refocus_pre = params->d_time / 2.0 - total_pulse_time;
            params->delay_refocus_post = params->d_time / 2.0 - total_pulse_time;
            params->delay_pre = 0.0;
            params->delay_post = 0.0;
        } else {
            // Simple delay
            params->delay_pre = params->d_time;
            params->delay_post = 0.0;
            params->delay_refocus_pre = 0.0;
            params->delay_refocus_post = 0.0;
        }
    }
}

/* Validate evolution parameters */
void validate_evolution_params(EvolutionParams* params) {
    // Check for negative delays
    if (params->delay_pre < 0 || params->delay_post < 0 || 
        params->delay_refocus_pre < 0 || params->delay_refocus_post < 0) {
        printf("ABORT: Negative delays calculated in evolution period\n");
        printf("Pre: %.1f us, Post: %.1f us, Ref_pre: %.1f us, Ref_post: %.1f us\n",
               params->delay_pre * 1e6, params->delay_post * 1e6,
               params->delay_refocus_pre * 1e6, params->delay_refocus_post * 1e6);
        psg_abort(1);
    }
    
    // Check evolution time limits
    if (params->d_time > 10e-3) {
        printf("ABORT: Evolution time %.1f ms > 10 ms limit\n", params->d_time * 1e3);
        psg_abort(1);
    }
    
    // Validate pulse widths
    if (params->pw90 <= 0 || params->pw180 <= 0) {
        printf("ABORT: Invalid pulse widths - pw90: %.1f us, pw180: %.1f us\n",
               params->pw90 * 1e6, params->pw180 * 1e6);
        psg_abort(1);
    }
}

/* Execute evolution period */
void execute_evolution_period(EvolutionParams* params) {
    // Calculate timing
    calculate_evolution_timing(params);
    
    // Validate parameters
    validate_evolution_params(params);
    
    // Extract values to local variables for clean dps display
    double pw90 = params->pw90;
    double pw180 = 2.0 * pw90;
    int ph_comp1 = params->ph_comp1;
    int ph_comp2 = params->ph_comp2; 
    int ph_refocus = params->ph_refocus;
    double delay_pre = params->delay_pre;
    double delay_post = params->delay_post;
    double delay_ref_pre = params->delay_refocus_pre;
    double delay_ref_post = params->delay_refocus_post;
    
    if (!strcmp(params->mode, "ct")) {
        // Constant time evolution
        if (delay_pre > 0) {
            delay(delay_pre);
        }
        
        // CT refocusing pulse
        if (!strcmp(params->nucleus, "H")) {
            rgpulse(pw180, ph_refocus, 0.0, 0.0);
        } else if (!strcmp(params->nucleus, "X")) {
            decrgpulse(pw180, ph_refocus, 0.0, 0.0);
        } else if (!strcmp(params->nucleus, "Y")) {
            dec2rgpulse(pw180, ph_refocus, 0.0, 0.0);
        } else if (!strcmp(params->nucleus, "Z")) {
            dec3rgpulse(pw180, ph_refocus, 0.0, 0.0);
        }
        
        if (delay_ref_pre > 0) {
            delay(delay_ref_pre);
        }
        
        // Decoupling pulses during evolution
        for (int i = 0; i < params->n_decouple; i++) {
            double pw_dec180 = 2.0 * params->pw_dec[i];
            if (!strcmp(params->decouple[i], "H")) {
                rgpulse(pw_dec180, 0, 0.0, 0.0);
            } else if (!strcmp(params->decouple[i], "X")) {
                decrgpulse(pw_dec180, 0, 0.0, 0.0);
            } else if (!strcmp(params->decouple[i], "Y")) {
                dec2rgpulse(pw_dec180, 0, 0.0, 0.0);
            } else if (!strcmp(params->decouple[i], "Z")) {
                dec3rgpulse(pw_dec180, 0, 0.0, 0.0);
            }
        }
        
        if (delay_ref_post > 0) {
            delay(delay_ref_post);
        }
        
        if (delay_post > 0) {
            delay(delay_post);
        }
        
    } else {
        // Regular evolution
        if (delay_pre > 0) {
            delay(delay_pre);
        } else {
            // Use composite pulse refocusing
            if (delay_ref_pre > 0) {
                delay(delay_ref_pre);
            }
            
            // Composite pulse sequence
            if (!strcmp(params->nucleus, "H")) {
                rgpulse(pw90, ph_comp1, 0.0, 0.0);
                rgpulse(pw180, ph_comp2, 0.0, 0.0);
                rgpulse(pw90, ph_comp1, 0.0, 0.0);
            } else if (!strcmp(params->nucleus, "X")) {
                decrgpulse(pw90, ph_comp1, 0.0, 0.0);
                decrgpulse(pw180, ph_comp2, 0.0, 0.0);
                decrgpulse(pw90, ph_comp1, 0.0, 0.0);
            } else if (!strcmp(params->nucleus, "Y")) {
                dec2rgpulse(pw90, ph_comp1, 0.0, 0.0);
                dec2rgpulse(pw180, ph_comp2, 0.0, 0.0);
                dec2rgpulse(pw90, ph_comp1, 0.0, 0.0);
            } else if (!strcmp(params->nucleus, "Z")) {
                dec3rgpulse(pw90, ph_comp1, 0.0, 0.0);
                dec3rgpulse(pw180, ph_comp2, 0.0, 0.0);
                dec3rgpulse(pw90, ph_comp1, 0.0, 0.0);
            }
            
            // Simultaneous decoupling pulses
            for (int i = 0; i < params->n_decouple; i++) {
                double pw_dec180 = 2.0 * params->pw_dec[i];
                if (!strcmp(params->decouple[i], "Y")) {
                    dec2rgpulse(pw_dec180, 0, 0.0, 0.0);
                } else if (!strcmp(params->decouple[i], "Z")) {
                    dec3rgpulse(pw_dec180, 0, 0.0, 0.0);
                }
            }
            
            if (delay_ref_post > 0) {
                delay(delay_ref_post);
            }
        }
        
        if (delay_post > 0) {
            delay(delay_post);
        }
    }
}

/* Convenience functions for common evolution periods */
/* These use internal struct handling but present clean dps display */

/* X evolution with Y,Z decoupling (most common case) */
void execute_x_evolution_yz_decouple(double d_time, const char* ct_flag, double t_const, 
                                    double pwX90, double pwY90, double pwZ90,
                                    int ph_comp1, int ph_comp2, double sw, int ni) {
    // Internal struct setup (not visible in dps)
    EvolutionParams params = init_evolution_params("X", d_time, ct_flag, t_const);
    set_evolution_pulses(&params, pwX90, 2.0 * pwX90, ph_comp1, ph_comp2, 0);
    set_evolution_spectral_params(&params, sw, ni);
    add_decoupling_nucleus(&params, "Y", pwY90);
    add_decoupling_nucleus(&params, "Z", pwZ90);
    execute_evolution_period(&params);
}

/* Y evolution with X,Z decoupling */
void execute_y_evolution_xz_decouple(double d_time, const char* ct_flag, double t_const,
                                    double pwY90, double pwX90, double pwZ90,
                                    int ph_comp1, int ph_comp2, double sw, int ni) {
    // Internal struct setup (not visible in dps)
    EvolutionParams params = init_evolution_params("Y", d_time, ct_flag, t_const);
    set_evolution_pulses(&params, pwY90, 2.0 * pwY90, ph_comp1, ph_comp2, 0);
    set_evolution_spectral_params(&params, sw, ni);
    add_decoupling_nucleus(&params, "X", pwX90);
    add_decoupling_nucleus(&params, "Z", pwZ90);
    execute_evolution_period(&params);
}

/* H evolution with X,Y,Z decoupling */
void execute_h_evolution_xyz_decouple(double d_time, const char* ct_flag, double t_const,
                                     double pwH90, double pwX90, double pwY90, double pwZ90,
                                     int ph_comp1, int ph_comp2, double sw, int ni) {
    // Internal struct setup (not visible in dps)
    EvolutionParams params = init_evolution_params("H", d_time, ct_flag, t_const);
    set_evolution_pulses(&params, pwH90, 2.0 * pwH90, ph_comp1, ph_comp2, 0);
    set_evolution_spectral_params(&params, sw, ni);
    add_decoupling_nucleus(&params, "X", pwX90);
    add_decoupling_nucleus(&params, "Y", pwY90);
    add_decoupling_nucleus(&params, "Z", pwZ90);
    execute_evolution_period(&params);
}

/* Simple evolution functions for cases where minimal setup is needed */

/* X evolution with Y decoupling only (common in 2D) */
void execute_x_evolution_y_decouple(double d_time, double pwX90, double pwY90, 
                                   int ph_comp1, int ph_comp2) {
    // Extract values for clean dps display
    double pw90 = pwX90;
    double pw180 = 2.0 * pwX90;
    double pwY180 = 2.0 * pwY90;
    
    if (d_time > 4.0 * pwY90) {
        delay(d_time/2.0 - 2.0*pwY90);
        decrgpulse(pw90, ph_comp1, 0.0, 0.0);
        decrgpulse(pw180, ph_comp2, 0.0, 0.0);
        decrgpulse(pw90, ph_comp1, 0.0, 0.0);
        dec2rgpulse(pwY180, 0, 0.0, 0.0);
        delay(d_time/2.0 - 2.0*pwY90);
    } else {
        delay(d_time);
    }
}

/* Y evolution with X decoupling only */
void execute_y_evolution_x_decouple(double d_time, double pwY90, double pwX90,
                                   int ph_comp1, int ph_comp2) {
    // Extract values for clean dps display  
    double pw90 = pwY90;
    double pw180 = 2.0 * pwY90;
    double pwX180 = 2.0 * pwX90;
    
    if (d_time > 4.0 * pwX90) {
        delay(d_time/2.0 - 2.0*pwX90);
        dec2rgpulse(pw90, ph_comp1, 0.0, 0.0);
        dec2rgpulse(pw180, ph_comp2, 0.0, 0.0);
        dec2rgpulse(pw90, ph_comp1, 0.0, 0.0);
        decrgpulse(pwX180, 0, 0.0, 0.0);
        delay(d_time/2.0 - 2.0*pwX90);
    } else {
        delay(d_time);
    }
}

#endif /* BIOSOLIDEVOLUTION_H */
