/* biosolidnus.h - Single-header NUS implementation for VnmrJ/OpenVJ pulse sequences
 * 
 * This header provides a unified, robust NUS implementation that can be used
 * across all biosolid pulse sequences. Simply #include this file and use the
 * NUS functions. All code is contained in this single header for compatibility
 * with the seqgen compilation system.
 * 
 * PARAMETER SETUP FOR NUS:
 * 
 * The key parameters that control NUS behavior are:
 * 
 * 1. SPARSE='y'           - Enables NUS mode
 * 2. ni, ni2, ni3         - Define the maximum increments for each dimension
 *                          These determine the range of valid increments in your NUS schedule
 *                          (e.g., ni=64 means increments 0-63 are valid for F2)
 * 3. nrows               - Number of NUS points to collect
 *                          This should match the number of lines in your NUS schedule file
 * 4. sparse_file         - Path to your NUS schedule file
 * 5. skiprows=0          - Number of header lines to skip in schedule file
 * 
 * EXAMPLE SETUPS:
 * 
 * For 3D experiment with full sampling of 64x32=2048 points, using 500 NUS points:
 *   ni=64, ni2=32, ni3=1, nrows=500, SPARSE='y'
 * 
 * For 2D experiment with full sampling of 128 points, using 64 NUS points:
 *   ni=128, ni2=1, nrows=64, SPARSE='y'
 * 
 * The schedule file should contain lines like:
 *   3D: "12 5" (meaning d2_inc=12, d3_inc=5)
 *   2D: "25"   (meaning d2_inc=25)
 * 
 * Usage:
 *   #include "biosolidnus.h"
 *   
 *   In pulsesequence():
 *     if (NUS_ACTIVE()) {
 *         if (NUS_INIT(3) != 0) psg_abort(1);  // 3D experiment
 *         if (NUS_CALC_DELAYS() != 0) psg_abort(1);
 *         if (NUS_SAFETY_CHECK(tRF, "n", 0.0) != 0) psg_abort(1);
 *     }
 */

#ifndef BIOSOLIDNUS_H
#define BIOSOLIDNUS_H

#include "standard.h"

/* NUS Constants */
#define MAXFIDS 400000
#define MAXDIM 4
#define NUS_LINE_BUFFER 256

/* Static NUS variables - private to each sequence */
static int nus_sel[MAXFIDS][MAXDIM-1];
static double nus_d_const[MAXDIM-1];
static int nus_initialized = 0;
static int nus_maxdim = 0;
static int nus_ni_vals[MAXDIM-1];
static double nus_sw_vals[MAXDIM-1];
static int nus_nrows;
static int nus_skiprows;
static char nus_sparse_file[MAXSTR];

/* Forward declarations */
static int nus_get_index_from_ix(int ix);
static void nus_print_error(const char *function, const char *message);

/*
 * nus_is_active - Check if NUS is enabled
 * Returns: 1 if SPARSE='y', 0 otherwise
 */
static int nus_is_active(void) {
    char sparse[MAXSTR];
    getstr("SPARSE", sparse);
    return (sparse[0] == 'y') ? 1 : 0;
}

/*
 * nus_print_error - Standardized error message printing
 */
static void nus_print_error(const char *function, const char *message) {
    printf("NUS ERROR in %s: %s\n", function, message);
}

/*
 * nus_get_index_from_ix - Calculate NUS array index from VnmrJ ix value
 * ix: Current increment number (1-based)
 * Returns: NUS array index (0-based), -1 on error
 */
static int nus_get_index_from_ix(int ix) {
    if (ix < 1) {
        return -1;
    }
    
    /* Count active dimensions */
    int active_dims = 0;
    for (int i = 0; i < nus_maxdim-1; i++) {
        if (nus_ni_vals[i] > 1) active_dims++;
    }
    
    /* Calculate index based on hypercomplex requirements */
    int nus_index;
    if (active_dims <= 1) {
        nus_index = (ix - 1) / 2;  /* 2D case */
    } else {
        nus_index = (ix - 1) / (1 << active_dims);  /* General case: 2^n */
    }
    
    return nus_index;
}

/*
 * nus_read_schedule - Read and validate NUS schedule file
 * Returns: 0 on success, -1 on error
 */
