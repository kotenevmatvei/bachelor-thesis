#ifndef IO_H
#define IO_H
#include <stdio.h>

void write_double_matrix_to_file(double **matrix, int n_rows, int n_cols,
                                 char fname[]);

void write_int_array_to_file(const int *array, const int array_len,
                             const char fname[]);

void write_double_array_to_file(const double *array, const int array_len,
                                const char *fname, char *array_name,
                                char *mode);

void print_double_array(double *a, int len_a, char *name);

void print_int_array(int *a, int len_a, char *name);

#endif // IO_H
