#ifndef OPTION_MODULE
#define OPTION_MODULE

#include <stdbool.h>
#include <stddef.h>

typedef struct opt opt;
struct opt {
    void * data;
    bool exists;
};

#define opt_get(opt, t) ((t *) (opt).data)
#define opt_init(opt, t, a) (opt).data=alloc(a, sizeof(t), _Alignof(t), 1)

#endif
