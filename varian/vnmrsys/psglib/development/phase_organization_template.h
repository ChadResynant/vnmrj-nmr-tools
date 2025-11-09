/*
 * phase_organization_template.h - Universal template for organizing NMR phase tables
 * 
 * This template provides a standardized approach for organizing phase tables
 * in biosolid NMR pulse sequences. Apply this pattern to all sequences for
 * consistency, maintainability, and easier debugging.
 * 
 * Usage:
 * 1. Copy this template structure
 * 2. Fill in sequence-specific phase values
 * 3. Update the phase assignment #defines
 * 4. Modify the setup functions for your sequence
 */

#ifndef PHASE_ORGANIZATION_TEMPLATE_H
#define PHASE_ORGANIZATION_TEMPLATE_H

// ================================================================
// PHASE ORGANIZATION TEMPLATE
// ================================================================

/*
 * NAMING CONVENTION:
 * 
 * Format: ph[Nucleus][Event][Details]_table
 * 
 * Nucleus: H, X, Y, Z (or specific like C, N)
 * Event: 90, 180, hx, hy, yx, xh, mix1, mix2, comp1, comp2, etc.
 * Details: soft, hard, dqf, ref, etc.
 * 
 * Examples:
 * - phH90_table        - H 90° pulse
 * - phHhx_table        - H→X CP, H channel
 * - phXhx_table        - H→X CP, X channel
 * - phXmix1_table      - X mixing, storage pulse
 * - phCompY1_table     - Y composite pulse 1
 * - phRec_table        - Receiver phase
 */

// ================================================================
// SECTION 1: EXCITATION AND PREPARATION
// ================================================================
// First pulses in the sequence - excitation, preparation, etc.

static int phH90_table[MAX_STEPS] = {
    // H 90° excitation pulse
    // Typical: 4-8 step cycle for basic artifact suppression
    0,0,2,2  // Example: simple 4-step
};

static int phX90_table[MAX_STEPS] = {
    // X 90° pulse (if direct excitation used)
    // Often matched to receiver for proper phasing
    1,1,3,3  // Example: 4-step
};

static int phY90_table[MAX_STEPS] = {
    // Y 90° pulse (if used for preparation)
    0,0,0,0  // Example: constant phase
};

// ================================================================
// SECTION 2: CROSS-POLARIZATION TRANSFERS
// ================================================================
// All CP transfers in order of appearance in sequence

static int phHhx_table[MAX_STEPS] = {
    // H→X Cross-polarization: H channel
    // Usually constant or simple 2-step
    1,1,1,1,1,1,1,1  // Example: constant
};

static int phXhx_table[MAX_STEPS] = {
    // H→X Cross-polarization: X channel
    // Often alternates for artifact suppression
    0,2,0,2,2,0,2,0  // Example: alternating
};

static int phHhy_table[MAX_STEPS] = {
    // H→Y Cross-polarization: H channel
    3,3,3,3,3,3,3,3  // Example: -Y preparation
};

static int phYhy_table[MAX_STEPS] = {
    // H→Y Cross-polarization: Y channel
    0,0,0,0,0,0,0,0  // Example: +X reception
};

static int phYyx_table[MAX_STEPS] = {
    // Y→X Cross-polarization: Y channel
    0,2,0,2,0,2,0,2  // Example: alternating
};

static int phXyx_table[MAX_STEPS] = {
    // Y→X Cross-polarization: X channel
    0,0,0,0,2,2,2,2  // Example: spin temp
};

static int phXxh_table[MAX_STEPS] = {
    // X→H Cross-polarization: X channel
    0,0,0,0,0,0,0,0  // Example: constant
};

static int phHxh_table[MAX_STEPS] = {
    // X→H Cross-polarization: H channel
    1,1,1,1,1,1,1,1  // Example: +Y detection prep
};

// ================================================================
// SECTION 3: EVOLUTION PERIODS
// ================================================================
// Phases for evolution periods in sequence order (F4→F3→F2→F1)

// F4 Evolution (if 4D)
static int phF4comp1_table[MAX_STEPS] = {
    // F4 composite pulse 1
    0,0,0,0  // Example: X
};

static int phF4comp2_table[MAX_STEPS] = {
    // F4 composite pulse 2  
    1,1,1,1  // Example: Y
};

static int phF4refocus_table[MAX_STEPS] = {
    // F4 refocusing pulse (for CT)
    0,0,1,1,0,0,1,1  // Example: CT refocusing
};

