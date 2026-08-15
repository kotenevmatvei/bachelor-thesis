#include "io.h"
#include "simulation.h"
#include "utils.h"

#include <gsl/gsl_rng.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int position_to_bin(double x, double lower, double dx, int n_bins)
{
    int bin = (int)((x - lower) / dx);
    if (bin < 0) bin = 0;
    if (bin >= n_bins) bin = n_bins - 1;
    return bin;
}

static double reflect_position(double x, double lower, double upper)
{
    const double range = upper - lower;
    const double period = 2.0 * range;
    double y = fmod(x - lower, period);
    if (y < 0.0) y += period;
    return lower + ((y <= range) ? y : period - y);
}

/* Correct baseline implementation. Atomic updates can later be replaced by
   thread-local histograms if profiling identifies this as the bottleneck. */
static void histogram_omp(const double *x, int *counts, int N, int M,
                          double lower, double dx)
{
    memset(counts, 0, (size_t)M * sizeof *counts);
#pragma omp parallel for schedule(static)
    for (int p = 0; p < N; ++p) {
        const int bin = position_to_bin(x[p], lower, dx, M);
#pragma omp atomic update
        counts[bin] += 1;
    }
}

/* Discrete normalized top-hat average. At endpoints the available part of the
   sensing window is used and the normalization is adjusted accordingly. */
static void sensed_density_omp(const int *counts, double *rho, int N, int M,
                               int rs, double dx)
{
#pragma omp parallel for schedule(static)
    for (int bin = 0; bin < M; ++bin) {
        int lo = bin - rs;
        int hi = bin + rs;
        if (lo < 0) lo = 0;
        if (hi >= M) hi = M - 1;
        long long sum = 0;
        for (int j = lo; j <= hi; ++j) sum += counts[j];
        rho[bin] = (double)sum / ((double)N * dx * (double)(hi - lo + 1));
    }
}

/* Updates one species from old-time density fields into a distinct next array. */
static void update_species_omp(const double *x, double *x_next,
                               const double *rho_other_1,
                               const double *rho_other_2,
                               int N, int M, double lower, double upper,
                               double dx, double D0, double c, int q,
                               double dt, gsl_rng **thread_rngs)
{
#pragma omp parallel for schedule(static)
    for (int p = 0; p < N; ++p) {
        const int tid = omp_get_thread_num();
        const int bin = position_to_bin(x[p], lower, dx, M);
        const double rho_competitors = rho_other_1[bin] + rho_other_2[bin];
        const double D_eff = D0 * (1.0 + c * pow(rho_competitors, (double)q));
        const double eta = gsl_ran_gaussian(thread_rngs[tid], 1.0);
        const double trial = x[p] + sqrt(2.0 * D_eff * dt) * eta;
        x_next[p] = reflect_position(trial, lower, upper);
    }
}

static void write_counts_frame(FILE *file, const int *Ac, const int *Bc,
                               const int *Cc, int M)
{
    write_int_array(file, Ac, M, "");
    write_int_array(file, Bc, M, "");
    write_int_array(file, Cc, M, "");
}

