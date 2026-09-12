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
    double p_0;
    double alpha;
    int n_t;
    int n_realizations;
    int n_bins;
    int q;
    int rs;
    int cnts_timestep;
    int crds_snapshot;
    char type[64];
    char run[64];
    char dependency[64];
    char boundary[64];
    char init_density[32];
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

double ref_boundary(double coordinate, double lower_bound, double upper_bound);

double per_boundary(double coordinate, double lower_bound, double upper_bound);

double sticky_top_refl_bottom_boundary(double coordinate, double lower_bound,
                                       double upper_bound);

double double_pow_diffuse(double coordinate, double D, double c, int q, double delta_t,
                            double density, gsl_rng *r);

void histogram(const double *array, int *cnts, const int array_len, const int n_bins,
               const double lower_bound, const double upper_bound);

void distribute_crds_uniformly(double *array, int array_len, double lower_bound,
                                      double upper_bound);

void distribute_crds_in_one_third(double *array, int array_len, double lower_bound,
                                         double upper_bound, int n_third);

double sym_tripple_pow_diffuse(double coordinate, double D, double c, int q,
                                       double delta_t, double density1, double density2,
                                       gsl_rng *r);

double double_log_diffuse(double coordinate, double D, double p_0, double alpha,
                               double delta_t, double density, gsl_rng *r);

double sym_tripple_log_diffuse(const double coordinate, const double D,
                                          double p_0, double alpha, const double delta_t,
                                          const double density1, const double density2,
                                          gsl_rng *r);

int load_checkpoint(char *filename, double *A_crds, double *B_crds,
                    double *C_crds, int n_realizations, int *i);

int check_for_existing_checkpoint(char *crds_filename);

#endif // SIMULATION_H
