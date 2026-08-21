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

typedef struct {
    double delta_t;
    double start;
    double lower_bound;
    double upper_bound;
    double d;
    double c;
    int n_t;
    int n_realizations;
    int n_bins;
    int q;
    int rs;
    int frame_timestep;
    char type[64];
} DiffusionConfig;

void fill_linear_axis(LinearAxis *axis, double start, double end, int n_points);

double simulate_next_V(double last_v, double delta_t, double gamma, double D, gsl_rng *r);

double **simulate_V_values(double D, double gamma, int N, int N_t, double delta_t,
                           gsl_rng *r);

double calculate_transition_probability(double V, double t, double D, double gamma,
                                        double v_0);

double *calculate_P_values(double D, double gamma, double t, double v_0,
                           LinearAxis *V_Axis);

void free_matrix_memory(double **matrix, int n_rows);

void find_min_and_max(double *array, int array_len, double *min, double *max);

void swap_double_elements(double array[], int i, int j);

void quicksort(double array[], int left, int right);

int *histogram_flexible_bounds(double *array, double *bin_bounds, int array_len,
                               int n_bins);

int *histogram_fixed_bins_write_bin_bounds(double *array, double *bin_bounds,
                                           int array_len, int n_bins, double lower_bound,
                                           double upper_bound);

double simple_diffuse(double last_coordinate, double D, double delta_t, gsl_rng *r);

double reflecting_boundary(double coordinate, double lower_bound, double upper_bound);

double periodic_boundary(double coordinate, double lower_bound, double upper_bound);

double sticky_top_refl_bottom_boundary(double coordinate, double lower_bound,
                                       double upper_bound);

double double_diffuse(double coordinate, double D, double c, int q, double delta_t,
                      double density, gsl_rng *r);

void histogram(const double *array, int *counts, const int array_len, const int n_bins,
               const double lower_bound, const double upper_bound);

void distribute_coordinates_uniformly(double *array, int array_len, double lower_bound,
                                      double upper_bound);

double symmetric_tripple_diffuse(double coordinate, double D, double c, int q,
                                 double delta_t, double density1, double density2,
                                 gsl_rng *r);

int load_checkpoint(char *filename, double *A_coordinates, double *B_coordinates,
                    double *C_coordinates, int n_realizations, int *i);

int check_for_existing_checkpoint(char *coordinates_filename);

#endif // SIMULATION_H
