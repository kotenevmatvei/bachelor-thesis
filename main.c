#include <assert.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double generate_next_V(double last_v, double delta_t, double gamma, double D, gsl_rng *r) {
    double eta = gsl_ran_gaussian(r, 1);
    double next_v = last_v * (1 - gamma * delta_t) + sqrt(2 * D * delta_t) * eta;
    return next_v;
}

double **calculate_V_values(double D, double gamma, int N, int N_t, double delta_t, gsl_rng *r) {
    double v_current;
    double v_min;
    double v_max;

    int i, j;

    // allocate memory V values in the heap - the caller has the responsibility to free
    double **v_data = malloc(N * sizeof(double));
    for (i = 0; i < N; i++) {
        v_data[i] = malloc(N_t * sizeof(double));
    }

    // calculate and save V values
    for (i = 0; i < N; ++i) {
        v_current = 1;
        v_data[i][0] = v_current;

        v_min = v_max = v_current;

        for (j = 1; j < N_t; ++j) {
            v_current = generate_next_V(v_current, delta_t, gamma, D, r);
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

void write_double_matrix_to_file(double **matrix, int n_rows, int n_cols, char fname[]) {
    int i, j;

    FILE *fptr = fopen(fname, "w");

    for (i = 0; i < n_rows; ++i) {
        fprintf(fptr, "row %d\n", i);
        for (j = 0; j < n_cols; ++j) {
            fprintf(fptr, "%lf\n", matrix[i][j]);
        }
    }
}

void write_int_array_to_file(const int *array, const int array_len, const char fname[]) {
    FILE *fptr = fopen(fname, "w");

    for (int i = 0; i < array_len; ++i) {
        fprintf(fptr, "%d\n", array[i]);
    }
}

void write_double_array_to_file(const double *array, const int array_len, const char fname[]) {
    FILE *fptr = fopen(fname, "w");

    for (int i = 0; i < array_len; ++i) {
        fprintf(fptr, "%f\n", array[i]);
    }
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
        if (array[i] <= current_bin * bin_size)
            counts[current_bin-1]++;
        else {
            current_bin++;
            i--;
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

    write_double_array_to_file(bin_bounds, n_bins+1, bd_fname);

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

    double **v_data = calculate_V_values(D, gamma, N, N_t, delta_t, r);

    write_double_matrix_to_file(v_data, N, N_t, "data/v_values.txt");

    write_hist_and_bounds(30, v_data, N, n_bins, "data/counts_t30.txt", "data/bounds_t30.txt");
    write_hist_and_bounds(50, v_data, N, n_bins, "data/counts_t50.txt", "data/bounds_t50.txt");
    write_hist_and_bounds(100, v_data, N, n_bins, "data/counts_t100.txt", "data/bounds_t100.txt");
    write_hist_and_bounds(400, v_data, N, n_bins, "data/counts_t400.txt", "data/bounds_t400.txt");

    free_matrix_memory(v_data, N);
    gsl_rng_free(r);
}
