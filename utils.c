#include <stdlib.h>

double *alloc_fill_double_array(const int len, const double default_val) {
    double *array = malloc(len * sizeof(double));
    for (int i = 0; i < len; i++) {
        array[i] = (double)default_val;
    }
    return array;
}

int sum_int_array(const int *array, const int len) {
    int sum = 0;
    for (int i = 0; i < len; i ++)
        sum += array[i];
    return sum;
}
