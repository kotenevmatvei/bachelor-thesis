#ifndef UTILS_H
#define UTILS_H

#include "simulation.h"

double *alloc_fill_double_array(const int len, const double default_value);
int sum_int_array(const int *array, const int len);
DiffusionConfig read_config(const char *filename);

#endif // UTILS_H
