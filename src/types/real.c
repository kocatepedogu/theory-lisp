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

#include "real.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "object-base.h"
#include "../types/boolean.h"
#include "../types/error.h"
#include "../utils/string.h"
#include "integer.h"

static const object_type_t real_type_id = {{.destroy = destroy_real,
                                            .equals = real_equals,
                                            .tostring = real_tostring,
                                            .op_add = real_op_add,
                                            .op_mul = real_op_mul,
                                            .op_sub = real_op_sub,
                                            .op_div = real_op_div,
                                            .less = real_less},
                                            "real"};

inline bool is_real(objectptr obj) {
  return strcmp(real_type_id.type_name, obj->type_id->type_name) == 0;
}

inline real_t real_value(objectptr obj) {
  assert(is_real(obj));
  return *(real_t *)obj->value;
}

objectptr make_real(real_t value) {
  real_t *real_value = malloc(sizeof *real_value);
  *real_value = value;

  return object_base_new(real_value, &real_type_id);
}

void destroy_real(objectptr self) {
  assert(is_real(self));
  free(self->value);
}

bool real_equals(objectptr self, objectptr other) {
  assert(is_real(self));
  if (is_real(other)) {
    return real_value(self) == real_value(other);
  }

  if (is_integer(other)) {
    return real_value(self) == (real_t)int_value(other);
  }

  return false;
}

objectptr real_less(objectptr self, objectptr other) {
  assert(is_real(self));
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_boolean(self_value < (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_boolean(self_value < real_value(other));
  }

  return make_error("A real number cannot be compared with a non-number.");
}

char *real_tostring(objectptr obj) {
  return format("%f", real_value(obj));
}

objectptr real_op_add(objectptr self, objectptr other) {
  assert(is_real(self));
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_real(self_value + (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_real(self_value + real_value(other));
  }

  return make_error("+ operand is not a number.");
}

objectptr real_op_mul(objectptr self, objectptr other) {
  assert(is_real(self));
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_real(self_value * (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_real(self_value * real_value(other));
  }

  return make_error("+ operand is not a number.");
}

objectptr real_op_sub(objectptr self, objectptr other) {
  assert(is_real(self));
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_real(self_value - (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_real(self_value - real_value(other));
  }

  return make_error("+ operand is not a number.");
}

objectptr real_op_div(objectptr self, objectptr other) {
  assert(is_real(self));
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_real(self_value / (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_real(self_value / real_value(other));
  }

  return make_error("+ operand is not a number.");
}
