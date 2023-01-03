#ifndef _LIGHT_OBJECT_H
#define _LIGHT_OBJECT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// type manipulation macros shamelessly borrowed from the Linux kernel
#define container_of(ptr, type, member) ({                          \
        void *__mptr = (void *)(ptr);                               \
        static_assert(__same_type(*(ptr), ((type *)0)->member) ||   \
                __same_type(*(ptr), void),                          \
                "pointer type mismatch in container_of()");         \
        ((type *)(__mptr - offsetof(type, member))); })

#ifndef __same_type
# define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#endif

#define LOM_OBJ_NAME_LENGTH 16

struct light_object {
    uint16_t ref_count;
    uint16_t is_static: 1;
    uint16_t is_readonly: 1;
    uint8_t name[LOM_OBJ_NAME_LENGTH];
};

#endif