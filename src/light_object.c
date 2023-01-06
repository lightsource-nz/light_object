/*
 *  light_object.c
 *  core definitions for the light object model
 * 
 *  authored by Alex Fulton
 *  created january 2023
 * 
 */

#include "light_object.h"

#include <stdio.h>
#include <string.h>

void light_object_init(struct light_object *obj, struct lobj_type *type)
{
    obj->ref_count = 0;
    obj->type = type;
}
// TODO implement saturation conditions and warnings
void light_object_get(struct light_object *obj)
{
        atomic_fetch_add(&obj->ref_count, 1);
}
void light_object_put(struct light_object *obj)
{
        atomic_fetch_sub(&obj->ref_count, 1);
}

int light_object_set_name(struct light_object *obj, uint8_t *format, ...)
{
        va_list vargs;
        int retval;

        va_start(format, vargs);
        retval = light_object_set_name_va(obj, format, vargs);
        va_end(vargs);

        return retval;

}
static int light_object_set_name_va(struct light_object *obj, const uint8_t *format, va_list vargs)
{
        if(!format || !format[0]) {
                // TODO log empty name field error
                return -1;
        }
        vasnprintf(obj->id, LOM_OBJ_NAME_LENGTH, format, vargs);
        return 0;
}
static int light_object_add_internal(struct light_object *obj)
{

}
static int light_object_add_va(struct light_object *obj, struct light_object *parent,
                               uint8_t *format, va_list vargs)
{
        int retval;

        retval = light_object_set_name_va(obj, format, vargs);

        if(retval) {
            // TODO log error
            return retval;
        }

        obj->parent = parent;
        return light_object_add_internal(obj);
}
int light_object_add(struct light_object *obj, struct light_object *parent,
                            uint8_t *format, ...)
{
        va_list vargs;
        int retval;

        va_start(format, vargs);
        retval = light_object_add_va(obj, format, vargs);
        va_end(vargs);

        return retval;
}
