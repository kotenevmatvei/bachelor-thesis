#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>
#include <stdlib.h>

#include "simulation.h"

double simulate_next_V(double last_v, double delta_t, double gamma, double D,
                       gsl_rng *r) {
    double eta = gsl_ran_gaussian(r, 1);
    double next_v = last_v * (1 - gamma * delta_t) + sqrt(2 * D * delta_t) * eta;
    return next_v;
}

double **simulate_V_values(double D, double gamma, int N, int N_t, double delta_t,
                           gsl_rng *r) {
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
        double v_current = 1;
        v_data[i][0] = v_current;

        double v_min = v_max = v_current;

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

double calculate_transition_probability(double V, double t, double D, double gamma,
                                        double v_0) {
    double v_mean = v_0 * exp(-gamma * t);
    double v_variance = (D / gamma) * (1 - exp(-2 * gamma * t));
    double factor_1 = 1 / sqrt(2 * M_PI * v_variance);
    double factor_2 = exp(-pow((V - v_mean), 2) / (2 * v_variance));
    double P = factor_1 * factor_2;

    return P;
}

void fill_linear_axis(LinearAxis *axis, double start, double end, int n_points) {
    axis->start = start;
    axis->end = end;
    axis->n_points = n_points;
    int n_steps = n_points - 1;
    axis->step = (end - start) / n_steps;

    axis->points = malloc(n_points * sizeof(double));
    for (int i = 0; i <= n_points; i++) {
        axis->points[i] = start + i * axis->step;
    }
}

double *calculate_P_values(double D, double gamma, double t, double v_0,
                           LinearAxis *V_Axis) {

    double *P_vals = malloc(V_Axis->n_points * sizeof(double));

    for (int i = 0; i <= V_Axis->n_points; i++) {
        P_vals[i] = calculate_transition_probability(V_Axis->points[i], t, D, gamma, v_0);
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
    double min_ = array[0];
    double max_ = min_ = array[0];
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

int *histogram_flexible_bounds(double *array, double *bin_bounds, int array_len,
                               int n_bins) {
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

int *histogram_fixed_bins_write_bin_bounds(double *array, double *bin_bounds,
                                           int array_len, int n_bins, double lower_bound,
                                           double upper_bound) {
    double range = upper_bound - lower_bound;
    double bin_size = range / n_bins;
    int *counts = calloc(n_bins, sizeof(int));
    for (int i = 0; i < array_len; i++) {
        int bin = (int)((array[i] - lower_bound) / bin_size);
        if (bin >= n_bins)
            bin = n_bins - 1;
        if (bin < 0)
            bin = 0;
        counts[bin]++;
    }
    for (int i = 0; i < n_bins + 1; i++) {
        bin_bounds[i] = lower_bound + i * bin_size;
    }
    return counts;
}

void histogram(const double *array, int *counts, const int array_len, const int n_bins,
               const double lower_bound, const double upper_bound) {
    // make sure the counts are zero
    for (int i = 0; i < n_bins; i++)
        counts[i] = 0;
    double range = upper_bound - lower_bound;
    double bin_size = range / n_bins;
    for (int i = 0; i < array_len; i++) {
        int bin = (int)((array[i] - lower_bound) / bin_size);
        if (bin >= n_bins)
            bin = n_bins - 1;
        if (bin < 0)
            bin = 0;
        counts[bin]++;
    }
}

double simple_diffuse(double last_coordinate, double D, double delta_t, gsl_rng *r) {
    double eta = gsl_ran_gaussian(r, 1);
    double next_coordinate = last_coordinate + sqrt(2 * D * delta_t) * eta;
    return next_coordinate;
}

double reflecting_boundary(double coordinate, double lower_bound, double upper_bound) {
    if (coordinate > upper_bound)
        return 2 * upper_bound - coordinate;

    if (coordinate < lower_bound)
        return 2 * lower_bound - coordinate;
    return coordinate;
}

double periodic_boundary(double coordinate, double lower_bound, double upper_bound) {
    if (coordinate > upper_bound)
        return lower_bound + coordinate - upper_bound;

    if (coordinate < lower_bound)
        return upper_bound - coordinate + lower_bound;

    return coordinate;
}

double sticky_top_refl_bottom_boundary(double coordinate, double lower_bound,
                                       double upper_bound) {
    if (coordinate >= upper_bound)
        return upper_bound;

    if (coordinate < lower_bound)
        return 2 * lower_bound - coordinate;

    return coordinate;
}

double double_diffuse(double coordinate, double D, double c, int q, double delta_t,
                      double density, gsl_rng *r) {

    double eta = gsl_ran_gaussian(r, 1);
    double new_coordinate =
        coordinate + sqrt((2 * D * (1 + c * pow(density, q)) * delta_t)) * eta;
    return new_coordinate;
}

double symmetric_tripple_diffuse(double coordinate, double D, double c, int q,
                                 double delta_t, double density1, double density2,
                                 gsl_rng *r) {

    const double eta = gsl_ran_gaussian(r, 1);
    const double competitor_density = density1 + density2;

    const double D_eff = D * (1.0 + c * pow(competitor_density, (double)q));

    double new_coordinate = coordinate + sqrt(2.0 * D_eff * delta_t) * eta;

    return new_coordinate;
}

void distribute_coordinates_uniformly(double *array, int array_len, double lower_bound,
                                      double upper_bound) {
    double range = upper_bound - lower_bound;
    double step = range / array_len;
    for (int i = 0; i < array_len; i++)
        array[i] = lower_bound + i * step;
}

int load_checkpoint(char *filename, double *A_coordinates, double *B_coordinates,
                    int n_realizations, int *i) {

    FILE *file = fopen(filename, "r");
    if (!file)
        return 0;

    if (fscanf(file, "%d ", i) != 1)
        printf("Could not read i...\n");

    for (int j = 0; j < n_realizations; j++) {
        if (fscanf(file, "%lf", &A_coordinates[j]) != 1) {
            printf("Error reading A_coordinate %d", j);
            return 0;
        }
    }
    for (int j = 0; j < n_realizations; j++) {
        if (fscanf(file, "%lf", &B_coordinates[j]) != 1) {
            printf("Error reading B_coordinate %d", j);
            return 0;
        }
    }
    return 1;
}
