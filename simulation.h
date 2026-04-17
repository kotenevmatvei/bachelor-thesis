#ifndef SIMULATION_H
#define SIMULATION_H

#include <gsl/gsl_rng.h>

double simulate_next_V(double last_v, double delta_t, double gamma, double D, gsl_rng *r);
double **simulate_V_values(double D, double gamma, int N, int N_t, double delta_t, gsl_rng *r);
double calculate_transition_probability(double V, double t, double D, double gamma, double v_0);
double *calculate_P_values(double D, double gamma, double t, double v_0, double *V_Axis);
void free_matrix_memory(double **matrix, int n_rows);
void find_min_and_max(double *array, int array_len, double *min, double *max);
void swap_double_elements(double array[], int i, int j);
void quicksort(double array[], int left, int right);
int *smart_histogram(double *array, int array_len, int n_bins, double *bin_bounds);
int *histogram(double array[], int array_len, int n_bins, double *bin_bounds);
void write_hist_and_bounds(int time, double **v_data, int N, int n_bins, char *hist_fname, char *bd_fname);

#endif // SIMULATION_H
