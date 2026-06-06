#include "io.h"
#include "simulation.h"
#include "utils.h"
#include <gsl/gsl_rng.h>
#include <omp.h>
#include <stdio.h>
#include <time.h>

void diffuse_and_save_histograms(DiffusionConfig config) {

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
    char counts_filename[64];
    snprintf(counts_filename, 63, "../data/dd_counts_dt%g_nt%d_nr%d_c%d_q%d.txt",
             delta_t, n_t, n_realizations, c, q);

    FILE *counts_file = fopen(counts_filename, "w");

    double *A_coordinates = malloc(n_realizations * sizeof(double));
    double *B_coordinates = malloc(n_realizations * sizeof(double));

    distribute_coordinates_uniformly(A_coordinates, n_realizations, lower_bound,
                                     upper_bound);
    distribute_coordinates_uniformly(B_coordinates, n_realizations, lower_bound,
                                     upper_bound);

    double *coordinates[2] = {A_coordinates, B_coordinates};

    int *A_counts = malloc(n_bins * sizeof(int));
    int *B_counts = malloc(n_bins * sizeof(int));
    histogram(A_coordinates, A_counts, n_realizations, n_bins, lower_bound, upper_bound);
    histogram(B_coordinates, B_counts, n_realizations, n_bins, lower_bound, upper_bound);
    int *counts[2] = {A_counts, B_counts};

    write_int_array(counts_file, A_counts, n_bins, "");
    write_int_array(counts_file, B_counts, n_bins, "");

    double range = upper_bound - lower_bound;
    double bin_size = range / n_bins;
    double delta_x = (upper_bound - lower_bound) / n_bins;

    for (int i = 1; i < n_t; i++) {
        // increment time for both particla sorts in parallel
        for (int k = 0; k <= 1; k++) {

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
                // get density of the OTHER particle sort in this bin
                double density = (double)counts[1 - k][bin] / (n_realizations * delta_x);

                double coordinate =
                    double_diffuse(coordinates[k][j], d, c, q, delta_t, density, local_r);

                coordinate = reflecting_boundary(coordinate, lower_bound, upper_bound);

                coordinates[k][j] = coordinate;
            }

            histogram(coordinates[k], counts[k], n_realizations, n_bins, lower_bound,
                      upper_bound);
            write_int_array(counts_file, counts[k], n_bins, "");
        }

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
            }
            printf("] %3d%%", (int)(progress * 100.0));

            fflush(stdout);
        }
    }
    printf("\n");

    fclose(counts_file);
    free(A_coordinates);
    free(B_coordinates);
    free(A_counts);
    free(B_counts);
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
        char config_path[64];
        snprintf(config_path, 63, "../configs/%s.txt", argv[1]);
    }
    DiffusionConfig config = read_config(config_path);
    diffuse_and_save_histograms(config);
    return 0;
}