static int nus_read_schedule(void) {
    FILE *fsparse_table;
    char line_buffer[NUS_LINE_BUFFER];
    int line_num = 0;
    
    if (!nus_initialized) {
        nus_print_error("nus_read_schedule", "NUS not initialized");
        return -1;
    }
    
    fsparse_table = fopen(nus_sparse_file, "r");
    if (NULL == fsparse_table) {
        printf("ERROR: Cannot open NUS schedule file: %s\n", nus_sparse_file);
        return -1;
    }
    
    printf("Reading NUS schedule from: %s\n", nus_sparse_file);
    
    /* Skip header rows if specified */
    for (int i = 0; i < nus_skiprows; i++) {
        if (!fgets(line_buffer, NUS_LINE_BUFFER, fsparse_table)) {
            printf("ERROR: Cannot skip %d header rows\n", nus_skiprows);
            fclose(fsparse_table);
            return -1;
        }
    }
    
    /* Read NUS schedule points */
    for (int i = 0; i < nus_nrows; i++) {
        line_num++;
        
        /* Read complete line */
        if (!fgets(line_buffer, NUS_LINE_BUFFER, fsparse_table)) {
            printf("ERROR: Unexpected end of file at line %d\n", line_num);
            fclose(fsparse_table);
            return -1;
        }
        
        /* Parse line based on dimensionality */
        int values[MAXDIM-1];
        int parsed_values = 0;
        
        if (nus_maxdim == 2) {
            parsed_values = sscanf(line_buffer, "%d", &values[0]);
            if (parsed_values == 1) {
                values[1] = 0;  /* No second dimension for 2D */
            }
        } else if (nus_maxdim == 3) {
            parsed_values = sscanf(line_buffer, "%d %d", &values[0], &values[1]);
        } else if (nus_maxdim == 4) {
            parsed_values = sscanf(line_buffer, "%d %d %d", &values[0], &values[1], &values[2]);
        }
        
        if (parsed_values != nus_maxdim-1) {
            printf("ERROR: Line %d has %d values, expected %d\n", 
                   line_num, parsed_values, nus_maxdim-1);
            printf("       Line content: '%s'\n", line_buffer);
            fclose(fsparse_table);
            return -1;
        }
        
        /* Validate and store increments */
        for (int j = 0; j < nus_maxdim-1; j++) {
            if (values[j] < 0 || values[j] >= nus_ni_vals[j]) {
                printf("ERROR: Line %d dim %d value %d out of range [0,%d]\n", 
                       line_num, j+1, values[j], nus_ni_vals[j]-1);
                fclose(fsparse_table);
                return -1;
            }
            nus_sel[i][j] = values[j];
        }
        
        /* Set unused dimensions to 0 */
        for (int j = nus_maxdim-1; j < MAXDIM-1; j++) {
            nus_sel[i][j] = 0;
        }
    }
    
    fclose(fsparse_table);
    printf("Successfully read %d NUS points\n", nus_nrows);
    
    return 0;
}

/*
 * nus_print_summary - Print detailed NUS schedule information
 */
static void nus_print_summary(void) {
    if (!nus_initialized) {
        nus_print_error("nus_print_summary", "NUS not initialized");
        return;
    }
    
    printf("\n============================ NUS Schedule Summary ============================\n");
    printf("Experiment: %dD\n", nus_maxdim);
    printf("NUS points: %d\n", nus_nrows);
    printf("Schedule file: %s\n", nus_sparse_file);
    printf("Skipped rows: %d\n", nus_skiprows);
    
    /* Print dimension info */
    printf("\nDimensions:\n");
    for (int i = 0; i < nus_maxdim-1; i++) {
        if (i == 0) {
            printf("  F2/d2: ni=%d, sw=%.1f Hz, const_delay=%.3f ms\n", 
                   nus_ni_vals[i], nus_sw_vals[i], nus_d_const[i]*1000);
        } else if (i == 1) {
            printf("  F1/d3: ni2=%d, sw=%.1f Hz, const_delay=%.3f ms\n", 
                   nus_ni_vals[i], nus_sw_vals[i], nus_d_const[i]*1000);
        } else if (i == 2) {
            printf("  F3/d4: ni3=%d, sw=%.1f Hz, const_delay=%.3f ms\n", 
                   nus_ni_vals[i], nus_sw_vals[i], nus_d_const[i]*1000);
        }
    }
    
    /* Print first few NUS points as examples */
    printf("\nFirst 10 NUS points:\n");
    printf("Point |");
    for (int j = 0; j < nus_maxdim-1; j++) {
        if (j == 0) {
            printf(" d2_inc | d2_time(ms) |");
        } else if (j == 1) {
            printf(" d3_inc | d3_time(ms) |");
        } else if (j == 2) {
            printf(" d4_inc | d4_time(ms) |");
        }
    }
    printf("\n");
    
    int show_points = (nus_nrows < 10) ? nus_nrows : 10;
    for (int i = 0; i < show_points; i++) {
        printf("%5d |", i+1);
        for (int j = 0; j < nus_maxdim-1; j++) {
            double time_ms = (nus_d_const[j] + nus_sel[i][j]/nus_sw_vals[j]) * 1000;
            printf("   %4d |      %6.3f |", nus_sel[i][j], time_ms);
        }
        printf("\n");
    }
    if (nus_nrows > 10) {
        printf("... and %d more points\n", nus_nrows - 10);
    }
    printf("=============================================================================\n\n");
}

