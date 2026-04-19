#include "simulation.h"
#include "testing.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_ARRAY_LENGTH 1000
#define MAX_COUNTS 100
#define MAX_NAME_LENGTH 50

typedef struct {
    char name[MAX_NAME_LENGTH];
    int n_array;
    double array[MAX_ARRAY_LENGTH];
    int n_bins;
    int expected_counts[MAX_COUNTS];
    double expected_bin_bounds[MAX_COUNTS + 1];
} histogram_test_case;

histogram_test_case test_cases[] = {
    {
        "simple_case",
        4,
        {1.2, 0, -2, 1},
        3,
        {1, 1, 4},
        {-2, -0.93333333, 0.13334, 1.2}
    }
};

int handle_histogram_test_case(histogram_test_case *tc) {
    double *bin_bounds = calloc(tc->n_bins + 1, sizeof(double));
    int *counts = histogram(tc->array, tc->n_array, tc->n_bins, bin_bounds);
    int counts_equal = compare_int_arrays(tc->expected_counts, counts, tc->n_bins);
    double bin_bounds_equal = compare_double_arrays(tc->expected_bin_bounds, bin_bounds,
                                                    tc->n_bins + 1);

    if (counts_equal && bin_bounds_equal) {
        printf("PASS: %s\n", tc->name);
    } else {
        printf("FAIL: %s\n", tc->name);
        printf("\t");
        print_double_array(tc->array, tc->n_array, "array");
        // print wrong arrays
        if (!counts_equal) {
            printf("\t");
            print_int_array(tc->expected_counts, tc->n_bins, "Expected counts");
            printf("\t");
            print_int_array(counts, tc->n_bins, "Got instead");
        }
        if (!bin_bounds_equal) {
            printf("\t");
            print_double_array(tc->expected_bin_bounds, tc->n_bins + 1,
                               "Expected bin bounds");
            printf("\t");
            print_double_array(bin_bounds, tc->n_bins, "Got instead");
        }
    }
}

int n_tcs = sizeof(test_cases) / sizeof(histogram_test_case);

int main() {
    for (int i = 0; i < n_tcs; i++) {
        handle_histogram_test_case(&test_cases[i]);
    }
}
