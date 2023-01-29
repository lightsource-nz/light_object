/*
 *  light_object.c
 *  core definitions for the light object model
 * 
 *  authored by Alex Fulton
 *  created january 2023
 * 
 */

#include "light_object.h"

#include <pico/platform.h>
#ifdef PICO_RP2040
#include <pico/critical_section.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

struct light_object_registry {
#ifdef PICO_RP2040
        critical_section_t mutex;
#endif
        // TODO add alloc/free function pointers
};

static bool _registry_loaded = false;
static struct light_object_registry _registry_default;

static void _registry_critical_enter(struct light_object_registry *reg)
{
#ifdef PICO_RP2040
        critical_section_enter_blocking(&reg->mutex);
#endif
}
static void _registry_critical_exit(struct light_object_registry *reg)
{
#ifdef PICO_RP2040
        critical_section_exit(&reg->mutex);
#endif
}

void light_object_setup()
{
        if(!_registry_loaded) {
                _registry_loaded = true;
#ifdef RP2040
                critical_section_init(&_registry_default.mutex);
#endif
        }
}
static struct light_object_registry *_get_default_registry()
{
        if(_registry_loaded)
                return &_registry_default;
        return NULL;
}
extern struct light_object_registry *light_object_registry_default()
{
        return _get_default_registry();
}

extern struct light_object *light_object_get(struct light_object *obj)
{
        return light_object_get_reg(_get_default_registry(), obj);
}
extern void light_object_put(struct light_object *obj)
{
        light_object_put_reg(_get_default_registry(), obj);
}

extern void light_object_init(struct light_object *obj, struct lobj_type *type)
{
        light_object_init_reg(_get_default_registry(), obj, type);
}

static uint8_t light_object_set_name_va(struct light_object *obj, const uint8_t *format, va_list vargs)
{
        if(!format || !format[0]) {
                // TODO log empty name field error
                return LIGHT_INVALID;
        }
        vsnprintf(obj->id, LOM_OBJ_NAME_LENGTH, format, vargs);
        return LIGHT_OK;
}
static int light_object_add_internal(struct light_object_registry *reg, struct light_object *obj)
{
        struct light_object *parent = light_object_get(obj->parent);

        if(parent->type->evt_child_add)
                parent->type->evt_child_add(parent, obj);
        obj->type->evt_add(obj, parent);

        return LIGHT_OK;
}
static int light_object_add_va_reg(struct light_object_registry *reg, struct light_object *obj, struct light_object *parent,
                               uint8_t *format, va_list vargs)
{
        int retval;

        retval = light_object_set_name_va(obj, format, vargs);

        if(retval) {
            light_warn("Could not set name of object at 0x%X: %s\n", obj, light_error_to_string(retval));
            return retval;
        }

        obj->parent = parent;
        return light_object_add_internal(reg, obj);
}
extern int light_object_add(struct light_object *obj, struct light_object *parent,
                            uint8_t *format, ...)
{
        va_list vargs;

        va_start(vargs, format);
        return light_object_add_va_reg(_get_default_registry(), obj, parent, format, vargs);
        va_end(vargs);
}
int light_object_del(struct light_object *obj)
{
        light_object_del_reg(&_registry_default, obj);
}
int light_object_del_reg(struct light_object_registry *reg, struct light_object *obj)
{
        light_object_put_reg(reg, obj->parent);
        if(obj->parent->type->evt_child_remove)
                obj->parent->type->evt_child_remove(obj->parent, obj);
        obj->parent = NULL;
}

void light_object_init_reg(struct light_object_registry *reg, struct light_object *obj, struct lobj_type *type)
{
    obj->ref_count = 1;
    obj->type = type;
}
// TODO implement saturation conditions and warnings
struct light_object *light_object_get_reg(struct light_object_registry *reg, struct light_object *obj)
{
        struct light_object *ref = obj;
        if(obj) {
#ifdef PICO_RP2040
                critical_section_enter_blocking(&reg->mutex);
                if(obj->ref_count > 0)
                        obj->ref_count++;
                else
                         ref = NULL;
                critical_section_exit(&reg->mutex);
#else
                uint32_t old = obj->ref_count;
                do {
                        if(old == 0 ) {
                                ref = NULL;
                                break;
                        }
                } while (!atomic_compare_exchange_strong(&obj->ref_count, &old, obj->ref_count + 1));
                
#endif
        }
        return ref;
}
void light_object_put_reg(struct light_object_registry *reg, struct light_object *obj)
{
#ifdef PICO_RP2040
        critical_section_enter_blocking(&_registry_default.mutex);
        obj->ref_count--;
        critical_section_exit(&_registry_default.mutex);
#else
        uint8_t status;
        uint32_t count = obj->ref_count;
        do {
                if(count > 0)
                        status = atomic_compare_exchange_strong(&obj->ref_count, &count, count + 1);
                else
                        return;
        } while (status);
#endif
}

int light_object_add_reg(struct light_object_registry *reg, struct light_object *obj, struct light_object *parent,
                            uint8_t *format, ...)
{
        va_list vargs;
        int retval;

        va_start(vargs, format);
        retval = light_object_add_va_reg(reg, obj, parent, format, vargs);
        va_end(vargs);

        return retval;
}

int light_ref_get(light_ref_t *ref)
{

}
void light_ref_put(light_ref_t *ref)
{

}
