#include "io.h"
#include "simulation.h"
#include "time.h"
#include "utils.h"
#include <fcntl.h>
#include <gsl/gsl_rng.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

void diffuse_and_save_histograms(DiffusionConfig config) {
    time_t start = time(NULL);

    char *type = config.type;
    char *run = config.run;
    char *dependency = config.dependency;
    char *boundary = config.boundary;
    char *init_density = config.init_density;
    double delta_t = config.delta_t;
    // double start = config.start;
    double lower_bound = config.lower_bound;
    double upper_bound = config.upper_bound;
    double d = config.d;
    int n_t = config.n_t;
    int n_realizations = config.n_realizations;
    int n_bins = config.n_bins;
    double c = config.c;
    double alpha = config.alpha;
    double p_0 = config.p_0;
    int q = config.q;
    int rs = config.rs;
    int cnts_timestep = config.cnts_timestep;
    int crds_snapshot = config.crds_snapshot;

    // set dependency index for faster branching later in the main loop
    int dependency_id = -1;
    if (strcmp(dependency, "sym") == 0)
        dependency_id = 1;
    else if (strcmp(dependency, "cy") == 0)
        dependency_id = 0;
    else {
        printf("Unknown dependency in config: %s, valid options are 'sym' and "
               "'cy'\n",
               dependency);
        exit(EXIT_FAILURE);
    }

    // same for boundary
    int boundary_id = -1;
    if (strcmp(boundary, "ref") == 0)
        boundary_id = 1;
    else if (strcmp(boundary, "per") == 0)
        boundary_id = 0;
    else {
        printf("Unknown boundary in config: %s, valid options are 'ref' and "
               "'per'\n",
               boundary);
        exit(EXIT_FAILURE);
    }

    // same for type (pow or log)
    int type_id = -1;
    if (strcmp(type, "pow") == 0)
        type_id = 1;
    else if (strcmp(type, "log") == 0)
        type_id = 0;
    else {
        printf("Unknown type in config: %s, valid options are 'pow' and 'log'\n", type);
        exit(EXIT_FAILURE);
    }

    printf("\ncnts_timestep = %d\n", cnts_timestep);

    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;

    int max_threads = omp_get_max_threads();
    printf("\nmax_threads = %d\n", max_threads);

    gsl_rng **thread_rngs = malloc(max_threads * sizeof(gsl_rng *));

    for (int i = 0; i < max_threads; i++) {
        thread_rngs[i] = gsl_rng_alloc(T);
        gsl_rng_set(thread_rngs[i], time(NULL) + i);
    }

    // construct the file name
    char config_name[256];
    if (type_id) { // running pow model
        snprintf(config_name, 255, "%s_%s_init-%s_q%d_c%g_dt%g_nr%d_rs%d_bins%d_ft%d",
                 dependency, boundary, init_density, q, c, delta_t, n_realizations, rs,
                 n_bins, cnts_timestep);
        printf("Built pow model config name: %s\n", config_name);
    } else { // running log model
        snprintf(config_name, 255,
                 "%s_%s_init-%s_p0%g_alpha%g_dt%g_nr%d_rs%d_bins%d_cnts-step%d", dependency,
                 boundary, init_density, p_0, alpha, delta_t, n_realizations, rs, n_bins,
                 cnts_timestep);
        printf("Built log model config name: %s\n", config_name);
    }

    char cnts_filename[1024];
    snprintf(cnts_filename, 1023, "../runs/%s/data/cnts_%s_%s.txt", run, type,
             config_name);
    printf("Counts file name: %s\n", cnts_filename);

    char crds_filename[1024];
    snprintf(crds_filename, 1023, "../runs/%s/data/crds_%s_%s.txt", run, type,
             config_name);
    printf("Coordinates file name: %s\n", crds_filename);

    char log_filename[1024];
    snprintf(log_filename, 1023, "../runs/%s/data/log_%s_%s.txt", run, type, config_name);
    printf("Log file name: %s\n", log_filename);

    // if the run directory doesnt exist yet, create it
    char run_dirname[128];
    snprintf(run_dirname, 127, "../runs/%s", run);
    printf("Run directory name: %s\n", run_dirname);
    char data_dirname[256];
    snprintf(data_dirname, 255, "../runs/%s/data", run);
    printf("Data directory name: %s\n", data_dirname);
    char animations_dirname[256];
    snprintf(animations_dirname, 255, "../runs/%s/animations", run);
    printf("Animations directory name: %s\n", animations_dirname);

    struct stat statbuf;
    if (stat(run_dirname, &statbuf) == 0) {
        printf("Run directory already exists.\n");
    } else {
        if (mkdir(run_dirname, 0755) == 0) {
            printf("Run directory created successfully: %s\n", run_dirname);
        } else {
            perror("Error creating run directory");
        }
        if (mkdir(data_dirname, 0755) == 0) {
            printf("Data directory created successfully: %s\n", data_dirname);
        } else {
            perror("Error creating data directory");
        }
        if (mkdir(animations_dirname, 0755) == 0) {
            printf("Animations directory created successfully: %s\n", animations_dirname);
        } else {
            perror("Error creating animations directory");
        }
    }

    double *A_crds = malloc(n_realizations * sizeof(double));
    double *B_crds = malloc(n_realizations * sizeof(double));
    double *C_crds = malloc(n_realizations * sizeof(double));
    double *crds[3] = {A_crds, B_crds, C_crds};

    int *A_cnts = malloc(n_bins * sizeof(int));
    int *B_cnts = malloc(n_bins * sizeof(int));
    int *C_cnts = malloc(n_bins * sizeof(int));
    int *cnts[3] = {A_cnts, B_cnts, C_cnts};

    int file_found = 0;

    FILE *cnts_file;
    FILE *crds_file;
    FILE *log_file;

    int i_mb_checkpoint = 1;
    int first_iter_offset = 1;

    // see if we can load a preexisting checkpoint:
    if (check_for_existing_checkpoint(crds_filename)) {
        if (load_checkpoint(crds_filename, A_crds, B_crds, C_crds, n_realizations,
                            &i_mb_checkpoint)) {
            printf("Since the checkpoint exists, open files in append mode\n");
            file_found = 1;
            cnts_file = fopen(cnts_filename, "a");
            crds_file = fopen(crds_filename, "a");
            log_file = fopen(log_filename, "a");
            first_iter_offset = 0;
            // fprintf(cnts_file, "\n");
            // fprintf(crds_file, "\n");
            // fprintf(log_file, "\n");
        }
    }
    if (file_found == 0) {
        printf("file_found = 0, so we are creating new files to write now\n");
        cnts_file = fopen(cnts_filename, "w");
        if (cnts_file == NULL) {
            printf("Error creating cnts file :(\n)");
            exit(EXIT_FAILURE);
        } else
            printf("Counts file created successfully\n");
        crds_file = fopen(crds_filename, "w");
        if (crds_file == NULL) {
            printf("Error creating crds file :(\n)");
            exit(EXIT_FAILURE);
        } else
            printf("Coordinates file created successfully\n");
        log_file = fopen(log_filename, "w");
        if (log_file == NULL) {
            printf("Error creating log file :(\n)");
            exit(EXIT_FAILURE);
        } else
            printf("Log file created successfully\n");

        if (strcmp(init_density, "un") == 0) {
            printf("Distributing the init crds uniformly\n");
            distribute_crds_uniformly(A_crds, n_realizations, lower_bound, upper_bound);
            distribute_crds_uniformly(B_crds, n_realizations, lower_bound, upper_bound);
            distribute_crds_uniformly(C_crds, n_realizations, lower_bound, upper_bound);
        } else if (strcmp(init_density, "dem") == 0) {
            printf("Preparing the init crds in the dem state\n");
            distribute_crds_in_one_third(A_crds, n_realizations, lower_bound, upper_bound,
                                         0);
            distribute_crds_in_one_third(B_crds, n_realizations, lower_bound, upper_bound,
                                         1);
            distribute_crds_in_one_third(C_crds, n_realizations, lower_bound, upper_bound,
                                         2);
        } else {
            printf("Invalid init_density in config: %s, valid options are 'un' and "
                   "'dem'\n",
                   init_density);
            exit(EXIT_FAILURE);
        }

        // write the initial cnts and crds
        printf("Writing init cnts to file\n");
        for (int k = 0; k <= 2; k++) {
            fprintf(cnts_file, "1 ");
            write_int_array(cnts_file, cnts[k], n_bins, "");
        }
        printf("Writing init crds to file\n");
        for (int k = 0; k <= 2; k++) {
            fprintf(crds_file, "1 ");
            write_double_array(crds_file, crds[k], n_realizations, "");
        }
    }

    printf("Calculating initial histograms\n");
    histogram(A_crds, A_cnts, n_realizations, n_bins, lower_bound, upper_bound);
    histogram(B_crds, B_cnts, n_realizations, n_bins, lower_bound, upper_bound);
    histogram(C_crds, C_cnts, n_realizations, n_bins, lower_bound, upper_bound);

    double range = upper_bound - lower_bound;
    double bin_size = range / n_bins;
    double delta_x = (upper_bound - lower_bound) / n_bins;

    // key is the indey of in the array, values are the densities on which the
    // density of the <key>-particle-sort depends) int

    int sym_dependencies_map[3][2] = {{1, 2}, {2, 0}, {0, 1}};
    int cy_dependencies_map[3] = {2, 0, 1};

    printf("Compiler chill, i am using both maps...\n");
    printf("sdm[0][0] = %d\n", sym_dependencies_map[0][0]);
    printf("cdm[0] = %d\n", cy_dependencies_map[0]);

    int n_bins_within_rs = 1 + 2 * rs;

    double io_time = 0.0;
    double iloop_time = 0.0;
    double histogram_time = 0.0;

    int time_loop_start = i_mb_checkpoint + 1;
    int time_loop_end = i_mb_checkpoint + n_t - first_iter_offset;
    printf("Starting the time loop from i = %d until %d\n",
           time_loop_start - first_iter_offset, time_loop_end);
    fflush(stdout);
    time_t start_iloop = time(NULL);
    for (int i = time_loop_start; i <= time_loop_end; i++) {
        // first compute all histograms for the current timestep so that every
        // particle sort sees the same density
        time_t start_histogram = time(NULL);
        for (int k = 0; k <= 2; k++) {
            histogram(crds[k], cnts[k], n_realizations, n_bins, lower_bound, upper_bound);
        }
        time_t end_histogram = time(NULL);
        histogram_time += end_histogram - start_histogram;

        // increment time for both particla sorts in parallel
        for (int k = 0; k <= 2; k++) {
            int *dependency_ind;
            if (dependency_id)
                dependency_ind = sym_dependencies_map[k];
            else
                dependency_ind = &cy_dependencies_map[k];

#pragma omp parallel for
            for (int j = 0; j < n_realizations; j++) {
                int thread_id = omp_get_thread_num();
                gsl_rng *local_r = thread_rngs[thread_id];

                // get the bin in which current coordinate falls
                int bin = (int)((crds[k][j] - lower_bound) / bin_size);
                if (bin >= n_bins)
                    bin = n_bins - 1;
                if (bin < 0)
                    bin = 0;

                double coordinate;

                if (dependency_id) {
                    // ----------this is for tripple sym diffusion ---------
                    double density1 =
                        (double)cnts[dependency_ind[0]][bin] / (n_realizations * delta_x);
                    double density2 =
                        (double)cnts[dependency_ind[1]][bin] / (n_realizations * delta_x);

                    // accumulate the densities from neighboring bins in non-local case
                    // (rs >= 1)
                    for (int offset = 1; offset <= rs; offset++) {
                        if (bin >= offset) {
                            density1 += (double)cnts[dependency_ind[0]][bin - offset] /
                                        (n_realizations * delta_x);
                            density2 += (double)cnts[dependency_ind[1]][bin - offset] /
                                        (n_realizations * delta_x);
                        }
                        if (bin <= n_bins - offset - 1) {
                            density1 += (double)cnts[dependency_ind[0]][bin + offset] /
                                        (n_realizations * delta_x);
                            density2 += (double)cnts[dependency_ind[1]][bin + offset] /
                                        (n_realizations * delta_x);
                        }
                    }
                    // normalize to the number of bins within the sensing radius
                    density1 /= n_bins_within_rs;
                    density2 /= n_bins_within_rs;

                    if (type_id)
                        coordinate = sym_tripple_pow_diffuse(crds[k][j], d, c, q, delta_t,
                                                             density1, density2, local_r);
                    else
                        coordinate =
                            sym_tripple_log_diffuse(crds[k][j], d, p_0, alpha, delta_t,
                                                    density1, density2, local_r);
                } else {
                    // -------------- this is for tripple cy diffusion ---------------
                    double density =
                        (double)cnts[*dependency_ind][bin] / (n_realizations * delta_x);

                    // accumulate the densities from neighboring bins in non-local case
                    // (rs >= 1)
                    for (int offset = 1; offset <= rs; offset++) {
                        if (bin >= offset) {
                            density += (double)cnts[*dependency_ind][bin - offset] /
                                       (n_realizations * delta_x);
                        }
                        if (bin <= n_bins - offset - 1) {
                            density += (double)cnts[*dependency_ind][bin + offset] /
                                       (n_realizations * delta_x);
                        }
                    }
                    density /= n_bins_within_rs;
                    if (type_id)
                        coordinate = double_pow_diffuse(crds[k][j], d, c, q, delta_t,
                                                        density, local_r);
                    else
                        coordinate = double_log_diffuse(crds[k][j], d, p_0, alpha,
                                                        delta_t, density, local_r);
                }

                if (boundary_id)
                    coordinate = ref_boundary(coordinate, lower_bound, upper_bound);
                else if (boundary_id == 0)
                    coordinate = per_boundary(coordinate, lower_bound, upper_bound);
                else {
                    printf("Uknown boundary_id: %d", boundary_id);
                    exit(EXIT_FAILURE);
                }

                crds[k][j] = coordinate;
            }
        }

        // save a snapshot of the simulation (cnts/crds) state every
        // cnts_timestep
        time_t start_io = time(NULL);
        if (i % cnts_timestep == 0) {
            for (int k = 0; k <= 2; k++) {
                fprintf(cnts_file, "%d ", i);
                write_int_array(cnts_file, cnts[k], n_bins, "");
            }
            fflush(stdout);
        }
        if (i % crds_snapshot == 0 || i == time_loop_end) {
            // overwrite the crds
            printf("Saving a crds snapshot for i = %d of the total %d\n", i,
                   time_loop_end);
            fclose(crds_file);
            crds_file = fopen(crds_filename, "w");
            for (int k = 0; k <= 2; k++) {
                fprintf(crds_file, "%d ", i);
                write_double_array(crds_file, crds[k], n_realizations, "");
            }
            fflush(stdout);
        }
        time_t end_io = time(NULL);
        io_time += end_io - start_io;
    }

    time_t end_iloop = time(NULL);
    iloop_time = end_iloop - start_iloop;

    double io_part = io_time / iloop_time;
    double histogram_part = histogram_time / iloop_time;

    printf("\nio_part = %f\n", io_part);
    printf("histogram_part = %f\n", histogram_part);

    time_t end = time(NULL);
    int timediff_sec = difftime(end, start);
    int hours = timediff_sec / 3600;
    int minutes = (timediff_sec - 3600 * hours) / 60;
    int seconds = timediff_sec - 3600 * hours - 60 * minutes;
    printf("\nThe simulation took %d hours %d minutes and %d seconds\n", hours, minutes,
           seconds);
    fprintf(log_file, "\nThe simulation took %d hours %d minutes and %d seconds\n", hours,
            minutes, seconds);

    fclose(cnts_file);
    free(A_crds);
    free(B_crds);
    free(C_crds);
    free(A_cnts);
    free(B_cnts);
    free(C_cnts);
    for (int i = 0; i < max_threads; i++) {
        gsl_rng_free(thread_rngs[i]);
    }
    free(thread_rngs);
}

int main(int argc, char *argv[]) {
    char config_path[128];
    if (argc == 1) {
        snprintf(config_path, 127, "../%s", "config.txt");
        printf("Using the default config path: %s\n", config_path);
    } else {
        snprintf(config_path, 127, "../%s", argv[1]);
        printf("Using the custom built config path: %s\n", config_path);
    }
    DiffusionConfig config = read_config(config_path);
    diffuse_and_save_histograms(config);

    return 0;
}
