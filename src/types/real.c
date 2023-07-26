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

#include "real.h"

#include <stdlib.h>
#include <string.h>

#include "../types/boolean.h"
#include "../types/error.h"
#include "../utils/heap-format.h"
#include "integer.h"

static const object_vtable_t real_vtable = {.copy = copy_real,
                                            .destroy = destroy_real,
                                            .equals = real_equals,
                                            .tostring = real_tostring,
                                            .op_add = real_op_add,
                                            .op_mul = real_op_mul,
                                            .op_sub = real_op_sub,
                                            .op_div = real_op_div,
                                            .less = real_less};

static const char real_type_name[] = "real";

/* Real constructor */
object_t make_real(real_t value) {
  real_t *real_value = (real_t *)malloc(sizeof(real_t));
  *real_value = value;
  object_t obj;
  obj.value = real_value;
  obj.type = real_type_name;
  obj.vtable = &real_vtable;
  obj.temporary = false;
  return obj;
}

object_t copy_real(object_t other) {
  real_t *real_value = (real_t *)malloc(sizeof(real_t));
  *real_value = *(real_t *)other.value;
  object_t obj;
  obj.value = real_value;
  obj.type = real_type_name;
  obj.vtable = &real_vtable;
  obj.temporary = false;
  return obj;
}

/* Real destructor */
void destroy_real(object_t obj) { free(obj.value); }

/* Real equals implementation */
bool real_equals(object_t val, object_t other) {
  if (is_real(other)) {
    return real_value(val) == real_value(other);
  }

  if (is_integer(other)) {
    return real_value(val) == (real_t)int_value(other);
  }

  return false;
}

/* Real less implementation */
object_t real_less(object_t self, object_t other) {
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_boolean(self_value < (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_boolean(self_value < real_value(other));
  }

  return make_error("A real number cannot be compared with a non-number.");
}

/* Real tostring implementation */
char *real_tostring(object_t obj) { return heap_format("%f", real_value(obj)); }

/* Real "+" implementation */
object_t real_op_add(object_t self, object_t other) {
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_real(self_value + (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_real(self_value + real_value(other));
  }

  return make_error("+ operand is not a number.");
}

/* Real "*" implementation */
object_t real_op_mul(object_t self, object_t other) {
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_real(self_value * (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_real(self_value * real_value(other));
  }

  return make_error("+ operand is not a number.");
}

/* Real "-" implementation */
object_t real_op_sub(object_t self, object_t other) {
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_real(self_value - (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_real(self_value - real_value(other));
  }

  return make_error("+ operand is not a number.");
}

/* Real "/" implementation */
object_t real_op_div(object_t self, object_t other) {
  real_t self_value = real_value(self);

  if (is_integer(other)) {
    return make_real(self_value / (real_t)int_value(other));
  }
  if (is_real(other)) {
    return make_real(self_value / real_value(other));
  }

  return make_error("+ operand is not a number.");
}

inline bool is_real(object_t obj) {
  return strcmp(real_type_name, obj.type) == 0;
}

inline real_t real_value(object_t obj) { return *(real_t *)obj.value; }
