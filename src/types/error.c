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

#include "error.h"

#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "../utils/string.h"
#include "object-base.h"

static const object_type_t error_type_id = {{.destroy = destroy_error,
                                             .equals = error_equals,
                                             .tostring = error_tostring},
                                            "error"};

static const char normal_exit_message[] = "NORMAL_EXIT";

bool is_error(objectptr obj) {
  return strcmp(error_type_id.type_name, obj->type_id->type_name) == 0;
}

bool is_exit(objectptr obj) {
  if (is_error(obj)) {
    return strcmp(obj->value, normal_exit_message) == 0;
  }

  return false;
}

objectptr make_error(error_t err, ...) {
  va_list args;
  va_start(args, err);
  char *formatted_string = vformat(err, args);
  va_end(args);

  return object_base_new(formatted_string, &error_type_id);
}

objectptr make_exit(void) { return make_error("%s", normal_exit_message); }

void destroy_error(objectptr self) {
  assert(is_error(self));
  free(self->value);
}

bool error_equals(objectptr self, objectptr other) {
  assert(is_error(self));
  if (!is_error(other)) {
    return false;
  }

  return strcmp(self->value, other->value) == 0;
}

char *error_tostring(objectptr self) {
  assert(is_error(self));
  return strdup(self->value);
}
