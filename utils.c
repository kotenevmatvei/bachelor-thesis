#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double *alloc_fill_double_array(const int len, const double default_val) {
    double *array = malloc(len * sizeof(double));
    for (int i = 0; i < len; i++) {
        array[i] = (double)default_val;
    }
    return array;
}

int sum_int_array(const int *array, const int len) {
    int sum = 0;
    for (int i = 0; i < len; i++)
        sum += array[i];
    return sum;
}

DiffusionConfig read_config(const char *filename) {
    DiffusionConfig config = {0};

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char key[64];
    char val_str[64];

    while (fscanf(file, "%63s %63s", key, val_str) == 2) {

        if (strcmp(key, "type") == 0) {
            strcpy(config.type, val_str);
        } else if (strcmp(key, "delta_t") == 0) {
            config.delta_t = atof(val_str);
        } else if (strcmp(key, "start") == 0) {
            config.start = atof(val_str);
        } else if (strcmp(key, "lower_bound") == 0) {
            config.lower_bound = atof(val_str);
        } else if (strcmp(key, "upper_bound") == 0) {
            config.upper_bound = atof(val_str);
        } else if (strcmp(key, "n_t") == 0) {
            config.n_t = atoi(val_str);
        } else if (strcmp(key, "n_realizations") == 0) {
            config.n_realizations = atoi(val_str);
        } else if (strcmp(key, "d") == 0) {
            config.d = atof(val_str);
        } else if (strcmp(key, "n_bins") == 0) {
            config.n_bins = atoi(val_str);
        } else if (strcmp(key, "c") == 0) {
            config.c = atof(val_str);
        } else if (strcmp(key, "q") == 0) {
            config.q = atoi(val_str);
        } else {
            printf("Warning: Unknown config key '%s' ignored.\n", key);
        }
    }

    if (!feof(file)) {
        fprintf(stderr, "FATAL ERROR: Config file formatting is broken or contains "
                        "non-numeric values.\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
    return config;

}
