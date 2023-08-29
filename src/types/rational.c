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

#include "rational.h"

#include <assert.h>
#include <string.h>

#include "../types/boolean.h"
#include "../types/error.h"
#include "../utils/string.h"
#include "object-base.h"
#include "real.h"

static const object_type_t rational_type_id = {{.destroy = destroy_rational,
                                                .equals = rational_equals,
                                                .tostring = rational_tostring,
                                                .op_add = rational_op_add,
                                                .op_mul = rational_op_mul,
                                                .op_sub = rational_op_sub,
                                                .op_div = rational_op_div,
                                                .less = rational_less},
                                               "rational"};

bool is_rational(objectptr obj) {
  return strcmp(rational_type_id.type_name, obj->type_id->type_name) == 0;
}

rational_t rational_value(objectptr obj) {
  assert(is_rational(obj));
  return *(rational_t *)obj->value;
}

static integer_t gcd(integer_t x, integer_t y) {
  if (x == 0) {
    return y;
  }

  x = x > 0 ? x : -x;
  y = y > 0 ? y : -y;

  while (x != y) {
    if (x > y) {
      x -= y;
    } else {
      y -= x;
    }
  }

  return x;
}

objectptr make_rational(integer_t x, integer_t y) {
  rational_t *rational_value = malloc(sizeof *rational_value);
  integer_t gcd_of_x_y = gcd(x, y);
  rational_value->x = x / gcd_of_x_y;
  rational_value->y = y / gcd_of_x_y;

  return object_base_new(rational_value, &rational_type_id);
}

void destroy_rational(objectptr self) {
  assert(is_rational(self));
  free(self->value);
}

char *rational_tostring(objectptr self) {
  assert(is_rational(self));
  rational_t r = rational_value(self);
  if (r.y == 1) {
    return format("%ld", r.x);
  } else {
    return format("%ld/%ld", r.x, r.y);
  }
}

bool rational_equals(objectptr self, objectptr other) {
  assert(is_rational(self));
  rational_t self_value = rational_value(self);

  if (is_integer(other)) {
    return self_value.x == int_value(other) && self_value.y == 1;
  }

  if (is_real(other)) {
    return ((real_t)self_value.x / (real_t)self_value.y) == real_value(other);
  }

  if (is_rational(other)) {
    rational_t other_value = rational_value(other);
    return self_value.x == other_value.x && self_value.y == other_value.y;
  }

  return false;
}

objectptr rational_op_add(objectptr self, objectptr other) {
  assert(is_rational(self));
  rational_t self_value = rational_value(self);

  if (is_integer(other)) {
    return make_rational(self_value.x + int_value(other) * self_value.y,
                         self_value.y);
  }

  if (is_real(other)) {
    return make_real((real_t)self_value.x / (real_t)self_value.y +
                     real_value(other));
  }

  if (is_rational(other)) {
    rational_t other_value = rational_value(other);
    return make_rational(
        self_value.x * other_value.y + self_value.y * other_value.x,
        self_value.y * other_value.y);
  }

  return make_error("+ operand is not a number.");
}

objectptr rational_op_mul(objectptr self, objectptr other) {
  assert(is_rational(self));
  rational_t self_value = rational_value(self);

  if (is_integer(other)) {
    return make_rational(self_value.x * int_value(other), self_value.y);
  }

  if (is_real(other)) {
    return make_real((real_t)self_value.x * real_value(other) /
                     (real_t)self_value.y);
  }

  if (is_rational(other)) {
    rational_t other_value = rational_value(other);
    return make_rational(self_value.x * other_value.x,
                         self_value.y * other_value.y);
  }

  return make_error("* operand is not a number.");
}

objectptr rational_op_sub(objectptr self, objectptr other) {
  assert(is_rational(self));
  rational_t self_value = rational_value(self);

  if (is_integer(other)) {
    return make_rational(self_value.x - int_value(other) * self_value.y,
                         self_value.y);
  }

  if (is_real(other)) {
    return make_real((real_t)self_value.x / (real_t)self_value.y -
                     real_value(other));
  }

  if (is_rational(other)) {
    rational_t other_value = rational_value(other);
    return make_rational(
        self_value.x * other_value.y - self_value.y * other_value.x,
        self_value.y * other_value.y);
  }

  return make_error("- operand is not a number.");
}

objectptr rational_op_div(objectptr self, objectptr other) {
  assert(is_rational(self));
  rational_t self_value = rational_value(self);

  if (is_integer(other)) {
    return make_rational(self_value.x, self_value.y * int_value(other));
  }

  if (is_real(other)) {
    return make_real((real_t)self_value.x /
                     (real_value(other) * (real_t)self_value.y));
  }

  if (is_rational(other)) {
    rational_t other_value = rational_value(other);
    return make_rational(self_value.x * other_value.y,
                         self_value.y * other_value.x);
  }

  return make_error("/ operand is not a number.");
}

objectptr rational_less(objectptr self, objectptr other) {
  assert(is_rational(self));
  rational_t self_value = rational_value(self);

  if (is_integer(other)) {
    return make_boolean((real_t)self_value.x / (real_t)self_value.y <
                        (real_t)int_value(other));
  }

  if (is_real(other)) {
    return make_boolean((real_t)self_value.x / (real_t)self_value.y <
                        real_value(other));
  }

  if (is_rational(other)) {
    rational_t other_value = rational_value(other);
    return make_boolean(self_value.x * other_value.y <
                        other_value.x * self_value.y);
  }

  return make_error("An rational cannot be compared with a non-number value.");
}
