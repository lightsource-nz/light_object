#ifndef _LIGHT_OBJECT_H
#define _LIGHT_OBJECT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <pico/platform.h>

#define LIGHT_OK                        (uint8_t) 0x0u
#define LIGHT_ARG_INVALID               (uint8_t) 0x1u
#define LIGHT_NO_MEMORY                 (uint8_t) 0x2u
#define LIGHT_NO_RESOURCE               (uint8_t) 0x3u

// C11 atomics are not supported on Cortex-M0/M0+ CPU cores, so RP2 targets
// must use hard spinlocks for synchronization
// #include <stdatomic.h>

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

struct light_object_registry;

__packed_aligned
struct light_object {
        uint32_t ref_count;
        struct light_object *parent;
        struct lobj_type *type;
        uint16_t is_static: 1;
        uint16_t is_readonly: 1;
        uint8_t id[LOM_OBJ_NAME_LENGTH];
};
 
struct lobj_type {
        uint8_t id[LOM_OBJ_NAME_LENGTH];
        void (*release)(struct light_object*);
};

// initialize default object registry, must be called before any light_object API functions
extern void light_object_setup();
extern struct light_object_registry *light_object_registry_default();

// TODO implement saturation conditions and warnings
extern struct light_object *light_object_get(struct light_object *obj);
extern void light_object_put(struct light_object *obj);

extern void light_object_init(struct light_object *obj, struct lobj_type *type);

static inline const uint8_t *light_object_get_name(struct light_object *obj)
{
        return obj->id;
}

extern int light_object_add(struct light_object *obj, struct light_object *parent,
                            uint8_t *format, ...);

extern struct light_object *light_object_get_reg(struct light_object_registry *reg, struct light_object *obj);
extern void light_object_put_reg(struct light_object_registry *reg, struct light_object *obj);

extern void light_object_init_reg(struct light_object_registry *reg, struct light_object *obj, struct lobj_type *type);

extern int light_object_add_reg(struct light_object_registry *reg, struct light_object *obj, struct light_object *parent,
                            uint8_t *format, ...);

#endif