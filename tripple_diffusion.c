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
    int frame_timestep = config.frame_timestep;

    // set dependency index for faster branching later in the main loop
    int dependency_id = -1;
    if (strcmp(dependency, "symmetric") == 0)
        dependency_id = 1;
    else if (strcmp(dependency, "cyclic") == 0)
        dependency_id = 0;
    else {
        printf("Unknown dependency in config: %s, valid options are 'symmetric' and "
               "'cyclic'\n",
               dependency);
        exit(EXIT_FAILURE);
    }

    // same for boundary
    int boundary_id = -1;
    if (strcmp(boundary, "reflecting") == 0)
        boundary_id = 1;
    else if (strcmp(boundary, "periodic") == 0)
        boundary_id = 0;
    else {
        printf("Unknown boundary in config: %s, valid options are 'reflecting' and "
               "'periodic'\n",
               boundary);
        exit(EXIT_FAILURE);
    }

    // same for type (power or logistic)
    int type_id = -1;
    if (strcmp(type, "power") == 0)
        type_id = 1;
    else if (strcmp(type, "logistic") == 0)
        type_id = 0;
    else {
        printf("Unknown type in config: %s, valid options are 'power' and 'logistic'\n",
               type);
        exit(EXIT_FAILURE);
    }

    printf("\nframe_timestep = %d\n", frame_timestep);

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
    if (type_id)
        snprintf(config_name, 255, "%s_%s_init-%s_q%d_c%g_dt%g_nt%d_nr%d_rs%d_bins%d_ft%d",
                 dependency, boundary, init_density, q, c, delta_t, n_t, n_realizations,
                 rs, n_bins, frame_timestep);
    else
        snprintf(config_name, 255,
                 "%s_%s_init-%s_p0%g_alpha%g_dt%g_nt%d_nr%d_rs%d_bins%d_ft%d", dependency,
                 boundary, init_density, p_0, alpha, delta_t, n_t, n_realizations, rs,
                 n_bins, frame_timestep);

    char counts_filename[1024];
    snprintf(counts_filename, 1023, "../runs/%s/data/counts_%s_%s.txt", run, type,
             config_name);

    char coordinates_filename[1024];
    snprintf(coordinates_filename, 1023, "../runs/%s/data/coordinates_%s_%s.txt", run,
             type, config_name);

    char log_filename[1024];
    snprintf(log_filename, 1023, "../runs/%s/data/log_%s_%s.txt", run, type, config_name);

    // if the run directory doesnt exist yet, create it
    char run_dirname[128];
    snprintf(run_dirname, 127, "../runs/%s", run);
    char data_dirname[256];
    snprintf(data_dirname, 255, "../runs/%s/data", run);
    char animations_dirname[256];
    snprintf(animations_dirname, 255, "../runs/%s/animations", run);

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

    double *A_coordinates = malloc(n_realizations * sizeof(double));
    double *B_coordinates = malloc(n_realizations * sizeof(double));
    double *C_coordinates = malloc(n_realizations * sizeof(double));
    double *coordinates[3] = {A_coordinates, B_coordinates, C_coordinates};

    int *A_counts = malloc(n_bins * sizeof(int));
    int *B_counts = malloc(n_bins * sizeof(int));
    int *C_counts = malloc(n_bins * sizeof(int));
    int *counts[3] = {A_counts, B_counts, C_counts};

    int file_found = 0;

    FILE *counts_file;
    FILE *coordinates_file;
    FILE *log_file;

    int i_mb_checkpoint = 0;
    int continue_offset = 0;

    // see if we can load a preexisting checkpoint:
    if (check_for_existing_checkpoint(coordinates_filename)) {
        if (load_checkpoint(coordinates_filename, A_coordinates, B_coordinates,
                            C_coordinates, n_realizations, &i_mb_checkpoint)) {
            printf("Since the checkpoint exists, open files in append mode\n");
            file_found = 1;
            counts_file = fopen(counts_filename, "a");
            coordinates_file = fopen(coordinates_filename, "a");
            log_file = fopen(log_filename, "a");
            continue_offset = 1;
            // fprintf(counts_file, "\n");
            // fprintf(coordinates_file, "\n");
            // fprintf(log_file, "\n");
        }
    }
    if (file_found == 0) {
        printf("file_found = 0, so we are creating new files to write now\n");
        counts_file = fopen(counts_filename, "w");
        coordinates_file = fopen(coordinates_filename, "w");
        log_file = fopen(log_filename, "w");
        if (strcmp(init_density, "uniform") == 0) {
            distribute_coordinates_uniformly(A_coordinates, n_realizations, lower_bound,
                                             upper_bound);
            distribute_coordinates_uniformly(B_coordinates, n_realizations, lower_bound,
                                             upper_bound);
            distribute_coordinates_uniformly(C_coordinates, n_realizations, lower_bound,
                                             upper_bound);
        } else if (strcmp(init_density, "demixed") == 0) {
            distribute_coordinates_in_one_third(A_coordinates, n_realizations,
                                                lower_bound, upper_bound, 0);
            distribute_coordinates_in_one_third(B_coordinates, n_realizations,
                                                lower_bound, upper_bound, 1);
            distribute_coordinates_in_one_third(C_coordinates, n_realizations,
                                                lower_bound, upper_bound, 2);
        } else {
            printf("Invalid init_density in config: %s, valid options are 'uniform' and "
                   "'demixed'\n",
                   init_density);
            exit(EXIT_FAILURE);
        }

        // write the initial counts and coordinates
        for (int k = 0; k <= 2; k++) {
            fprintf(counts_file, "0 ");
            write_int_array(counts_file, counts[k], n_bins, "");
        }
        for (int k = 0; k <= 2; k++) {
            fprintf(coordinates_file, "0 ");
            write_double_array(coordinates_file, coordinates[k], n_realizations, "");
        }
    }

    histogram(A_coordinates, A_counts, n_realizations, n_bins, lower_bound, upper_bound);
    histogram(B_coordinates, B_counts, n_realizations, n_bins, lower_bound, upper_bound);
    histogram(C_coordinates, C_counts, n_realizations, n_bins, lower_bound, upper_bound);

    double range = upper_bound - lower_bound;
    double bin_size = range / n_bins;
    double delta_x = (upper_bound - lower_bound) / n_bins;

    // key is the indey of in the array, values are the densities on which the
    // density of the <key>-particle-sort depends) int

    int symmetric_dependencies_map[3][2] = {{1, 2}, {2, 0}, {0, 1}};
    int cyclic_dependencies_map[3] = {2, 0, 1};

    printf("Compiler chill, i am using both maps...\n");
    printf("sdm[0][0] = %d\n", symmetric_dependencies_map[0][0]);
    printf("cdm[0] = %d\n", cyclic_dependencies_map[0]);

    int n_bins_within_rs = 1 + 2 * rs;

    double io_time = 0.0;
    double progress_bar_time = 0.0;
    double iloop_time = 0.0;
    double histogram_time = 0.0;

    time_t start_iloop = time(NULL);
    for (int i = i_mb_checkpoint + 1; i < i_mb_checkpoint + continue_offset + n_t; i++) {
        // first compute all histograms for the current timestep so that every
        // particle sort sees the same density
        time_t start_histogram = time(NULL);
        for (int k = 0; k <= 2; k++) {
            histogram(coordinates[k], counts[k], n_realizations, n_bins, lower_bound,
                      upper_bound);
        }
        time_t end_histogram = time(NULL);
        histogram_time += end_histogram - start_histogram;

        // increment time for both particla sorts in parallel
        for (int k = 0; k <= 2; k++) {
            int *dependency_ind;
            if (dependency_id)
                dependency_ind = symmetric_dependencies_map[k];
            else
                dependency_ind = &cyclic_dependencies_map[k];

#pragma omp parallel for
            for (int j = 0; j < n_realizations; j++) {
                int thread_id = omp_get_thread_num();
                gsl_rng *local_r = thread_rngs[thread_id];

                // get the bin in which current coordinate falls
                int bin = (int)((coordinates[k][j] - lower_bound) / bin_size);
                if (bin >= n_bins)
                    bin = n_bins - 1;
                if (bin < 0)
                    bin = 0;

                double coordinate;

                if (dependency_id) {
                    // ----------this is for tripple symmetric diffusion ---------
                    double density1 = (double)counts[dependency_ind[0]][bin] /
                                      (n_realizations * delta_x);
                    double density2 = (double)counts[dependency_ind[1]][bin] /
                                      (n_realizations * delta_x);

                    // accumulate the densities from neighboring bins in non-local case
                    // (rs >= 1)
                    for (int offset = 1; offset <= rs; offset++) {
                        if (bin >= offset) {
                            density1 += (double)counts[dependency_ind[0]][bin - offset] /
                                        (n_realizations * delta_x);
                            density2 += (double)counts[dependency_ind[1]][bin - offset] /
                                        (n_realizations * delta_x);
                        }
                        if (bin <= n_bins - offset - 1) {
                            density1 += (double)counts[dependency_ind[0]][bin + offset] /
                                        (n_realizations * delta_x);
                            density2 += (double)counts[dependency_ind[1]][bin + offset] /
                                        (n_realizations * delta_x);
                        }
                    }
                    // normalize to the number of bins within the sensing radius
                    density1 /= n_bins_within_rs;
                    density2 /= n_bins_within_rs;

                    if (type_id)
                        coordinate = symmetric_tripple_power_diffuse(
                            coordinates[k][j], d, c, q, delta_t, density1, density2,
                            local_r);
                    else
                        coordinate = symmetric_tripple_logistic_diffuse(
                            coordinates[k][j], d, p_0, alpha, delta_t, density1, density2,
                            local_r);
                } else {
                    // -------------- this is for tripple cyclic diffusion ---------------
                    double density =
                        (double)counts[*dependency_ind][bin] / (n_realizations * delta_x);

                    // accumulate the densities from neighboring bins in non-local case
                    // (rs >= 1)
                    for (int offset = 1; offset <= rs; offset++) {
                        if (bin >= offset) {
                            density += (double)counts[*dependency_ind][bin - offset] /
                                       (n_realizations * delta_x);
                        }
                        if (bin <= n_bins - offset - 1) {
                            density += (double)counts[*dependency_ind][bin + offset] /
                                       (n_realizations * delta_x);
                        }
                    }
                    density /= n_bins_within_rs;
                    if (type_id)
                        coordinate = double_power_diffuse(coordinates[k][j], d, c, q,
                                                          delta_t, density, local_r);
                    else
                        coordinate = double_logistic_diffuse(
                            coordinates[k][j], d, p_0, alpha, delta_t, density, local_r);
                }

                if (boundary_id)
                    coordinate =
                        reflecting_boundary(coordinate, lower_bound, upper_bound);
                else if (boundary_id == 0)
                    coordinate = periodic_boundary(coordinate, lower_bound, upper_bound);
                else {
                    printf("Uknown boundary_id: %d", boundary_id);
                    exit(EXIT_FAILURE);
                }

                coordinates[k][j] = coordinate;
            }
        }

        // save a snapshot of the simulation (counts/coordinates) state every
        // frame_timestep
        time_t start_io = time(NULL);
        if (i % frame_timestep == 0) {
            for (int k = 0; k <= 2; k++) {
                fprintf(counts_file, "%d ", i);
                write_int_array(counts_file, counts[k], n_bins, "");
            }
            // overwrite the coordinates
            fclose(coordinates_file);
            coordinates_file = fopen(coordinates_filename, "w");
            for (int k = 0; k <= 2; k++) {
                fprintf(coordinates_file, "%d ", i);
                write_double_array(coordinates_file, coordinates[k], n_realizations, "");
            }
        }
        time_t end_io = time(NULL);
        io_time += end_io - start_io;

        // update the progress bar
        time_t start_progress_bar = time(NULL);
        if (i % (n_t / 100) == 0 || i == n_t - 1) {
            float progress = (float)i / (n_t - 1);
            int bar_width = 100;
            int pos = bar_width * progress;

            printf("\r[");
            for (int p = 0; p < bar_width; p++) {
                if (p < pos)
                    printf("=");
                else if (p == pos)
                    printf(">");
                else
                    printf(" ");
                fflush(stdout);
            }
            printf("] %3d%%", (int)(progress * 100.0));

            fflush(stdout);
        }
        time_t end_progress_bar = time(NULL);
        progress_bar_time += end_progress_bar - start_progress_bar;
    }
    time_t end_iloop = time(NULL);
    iloop_time = end_iloop - start_iloop;

    double io_part = io_time / iloop_time;
    double progress_bar_part = progress_bar_time / iloop_time;
    double histogram_part = histogram_time / iloop_time;

    printf("\nio_part = %f\n", io_part);
    printf("progress_bar_part = %f\n", progress_bar_part);
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

    fclose(counts_file);
    free(A_coordinates);
    free(B_coordinates);
    free(C_coordinates);
    free(A_counts);
    free(B_counts);
    free(C_counts);
    for (int i = 0; i < max_threads; i++) {
        gsl_rng_free(thread_rngs[i]);
    }
    free(thread_rngs);
}

int main(int argc, char *argv[]) {
    char config_path[64];
    if (argc == 1) {
        snprintf(config_path, 63, "../configs/%s", "config.txt");
    } else {
        snprintf(config_path, 63, "../configs/%s.txt", argv[1]);
    }
    DiffusionConfig config = read_config(config_path);
    diffuse_and_save_histograms(config);

    return 0;
}
