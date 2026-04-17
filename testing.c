#include <stdio.h>

int compare_int_arrays(int *a, int *b, int len) {
    int result = 1;
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i]) {
            printf("a[%d] = %d, b[%d] = %d\n", i, a[i], i, b[i]);
            result = 0;
        }
    }
    return result;
}

// precision: delta = 1e-8
double compare_double_arrays(double *a, double *b, int len) {
    int result = 1;
    for (int i = 0; i < len; i++) {
        if ((a[i] < b[i] - 1e-8) || (a[i] > b[i] + 1e-8)) {
            printf("a[%d] = %.15f, b[%d] = %.15f\n", i, a[i], i, b[i]);
            result = 0;
        }
    }
    return result;
}