/*
 * nus_init_sequence - Initialize NUS for a specific dimensionality
 * maxdim: 2 for 2D, 3 for 3D, 4 for 4D experiments
 * Returns: 0 on success, -1 on error
 */
static int nus_init_sequence(int maxdim) {
    if (maxdim < 2 || maxdim > MAXDIM) {
        nus_print_error("nus_init_sequence", "Invalid maxdim (must be 2-4)");
        return -1;
    }
    
    nus_maxdim = maxdim;
    
    /* Get experimental parameters */
    nus_ni_vals[0] = (int)getval("ni");
    if (maxdim >= 3) nus_ni_vals[1] = (int)getval("ni2");
    if (maxdim >= 4) nus_ni_vals[2] = (int)getval("ni3");
    
    nus_sw_vals[0] = getval("sw1");
    if (maxdim >= 3) nus_sw_vals[1] = getval("sw2");
    if (maxdim >= 4) nus_sw_vals[2] = getval("sw3");
    
    /* Store original delay values */
    nus_d_const[0] = d2;
    if (maxdim >= 3) nus_d_const[1] = d3;
    if (maxdim >= 4) nus_d_const[2] = d4;
    
    /* Get NUS parameters with intelligent handling */
    nus_nrows = (int)getval("nrows");
    nus_skiprows = (int)getval("skiprows");
    getstr("sparse_file", nus_sparse_file);
    
    /* Intelligent parameter validation and correction */
    int expected_full_sampling = 1;
    for (int i = 0; i < maxdim-1; i++) {
        if (nus_ni_vals[i] > 1) {
            expected_full_sampling *= nus_ni_vals[i];
        }
    }
    
    /* Check for parameter consistency and provide helpful guidance */
    if (nus_nrows <= 0) {
        printf("ERROR: nrows must be > 0\n");
        printf("HINT: For NUS, set nrows to the number of points in your NUS schedule\n");
        return -1;
    }
    
    if (nus_nrows > MAXFIDS) {
        printf("ERROR: nrows (%d) exceeds MAXFIDS (%d)\n", nus_nrows, MAXFIDS);
        return -1;
    }
    
    /* Validate ni parameters */
    for (int i = 0; i < maxdim-1; i++) {
        if (nus_ni_vals[i] <= 0) {
            printf("ERROR: ni%s must be > 0 for %dD experiment\n", 
                   (i==0) ? "" : (i==1) ? "2" : "3", maxdim);
            return -1;
        }
    }
    
    /* Print parameter guidance for users */
    printf("\nNUS Parameter Setup:\n");
    printf("  Experiment: %dD\n", maxdim);
    printf("  ni values: ");
    for (int i = 0; i < maxdim-1; i++) {
        if (i == 0) {
            printf("ni=%d ", nus_ni_vals[i]);
        } else if (i == 1) {
            printf("ni2=%d ", nus_ni_vals[i]);
        } else if (i == 2) {
            printf("ni3=%d ", nus_ni_vals[i]);
        }
    }
    printf("\n");
    printf("  Full sampling would be: %d points\n", expected_full_sampling);
    printf("  NUS sampling: %d points (%.1f%% of full)\n", 
           nus_nrows, 100.0 * nus_nrows / expected_full_sampling);
    
    /* Consistency warnings */
    if (nus_nrows > expected_full_sampling) {
        printf("WARNING: nrows (%d) > full sampling (%d) - check your parameters!\n", 
               nus_nrows, expected_full_sampling);
    }
    
    /* Check for common parameter setup mistakes */
    int total_ni_product = nus_ni_vals[0];
    for (int i = 1; i < maxdim-1; i++) {
        total_ni_product *= nus_ni_vals[i];
    }
    
    if (nus_nrows == total_ni_product && nus_nrows < expected_full_sampling) {
        printf("NOTE: nrows equals ni product but less than expected full sampling\n");
        printf("      This suggests uniform undersampling rather than NUS\n");
    }
    
    nus_initialized = 1;
    
    /* Read schedule on first increment */
    if (ix == 1) {
        if (nus_read_schedule() != 0) {
            return -1;
        }
        nus_print_summary();
    }
    
    return 0;
}

