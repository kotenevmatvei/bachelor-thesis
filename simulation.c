#include <assert.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "simulation.h"

double simulate_next_V(double last_v, double delta_t, double gamma, double D,
                       gsl_rng *r) {
  double eta = gsl_ran_gaussian(r, 1);
  double next_v = last_v * (1 - gamma * delta_t) + sqrt(2 * D * delta_t) * eta;
  return next_v;
}

double **simulate_V_values(double D, double gamma, int N, int N_t,
                           double delta_t, gsl_rng *r) {
  double v_current;
  double v_min;
  double v_max;

  int i, j;

  // allocate memory V values in the heap - the caller has the responsibility to
  // free
  double **v_data = malloc(N * sizeof(double));
  for (i = 0; i < N; i++) {
    v_data[i] = malloc(N_t * sizeof(double));
  }

  // simulate and save V values
  for (i = 0; i < N; ++i) {
    v_current = 1;
    v_data[i][0] = v_current;

    v_min = v_max = v_current;

    for (j = 1; j < N_t; ++j) {
      v_current = simulate_next_V(v_current, delta_t, gamma, D, r);
      v_data[i][j] = v_current;

      if (v_current < v_min) {
        v_min = v_current;
      } else if (v_current > v_max) {
        v_max = v_current;
      }
    }
  }

  return v_data;
}

double calculate_transition_probability(double V, double t, double D,
                                        double gamma, double v_0) {

  double v_mean = v_0 * exp(-gamma * t);
  double v_variance = (D / gamma) * (1 - exp(-2 * gamma * t));
  double factor_1 = 1 / sqrt(2 * M_PI * v_variance);
  double factor_2 = exp(-pow((V - v_mean), 2) / (2 * v_variance));
  double P = factor_1 * factor_2;

  return P;
}

double *calculate_P_values(double D, double gamma, double t, double v_0,
                           double *V_Axis) {
  double V_step = 3. / 100.;
  printf("V_step = %f\n", V_step);

  for (int i = 0; i < 100; i++) {
    V_Axis[i] = -1.5 + i * V_step;
  }

  double *P_vals = malloc(100 * sizeof(double));

  for (int i = 0; i < 100; i++) {
    P_vals[i] = calculate_transition_probability(V_Axis[i], t, D, gamma, v_0);
  }

  return P_vals;
}

void free_matrix_memory(double **matrix, int n_rows) {
  int i;
  for (i = 0; i < n_rows; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

void find_min_and_max(double *array, int array_len, double *min, double *max) {
  double max_, min_;
  min_ = array[0];
  max_ = min_ = array[0];
  for (int i = 0; i < array_len; i++) {
    if (array[i] < min_)
      min_ = array[i];
    else if (array[i] > max_)
      max_ = array[i];
  }
  *min = min_;
  *max = max_;
}

void swap_double_elements(double array[], int i, int j) {
  double tmp = array[i];
  array[i] = array[j];
  array[j] = tmp;
}

void quicksort(double array[], int left, int right) {
  if (left >= right)
    return;

  swap_double_elements(array, left, (left + right) / 2);
  int last = left;

  for (int i = left + 1; i <= right; i++) {
    if (array[i] < array[left]) {
      swap_double_elements(array, ++last, i);
    }
  }

  swap_double_elements(array, left, last);
  quicksort(array, left, last - 1);
  quicksort(array, last + 1, right);
}

int *smart_histogram(double *array, int array_len, int n_bins,
                     double *bin_bounds) {
  double min, max;
  find_min_and_max(array, array_len, &min, &max);
  double range = max - min;
  double bin_size = range / n_bins;
  int *counts = calloc(n_bins, sizeof(int));
  for (int i = 0; i < array_len; i++) {
    int bin = (int)((array[i] - min) / bin_size);
    if (bin >= n_bins)
      bin = n_bins - 1;
    if (bin < 0)
      bin = 0;
    counts[bin]++;
  }
  for (int i = 0; i < n_bins + 1; i++) {
    bin_bounds[i] = min + i * bin_size;
  }
  return counts;
}

int *histogram(double array[], int array_len, int n_bins, double *bin_bounds) {
  quicksort(array, 0, array_len - 1);

  const double min = array[0];
  const double max = array[array_len - 1];
  double bin_size = (max - min) / n_bins;

  int *counts = calloc(n_bins, sizeof(int));
  int current_bin = 1;

  for (int i = 0; i < array_len; i++) {
    if (array[i] <= min + current_bin * bin_size)
      counts[current_bin - 1]++;
    else {
      current_bin++;
      if (current_bin > n_bins) {
        // if we ran out of bins, put the remaining max values in the last one
        counts[n_bins - 1]++;
      } else {
        i--;
      }
    }
  }

  // calculate bin bounds
  bin_bounds[0] = min;
  for (int i = 1; i <= n_bins + 1; i++) {
    bin_bounds[i] = min + i * bin_size;
  }

  return counts;
}

void write_hist_and_bounds(int time, double **v_data, int N, int n_bins,
                           char *hist_fname, char *bd_fname) {
  double *time_0 = calloc(N, sizeof(double));

  for (int i = 0; i < N; i++) {
    time_0[i] = v_data[i][time];
  }

  double *bin_bounds = calloc(n_bins + 2, sizeof(double));

  int *counts = histogram(time_0, N, n_bins, bin_bounds);

  write_int_array_to_file(counts, n_bins, hist_fname);

  write_double_array_to_file(bin_bounds, n_bins + 1, bd_fname, "bin_bounds",
                             "w");

  free(time_0);
  free(counts);
  free(bin_bounds);
}
