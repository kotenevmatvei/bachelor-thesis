#include "io.h"
#include "simulation.h"
#include <omp.h>
#include <gsl/gsl_rng.h>
#include <time.h>

#define DELTA_T 0.001
#define START 0
#define LOWER_BOUND -1
#define UPPER_BOUND 1
#define N_T 1000000
#define N_REALIZATIONS 10000
#define D 1
#define N_BINS 100
#define C 7
#define Q 4

void diffuse_and_save_histograms(double lower_bound, double upper_bound,
                                 double delta_t, int n_t, int n_realizations, int n_bins,
                                 double d, double c, int q) {
    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;

    int max_threads = omp_get_max_threads();
    printf("\nmax_threads = %d\n", max_threads);

    gsl_rng **thread_rngs = malloc(max_threads * sizeof(gsl_rng *));

    for (int i = 0; i < max_threads; i++) {
        thread_rngs[i] = gsl_rng_alloc(T);
        gsl_rng_set(thread_rngs[i], time(NULL) + i);
    }

    FILE *counts_file = fopen("../data/double_diffusion_counts.txt", "w");
    FILE *bin_bounds_file = fopen("../data/double_diffusion_bin_bounds.txt", "w");

    double *A_coordinates = malloc(n_realizations * sizeof(double));
    double *B_coordinates = malloc(n_realizations * sizeof(double));

    distribute_coordinates_uniformly(A_coordinates, n_realizations, lower_bound, upper_bound);
    distribute_coordinates_uniformly(B_coordinates, n_realizations, lower_bound, upper_bound);

    // print_double_array(A_coordinates, n_realizations, "A_coordinates");
    // print_double_array(B_coordinates, n_realizations, "B_coordinates");

    double *coordinates[2] = {A_coordinates, B_coordinates};

    int *A_counts = malloc(n_bins * sizeof(int));
    int *B_counts = malloc(n_bins * sizeof(int));
    histogram(A_coordinates, A_counts, n_realizations, n_bins, lower_bound, upper_bound);
    histogram(B_coordinates, B_counts, n_realizations, n_bins, lower_bound, upper_bound);
    int *counts[2] = {A_counts, B_counts};

    // print_int_array(A_counts, n_bins, "A_counts");
    // print_int_array(B_counts, n_bins, "B_counts");

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
                if (p < pos) printf("=");
                else if (p == pos) printf(">");
                else printf(" ");
            }
            printf("] %3d%%", (int)(progress * 100.0));
            
            fflush(stdout);
        }
    }
    printf("\n");

    fclose(counts_file);
    fclose(bin_bounds_file);
    free(A_coordinates);
    free(B_coordinates);
    free(A_counts);
    free(B_counts);
    for (int i = 0; i < max_threads; i++) {
        gsl_rng_free(thread_rngs[i]);
    }
    free(thread_rngs);
}

int main() {
    diffuse_and_save_histograms(LOWER_BOUND, UPPER_BOUND, DELTA_T, N_T,
                                N_REALIZATIONS, N_BINS, D, C, Q);
    return 0;
}
