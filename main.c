#include "simulation.h"
#include "io.h"

int main(void) {
    // Simulate V values
    // setup gsl
    const gsl_rng_type *T;
    gsl_rng *r;

    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    // simulation paramteres
    const double D = 0.1;
    const double gamma = 1;
    const int N = 10000; // number of realizations (ensembles)
    const int N_t = 500; // number of time steps of length delta_t
    const double delta_t = 0.01; // timestep length in seconds

    const int n_bins = 30;

    // simulate and write to files
    double **v_data = simulate_V_values(D, gamma, N, N_t, delta_t, r);

    write_double_matrix_to_file(v_data, N, N_t, "data/v_values.txt");

    simple_write_double_matrix_to_file(v_data, N, N_t, "data/v_values_simple.txt");

    int times_[] = {30, 50, 100, 400};
    char *hist_fname = "data/hist_data.txt";
    int n_times = sizeof(times_) / sizeof(int);
    for (int i = 0; i < n_times; i++) {
        int time_ = times_[i];
        double *v_time_snapshot = malloc(N * sizeof(double));
        for (int i = 0; i < N; i++) {
            v_time_snapshot[i] = v_data[i][time_];
        }
        double *bin_bounds = malloc((n_bins + 2) * sizeof(double));
        int *counts = smart_histogram(v_time_snapshot, N, n_bins, bin_bounds);
        char *mode = i == 0 ? "w" : "a";
        char *comment = malloc(20 * sizeof(char));
        sprintf(comment, "# time = %.2fs\n", (double)time_ / 100);
        write_simple_int_array_to_file(counts, n_bins, hist_fname, mode, comment);
        write_simple_double_array_to_file(bin_bounds, n_bins + 1, hist_fname, "a", "");
    }

    /*
    write_hist_and_bounds(30, v_data, N, n_bins, "data/counts_t30.txt",
                          "data/bounds_t30.txt");
    write_hist_and_bounds(50, v_data, N, n_bins, "data/counts_t50.txt",
                          "data/bounds_t50.txt");
    write_hist_and_bounds(100, v_data, N, n_bins, "data/counts_t100.txt",
                          "data/bounds_t100.txt");
    write_hist_and_bounds(400, v_data, N, n_bins, "data/counts_t400.txt",
                          "data/bounds_t400.txt");
    */

    // calculate theoretical curves and write to files
    double v_0 = 1;
    char *array_P_names[4] = {"P_vals30", "P_vals50", "P_vals100", "P_vals400"};

    LinearAxis *V_Axis = malloc((4 + 100) * sizeof(double) + sizeof(int));
    fill_linear_axis(V_Axis, -1.5, +1.5, 100);
    write_double_array_to_file(V_Axis->points, 100, "data/P_vals.txt",
                               "V_axis", "w");
    write_simple_double_array_to_file(V_Axis->points, 100, "data/theor_curves.txt",
                               "w", "# V_axis\n");

    for (int i = 0; i < 4; i++) {
        double time_ = (double) times_[i] / 100;
        double *P_vals = calculate_P_values(D, gamma, time_, v_0, V_Axis);

        write_double_array_to_file(P_vals, 100, "data/P_vals.txt",
                                   array_P_names[i], "a");

        char *comment = malloc(100 * sizeof(char));
        sprintf(comment, "# P values for time %.1fs\n", time_);
        write_simple_double_array_to_file(P_vals, 100, "data/theor_curves.txt",
                                   "a", comment);

        free(P_vals);
    }

    free(V_Axis->points);
    free_matrix_memory(v_data, N);
    gsl_rng_free(r);
}
