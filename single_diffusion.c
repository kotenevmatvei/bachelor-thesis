#include "io.h"
#include "simulation.h"
#include "utils.h"

// simulate 1 particle and record trajectory

#define DELTA_T 0.01
#define START 0
#define LOWER_BOUND -1
#define UPPER_BOUND 1
#define N_T 200
#define N_REALIZATIONS 10000
#define D 0.1
#define N_BINS 100

void diffuse_save_histograms(double start, double lower_bound, double upper_bound,
                             double delta_t, int n_t, int n_realizations, int n_bins,
                             double d) {

    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);

    double *coordinates = alloc_fill_double_array(start, n_realizations);
    for (int i = 0; i < n_t; i++) {
        for (int j = 0; j < n_realizations; j++) {
            double coordinate = simple_diffuse(coordinates[j], d, delta_t, r);
            coordinates[j] = reflective_boundary(coordinate, lower_bound, upper_bound);
        }
        double *bin_bounds = malloc((n_bins + 1) * sizeof(double));
        int *counts = histogram_fixed_bins(coordinates, bin_bounds, n_realizations,
                                           n_bins, lower_bound, upper_bound);
        write_int_array_to_file(counts, n_bins, "../data/diffusion_hist_reflective.txt", "a",
                                "");
        write_double_array_to_file(bin_bounds, n_bins + 1,
                                   "../data/diffusion_hist_reflective.txt", "a", "");
        free(bin_bounds);
        free(counts);
    }
    free(coordinates);
}

void diffuse_save_trajectories(double start, double lower_bound, double upper_bound,
                               double delta_t, int n_t, int n_realizations, double d) {

    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);

    for (int j = 0; j < n_realizations; j++) {

        double coordinate = start;
        double *trajectory = malloc(n_t * sizeof(double));

        for (int j = 0; j < n_t; j++) {
            coordinate = simple_diffuse(coordinate, d, delta_t, r);
            coordinate = reflective_boundary(coordinate, lower_bound, upper_bound);
            trajectory[j] = coordinate;
        }

        write_double_array_to_file(trajectory, n_t, "../data/diffusion_trajectories_reflective.txt",
                                   "a", "");
        free(trajectory);
    }
}

int main() {
    diffuse_save_trajectories(START, LOWER_BOUND, UPPER_BOUND, DELTA_T, 1000, 10, D);
    diffuse_save_histograms(START, LOWER_BOUND, UPPER_BOUND, DELTA_T, N_T, N_REALIZATIONS,
                            N_BINS, D);
}
