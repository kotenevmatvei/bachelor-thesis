#include "simulation.h"
#include "testing.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int test_count = 0;
int passed_count = 0;

void assert_test(int condition, const char *test_name) {
  test_count++;
  if (condition) {
    printf("✓ PASS: %s\n", test_name);
    passed_count++;
  } else {
    printf("✗ FAIL: %s\n", test_name);
  }
}

void print_test_failure(const char *test_name, int *expected, int *actual, int len, double *expected_bounds, double *actual_bounds, int n_bounds) {
  printf("✗ FAIL: %s\n", test_name);
  printf("  Expected counts: ");
  for (int i = 0; i < len; i++) printf("%d ", expected[i]);
  printf("\n  Actual counts:   ");
  for (int i = 0; i < len; i++) printf("%d ", actual[i]);
  printf("\n  Expected bounds: ");
  for (int i = 0; i < n_bounds; i++) printf("%.6f ", expected_bounds[i]);
  printf("\n  Actual bounds:   ");
  for (int i = 0; i < n_bounds; i++) printf("%.6f ", actual_bounds[i]);
  printf("\n");
}

// Test 1: Single element array
void test_single_element() {
  double array[] = {1.44};
  int len = 1;
  int n_bins = 3;
  int expected_counts[] = {1, 0, 0};
  double expected_bounds[] = {1.44, 1.44, 1.44, 1.44};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Single element\n");
    passed_count++;
  } else {
    print_test_failure("Single element", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 2: All identical values (zero range)
void test_identical_values() {
  double array[] = {5.0, 5.0, 5.0, 5.0, 5.0};
  int len = 5;
  int n_bins = 4;
  int expected_counts[] = {5, 0, 0, 0};
  double expected_bounds[] = {5.0, 5.0, 5.0, 5.0, 5.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Identical values\n");
    passed_count++;
  } else {
    print_test_failure("Identical values", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 3: Two distinct values
void test_two_values() {
  double array[] = {1.0, 2.0};
  int len = 2;
  int n_bins = 2;
  int expected_counts[] = {1, 1};
  double expected_bounds[] = {1.0, 1.5, 2.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Two distinct values\n");
    passed_count++;
  } else {
    print_test_failure("Two distinct values", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 4: Uniformly distributed values
void test_uniform_distribution() {
  double array[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  int len = 10;
  int n_bins = 5;
  int expected_counts[] = {2, 2, 2, 2, 2};
  double expected_bounds[] = {0.0, 1.8, 3.6, 5.4, 7.2, 9.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Uniform distribution\n");
    passed_count++;
  } else {
    print_test_failure("Uniform distribution", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 5: All values at boundaries
void test_boundary_values() {
  double array[] = {0.0, 5.0, 10.0};
  int len = 3;
  int n_bins = 2;
  int expected_counts[] = {1, 2};
  double expected_bounds[] = {0.0, 5.0, 10.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Boundary values\n");
    passed_count++;
  } else {
    print_test_failure("Boundary values", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 6: Negative values
void test_negative_values() {
  double array[] = {-5.0, -3.0, -1.0, 1.0, 3.0, 5.0};
  int len = 6;
  int n_bins = 3;
  int expected_counts[] = {2, 2, 2};
  double expected_bounds[] = {-5.0, -1.666666667, 1.666666667, 5.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Negative values\n");
    passed_count++;
  } else {
    print_test_failure("Negative values", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 7: Very small differences
void test_small_differences() {
  double array[] = {1.0000, 1.0001, 1.0002, 1.0003};
  int len = 4;
  int n_bins = 2;
  int expected_counts[] = {2, 2};
  double expected_bounds[] = {1.0000, 1.00015, 1.0003};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Small differences\n");
    passed_count++;
  } else {
    print_test_failure("Small differences", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 8: Large array
void test_large_array() {
  int len = 1000;
  double *array = malloc(len * sizeof(double));
  for (int i = 0; i < len; i++) {
    array[i] = (double)i / 10.0;
  }

  int n_bins = 10;
  int expected_counts[] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
  double expected_bounds[] = {0.0, 9.99, 19.98, 29.97, 39.96, 49.95, 59.94, 69.93, 79.92, 89.91, 99.9};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Large array\n");
    passed_count++;
  } else {
    print_test_failure("Large array", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(array);
  free(bin_bounds);
  free(counts);
}

// Test 9: Single bin
void test_single_bin() {
  double array[] = {1.0, 2.0, 3.0, 4.0, 5.0};
  int len = 5;
  int n_bins = 1;
  int expected_counts[] = {5};
  double expected_bounds[] = {1.0, 5.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Single bin\n");
    passed_count++;
  } else {
    print_test_failure("Single bin", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 10: Maximum value edge case
void test_max_value_placement() {
  double array[] = {1.0, 1.5, 2.0, 2.5, 3.0};
  int len = 5;
  int n_bins = 2;
  int expected_counts[] = {2, 3};
  double expected_bounds[] = {1.0, 2.0, 3.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Max value placement\n");
    passed_count++;
  } else {
    print_test_failure("Max value placement", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 11: Unsorted array
void test_unsorted_array() {
  double array[] = {5.0, 1.0, 3.0, 2.0, 4.0};
  int len = 5;
  int n_bins = 5;
  int expected_counts[] = {1, 1, 1, 1, 1};
  double expected_bounds[] = {1.0, 1.8, 2.6, 3.4, 4.2, 5.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Unsorted array\n");
    passed_count++;
  } else {
    print_test_failure("Unsorted array", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 12: Mixed positive and negative with zero
void test_mixed_with_zero() {
  // array = {-2.0, -1.0, 0.0, 1.0, 2.0}
  // min = -2.0, max = 2.0, range = 4.0, bin_size = 1.0
  // bins: [-2,-1), [-1,0), [0,1), [1,2]
  // -2.0: bin = (int)((−2−(−2))/1.0) = 0 -> counts[0]++
  // -1.0: bin = (int)((−1−(−2))/1.0) = 1 -> counts[1]++
  //  0.0: bin = (int)((0−(−2))/1.0) = 2 -> counts[2]++
  //  1.0: bin = (int)((1−(−2))/1.0) = 3 -> counts[3]++
  //  2.0: bin = (int)((2−(−2))/1.0) = 4 -> clamped to 3 -> counts[3]++
  double array[] = {-2.0, -1.0, 0.0, 1.0, 2.0};
  int len = 5;
  int n_bins = 4;
  int expected_counts[] = {1, 1, 1, 2};
  double expected_bounds[] = {-2.0, -1.0, 0.0, 1.0, 2.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Mixed with zero\n");
    passed_count++;
  } else {
    print_test_failure("Mixed with zero", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

// Test 13: Bin bounds calculation
void test_bin_bounds_calculation() {
  double array[] = {0.0, 10.0};
  int len = 2;
  int n_bins = 5;
  int expected_counts[] = {1, 0, 0, 0, 1};
  double expected_bounds[] = {0.0, 2.0, 4.0, 6.0, 8.0, 10.0};

  double *bin_bounds = calloc(n_bins + 1, sizeof(double));
  int *counts = smart_histogram(array, len, n_bins, bin_bounds);

  int counts_match = compare_int_arrays(counts, expected_counts, n_bins);
  int bounds_match = compare_double_arrays(bin_bounds, expected_bounds, n_bins + 1);

  if (counts_match && bounds_match) {
    test_count++;
    printf("✓ PASS: Bin bounds calculation\n");
    passed_count++;
  } else {
    print_test_failure("Bin bounds calculation", expected_counts, counts, n_bins, expected_bounds, bin_bounds, n_bins + 1);
    test_count++;
  }

  free(bin_bounds);
  free(counts);
}

int main() {
  printf("Running smart_histogram test suite...\n\n");

  test_single_element();
  test_identical_values();
  test_two_values();
  test_uniform_distribution();
  test_boundary_values();
  test_negative_values();
  test_small_differences();
  test_large_array();
  test_single_bin();
  test_max_value_placement();
  test_unsorted_array();
  test_mixed_with_zero();
  test_bin_bounds_calculation();

  printf("Tests passed: %d/%d\n", passed_count, test_count);

  return (passed_count == test_count) ? 0 : 1;
}
