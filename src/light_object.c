/*
 *  light_object.c
 *  core definitions for the light object model
 * 
 *  authored by Alex Fulton
 *  created january 2023
 * 
 */

#include <light_object.h>

#if(LIGHT_SYSTEM == SYSTEM_PICO_SDK && LIGHT_PLATFORM == PLATFORM_TARGET)
#define USE_PICO_SPINLOCKS
#include <pico/critical_section.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

struct light_object_registry {
#ifdef USE_PICO_SPINLOCKS
        critical_section_t mutex;
#endif
        void *(*alloc)(size_t);
        void (*free)(void *);
};

static bool _registry_loaded = false;
static struct light_object_registry _registry_default;

static uint8_t light_object_set_name_va(struct light_object *obj, const uint8_t *format, va_list vargs);

static void _registry_critical_enter(struct light_object_registry *reg)
{
#ifdef USE_PICO_SPINLOCKS
        critical_section_enter_blocking(&reg->mutex);
#endif
}
static void _registry_critical_exit(struct light_object_registry *reg)
{
#ifdef USE_PICO_SPINLOCKS
        critical_section_exit(&reg->mutex);
#endif
}

void light_object_setup()
{
        if(!_registry_loaded) {
#ifdef USE_PICO_SPINLOCKS
                critical_section_init(&_registry_default.mutex);
#endif
                _registry_default.alloc = light_alloc;
                _registry_default.free = light_free;
                _registry_loaded = true;
        }
}
static struct light_object_registry *_get_default_registry()
{
        if(_registry_loaded)
                return &_registry_default;
        return NULL;
}
struct light_object_registry *light_object_registry_default()
{
        return _get_default_registry();
}

struct light_object *light_object_get(struct light_object *obj)
{
        return light_object_get_reg(_get_default_registry(), obj);
}
void light_object_put(struct light_object *obj)
{
        light_object_put_reg(_get_default_registry(), obj);
}

void light_object_init(
        struct light_object *obj, const struct lobj_type *type, const uint8_t *format, ...)
{
        va_list args;
        va_start(args, format);
        light_object_init_va_reg(_get_default_registry(), obj, type, format, args);
        va_end(args);
}
void light_object_init_va(
        struct light_object *obj, const struct lobj_type *type, const uint8_t *format, va_list args)
{
        light_object_init_va_reg(_get_default_registry(), obj, type, format, args);
}
void light_object_init_reg(struct light_object_registry *reg, struct light_object *obj, const struct lobj_type *type, const uint8_t *format, ...)
{
        va_list args;
        va_start(args, format);
        light_object_init_va_reg(reg, obj, type, format, args);
        va_end(args);
}
void light_object_init_va_reg(struct light_object_registry *reg, struct light_object *obj, const struct lobj_type *type, const uint8_t *format, va_list args)
{
        if(!light_object_set_name_va(obj, format, args)) {
                // TODO these functions really need to return an error code or something
                light_error("error initializing object: could not write name field with format [%s]", format);
                return;
        }
#ifdef USE_PICO_SPINLOCKS
        // TODO pretty sure there should be some actual locking here
        obj->ref_count = 1;
#else
        atomic_store(&obj->ref_count, 1);
#endif
    obj->type = type;

}
void *light_object_alloc(size_t size)
{
        return light_object_alloc_reg(_get_default_registry(), size);
}
void *light_object_alloc_reg(struct light_object_registry *reg, size_t size)
{
        return reg->alloc(size);
}
void light_object_free(void *obj)
{
        light_object_free_reg(_get_default_registry(), obj);
}
void light_object_free_reg(struct light_object_registry *reg, void *obj)
{
        reg->free(obj);
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
int light_object_add_reg(struct light_object_registry *reg, struct light_object *parent, struct light_object *child)
{
        light_trace("parent=%s, child=%s",light_object_get_name(parent), light_object_get_name(child));
        child->parent = light_object_get(parent);

        if(parent && parent->type->evt_child_add)
                parent->type->evt_child_add(parent, child);
        if(child->type->evt_add)
                child->type->evt_add(child, parent);

        return LIGHT_OK;
}
int light_object_add(struct light_object *parent, struct light_object *child)
{
        return light_object_add_reg(&_registry_default, parent, child);
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

// TODO implement saturation conditions and warnings
struct light_object *light_object_get_reg(struct light_object_registry *reg, struct light_object *obj)
{
        struct light_object *ref = obj;
        if(obj) {
#ifdef USE_PICO_SPINLOCKS
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
#ifdef USE_PICO_SPINLOCKS
        critical_section_enter_blocking(&reg->mutex);
        obj->ref_count--;
        critical_section_exit(&reg->mutex);
#else
        uint8_t status;
        uint32_t count = obj->ref_count;
        do {
                if(count > 0)
                        status = atomic_compare_exchange_strong(&obj->ref_count, &count, count - 1);
                else
                        return;
        } while (status);
#endif
}
int light_ref_get(light_ref_t *ref)
{

}
void light_ref_put(light_ref_t *ref)
{

}
