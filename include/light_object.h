#ifndef _LIGHT_OBJECT_H
#define _LIGHT_OBJECT_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <light_util.h>

// C11 atomics are not supported on Cortex-M0/M0+ CPU cores, so RP2 targets
// must use hard spinlocks for synchronization
#include <pico/platform.h>
#ifdef PICO_RP2040
typedef uint32_t light_ref_t;
#else
#include <stdatomic.h>
typedef atomic_char32_t light_ref_t;
#endif

#define LOM_OBJ_NAME_LENGTH 16

struct light_object_registry;

struct light_object {
        light_ref_t ref_count;
        struct light_object *parent;
        struct lobj_type *type;
        uint16_t state_initialized: 1;
        uint16_t is_static: 1;
        uint16_t is_readonly: 1;
        uint8_t id[LOM_OBJ_NAME_LENGTH];
} __packed_aligned;
 
struct lobj_type {
        uint8_t id[LOM_OBJ_NAME_LENGTH];
        void (*release)(struct light_object*);
        void (*evt_add)(struct light_object *obj, struct light_object *parent);
        void (*evt_child_add)(struct light_object *obj, struct light_object *child);
        void (*evt_child_remove)(struct light_object *obj, struct light_object *child);
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

extern void *light_object_alloc(size_t size);
extern void *light_object_alloc_reg(struct light_object_registry *reg, size_t size);
extern void light_object_free(void *obj);
extern void light_object_free_reg(struct light_object_registry *reg, void *obj);

extern int light_object_add(struct light_object *obj, struct light_object *parent,
                            uint8_t *format, ...);
extern int light_object_add_reg(struct light_object_registry *reg, struct light_object *obj, struct light_object *parent,
                            uint8_t *format, ...);
extern int light_object_del(struct light_object *obj);
extern int light_object_del_reg(struct light_object_registry *reg, struct light_object *obj);

extern struct light_object *light_object_get_reg(struct light_object_registry *reg, struct light_object *obj);
extern void light_object_put_reg(struct light_object_registry *reg, struct light_object *obj);

extern void light_object_init_reg(struct light_object_registry *reg, struct light_object *obj, struct lobj_type *type);


#endif