#include "io.h"
#include <stdio.h>

void write_double_matrix_to_file(const double *const *matrix, const int n_rows,
                                 const int n_cols, const char fname[]) {
    FILE *fptr = fopen(fname, "w");
    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols - 1; j++) {
            fprintf(fptr, "%f ", matrix[i][j]);
        }
        fprintf(fptr, "%f\n", matrix[i][n_cols - 1]); // new line instead of space
    }
    fclose(fptr);
}

void write_int_array_to_file(const int *array, const int array_len, const char *fname,
                             const char *mode, const char *comment) {
    FILE *fptr = fopen(fname, mode);

    fprintf(fptr, "%s", comment);

    for (int i = 0; i < array_len - 1; ++i) {
        fprintf(fptr, "%d ", array[i]);
    }
    fprintf(fptr, "%d\n", array[array_len - 1]);

    fclose(fptr);
}

void write_double_array_to_file(const double *array, const int array_len,
                                const char *fname, const char *mode,
                                const char *comment) {
    FILE *fptr = fopen(fname, mode);

    fprintf(fptr, "%s", comment);

    for (int i = 0; i < array_len - 1; ++i) {
        fprintf(fptr, "%f ", array[i]);
    }
    fprintf(fptr, "%f\n", array[array_len - 1]);

    fclose(fptr);
}

void print_double_array(const double *a, const int len_a, const char *name) {
    printf("%s: {", name);
    for (int i = 0; i < len_a - 1; i++) {
        printf("%f, ", a[i]);
    }
    printf("%f}\n", a[len_a - 1]);
}

void print_int_array(const int *a, const int len_a, const char *name) {
    printf("%s: {", name);
    for (int i = 0; i < len_a - 1; i++) {
        printf("%d, ", a[i]);
    }
    printf("%d}\n", a[len_a - 1]);
}
