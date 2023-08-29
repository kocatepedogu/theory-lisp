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

#include "../utils/string.h"
#include "error.h"
#include "object-base.h"

static const object_type_t null_type_id = {
    {.destroy = destroy_null, 
     .tostring = null_tostring,
     .equals = null_equals},
    "null"};

bool is_null(objectptr obj) {
  return strcmp(null_type_id.type_name, obj->type_id->type_name) == 0;
}

objectptr make_null(void) { return object_base_new(NULL, &null_type_id); }

void destroy_null(objectptr self) { assert(is_null(self)); }

char *null_tostring(objectptr self) {
  assert(is_null(self));
  return strdup("null");
}

bool null_equals(objectptr self, objectptr other) {
  assert(is_null(self));
  if (!is_null(other)) {
    return false;
  }
  return true;
}
