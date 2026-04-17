#include <stdio.h>

void write_double_matrix_to_file(double **matrix, int n_rows, int n_cols, char fname[]) {
    int i, j;

    FILE *fptr = fopen(fname, "w");

    for (i = 0; i < n_rows; ++i) {
        fprintf(fptr, "row %d\n", i);
        for (j = 0; j < n_cols; ++j) {
            fprintf(fptr, "%lf\n", matrix[i][j]);
        }
    }

    fclose(fptr);
}

void write_int_array_to_file(const int *array, const int array_len, const char fname[]) {
    FILE *fptr = fopen(fname, "w");

    for (int i = 0; i < array_len; ++i) {
        fprintf(fptr, "%d\n", array[i]);
    }

    fclose(fptr);
}

void write_double_array_to_file(const double *array, const int array_len, const char *fname, char *array_name, char *mode) {
    FILE *fptr = fopen(fname, mode);

    fprintf(fptr, "%s = [", array_name);

    for (int i = 0; i < array_len-1; ++i) {
        fprintf(fptr, "%f, ", array[i]);
    }

    fprintf(fptr, "%f]\n", array[array_len - 1]); // last element without a trailing space

    fclose(fptr);
}

void print_double_array(double *a, int len_a, char *name) {
    printf("\n%s = [", name);
    for (int i = 0; i < len_a - 1; i++) {
        printf("%f, ", a[i]);
    }
    printf("%f]\n", a[len_a - 1]);
}

void print_int_array(int *a, int len_a, char *name) {
    printf("\n%s = [", name);
    for (int i = 0; i < len_a - 1; i++) {
        printf("%d, ", a[i]);
    }
    printf("%d]\n", a[len_a - 1]);
}