// F3 Evolution (if 3D+)
static int phF3comp1_table[MAX_STEPS] = {
    // F3 composite pulse 1
    0,0,0,0
};

static int phF3comp2_table[MAX_STEPS] = {
    // F3 composite pulse 2
    1,1,1,1
};

static int phF3refocus_table[MAX_STEPS] = {
    // F3 refocusing pulse (for CT)
    0,0,1,1,0,0,1,1
};

// F2 Evolution  
static int phF2comp1_table[MAX_STEPS] = {
    // F2 composite pulse 1 (most common)
    0,0,0,0
};

static int phF2comp2_table[MAX_STEPS] = {
    // F2 composite pulse 2
    1,1,1,1
};

static int phF2refocus_table[MAX_STEPS] = {
    // F2 refocusing pulse (for CT or echo)
    0,0,1,1
};

// ================================================================
// SECTION 4: MIXING SEQUENCES
// ================================================================
// All mixing-related phases in order of appearance

static int phXmix1_table[MAX_STEPS] = {
    // X mixing: storage pulse
    // Critical for coherence selection
    3,3,1,1,1,1,3,3,3,3,1,1,1,1,3,3  // Example: RFDR-optimized
};

static int phXmix2_table[MAX_STEPS] = {
    // X mixing: readout pulse
    // Often extended cycling for artifact suppression
    1,1,3,3,1,1,3,3,1,1,3,3,1,1,3,3,  // First 16 steps
    2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0   // Extended to 32
};

static int phXmix2dqf_table[MAX_STEPS] = {
    // X mixing: readout (DQF version)
    // Enhanced artifact suppression
    1,3,3,1,1,3,3,1,1,3,3,1,1,3,3,1,  // DQF pattern
    2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2
};

static int phYmix1_table[MAX_STEPS] = {
    // Y mixing: storage pulse (if Y-Y mixing)
    0,0,0,0,2,2,2,2
};

static int phYmix2_table[MAX_STEPS] = {
    // Y mixing: readout pulse
    1,1,1,1,1,1,1,1
};

// Recoupling-specific phases
static int phXspc5_table[MAX_STEPS] = {
    // SPC5 recoupling base phase
    0,0,0,0,0,0,0,0
};

static int phXspc5ref_table[MAX_STEPS] = {
    // SPC5 reference (for DQF)
    0,1,0,1,0,1,0,1
};

static int phXrfdr_table[MAX_STEPS] = {
    // RFDR mixing phases
    0,0,0,0,0,0,0,0
};

// ================================================================
// SECTION 5: WATER SUPPRESSION AND SATURATION
// ================================================================
// Phases for MISSISSIPPI and other suppression schemes

static int phHsat_table[MAX_STEPS] = {
    // H saturation pulses
    0,0,0,0,0,0,0,0
};

static int phXsat_table[MAX_STEPS] = {
    // X saturation pulses  
    0,0,0,0,0,0,0,0
};

// ================================================================
// SECTION 6: DETECTION AND RECEIVER
// ================================================================
// Receiver phases - last since they depend on all previous phases

static int phRec_table[MAX_STEPS] = {
    // Standard receiver phase
    // Computed to cancel artifacts from all previous phases
    0,2,0,2,2,0,2,0,  // Basic F2 cycling
    2,0,2,0,0,2,0,2,  // Extended cycling
    1,3,1,3,3,1,3,1,  // Spin temperature
    3,1,3,1,1,3,1,3   // compensation
};

static int phRecSoft_table[MAX_STEPS] = {
    // Receiver for soft pulse sequences
    // May differ due to different phase evolution
    0,2,2,0,2,0,0,2,  // Adapted for soft pulses
    0,2,2,0,2,0,0,2,
    1,3,3,1,3,1,1,3,
    1,3,3,1,3,1,1,3
};

static int phRecCT_table[MAX_STEPS] = {
    // Receiver for constant time sequences
    // Accounts for CT phase evolution
    0,2,3,1,2,0,1,3,  // CT-specific pattern
    2,0,1,3,0,2,3,1,
    3,1,0,2,1,3,2,0,
    1,3,2,0,3,1,0,2
};

// ================================================================
// PHASE ASSIGNMENTS - SEQUENCE ORDER
// ================================================================
// Map phase tables to vnmr phase variables in sequence order

