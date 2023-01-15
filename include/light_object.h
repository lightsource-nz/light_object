#ifndef _LIGHT_OBJECT_H
#define _LIGHT_OBJECT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <light_util.h>
#include <pico/platform.h>

// C11 atomics are not supported on Cortex-M0/M0+ CPU cores, so RP2 targets
// must use hard spinlocks for synchronization
// #include <stdatomic.h>

#define LOM_OBJ_NAME_LENGTH 16

struct light_object_registry;

__packed_aligned
struct light_object {
        uint32_t ref_count;
        struct light_object *parent;
        struct lobj_type *type;
        uint16_t state_initialized: 1;
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