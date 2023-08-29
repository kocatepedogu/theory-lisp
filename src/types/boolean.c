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
#include "object-base.h"

#define ERR_OPERAND_NOT_BOOL "Boolean AND operands must be booleans"

static const object_type_t boolean_type_id = {{.destroy = destroy_boolean,
                                               .tostring = boolean_tostring,
                                               .equals = boolean_equals,
                                               .op_and = boolean_op_and,
                                               .op_or = boolean_op_or,
                                               .op_xor = boolean_op_xor,
                                               .op_not = boolean_op_not},
                                              "boolean"};

inline boolean_t boolean_value(objectptr obj) {
  assert(is_boolean(obj));
  return *(boolean_t *)obj->value;
}

bool is_boolean(objectptr obj) {
  return strcmp(obj->type_id->type_name, boolean_type_id.type_name) == 0;
}

objectptr make_boolean(boolean_t value) {
  boolean_t *bool_value = malloc(sizeof *bool_value);
  *bool_value = value;

  return object_base_new(bool_value, &boolean_type_id);
}

void destroy_boolean(objectptr obj) {
  assert(is_boolean(obj));
  free(obj->value);
}

char *boolean_tostring(objectptr obj) {
  assert(is_boolean(obj));
  return strdup(boolean_value(obj) ? "#t" : "#f");
}

bool boolean_equals(objectptr obj, objectptr other) {
  assert(is_boolean(obj));
  if (!is_boolean(other)) {
    return false;
  }

  return boolean_value(obj) == boolean_value(other);
}

objectptr boolean_op_and(objectptr obj, objectptr other) {
  assert(is_boolean(obj));
  if (!is_boolean(other)) {
    return make_error(ERR_OPERAND_NOT_BOOL);
  }

  return make_boolean(boolean_value(obj) && boolean_value(other));
}

objectptr boolean_op_or(objectptr obj, objectptr other) {
  assert(is_boolean(obj));
  if (!is_boolean(other)) {
    return make_error(ERR_OPERAND_NOT_BOOL);
  }

  return make_boolean(boolean_value(obj) || boolean_value(other));
}

objectptr boolean_op_xor(objectptr obj, objectptr other) {
  assert(is_boolean(obj));
  if (!is_boolean(other)) {
    return make_error(ERR_OPERAND_NOT_BOOL);
  }

  return make_boolean(boolean_value(obj) ^ boolean_value(other));
}

objectptr boolean_op_not(objectptr obj) {
  assert(is_boolean(obj));
  return make_boolean(!boolean_value(obj));
}