// Excitation phases
#define phH90        t1     // Initial H excitation
#define phX90        t2     // Initial X pulse (if used)
#define phY90        t3     // Initial Y pulse (if used)

// Cross-polarization phases (in order of appearance)
#define phHhx        t4     // H→X CP: H channel
#define phXhx        t5     // H→X CP: X channel
#define phHhy        t6     // H→Y CP: H channel  
#define phYhy        t7     // H→Y CP: Y channel
#define phYyx        t8     // Y→X CP: Y channel
#define phXyx        t9     // Y→X CP: X channel
#define phXxh        t10    // X→H CP: X channel
#define phHxh        t11    // X→H CP: H channel

// Evolution phases (F4→F3→F2 order)
#define phF4comp1    t12    // F4 composite pulse 1
#define phF4comp2    t13    // F4 composite pulse 2  
#define phF4refocus  t14    // F4 refocusing (CT)
#define phF3comp1    t15    // F3 composite pulse 1
#define phF3comp2    t16    // F3 composite pulse 2
#define phF3refocus  t17    // F3 refocusing (CT)
#define phF2comp1    t18    // F2 composite pulse 1
#define phF2comp2    t19    // F2 composite pulse 2
#define phF2refocus  t20    // F2 refocusing (CT/echo)

// Mixing phases
#define phXmix1      t21    // X mixing: storage
#define phXmix2      t22    // X mixing: readout
#define phXmix2dqf   t23    // X mixing: readout (DQF)
#define phYmix1      t24    // Y mixing: storage
#define phYmix2      t25    // Y mixing: readout
#define phXspc5      t26    // SPC5 recoupling
#define phXspc5ref   t27    // SPC5 reference
#define phXrfdr      t28    // RFDR mixing

// Suppression phases
#define phHsat       t29    // H saturation
#define phXsat       t30    // X saturation

// Receiver phases
#define phRec        t31    // Standard receiver
#define phRecSoft    t32    // Soft pulse receiver
#define phRecCT      t33    // Constant time receiver

// ================================================================
// PHASE TABLE SETUP FUNCTION TEMPLATE
// ================================================================

void setup_phase_tables_SEQUENCE_NAME(void) {
    printf("Setting up SEQUENCE_NAME phase tables...\n");
    
    // 1. Excitation phases
    settable(phH90,        STEPS, phH90_table);
    settable(phX90,        STEPS, phX90_table);
    settable(phY90,        STEPS, phY90_table);
    
    // 2. Cross-polarization phases (in sequence order)
    settable(phHhx,        STEPS, phHhx_table);
    settable(phXhx,        STEPS, phXhx_table);
    settable(phHhy,        STEPS, phHhy_table);
    settable(phYhy,        STEPS, phYhy_table);
    settable(phYyx,        STEPS, phYyx_table);
    settable(phXyx,        STEPS, phXyx_table);
    settable(phXxh,        STEPS, phXxh_table);
    settable(phHxh,        STEPS, phHxh_table);
    
    // 3. Evolution phases (F4→F3→F2 order)
    settable(phF4comp1,    STEPS, phF4comp1_table);
    settable(phF4comp2,    STEPS, phF4comp2_table);
    settable(phF4refocus,  STEPS, phF4refocus_table);
    settable(phF3comp1,    STEPS, phF3comp1_table);
    settable(phF3comp2,    STEPS, phF3comp2_table);
    settable(phF3refocus,  STEPS, phF3refocus_table);
    settable(phF2comp1,    STEPS, phF2comp1_table);
    settable(phF2comp2,    STEPS, phF2comp2_table);
    settable(phF2refocus,  STEPS, phF2refocus_table);
    
    // 4. Mixing phases
    settable(phXmix1,      STEPS, phXmix1_table);
    settable(phXmix2,      STEPS, phXmix2_table);
    settable(phXmix2dqf,   STEPS, phXmix2dqf_table);
    settable(phYmix1,      STEPS, phYmix1_table);
    settable(phYmix2,      STEPS, phYmix2_table);
    settable(phXspc5,      STEPS, phXspc5_table);
    settable(phXspc5ref,   STEPS, phXspc5ref_table);
    settable(phXrfdr,      STEPS, phXrfdr_table);
    
    // 5. Suppression phases
    settable(phHsat,       STEPS, phHsat_table);
    settable(phXsat,       STEPS, phXsat_table);
    
    // 6. Receiver phases (last)
    settable(phRec,        STEPS, phRec_table);
    settable(phRecSoft,    STEPS, phRecSoft_table);
    settable(phRecCT,      STEPS, phRecCT_table);
    
    printf("Phase tables configured for %d-step cycling\n", STEPS);
}

