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

#include "internal.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "object-base.h"

static const object_type_t internal_type_id = {{
    .destroy = destroy_internal,
    .tostring = internal_tostring,
    .equals = internal_equals,
    .get_raw_data = internal_get_raw_data},
    "internal"};

inline bool is_internal(objectptr obj) {
  return strcmp(internal_type_id.type_name, obj->type_id->type_name) == 0;
}

objectptr make_internal(void *ptr) {
  return object_base_new(ptr, &internal_type_id);
}

void destroy_internal(objectptr self) {}

char *internal_tostring(objectptr self) {
  return strdup("");
}

bool internal_equals(objectptr self, objectptr other) {
  return false;
}

void *internal_get_raw_data(objectptr self) {
  return self->value;
}
