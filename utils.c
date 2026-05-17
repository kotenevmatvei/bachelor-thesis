#include <stdlib.h>

double *alloc_fill_double_array(double default_val, int len) {
    double *array = malloc(len * sizeof(double));
    for (int i = 0; i < len; i++)
        array[i] = default_val;
    return array;
}
