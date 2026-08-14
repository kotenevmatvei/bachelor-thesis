#include "io.h"
#include "simulation.h"
#include "time.h"
#include "utils.h"
#include <gsl/gsl_rng.h>
#include <omp.h>
#include <stdio.h>
#include <time.h>

void diffuse_and_save_histograms(DiffusionConfig config) {
    time_t start = time(NULL);

    char *type = config.type;
    double delta_t = config.delta_t;
    // double start = config.start;
    double lower_bound = config.lower_bound;
    double upper_bound = config.upper_bound;
    double d = config.d;
    int n_t = config.n_t;
    int n_realizations = config.n_realizations;
    int n_bins = config.n_bins;
    int c = config.c;
    int q = config.q;
    int rs = config.rs;

    int frame_timestep = n_t / 1000;

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
    char config_name[128];
    snprintf(config_name, 127, "dt%g_nt%d_nr%d_c%d_q%d_rs%d_bins%d", delta_t, n_t,
             n_realizations, c, q, rs, n_bins);

    char counts_filename[256];
    snprintf(counts_filename, 255, "../data/%s_counts_%s.txt", type, config_name);

    char coordinates_filename[256];
    snprintf(coordinates_filename, 255, "../data/%s_coordinates_%s.txt", type,
             config_name);

    char log_filename[256];
    snprintf(log_filename, 255, "../data/%s_log_%s.txt", type, config_name);

    FILE *counts_file = fopen(counts_filename, "w");
    FILE *coordinates_file = fopen(coordinates_filename, "w");
    FILE *log_file = fopen(log_filename, "w");

    double *A_coordinates = malloc(n_realizations * sizeof(double));
    double *B_coordinates = malloc(n_realizations * sizeof(double));
    double *C_coordinates = malloc(n_realizations * sizeof(double));

    distribute_coordinates_uniformly(A_coordinates, n_realizations, lower_bound,
                                     upper_bound);
    distribute_coordinates_uniformly(B_coordinates, n_realizations, lower_bound,
                                     upper_bound);
    distribute_coordinates_uniformly(C_coordinates, n_realizations, lower_bound,
                                     upper_bound);

    double *coordinates[3] = {A_coordinates, B_coordinates, C_coordinates};

    int *A_counts = malloc(n_bins * sizeof(int));
    int *B_counts = malloc(n_bins * sizeof(int));
    int *C_counts = malloc(n_bins * sizeof(int));

    histogram(A_coordinates, A_counts, n_realizations, n_bins, lower_bound, upper_bound);
    histogram(B_coordinates, B_counts, n_realizations, n_bins, lower_bound, upper_bound);
    histogram(C_coordinates, C_counts, n_realizations, n_bins, lower_bound, upper_bound);
    int *counts[3] = {A_counts, B_counts, C_counts};

    write_int_array(counts_file, A_counts, n_bins, "");
    write_int_array(counts_file, B_counts, n_bins, "");
    write_int_array(counts_file, C_counts, n_bins, "");

    double range = upper_bound - lower_bound;
    double bin_size = range / n_bins;
    double delta_x = (upper_bound - lower_bound) / n_bins;

    // key is the indey of in the array, values are the densities on which the
    // density of the <key>-particle-sort depends) int
    // cyclic_dependencies_map[3] = {1, 2, 0};
    int symmetric_dependencies_map[3][2] = {{1, 2}, {2, 0}, {0, 1}};

    for (int i = 1; i < n_t; i++) {
        // increment time for both particla sorts in parallel
        for (int k = 0; k <= 2; k++) {
            int *dependency_ind = symmetric_dependencies_map[k];

#pragma omp parallel for
            for (int j = 0; j < n_realizations - 1; j++) {
                int thread_id = omp_get_thread_num();
                gsl_rng *local_r = thread_rngs[thread_id];

                // get the bin in which current coordinate falls
                int bin = (int)((coordinates[k][j] - lower_bound) / bin_size);
                if (bin >= n_bins)
                    bin = n_bins - 1;
                if (bin < 0)
                    bin = 0;
                // ------ this is for double diffusion -------
                // get density of the OTHER particle sort in this bin
                // double density =
                //     (double)counts[dependency_ind][bin] / (n_realizations *
                //     delta_x);

                // double coordinate =
                //     double_diffuse(coordinates[k][j], d, c, q, delta_t,
                //     density, local_r);

                //-----------this is for tripple diffusion ---------
                double density1 =
                    (double)counts[dependency_ind[0]][bin] / (n_realizations * delta_x);
                double density2 =
                    (double)counts[dependency_ind[1]][bin] / (n_realizations * delta_x);

                // accumulate the densities from neighboring bins in non-local case
                // (rs >= 1)
                for (int offset = 1; offset <= rs; offset++) {
                    if (bin >= rs) {
                        density1 += (double)counts[dependency_ind[0]][bin - offset] /
                                    (n_realizations * delta_x);
                        density2 += (double)counts[dependency_ind[1]][bin - offset] /
                                    (n_realizations * delta_x);
                    }
                    if (bin <= n_bins - rs - 1) {
                        density1 += (double)counts[dependency_ind[0]][bin + offset] /
                                    (n_realizations * delta_x);
                        density2 += (double)counts[dependency_ind[1]][bin + offset] /
                                    (n_realizations * delta_x);
                    }
                }

                double coordinate = symmetric_tripple_diffuse(
                    coordinates[k][j], d, c, q, delta_t, density1, density2, local_r);

                coordinate = reflecting_boundary(coordinate, lower_bound, upper_bound);

                coordinates[k][j] = coordinate;
            }

            histogram(coordinates[k], counts[k], n_realizations, n_bins, lower_bound,
                      upper_bound);

            if (i % frame_timestep == 0)
                write_int_array(counts_file, counts[k], n_bins, "");
        }

        // save a snapshot of the simulation state every 10000 timesteps
        if (i % 1000 == 0 || i == n_t - 1) {
            fseek(coordinates_file, 0, SEEK_SET);
            fprintf(coordinates_file, "%d ", i);
            for (int m = 0; m < n_realizations; m++) {
                fprintf(coordinates_file, "%lf ", A_coordinates[m]);
            }
            for (int m = 0; m < n_realizations; m++) {
                fprintf(coordinates_file, "%lf ", B_coordinates[m]);
            }
            for (int m = 0; m < n_realizations; m++) {
                fprintf(coordinates_file, "%lf ", C_coordinates[m]);
            }
        }

        // update the progress bar
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
    }

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
