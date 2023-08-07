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

#include "integer.h"

#include <assert.h>
#include <string.h>

#include "../types/boolean.h"
#include "../types/error.h"
#include "../utils/string.h"
#include "object-base.h"
#include "real.h"

static const object_type_t integer_type_id = {{.destroy = destroy_integer,
                                               .equals = integer_equals,
                                               .tostring = integer_tostring,
                                               .op_add = integer_op_add,
                                               .op_mul = integer_op_mul,
                                               .op_sub = integer_op_sub,
                                               .op_div = integer_op_div,
                                               .less = integer_less},
                                               "integer"};

inline bool is_integer(objectptr obj) {
  return strcmp(integer_type_id.type_name, obj->type_id->type_name) == 0;
}

inline integer_t int_value(objectptr obj) {
  assert(is_integer(obj));
  return *(integer_t *)obj->value;
}

objectptr make_integer(integer_t value) {
  integer_t *int_value = malloc(sizeof *int_value);
  *int_value = value;

  return object_base_new(int_value, &integer_type_id);
}

void destroy_integer(objectptr self) {
  assert(is_integer(self));
  free(self->value);
}

char *integer_tostring(objectptr self) {
  assert(is_integer(self));
  return format("%ld", int_value(self));
}

bool integer_equals(objectptr self, objectptr other) {
  assert(is_integer(self));
  if (is_integer(other)) {
    return int_value(self) == int_value(other);
  }

  if (is_real(other)) {
    return (real_t)int_value(self) == real_value(other);
  }

  return false;
}

objectptr integer_op_add(objectptr self, objectptr other) {
  assert(is_integer(self));
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_integer(self_value + int_value(other));
  }

  if (is_real(other)) {
    return make_real((real_t)self_value + real_value(other));
  }

  return make_error("+ operand is not a number.");
}

objectptr integer_op_mul(objectptr self, objectptr other) {
  assert(is_integer(self));
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_integer(self_value * int_value(other));
  }
  if (is_real(other)) {
    return make_real((real_t)self_value * real_value(other));
  }

  return make_error("* operand is not a number.");
}

objectptr integer_op_sub(objectptr self, objectptr other) {
  assert(is_integer(self));
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_integer(self_value - int_value(other));
  }
  if (is_real(other)) {
    return make_real((real_t)self_value - real_value(other));
  }

  return make_error("- operand is not a number.");
}

objectptr integer_op_div(objectptr self, objectptr other) {
  assert(is_integer(self));
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_integer(self_value / int_value(other));
  }
  if (is_real(other)) {
    return make_real((real_t)self_value / real_value(other));
  }

  return make_error("/ operand is not a number.");
}

objectptr integer_less(objectptr self, objectptr other) {
  assert(is_integer(self));
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_boolean(self_value < int_value(other));
  }
  if (is_real(other)) {
    return make_boolean((real_t)self_value < real_value(other));
  }

  return make_error("An integer cannot be compared with a non-number value.");
}