void diffuse_and_save_histograms(DiffusionConfig config)
{
    const time_t start = time(NULL);
    const double dt = config.delta_t;
    const double lower = config.lower_bound;
    const double upper = config.upper_bound;
    const double D0 = config.d;
    const int n_t = config.n_t;
    const int N = config.n_realizations;
    const int M = config.n_bins;
    const double c = (double)config.c;
    const int q = config.q;
    const int rs = config.rs;
    const double dx = (upper - lower) / (double)M;
    const int frame_step = (n_t >= 1000) ? n_t / 1000 : 1;
    const int progress_step = (n_t >= 100) ? n_t / 100 : 1;
    const unsigned long base_seed = 20260815UL;

    if (N <= 0 || M <= 0 || n_t <= 0 || rs < 0 || D0 < 0.0) {
        fprintf(stderr, "Invalid simulation configuration.\n");
        return;
    }

    const int max_threads = omp_get_max_threads();
    printf("OpenMP threads available: %d\n", max_threads);
    gsl_rng_env_setup();
    const gsl_rng_type *rng_type = gsl_rng_default;
    gsl_rng **thread_rngs = malloc((size_t)max_threads * sizeof *thread_rngs);
    if (thread_rngs == NULL) {
        fprintf(stderr, "RNG allocation failure.\n");
        return;
    }
    for (int t = 0; t < max_threads; ++t) {
        thread_rngs[t] = gsl_rng_alloc(rng_type);
        if (thread_rngs[t] == NULL) {
            for (int j = 0; j < t; ++j) gsl_rng_free(thread_rngs[j]);
            free(thread_rngs);
            fprintf(stderr, "GSL RNG allocation failure.\n");
            return;
        }
        gsl_rng_set(thread_rngs[t], base_seed + 104729UL * (unsigned long)t);
    }

    char config_name[128], counts_name[256], log_name[256];
    snprintf(config_name, sizeof config_name, "dt%g_nt%d_nr%d_c%g_q%d_rs%d_bins%d",
             dt, n_t, N, c, q, rs, M);
    snprintf(counts_name, sizeof counts_name, "../data/%s_counts_%s.txt",
             config.type, config_name);
    snprintf(log_name, sizeof log_name, "../data/%s_log_%s.txt",
             config.type, config_name);
    FILE *counts_file = fopen(counts_name, "w");
    FILE *log_file = fopen(log_name, "w");
    if (counts_file == NULL || log_file == NULL) {
        perror("fopen");
        if (counts_file != NULL) fclose(counts_file);
        if (log_file != NULL) fclose(log_file);
        for (int t = 0; t < max_threads; ++t) gsl_rng_free(thread_rngs[t]);
        free(thread_rngs);
        return;
    }

    double *A = malloc((size_t)N * sizeof *A);
    double *B = malloc((size_t)N * sizeof *B);
    double *C = malloc((size_t)N * sizeof *C);
    double *A_next = malloc((size_t)N * sizeof *A_next);
    double *B_next = malloc((size_t)N * sizeof *B_next);
    double *C_next = malloc((size_t)N * sizeof *C_next);
    int *Ac = malloc((size_t)M * sizeof *Ac);
    int *Bc = malloc((size_t)M * sizeof *Bc);
    int *Cc = malloc((size_t)M * sizeof *Cc);
    double *Ar = malloc((size_t)M * sizeof *Ar);
    double *Br = malloc((size_t)M * sizeof *Br);
    double *Cr = malloc((size_t)M * sizeof *Cr);

    if (!A || !B || !C || !A_next || !B_next || !C_next ||
        !Ac || !Bc || !Cc || !Ar || !Br || !Cr) {
        fprintf(stderr, "Array allocation failure.\n");
        fclose(counts_file); fclose(log_file);
        free(A); free(B); free(C); free(A_next); free(B_next); free(C_next);
        free(Ac); free(Bc); free(Cc); free(Ar); free(Br); free(Cr);
        for (int t = 0; t < max_threads; ++t) gsl_rng_free(thread_rngs[t]);
        free(thread_rngs);
        return;
    }

    distribute_coordinates_uniformly(A, N, lower, upper);
    distribute_coordinates_uniformly(B, N, lower, upper);
    distribute_coordinates_uniformly(C, N, lower, upper);

    double *Acur = A, *Bcur = B, *Ccur = C;
    double *Anext = A_next, *Bnext = B_next, *Cnext = C_next;

    histogram_omp(Acur, Ac, N, M, lower, dx);
    histogram_omp(Bcur, Bc, N, M, lower, dx);
    histogram_omp(Ccur, Cc, N, M, lower, dx);
    write_counts_frame(counts_file, Ac, Bc, Cc, M);

    for (int step = 1; step < n_t; ++step) {
        /* Build all fields from the same time level t_n. */
        histogram_omp(Acur, Ac, N, M, lower, dx);
        histogram_omp(Bcur, Bc, N, M, lower, dx);
        histogram_omp(Ccur, Cc, N, M, lower, dx);
        sensed_density_omp(Ac, Ar, N, M, rs, dx);
        sensed_density_omp(Bc, Br, N, M, rs, dx);
        sensed_density_omp(Cc, Cr, N, M, rs, dx);

        update_species_omp(Acur, Anext, Br, Cr, N, M, lower, upper, dx,
                           D0, c, q, dt, thread_rngs);
        update_species_omp(Bcur, Bnext, Cr, Ar, N, M, lower, upper, dx,
                           D0, c, q, dt, thread_rngs);
        update_species_omp(Ccur, Cnext, Ar, Br, N, M, lower, upper, dx,
                           D0, c, q, dt, thread_rngs);

        double *tmp;
        tmp = Acur; Acur = Anext; Anext = tmp;
        tmp = Bcur; Bcur = Bnext; Bnext = tmp;
        tmp = Ccur; Ccur = Cnext; Cnext = tmp;

        if (step % frame_step == 0 || step == n_t - 1) {
            histogram_omp(Acur, Ac, N, M, lower, dx);
            histogram_omp(Bcur, Bc, N, M, lower, dx);
            histogram_omp(Ccur, Cc, N, M, lower, dx);
            write_counts_frame(counts_file, Ac, Bc, Cc, M);
        }
        if (step % progress_step == 0 || step == n_t - 1) {
            fprintf(stderr, "\r%6.2f%%", 100.0 * (double)step / (double)(n_t - 1));
            fflush(stderr);
        }
    }

    const int elapsed = (int)difftime(time(NULL), start);
    fprintf(stderr, "\nElapsed time: %d h %d min %d s\n", elapsed / 3600,
            (elapsed % 3600) / 60, elapsed % 60);
    fprintf(log_file, "base_seed=%lu\n", base_seed);
    fprintf(log_file, "threads=%d\n", max_threads);
    fprintf(log_file, "elapsed_seconds=%d\n", elapsed);
    fprintf(log_file, "model: D_eff=D0*[1+c*(rho_other_1+rho_other_2)^q]\n");
    fprintf(log_file, "sensing=normalized_top_hat; rs=%d cells\n", rs);
    fprintf(log_file, "species_update=synchronous\n");

    fclose(counts_file); fclose(log_file);
    free(A); free(B); free(C); free(A_next); free(B_next); free(C_next);
    free(Ac); free(Bc); free(Cc); free(Ar); free(Br); free(Cr);
    for (int t = 0; t < max_threads; ++t) gsl_rng_free(thread_rngs[t]);
    free(thread_rngs);
}

int main(int argc, char *argv[])
{
    char config_path[64];
    if (argc == 1)
        snprintf(config_path, sizeof config_path, "../configs/config.txt");
    else
        snprintf(config_path, sizeof config_path, "../configs/%s.txt", argv[1]);
    DiffusionConfig config = read_config(config_path);
    diffuse_and_save_histograms(config);
    return 0;
}
