/*
 *
 * Copyright 2023 Doğu Kocatepe
 * This file is part of Theory Lisp.

 * Theory Lisp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Theory Lisp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along with Theory Lisp.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "void.h"

#include <string.h>

#include "error.h"

static const object_vtable_t void_vtable = {
    .copy = copy_void,
    .destroy = destroy_void,
    .tostring = void_tostring,
    .equals = void_equals,
};

static const char void_type_name[] = "void";

object_t make_void(void) {
  object_t obj;
  obj.type = void_type_name;
  obj.value = NULL;
  obj.vtable = &void_vtable;
  obj.temporary = false;
  return obj;
}

void destroy_void(object_t obj) {}

object_t copy_void(object_t other) { return other; }

char *void_tostring(object_t obj) { return strdup("null"); }

bool void_equals(object_t obj, object_t other) { return true; }

bool is_void(object_t obj) { return strcmp(void_type_name, obj.type) == 0; }
