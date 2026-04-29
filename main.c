#include "simulation.h"
#include "io.h"
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

#define COMMENT_LENGTH 50


#include <stdio.h>
#include <stdlib.h>

void calculate_and_save_correlation(
    int N_realizations,
    int N_t,
    double delta_t,
    const char* filename,
    gsl_rng *r,
    double D,
    double gamma)
{
    int max_k = N_t / 2;

    // Accumulator for the correlation function
    double *corr_accumulator = calloc(N_t, sizeof(double));

    // Allocate GSL FFT structures once to reuse
    gsl_fft_halfcomplex_wavetable *hcwavetable = gsl_fft_halfcomplex_wavetable_alloc(N_t);
    gsl_fft_real_wavetable *wavetable = gsl_fft_real_wavetable_alloc(N_t);
    gsl_fft_real_workspace *workspace = gsl_fft_real_workspace_alloc(N_t);

    printf("Simulating %d trajectories and computing correlations...\n", N_realizations);

    // Simulate all trajectories at once (using your updated style)
    double **v_data = simulate_V_values(D, gamma, N_realizations, N_t, delta_t, r);

    for (int i = 0; i < N_realizations; i++) {

        // 1. FORWARD FFT: Time Domain -> Frequency Domain
        gsl_fft_real_transform(v_data[i], 1, N_t, wavetable, workspace);

        // 2. REPACK INTO HALF-COMPLEX LAYOUT IN-PLACE
        // We calculate the squared magnitude (power) and pack it into the real slots,
        // while putting 0.0 into the imaginary slots.

        double dc = v_data[i][0];
        v_data[i][0] = dc * dc; // DC slot

        for (int k = 1; k < max_k; k++) {
            int real_idx = 2 * k - 1;
            int imag_idx = 2 * k;

            double real_part = v_data[i][real_idx];
            double imag_part = v_data[i][imag_idx];
            double mag_squared = (real_part * real_part) + (imag_part * imag_part);

            v_data[i][real_idx] = mag_squared;      // Power goes back into the Real slot
            v_data[i][imag_idx] = 0.0;              // Imaginary slot is zeroed
        }

        if (N_t % 2 == 0) {
            double nyquist = v_data[i][N_t - 1];    // Nyquist is the very last element
            v_data[i][N_t - 1] = nyquist * nyquist;
        }

        // 3. INVERSE FFT: Frequency Domain -> Time Domain
        // v_data[i] is now perfectly laid out as a "half-complex" array,
        // so the inverse function can read it.
        gsl_fft_halfcomplex_inverse(v_data[i], 1, N_t, hcwavetable, workspace);

        // 4. Accumulate the results
        for (int k = 0; k < N_t; k++) {
            corr_accumulator[k] += v_data[i][k];
        }
    }

    // 5. Normalize and write out to file
    FILE *fp = fopen(filename, "w");

    // Figure 1.3c plots tau from 0 to 10 seconds.
    // tau = k * delta_t. To reach 10s with delta_t=0.01, we loop up to k = 1000.
    int plot_limit_k = (int)(10.0 / delta_t);

    for (int k = 0; k <= plot_limit_k && k < N_t; k++) {

        // Two normalizations are happening here:
        // 1. Divide by N_realizations for the ensemble average.
        // 2. Divide by N_t because the FFT algorithm inherently scales the result by N_t.
        double C_tau = corr_accumulator[k] / ((double)N_t * N_realizations);

        double tau = k * delta_t;
        fprintf(fp, "%.4f %e\n", tau, C_tau);
    }

    fclose(fp);

    // Clean up
    free_matrix_memory(v_data, N_realizations);
    free(corr_accumulator);
    gsl_fft_halfcomplex_wavetable_free(hcwavetable);
    gsl_fft_real_workspace_free(workspace);

    printf("Correlation function successfully written to %s\n", filename);
}
double *calculate_and_save_power_spectrum(
    int N_realizations, // e.g., 1000
    int N_t,            // e.g., 100000
    double delta_t,     // e.g., 0.01
    const char* filename,
    gsl_rng *r,
    double D,
    double gamma)
{
    double T = N_t * delta_t;
    int max_k = N_t / 2;

    // Allocate array to accumulate the sum of powers across all realizations
    // We need max_k + 1 bins to hold frequencies from 0 Hz up to the Nyquist limit
    double *power_accumulator = calloc(max_k + 1, sizeof(double));

    // Allocate GSL FFT structures once to reuse across all realizations
    gsl_fft_real_wavetable *wavetable = gsl_fft_real_wavetable_alloc(N_t);
    gsl_fft_real_workspace *workspace = gsl_fft_real_workspace_alloc(N_t);

    printf("Simulating %d trajectories and computing FFTs...\n", N_realizations);

    // Simulate a single trajectory (N=1)
    double **v_data = simulate_V_values(D, gamma, N_realizations, N_t, delta_t, r);

    // Perform the FFT in-place on the trajectory
    for (int i = 0; i < N_realizations; i++) {
        gsl_fft_real_transform(v_data[i], 1, N_t, wavetable, workspace);

        // 2. Extract half-complex data and accumulate the squared magnitude

        // DC Component (k = 0)
        double real_i = v_data[i][0];
        power_accumulator[0] += (real_i * real_i);

        // Middle Frequencies (Mixed-Radix format)
        for (int k = 1; k < max_k; k++) {
            int real_idx = 2 * k - 1;
            int imag_idx = 2 * k;

            double real_part = v_data[i][real_idx];
            double imag_part = v_data[i][imag_idx];
            power_accumulator[k] += (real_part * real_part) + (imag_part * imag_part);
        }

        if (N_t % 2 == 0) {
            double real_nyquist = v_data[i][N_t - 1];
            power_accumulator[max_k] += (real_nyquist * real_nyquist);
        }

    }


    // 3. Average the accumulated power and write to file
    FILE *fp = fopen(filename, "w");

    // We only loop up to k = 10000 (which is 10 Hz) to match Figure 1.3d
    // If you want the full spectrum, change the limit to: k <= max_k
    int plot_limit_k = (int)(10 * T); // 10 Hz * 1000 seconds = index 10000

    for (int k = 0; k <= plot_limit_k && k <= max_k; k++) {
        // Divide by N_realizations to get the average (variance)
        double average_mag_squared = power_accumulator[k] / N_realizations;

        // Apply the continuous scaling physics from Equation 1.16
        double S_f = (average_mag_squared * delta_t * delta_t) / T;

        // Convert index k to physical frequency f
        double f = (double)k / T;

        fprintf(fp, "%.5f %e\n", f, S_f);
    }

    fclose(fp);

    // Clean up
    free_matrix_memory(v_data, N_realizations);
    gsl_fft_real_wavetable_free(wavetable);
    gsl_fft_real_workspace_free(workspace);

    printf("Power spectrum successfully written to %s\n", filename);
    return power_accumulator;
}

