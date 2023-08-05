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

#include "null.h"

#include <assert.h>
#include <string.h>

#include "error.h"
#include "../utils/string.h"

static const object_vtable_t null_vtable = {
    .clone = clone_null,
    .destroy = destroy_null,
    .tostring = null_tostring,
    .equals = null_equals,
};

static const char null_type_name[] = "null";

inline bool is_null(object_t obj) {
  return strcmp(null_type_name, obj.type) == 0;
}

object_t make_null(void) {
  object_t obj;
  obj.type = null_type_name;
  obj.value = NULL;
  obj.vtable = &null_vtable;
  obj.temporary = false;
  return obj;
}

void destroy_null(object_t self) { assert(is_null(self)); }

object_t clone_null(object_t self) {
  assert(is_null(self));
  return self;
}

char *null_tostring(object_t self) {
  assert(is_null(self));
  return strdup("null");
}

bool null_equals(object_t self, object_t other) {
  assert(is_null(self));
  if (!is_null(other)) {
    return false;
  }
  return true;
}