// ================================================================
// PHASE CYCLING SETUP FUNCTION TEMPLATE
// ================================================================

void setup_phase_cycling_SEQUENCE_NAME(void) {
    printf("Setting up phase cycling...\n");
    
    // F2 (d2) phase cycling
    int id2_ = (int)(d2 * getval("sw1") + 0.1);
    if ((phase1 == 1) || (phase1 == 2)) {
        tsadd(phRec,     2*id2_, 4);  // Receiver
        tsadd(phRecSoft, 2*id2_, 4);  // Soft receiver
        tsadd(phRecCT,   2*id2_, 4);  // CT receiver
        tsadd(phXmix1,   2*id2_, 4);  // Key storage pulse for F2
        printf("F2 States/TPPI: increment = %d\n", 2*id2_);
    }
    if (phase1 == 2) {
        tsadd(phXmix1, 3, 4);  // Hypercomplex
        printf("F2 Hypercomplex enabled\n");
    }
    
    // F3 (d3) phase cycling (if 3D+)
    int id3_ = (int)(d3 * getval("sw2") + 0.1);
    if ((phase2 == 1) || (phase2 == 2)) {
        tsadd(phRec,     2*id3_, 4);
        tsadd(phRecSoft, 2*id3_, 4);
        tsadd(phRecCT,   2*id3_, 4);
        tsadd(phYmix1,   2*id3_, 4);  // Key storage pulse for F3
        printf("F3 States/TPPI: increment = %d\n", 2*id3_);
    }
    if (phase2 == 2) {
        tsadd(phYmix1, 3, 4);
        printf("F3 Hypercomplex enabled\n");
    }
    
    // F4 (d4) phase cycling (if 4D)
    int id4_ = (int)(d4 * getval("sw3") + 0.1);
    if ((phase3 == 1) || (phase3 == 2)) {
        tsadd(phRec,     2*id4_, 4);
        tsadd(phRecSoft, 2*id4_, 4);
        tsadd(phRecCT,   2*id4_, 4);
        tsadd(phH90,     2*id4_, 4);  // Key excitation pulse for F4
        printf("F4 States/TPPI: increment = %d\n", 2*id4_);
    }
    if (phase3 == 2) {
        tsadd(phH90, 3, 4);
        printf("F4 Hypercomplex enabled\n");
    }
}

// ================================================================
// RECEIVER SELECTION FUNCTION TEMPLATE
// ================================================================

void select_receiver_SEQUENCE_NAME(const char* options[]) {
    // Logic for selecting appropriate receiver based on sequence options
    // e.g., CT mode, soft pulses, mixing schemes, etc.
    
    if (/* condition for standard receiver */) {
        setreceiver(phRec);
        printf("Receiver: Standard\n");
    }
    else if (/* condition for soft pulse receiver */) {
        setreceiver(phRecSoft);
        printf("Receiver: Soft pulse mode\n");
    }
    else if (/* condition for CT receiver */) {
        setreceiver(phRecCT);
        printf("Receiver: Constant time mode\n");
    }
    else {
        NMR_ABORT("Invalid receiver configuration");
    }
}

#endif /* PHASE_ORGANIZATION_TEMPLATE_H */

/*
 * USAGE INSTRUCTIONS:
 * ==================
 * 
 * 1. Copy this template for each sequence
 * 2. Replace SEQUENCE_NAME with actual sequence name (e.g., hXX, hYXX)
 * 3. Fill in actual phase values for each table
 * 4. Remove unused phase tables for simpler sequences
 * 5. Adjust MAX_STEPS and STEPS as needed
 * 6. Customize the setup and cycling functions
 * 
 * BENEFITS:
 * - Consistent organization across all sequences
 * - Easy to trace phase cycling logic
 * - Self-documenting code structure
 * - Simplified debugging and maintenance
 * - Clear separation of concerns
 * 
 * MIGRATION STRATEGY:
 * - Start with simplest sequences (2D)
 * - Test thoroughly before moving to complex sequences
 * - Use diff tools to verify phase table values match original
 * - Validate with actual NMR experiments
 */
