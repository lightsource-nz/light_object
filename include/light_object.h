#ifndef _LIGHT_OBJECT_H
#define _LIGHT_OBJECT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdatomic.h>

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
        struct light_object *parent;
        struct lobj_type *type;
        atomic_uint_fast16_t ref_count;
        uint16_t is_static: 1;
        uint16_t is_readonly: 1;
        uint8_t id[LOM_OBJ_NAME_LENGTH];
};

struct lobj_type {
        uint8_t id[LOM_OBJ_NAME_LENGTH];
        void (*release)(struct light_object*);
};

// TODO implement saturation conditions and warnings
extern struct light_object *light_object_get(struct light_object *obj);
extern void light_object_put(struct light_object *obj);

extern void light_object_init(struct light_object *obj, struct lobj_type *type);

extern int light_object_set_name(struct light_object *obj, uint8_t *format, ...);

static inline const uint8_t *light_object_get_name(struct light_object *obj)
{
        return obj->id;
}

extern int light_object_add(struct light_object *obj, struct light_object *parent,
                            uint8_t *format, ...);

#endif