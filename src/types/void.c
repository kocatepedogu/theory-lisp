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

#include "object-base.h"
#include "error.h"
#include "../utils/string.h"

static const object_type_t void_type_id = {{
    .destroy = destroy_void,
    .tostring = void_tostring,
    .equals = void_equals},
    "void"};

bool is_void(objectptr obj) {
  return strcmp(void_type_id.type_name, obj->type_id->type_name) == 0;
}

objectptr make_void(void) {
  return object_base_new(NULL, &void_type_id);
}

void destroy_void(objectptr self) { assert(is_void(self)); }

char *void_tostring(objectptr self) {
  assert(is_void(self));
  return strdup("(void)");
}

bool void_equals(objectptr self, objectptr other) {
  assert(is_void(self));
  if (!is_void(other)) {
    return false;
  }
  return true;
}
