#include "io.h"
#include "simulation.h"
#include "utils.h"
#include <gsl/gsl_rng.h>

#define DELTA_T 0.01
#define START 0
#define LOWER_BOUND -1
#define UPPER_BOUND 1
#define N_T 5000
#define N_REALIZATIONS 10000
#define D 1
#define N_BINS 100
#define C 5
#define Q 2

void diffuse_and_save_histograms(double start, double lower_bound, double upper_bound,
                                 double delta_t, int n_t, int n_realizations, int n_bins,
                                 double d, double c, int q) {
    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);

    FILE *counts_file = fopen("../data/double_diffusion_counts.txt", "w");
    FILE *bin_bounds_file = fopen("../data/double_diffusion_bin_bounds.txt", "w");

    double *A_coordinates = alloc_fill_double_array(n_realizations, start);
    double *B_coordinates = alloc_fill_double_array(n_realizations, start);
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

    for (int i = 0; i < n_t; i++) {
        for (int k = 0; k <= 1; k++) {
            // increment time for both particla sorts
            for (int j = 0; j < n_realizations - 1; j++) {
                // get the bin in which current coordinate falls
                int bin = (int)((coordinates[k][j] - lower_bound) / bin_size);
                if (bin >= n_bins)
                    bin = n_bins - 1;
                if (bin < 0)
                    bin = 0;
                // get density of the OTHER particle sort in this bin
                double density = (double)counts[1 - k][bin] / (n_realizations * delta_x);

                double coordinate =
                    double_diffuse(coordinates[k][j], d, c, q, delta_t, density, r);

                coordinate = reflecting_boundary(coordinate, lower_bound, upper_bound);

                coordinates[k][j] = coordinate;
            }

            histogram(coordinates[k], counts[k], n_realizations, n_bins, lower_bound,
                      upper_bound);
            write_int_array(counts_file, counts[k], n_bins, "");
        }
    }

    fclose(counts_file);
    fclose(bin_bounds_file);
    free(A_coordinates);
    free(B_coordinates);
    free(A_counts);
    free(B_counts);
}

int main() {
    diffuse_and_save_histograms(START, LOWER_BOUND, UPPER_BOUND, DELTA_T, N_T,
                                N_REALIZATIONS, N_BINS, D, C, Q);
    return 0;
}
