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

#include "boolean.h"

#include <assert.h>
#include <string.h>

#include "error.h"

#define ERR_OPERAND_NOT_BOOL "Boolean AND operands must be booleans"

static const object_vtable_t boolean_vtable = {.clone = clone_boolean,
                                               .destroy = destroy_boolean,
                                               .tostring = boolean_tostring,
                                               .equals = boolean_equals,
                                               .op_and = boolean_op_and,
                                               .op_or = boolean_op_or,
                                               .op_xor = boolean_op_xor,
                                               .op_not = boolean_op_not};

static const char boolean_type_name[] = "boolean";

inline boolean_t boolean_value(object_t obj) {
  assert(is_boolean(obj));
  return *(boolean_t *)obj.value;
}

inline bool is_boolean(object_t obj) {
  return strcmp(obj.type, boolean_type_name) == 0;
}

object_t make_boolean(boolean_t value) {
  boolean_t *bool_value = (boolean_t *)malloc(sizeof(boolean_t));
  *bool_value = value;

  object_t obj;
  obj.value = bool_value;
  obj.type = boolean_type_name;
  obj.vtable = &boolean_vtable;
  obj.temporary = false;
  return obj;
}

object_t clone_boolean(object_t self) {
  assert(is_boolean(self));

  boolean_t *bool_value = (boolean_t *)malloc(sizeof(boolean_t));
  *bool_value = *(boolean_t *)self.value;
  object_t obj;

  obj.value = bool_value;
  obj.type = boolean_type_name;
  obj.vtable = &boolean_vtable;
  obj.temporary = false;
  return obj;
}

void destroy_boolean(object_t obj) {
  assert(is_boolean(obj));
  free(obj.value);
}

char *boolean_tostring(object_t obj) {
  assert(is_boolean(obj));
  return strdup(boolean_value(obj) ? "#t" : "#f");
}

bool boolean_equals(object_t obj, object_t other) {
  assert(is_boolean(obj));
  if (!is_boolean(other)) {
    return false;
  }

  return boolean_value(obj) == boolean_value(other);
}

object_t boolean_op_and(object_t obj, object_t other) {
  assert(is_boolean(obj));
  if (!is_boolean(other)) {
    return make_error(ERR_OPERAND_NOT_BOOL);
  }

  return make_boolean(boolean_value(obj) && boolean_value(other));
}

object_t boolean_op_or(object_t obj, object_t other) {
  assert(is_boolean(obj));
  if (!is_boolean(other)) {
    return make_error(ERR_OPERAND_NOT_BOOL);
  }

  return make_boolean(boolean_value(obj) || boolean_value(other));
}

object_t boolean_op_xor(object_t obj, object_t other) {
  assert(is_boolean(obj));
  if (!is_boolean(other)) {
    return make_error(ERR_OPERAND_NOT_BOOL);
  }

  return make_boolean(boolean_value(obj) ^ boolean_value(other));
}

object_t boolean_op_not(object_t obj) {
  assert(is_boolean(obj));
  return make_boolean(!boolean_value(obj));
}
