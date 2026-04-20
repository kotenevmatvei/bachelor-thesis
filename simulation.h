#ifndef SIMULATION_H
#define SIMULATION_H

#include <gsl/gsl_rng.h>

typedef struct {
    double start;
    double end;
    double step;
    int n_points;
    double *points;
} LinearAxis;

void fill_linear_axis(LinearAxis *axis, double start, double end, int n_points);

double simulate_next_V(double last_v, double delta_t, double gamma, double D, gsl_rng *r);

double **simulate_V_values(double D, double gamma, long N, int N_t, double delta_t, gsl_rng *r);

double calculate_transition_probability(double V, double t, double D, double gamma, double v_0);

double *calculate_P_values(double D, double gamma, double t, double v_0, LinearAxis *V_Axis);

void free_matrix_memory(double **matrix, int n_rows);

void find_min_and_max(double *array, int array_len, double *min, double *max);

void swap_double_elements(double array[], int i, int j);

void quicksort(double array[], int left, int right);

int *histogram(double *array, int array_len, int n_bins, double *bin_bounds);

#endif // SIMULATION_H
