#include "simulation.h"
#include "testing.h"
#include <stdio.h>
#include <stdlib.h>
#include "io.h"

#define MAX_ARRAY_LENGTH 1000
#define MAX_COUNTS 100
#define MAX_NAME_LENGTH 50

typedef struct {
    char name[MAX_NAME_LENGTH];
    int n_array;
    double array[MAX_ARRAY_LENGTH];
    int n_bins;
    int expected_cnts[MAX_COUNTS];
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

void handle_histogram_test_case(histogram_test_case *tc) {
    double *bin_bounds = calloc(tc->n_bins + 1, sizeof(double));
    int *cnts = histogram_flexible_bounds(tc->array, bin_bounds, tc->n_array, tc->n_bins);
    int cnts_equal = compare_int_arrays(tc->expected_cnts, cnts, tc->n_bins);
    double bin_bounds_equal = compare_double_arrays(tc->expected_bin_bounds, bin_bounds,
                                                    tc->n_bins + 1);

    if (cnts_equal && bin_bounds_equal) {
        printf("PASS: %s\n", tc->name);
    } else {
        printf("FAIL: %s\n", tc->name);
        printf("\t");
        print_double_array(tc->array, tc->n_array, "array");
        // print wrong arrays
        if (!cnts_equal) {
            printf("\t");
            print_int_array(tc->expected_cnts, tc->n_bins, "Expected cnts");
            printf("\t");
            print_int_array(cnts, tc->n_bins, "Got instead");
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
