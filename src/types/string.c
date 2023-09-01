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

#include "string.h"

#include <assert.h>
#include <string.h>

#include "../types/error.h"
#include "../types/integer.h"
#include "../utils/string.h"
#include "object-base.h"
#include "object.h"

static const object_type_t string_type_id = {{
    .destroy = destroy_string,
    .equals = string_equals,
    .tostring = string_tostring},
    "string"};

bool is_string(objectptr obj) {
  return strcmp(string_type_id.type_name, obj->type_id->type_name) == 0;
}

string_t string_value(objectptr obj) {
  assert(is_string(obj));
  return obj->value;
}

objectptr make_string(const string_t value) {
  return object_base_new(strdup(value), &string_type_id);
}

void destroy_string(objectptr self) {
  assert(is_string(self));
  free(self->value);
}

char *string_tostring(objectptr self) {
  assert(is_string(self));
  return format("\"%s\"", self->value);
}

bool string_equals(objectptr self, objectptr other) {
  assert(is_string(self));
  if (!is_string(other)) {
    return false;
  }

  return strcmp(self->value, other->value) == 0;
}

objectptr string_length(objectptr obj) {
  assert(is_string(obj));
  return make_integer((long)strlen(string_value(obj)));
}

objectptr string_concat(objectptr first, objectptr second) {
  objectptr first_str = NULL;
  if (is_string(first)) {
    first_str = clone_object(first);
  } else {
    char *str = object_tostring(first);
    first_str = make_string(str);
    free(str);
  }

  objectptr second_str = NULL;
  if (is_string(second)) {
    second_str = clone_object(second);
  } else {
    char *str = object_tostring(second);
    second_str = make_string(str);
    free(str);
  }

  char *concat = append(string_value(first_str), string_value(second_str));
  objectptr result = make_string(concat);
  free(concat);

  delete_object(first_str);
  delete_object(second_str);

  return result;
}

objectptr string_charat(objectptr obj, size_t index) {
  if (!is_string(obj)) {
    return make_error("charat operand is not string");
  }

  objectptr strlen_obj = string_length(obj);
  if (is_error(strlen_obj)) {
    return strlen_obj;
  }

  if (index >= int_value(strlen_obj)) {
    delete_object(strlen_obj);
    return make_error("Index is outside of string");
  }

  delete_object(strlen_obj);
  char result_string[] = "0";
  result_string[0] = string_value(obj)[index];
  return make_string(result_string);
}

objectptr string_substring(objectptr obj, size_t begin, size_t end) {
  if (!is_string(obj)) {
    return make_error("substring operand is not string");
  }

  objectptr strlen_obj = string_length(obj);
  if (is_error(strlen_obj)) {
    return strlen_obj;
  }

  if (begin >= int_value(strlen_obj)) {
    return make_error("Begin index is outside of string");
  }

  if (end >= int_value(strlen_obj)) {
    return make_error("End index is outside of string");
  }

  if (begin > end) {
    return make_error("Begin index is larger than end index");
  }

  char *substr = malloc(end - begin + 1);
  strncpy(substr, &string_value(obj)[begin], end - begin);
  objectptr result = make_string(substr);
  free(substr);
  return result;
}
