#include "simulation.h"
#include "io.h"

int main(void) {

  // setup gsl
  const gsl_rng_type *T;
  gsl_rng *r;

  gsl_rng_env_setup();

  T = gsl_rng_default;
  r = gsl_rng_alloc(T);

  // simulation paramteres
  const double D = 0.1;
  const double gamma = 1;
  const int N = 10000;         // number of realizations (ensembles)
  const int N_t = 500;         // number of time steps of length delta_t
  const double delta_t = 0.01; // timestep length in seconds

  const int n_bins = 30;

  double **v_data = simulate_V_values(D, gamma, N, N_t, delta_t, r);

  write_double_matrix_to_file(v_data, N, N_t, "../data/v_values.txt");

  write_hist_and_bounds(30, v_data, N, n_bins, "../data/counts_t30.txt",
                        "../data/bounds_t30.txt");
  write_hist_and_bounds(50, v_data, N, n_bins, "../data/counts_t50.txt",
                        "../data/bounds_t50.txt");
  write_hist_and_bounds(100, v_data, N, n_bins, "../data/counts_t100.txt",
                        "../data/bounds_t100.txt");
  write_hist_and_bounds(400, v_data, N, n_bins, "../data/counts_t400.txt",
                        "../data/bounds_t400.txt");

  double v_0 = 1;
  double times[4] = {0.3, 0.5, 1, 4};
  char *array_P_names[4] = {"P_vals03", "P_vals05", "P_vals1", "P_vals43"};
  char *array_V_names[4] = {"V_vals03", "V_vals05", "V_vals1", "V_vals43"};

  double *V_axis = malloc(100 * sizeof(double));
  for (int i = 0; i < 3; i++) {
    double *P_vals = calculate_P_values(D, gamma, times[i], v_0, V_axis);

    write_double_array_to_file(P_vals, 100, "../data/P_vals.txt",
                               array_P_names[i], "w");
    write_double_array_to_file(V_axis, 100, "../data/P_vals.txt",
                               array_V_names[i], "a");

    free(P_vals);
  }

  free(V_axis);
  free_matrix_memory(v_data, N);
  gsl_rng_free(r);
}
