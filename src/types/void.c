/*
 *
 * Copyright 2023 Doğu Kocatepe
 * This file is part of Theory Lisp.

 * Theory Lisp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Theory Lisp is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.

 * You should have received a copy of the GNU General Public License along
 * with Theory Lisp. If not, see <https://www.gnu.org/licenses/>.
 */

#include "void.h"

#include <assert.h>
#include <string.h>

#include "error.h"
#include "../utils/string.h"

static const object_vtable_t void_vtable = {
    .clone = clone_void,
    .destroy = destroy_void,
    .tostring = void_tostring,
    .equals = void_equals,
};

static const char void_type_name[] = "void";

inline bool is_void(object_t obj) {
  return strcmp(void_type_name, obj.type) == 0;
}

object_t make_void(void) {
  object_t obj;
  obj.type = void_type_name;
  obj.value = NULL;
  obj.vtable = &void_vtable;
  obj.temporary = false;
  return obj;
}

void destroy_void(object_t self) { assert(is_void(self)); }

object_t clone_void(object_t self) {
  assert(is_void(self));
  return self;
}

char *void_tostring(object_t self) {
  assert(is_void(self));
  return strdup("#<void>");
}

bool void_equals(object_t self, object_t other) {
  assert(is_void(self));
  if (!is_void(other)) {
    return false;
  }
  return true;
}
