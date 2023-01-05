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

void light_object_init(struct light_object *obj)
{
    obj->ref_count = 0;
}
