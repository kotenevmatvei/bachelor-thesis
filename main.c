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

    write_hist_and_bounds(30, v_data, N, n_bins, "data/counts_t30.txt",
                          "data/bounds_t30.txt");
    write_hist_and_bounds(50, v_data, N, n_bins, "data/counts_t50.txt",
                          "data/bounds_t50.txt");
    write_hist_and_bounds(100, v_data, N, n_bins, "data/counts_t100.txt",
                          "data/bounds_t100.txt");
    write_hist_and_bounds(400, v_data, N, n_bins, "data/counts_t400.txt",
                          "data/bounds_t400.txt");

    // calculate theoretical curves and write to files
    double v_0 = 1;
    double times[4] = {0.3, 0.5, 1, 4};
    char *array_P_names[4] = {"P_vals30", "P_vals50", "P_vals100", "P_vals400"};

    LinearAxis *V_Axis = malloc((4 + 100) * sizeof(double) + sizeof(int));
    fill_linear_axis(V_Axis, -1.5, +1.5, 100);
    write_double_array_to_file(V_Axis->points, 100, "data/P_vals.txt",
                               "V_axis", "w");

    for (int i = 0; i < 4; i++) {
        double *P_vals = calculate_P_values(D, gamma, times[i], v_0, V_Axis);

        write_double_array_to_file(P_vals, 100, "data/P_vals.txt",
                                   array_P_names[i], "a");

        free(P_vals);
    }

    free(V_Axis->points);
    free_matrix_memory(v_data, N);
    gsl_rng_free(r);
}
