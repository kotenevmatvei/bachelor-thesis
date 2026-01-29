#include <assert.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "io.h"

double simulate_next_V(double last_v, double delta_t, double gamma, double D, gsl_rng *r) {
    double eta = gsl_ran_gaussian(r, 1);
    double next_v = last_v * (1 - gamma * delta_t) + sqrt(2 * D * delta_t) * eta;
    return next_v;
}

double **simulate_V_values(double D, double gamma, int N, int N_t, double delta_t, gsl_rng *r) {
    double v_current;
    double v_min;
    double v_max;

    int i, j;

    // allocate memory V values in the heap - the caller has the responsibility to free
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

double calculate_transition_probability(double V, double t, double D, double gamma, double v_0) {

	double v_mean = v_0 * pow(M_E, -gamma * t);
	double v_variance = (D/gamma) * (1 - pow(M_E, -2*gamma*t));
	double factor_1 = 1 / sqrt(2*M_PI*v_variance);
	double factor_2 = pow(M_E, -pow((V-v_mean), 2) / (2*v_variance));
	double P = factor_1 * factor_2;

	return P;
}

double *calculate_P_values(double D, double gamma, double t, double v_0, double *V_Axis) {
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
    int i;
    double max_, min_;
    max_ = min_ = array[0];
    for (i = 0; i < array_len; i++) {
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

void quicksort(double  array[], int left, int right) {
    if (left >= right)
        return;

    swap_double_elements(array, left, (left + right)/2);
    int last = left;

    for (int i = left+1; i <= right; i++) {
        if (array[i] < array[left]) {
            swap_double_elements(array, ++last, i);
        }
    }

    swap_double_elements(array, left, last);
    quicksort(array, left, last-1);
    quicksort(array, last+1, right);

}

int *histogram(double array[], int array_len, int n_bins, double *bin_bounds) {
    quicksort(array, 0, array_len-1);

    const double min = array[0];
    const double max = array[array_len-1];
    double bin_size  = (max - min) / n_bins;

    int *counts = calloc(n_bins, sizeof(double));
    int current_bin = 1;

    for (int i = 0; i < array_len; i++) {
        if (array[i] <= min + current_bin * bin_size)
            counts[current_bin-1]++;
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
    for (int i = 1; i <= n_bins+1; i++) {
        bin_bounds[i] = min + i*bin_size;
    }

    return counts;
}

void write_hist_and_bounds(int time, double **v_data, int N, int n_bins, char *hist_fname, char *bd_fname) {
    double *time_0 = calloc(N, sizeof(double));

    for (int i = 0; i < N; i++) {
        time_0[i] = v_data[i][time];
    }

    double *bin_bounds = calloc(n_bins+2, sizeof(double));

    int *counts = histogram(time_0, N, n_bins, bin_bounds);

    write_int_array_to_file(counts, n_bins, hist_fname);

    write_double_array_to_file(bin_bounds, n_bins+1, bd_fname, "bin_bounds", "w");

    free(time_0);
    free(counts);
    free(bin_bounds);

}


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
    const int N = 10000; // number of realizations (ensembles)
    const int N_t = 500; // number of time steps of length delta_t
    const double delta_t = 0.01; // timestep length in seconds

    const int n_bins = 30;

    double **v_data = simulate_V_values(D, gamma, N, N_t, delta_t, r);

    write_double_matrix_to_file(v_data, N, N_t, "../data/v_values.txt");

    write_hist_and_bounds(30, v_data, N, n_bins, "../data/counts_t30.txt", "../data/bounds_t30.txt");
    write_hist_and_bounds(50, v_data, N, n_bins, "../data/counts_t50.txt", "../data/bounds_t50.txt");
    write_hist_and_bounds(100, v_data, N, n_bins, "../data/counts_t100.txt", "../data/bounds_t100.txt");
    write_hist_and_bounds(400, v_data, N, n_bins, "../data/counts_t400.txt", "../data/bounds_t400.txt");

    double v_0 = 1;
    double times[4] = {0.3, 0.5, 1, 4};
    char *array_P_names[4] = {"P_vals03", "P_vals05", "P_vals1", "P_vals43"};
    char *array_V_names[4] = {"V_vals03", "V_vals05", "V_vals1", "V_vals43"};

    double *V_axis = malloc(100 * sizeof(double));
    for (int i = 0; i < 3; i++) {
        double *P_vals = calculate_P_values(D, gamma, times[i], v_0, V_axis);

        write_double_array_to_file(P_vals, 100, "../data/P_vals.txt", array_P_names[i], "w");
        write_double_array_to_file(V_axis, 100, "../data/P_vals.txt", array_V_names[i], "a");

        free(P_vals);
    }

    free(V_axis);
    free_matrix_memory(v_data, N);
    gsl_rng_free(r);
}