int main(void) {
    // Simulate V values
    // setup gsl

    gsl_rng_env_setup();

    const gsl_rng_type *T = gsl_rng_default;
    gsl_rng *r = gsl_rng_alloc(T);

    // simulation paramteres
    const double D = 0.1;
    const double gamma = 1;
    int N = 10000; // number of realizations (ensembles)
    int N_t = 500; // number of time steps of length delta_t
    const double delta_t = 0.01; // timestep length in seconds
    const int n_hist_datapoints = 100;
    const double hist_range[2] = {-1.5, 1.5};
    const char *hist_fname = "../data/hist_data.txt";
    const int n_bins = 30;

    // simulate and write to files
    double **v_data = simulate_V_values(D, gamma, N, N_t, delta_t, r);

    write_double_matrix_to_file((const double *const *) v_data, N, N_t,
                                "../data/v_values.txt");

    // calculate histograms and write to file
    const int times[] = {30, 50, 100, 400};
    int n_times = sizeof(times) / sizeof(int);
    for (int i = 0; i < n_times; i++) {
        int time_ = times[i];
        double *v_time_snapshot = malloc(N * sizeof(double));
        for (int j = 0; j < N; j++) {
            v_time_snapshot[j] = v_data[j][time_];
        }
        double *bin_bounds = malloc((n_bins + 2) * sizeof(double));
        int *counts = histogram(v_time_snapshot, N, n_bins, bin_bounds);
        char *mode = i == 0 ? "w" : "a";
        char *comment = malloc(20 * sizeof(char));
        sprintf(comment, "# t=%.1fs\n", (double) time_ * delta_t);
        write_int_array_to_file(counts, n_bins, hist_fname, mode, comment);
        write_double_array_to_file(bin_bounds, n_bins + 1, hist_fname, "a", "");
        free(counts);
        free(bin_bounds);
    }

    // calculate theoretical curves and write to files
    double v_0 = 1;

    LinearAxis *V_Axis = malloc((4 + n_hist_datapoints) * sizeof(double) + sizeof(int));
    fill_linear_axis(V_Axis, hist_range[0], hist_range[1], n_hist_datapoints);
    write_double_array_to_file(V_Axis->points, n_hist_datapoints,
                               "../data/theor_curves.txt",
                               "w", "# V_axis\n");

    for (int i = 0; i < 4; i++) {
        double time_ = (double) times[i] * delta_t;
        double *P_vals = calculate_P_values(D, gamma, time_, v_0, V_Axis);

        char *comment = malloc(COMMENT_LENGTH * sizeof(char));
        sprintf(comment, "# P values for time %.1fs\n", time_);
        write_double_array_to_file(P_vals, n_hist_datapoints, "../data/theor_curves.txt",
                                   "a", comment);

        free(P_vals);
        free(comment);
    }

    free(V_Axis);

    // calculate long time average
    N = 1;
    N_t = (int) 1e7;
    v_data = simulate_V_values(D, gamma, N, N_t, delta_t, r);
    double *bin_bounds = malloc(n_bins * sizeof(int));
    int *counts = histogram(v_data[0], N_t, n_bins, bin_bounds);
    write_int_array_to_file(counts, n_bins, "../data/long_time_average.txt", "w",
                            "");
    write_double_array_to_file(bin_bounds, n_bins + 1, "../data/long_time_average.txt",
                               "a", "");

    free(counts);
    free(bin_bounds);

    // save a trajectory
    N = 1;
    N_t = 100000;
    v_data = simulate_V_values(D, gamma, N, N_t, delta_t, r);
    write_double_array_to_file(v_data[0], N_t, "../data/trajectory.txt", "w", "");

    gsl_fft_real_wavetable *wavetable = gsl_fft_real_wavetable_alloc(N_t);
    gsl_fft_real_workspace *workspace = gsl_fft_real_workspace_alloc(N_t);

    int result = gsl_fft_real_transform(v_data[0], 1, N_t, wavetable, workspace);
    printf("FFT result: %d\n", result);

    gsl_fft_real_wavetable_free(wavetable);
    gsl_fft_real_workspace_free(workspace);

    int max_k = N_t / 2; // we only look at the first half

    double *real_transform = malloc((max_k + 1) * sizeof(double));

    // DC component
    real_transform[0] = v_data[0][0];

    // DC component
    real_transform[0] = v_data[0][0];

    if (N_t % 2 == 0) {
        for (int k = 1; k < max_k; k++) {
            // Extract only the real slots
            real_transform[k] = v_data[0][2 * k - 1];
        }
        // max_k is the standalone nyquist frequency
        real_transform[max_k] = v_data[0][N_t - 1];
    }
    // (You can safely delete your extra loop for the odd N_t case here,
    // since you statically defined N_t = 100000)

    write_double_array_to_file(real_transform, max_k + 1, "../data/fft.txt", "w", "");
    free(real_transform);

    // compute the power spectrum
    // double *ps = calculate_and_save_power_spectrum(10000, N_t, delta_t, "../data/ps.txt", r, D, gamma);

    // fft-transform the power spectrum back to time domain to get the correlation function
    calculate_and_save_correlation(10000, N_t, delta_t, "../data/corr.txt", r, D, gamma);


    free_matrix_memory(v_data, N);
    gsl_rng_free(r);
}
