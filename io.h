#ifndef IO_H
#define IO_H

void write_double_matrix_to_file(const double *const *matrix, int n_rows,
                                 int n_cols, const char fname[]);

void write_int_array_to_file(const int *array, int array_len, const char *fname,
                             const char *mode, const char *comment);

void write_double_array_to_file(const double *array, int array_len, const char *fname,
                                const char *mode, const char *comment);

void print_double_array(const double *a, int len_a, const char *name);

void print_int_array(const int *a, int len_a, const char *name);

#endif // IO_H