/*
 * nus_calculate_delays - Calculate d2, d3, d4 for current increment
 * Returns: 0 on success, -1 on error
 */
static int nus_calculate_delays(void) {
    if (!nus_initialized) {
        nus_print_error("nus_calculate_delays", "NUS not initialized");
        return -1;
    }
    
    int nus_index = nus_get_index_from_ix(ix);
    if (nus_index < 0 || nus_index >= nus_nrows) {
        printf("ERROR: NUS index %d out of range [0,%d] (ix=%d)\n", nus_index, nus_nrows-1, ix);
        return -1;
    }
    
    /* Calculate delays for each active dimension */
    d2 = nus_d_const[0] + nus_sel[nus_index][0] / nus_sw_vals[0];
    
    if (nus_maxdim >= 3) {
        d3 = nus_d_const[1] + nus_sel[nus_index][1] / nus_sw_vals[1];
    }
    
    if (nus_maxdim >= 4) {
        d4 = nus_d_const[2] + nus_sel[nus_index][2] / nus_sw_vals[2];
    }
    
    return 0;
}

/*
 * nus_validate_safety - Check that tRF is sufficient for maximum evolution times
 * tRF: Relaxation delay parameter
 * ctN: Constant time flag ("y" or "n")
 * t3max: Maximum constant time value (for ctN="y")
 * Returns: 0 if safe, -1 if unsafe
 */
static int nus_validate_safety(double tRF, char *ctN, double t3max) {
    if (!nus_initialized) {
        nus_print_error("nus_validate_safety", "NUS not initialized");
        return -1;
    }
    
    double max_total_time = 0.0;
    
    for (int i = 0; i < nus_nrows; i++) {
        double total_time = 0.0;
        
        /* Calculate total evolution time for this NUS point */
        for (int j = 0; j < nus_maxdim-1; j++) {
            double evo_time = nus_d_const[j] + nus_sel[i][j] / nus_sw_vals[j];
            
            /* Handle constant time dimension (typically d3/F2) */
            if (j == 1 && ctN && !strcmp(ctN, "y") && t3max > 0) {
                total_time += t3max;  /* Use constant time value */
            } else {
                total_time += evo_time;
            }
        }
        
        if (total_time > max_total_time) {
            max_total_time = total_time;
        }
        
        if (tRF <= total_time) {
            printf("ERROR: NUS point %d total evolution time %.3f ms exceeds tRF %.3f ms\n", 
                   i+1, total_time*1000, tRF*1000);
            printf("       Consider increasing tRF or reducing maximum evolution times\n");
            return -1;
        }
    }
    
    printf("NUS safety check PASSED: max evolution %.3f ms < tRF %.3f ms\n", 
           max_total_time*1000, tRF*1000);
    return 0;
}

/* Convenience Macros */
#define NUS_ACTIVE() nus_is_active()
#define NUS_INIT(dim) nus_init_sequence(dim)
#define NUS_CALC_DELAYS() nus_calculate_delays()
#define NUS_SAFETY_CHECK(trf, ctn, t3max) nus_validate_safety(trf, ctn, t3max)

#endif /* BIOSOLIDNUS_H */
