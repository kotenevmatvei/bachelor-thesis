#include "simulation.h"
#include "io.h"

#define COMMENT_LENGTH 50


int main(void) {
    // Simulate V values
    // setup gsl

    gsl_rng_env_setup();

    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);

    // simulation paramteres
    const double D = 0.1;
    const double gamma = 1;
    const int N = 10000; // number of realizations (ensembles)
    const int N_t = 500; // number of time steps of length delta_t
    const double delta_t = 0.01; // timestep length in seconds
    const int n_hist_datapoints = 100;
    const double hist_range[2] = {-1.5, 1.5};
    const char *hist_fname = "../data/hist_data.txt";
    const int n_bins = 30;

    // simulate and write to files
    double **v_data = simulate_V_values(D, gamma, N, N_t, delta_t, r);

    write_double_matrix_to_file((const double *const *) v_data, N, N_t,
                                "../data/v_values.txt");

    // calculate histograms and write to file
    const int times[] = {30, 50, 100, 400};
    int n_times = sizeof(times) / sizeof(int);
    for (int i = 0; i < n_times; i++) {
        int time_ = times[i];
        double *v_time_snapshot = malloc(N * sizeof(double));
        for (int j = 0; j < N; j++) {
            v_time_snapshot[j] = v_data[j][time_];
        }
        double *bin_bounds = malloc((n_bins + 2) * sizeof(double));
        int *counts = histogram(v_time_snapshot, N, n_bins, bin_bounds);
        char *mode = i == 0 ? "w" : "a";
        char *comment = malloc(20 * sizeof(char));
        sprintf(comment, "# t=%.1fs\n", (double) time_ * delta_t);
        write_int_array_to_file(counts, n_bins, hist_fname, mode, comment);
        write_double_array_to_file(bin_bounds, n_bins + 1, hist_fname, "a", "");
    }

    // calculate theoretical curves and write to files
    double v_0 = 1;

    LinearAxis *V_Axis = malloc((4 + n_hist_datapoints) * sizeof(double) + sizeof(int));
    fill_linear_axis(V_Axis, hist_range[0], hist_range[1], n_hist_datapoints);
    write_double_array_to_file(V_Axis->points, n_hist_datapoints,
                               "../data/theor_curves.txt",
                               "w", "# V_axis\n");

    for (int i = 0; i < 4; i++) {
        double time_ = (double) times[i] * delta_t;
        double *P_vals = calculate_P_values(D, gamma, time_, v_0, V_Axis);

        char *comment = malloc(COMMENT_LENGTH * sizeof(char));
        sprintf(comment, "# P values for time %.1fs\n", time_);
        write_double_array_to_file(P_vals, n_hist_datapoints, "../data/theor_curves.txt",
                                   "a", comment);

        free(P_vals);
        free(comment);
    }

    free(V_Axis->points);
    free_matrix_memory(v_data, N);
    gsl_rng_free(r);
}
