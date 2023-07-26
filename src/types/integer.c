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

#include "integer.h"

#include <string.h>

#include "../types/boolean.h"
#include "../types/error.h"
#include "../utils/heap-format.h"
#include "real.h"

static const object_vtable_t integer_vtable = {.copy = copy_integer,
                                               .destroy = destroy_integer,
                                               .equals = integer_equals,
                                               .tostring = integer_tostring,
                                               .op_add = integer_op_add,
                                               .op_mul = integer_op_mul,
                                               .op_sub = integer_op_sub,
                                               .op_div = integer_op_div,
                                               .less = integer_less};

static const char integer_type_name[] = "integer";

/* Integer constructor */
object_t make_integer(integer_t value) {
  integer_t *int_value = (integer_t *)malloc(sizeof(integer_t));
  *int_value = value;
  object_t obj;
  obj.value = int_value;
  obj.type = integer_type_name;
  obj.vtable = &integer_vtable;
  obj.temporary = false;
  return obj;
}

object_t copy_integer(object_t other) {
  integer_t *int_value = (integer_t *)malloc(sizeof(integer_t));
  *int_value = *(integer_t *)other.value;
  object_t obj;
  obj.value = int_value;
  obj.type = integer_type_name;
  obj.vtable = &integer_vtable;
  obj.temporary = false;
  return obj;
}

/* Integer destructor */
void destroy_integer(object_t val) { free(val.value); }

/* Integer tostring implementation */
char *integer_tostring(object_t obj) {
  return heap_format("%ld", int_value(obj));
}

/* Integer equals implementation */
bool integer_equals(object_t val, object_t other) {
  if (is_integer(other)) {
    return int_value(val) == int_value(other);
  }

  if (is_real(other)) {
    return (real_t)int_value(val) == real_value(other);
  }

  return false;
}

/* Integer "+" implementation */
object_t integer_op_add(object_t self, object_t other) {
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_integer(self_value + int_value(other));
  } 
  
  if (is_real(other)) {
    return make_real((real_t)self_value + real_value(other));
  }

  return make_error("+ operand is not a number.");
}

/* Integer "*" implementation */
object_t integer_op_mul(object_t self, object_t other) {
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_integer(self_value * int_value(other));
  } 
  if (is_real(other)) {
    return make_real((real_t)self_value * real_value(other));
  }

  return make_error("* operand is not a number.");
}

/* Integer "-" implementation */
object_t integer_op_sub(object_t self, object_t other) {
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_integer(self_value - int_value(other));
  }
  if (is_real(other)) {
    return make_real((real_t)self_value - real_value(other));
  }

  return make_error("- operand is not a number.");
}

/* Integer "/" implementation */
object_t integer_op_div(object_t self, object_t other) {
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_integer(self_value / int_value(other));
  } 
  if (is_real(other)) {
    return make_real((real_t)self_value / real_value(other));
  }

  return make_error("/ operand is not a number.");
}

/* Integer "<" implementation */
object_t integer_less(object_t self, object_t other) {
  integer_t self_value = int_value(self);

  if (is_integer(other)) {
    return make_boolean(self_value < int_value(other));
  } 
  if (is_real(other)) {
    return make_boolean((real_t)self_value < real_value(other));
  }

  return make_error("An integer cannot be compared with a non-number value.");
}

inline bool is_integer(object_t obj) {
  return strcmp(integer_type_name, obj.type) == 0;
}

inline integer_t int_value(object_t obj) { return *(integer_t *)obj.value; }
