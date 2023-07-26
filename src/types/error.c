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

#include "error.h"

#include <stdarg.h>
#include <string.h>

#include "../utils/heap-format.h"

static const object_vtable_t error_vtable = {.copy = copy_error,
                                             .destroy = destroy_error,
                                             .equals = error_equals,
                                             .tostring = error_tostring};

static const char error_type_name[] = "error";

object_t make_error(error_t err, ...) {
  va_list args;
  va_start(args, err);
  char *formatted_string = vheap_format(err, args);
  va_end(args);

  object_t obj;
  obj.type = error_type_name;
  obj.value = formatted_string;
  obj.vtable = &error_vtable;
  obj.temporary = false;
  return obj;
}

object_t copy_error(object_t other) {
  object_t obj;
  obj.type = error_type_name;
  obj.value = strdup(other.value);
  obj.vtable = &error_vtable;
  obj.temporary = false;
  return obj;
}

void destroy_error(object_t err) { free(err.value); }

bool error_equals(object_t err, object_t other) {
  return strcmp(err.value, other.value) == 0;
}

char *error_tostring(object_t err) { return strdup(err.value); }

bool is_error(object_t err) { return strcmp(error_type_name, err.type) == 0; }
