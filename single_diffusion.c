#include "io.h"
#include "simulation.h"

// simulate 1 particle and record trajectory

#define DELTA_T 0.01
#define START 0
#define N_T 1000
#define D 0.01

void diffuse_one_particle(double start, double delta_t, int n_t) {

    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);

    for (int j = 0; j < 20; j++) {

        double coordinate = start;
        double *trajectory = malloc(n_t * sizeof(double));

        for (int j = 0; j < n_t; j++) {
            trajectory[j] = coordinate = simple_diffuse(coordinate, D, delta_t, r);
        }

        write_double_array_to_file(trajectory, n_t, "../data/diffusion_trajectories.txt",
                                   "a", "");
        free(trajectory);
    }
}

int main() { diffuse_one_particle(START, DELTA_T, N_T); }
