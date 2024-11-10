#ifndef OPTION_MODULE
#define OPTION_MODULE

#include <stdbool.h>
#include <stdio.h>

typedef struct opt {
    void * data;
    bool has_data;
} opt;

opt * opt_init(const size_t size);
void opt_free(opt * p);

#endif
