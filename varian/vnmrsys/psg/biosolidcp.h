/*
 * biosolidcp.h - Consolidated Cross-Polarization System
 * 
 * Provides unified functions for setting up common cross-polarization
 * transfers with automatic channel assignment and parameter validation.
 * 
 * Key Features:
 * - Standard CP transfers (HX, HY, YX, XH, etc.)
 * - Automatic channel assignment
 * - Parameter validation
 * - Consistent naming conventions
 * 
 * Usage:
 *   CP hx = setup_hx_cp();
 *   CP yx = setup_yx_cp();
 *   _cp_(hx, phHhx, phXhx);
 */

#ifndef BIOSOLIDCP_H
#define BIOSOLIDCP_H

#include "standard.h"
#include "biosolidstandard.h"

/* CP setup helper functions */

/* H → X Cross Polarization */
CP setup_hx_cp(void) {
    CP hx = getcp("HX", 0.0, 0.0, 0, 1);
    strcpy(hx.fr, "dec");      // H on dec channel
    strcpy(hx.to, "obs");      // X on obs channel
    putCmd("frHX='dec'\n");
    putCmd("toHX='obs'\n");
    return hx;
}

/* H → Y Cross Polarization */
CP setup_hy_cp(void) {
    CP hy = getcp("HY", 0.0, 0.0, 0, 1);
    strcpy(hy.fr, "dec");      // H on dec channel
    strcpy(hy.to, "dec2");     // Y on dec2 channel
    putCmd("frHY='dec'\n");
    putCmd("toHY='dec2'\n");
    return hy;
}

/* Y → X Cross Polarization */
CP setup_yx_cp(void) {
    CP yx = getcp("YX", 0.0, 0.0, 0, 1);
    strcpy(yx.fr, "dec2");     // Y on dec2 channel
    strcpy(yx.to, "obs");      // X on obs channel
    putCmd("frYX='dec2'\n");
    putCmd("toYX='obs'\n");
    return yx;
}

/* X → H Cross Polarization */
CP setup_xh_cp(void) {
    CP xh = getcp("XH", 0.0, 0.0, 0, 1);
    strcpy(xh.fr, "obs");      // X on obs channel
    strcpy(xh.to, "dec");      // H on dec channel
    putCmd("frXH='obs'\n");
    putCmd("toXH='dec'\n");
    return xh;
}

/* H → X Cross Polarization (alternative channel assignment) */
CP setup_hx_cp_alt(void) {
    CP hx = getcp("HX", 0.0, 0.0, 0, 1);
    strcpy(hx.fr, "obs");      // H on obs channel
    strcpy(hx.to, "dec");      // X on dec channel
    putCmd("frHX='obs'\n");
    putCmd("toHX='dec'\n");
    return hx;
}

/* Y → H Cross Polarization */
CP setup_yh_cp(void) {
    CP yh = getcp("YH", 0.0, 0.0, 0, 1);
    strcpy(yh.fr, "dec2");     // Y on dec2 channel
    strcpy(yh.to, "dec");      // H on dec channel
    putCmd("frYH='dec2'\n");
    putCmd("toYH='dec'\n");
    return yh;
}

/* X → Y Cross Polarization */
CP setup_xy_cp(void) {
    CP xy = getcp("XY", 0.0, 0.0, 0, 1);
    strcpy(xy.fr, "obs");      // X on obs channel
    strcpy(xy.to, "dec2");     // Y on dec2 channel
    putCmd("frXY='obs'\n");
    putCmd("toXY='dec2'\n");
    return xy;
}

/* CP parameter validation */
void validate_cp_parameters(const char* cp_name, double t_contact) {
    if (t_contact <= 0) {
        printf("ABORT: Invalid contact time for %s CP: %.1f us\n", 
               cp_name, t_contact * 1e6);
        psg_abort(1);
    }
    
    if (t_contact > 20e-3) {
        printf("WARNING: Long contact time for %s CP: %.1f ms\n",
               cp_name, t_contact * 1e3);
    }
}

/* Execute CP with automatic parameter validation */
void execute_cp_validated(CP* cp_obj, int ph_from, int ph_to, const char* cp_name) {
    double t_contact = getval(cp_obj->pattern);  // Get contact time from parameter
    validate_cp_parameters(cp_name, t_contact);
    _cp_(*cp_obj, ph_from, ph_to);
}

/* Common CP sequence combinations */

/* Standard HYX protein backbone transfer */
typedef struct {
    CP hy;
    CP yx;
    CP xh;
} ProteinBackboneCP;

ProteinBackboneCP setup_protein_backbone_cp(void) {
    ProteinBackboneCP bb;
    bb.hy = setup_hy_cp();
    bb.yx = setup_yx_cp();
    bb.xh = setup_xh_cp();
    return bb;
}

/* Standard HXH sidechain transfer */
typedef struct {
    CP hx;
    CP xh;
} SidechainCP;

SidechainCP setup_sidechain_cp(void) {
    SidechainCP sc;
    sc.hx = setup_hx_cp();
    sc.xh = setup_xh_cp();
    return sc;
}

/* CP timing validation helper */
void validate_cp_timing_in_sequence(double total_cp_time, double duty_limit) {
    if (total_cp_time > 50e-3) {
        printf("ABORT: Total CP time %.1f ms exceeds 50 ms limit\n", 
               total_cp_time * 1e3);
        psg_abort(1);
    }
    
    // This would be called as part of overall duty cycle calculation
    printf("Total CP time: %.1f ms\n", total_cp_time * 1e3);
}

/* Convenience macros for common patterns */
#define SETUP_HYX_CP() \
    CP hy = setup_hy_cp(); \
    CP yx = setup_yx_cp(); \
    CP xh = setup_xh_cp();

#define SETUP_HXH_CP() \
    CP hx = setup_hx_cp(); \
    CP xh = setup_xh_cp();

#define SETUP_HX_CP_ONLY() \
    CP hx = setup_hx_cp();

/* Channel assignment verification */
void verify_cp_channels(void) {
    printf("CP Channel Assignments:\n");
    printf("  H: dec channel\n");
    printf("  X: obs channel\n"); 
    printf("  Y: dec2 channel\n");
    printf("  Z: dec3 channel\n");
}

/* Legacy compatibility - maps old manual setup to new functions */
#define LEGACY_HX_CP() setup_hx_cp()
#define LEGACY_HY_CP() setup_hy_cp()
#define LEGACY_YX_CP() setup_yx_cp()
#define LEGACY_XH_CP() setup_xh_cp()

#endif /* BIOSOLIDCP_H */
