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
#include "object.h"

static const object_vtable_t string_vtable = {
    .clone = clone_string,
    .destroy = destroy_string,
    .equals = string_equals,
    .tostring = string_tostring,
};

static const char string_type_name[] = "string";

inline bool is_string(object_t obj) {
  return strcmp(string_type_name, obj.type) == 0;
}

inline string_t string_value(object_t obj) {
  assert(is_string(obj));
  return obj.value;
}

object_t make_string(const string_t value) {
  char *copy = strdup(value);
  object_t obj;
  obj.value = copy;
  obj.type = string_type_name;
  obj.vtable = &string_vtable;
  obj.temporary = false;
  return obj;
}

object_t clone_string(object_t self) {
  assert(is_string(self));
  char *copy = strdup(self.value);
  object_t obj;
  obj.value = copy;
  obj.type = string_type_name;
  obj.vtable = &string_vtable;
  obj.temporary = false;
  return obj;
}

void destroy_string(object_t self) {
  assert(is_string(self));
  free(self.value);
}

char *string_tostring(object_t self) {
  assert(is_string(self));
  return format("\"%s\"", self.value);
}

bool string_equals(object_t self, object_t other) {
  assert(is_string(self));
  if (!is_string(other)) {
    return false;
  }

  return strcmp(self.value, other.value) == 0;
}

object_t string_length(object_t obj) {
  assert(is_string(obj));
  return make_integer((long)strlen(string_value(obj)));
}

object_t string_concat(object_t first, object_t second) {
  object_t first_str;
  if (is_string(first)) {
    first_str = clone_object(first);
  } else {
    char* str = object_tostring(first);
    first_str = make_string(str);
    free(str);
  }

  object_t second_str;
  if (is_string(second)) {
    second_str = clone_object(second);
  } else {
    char *str = object_tostring(second);
    second_str = make_string(str);
    free(str);
  }

  char *concat = append(string_value(first_str), string_value(second_str));
  object_t result = make_string(concat);
  free(concat);

  destroy_object(first_str);
  destroy_object(second_str);

  return result;
}

object_t string_charat(object_t obj, size_t index) {
  if (!is_string(obj)) {
    return make_error("charat operand is not string");
  }

  object_t strlen_obj = string_length(obj);
  if (is_error(strlen_obj)) {
    return strlen_obj;
  }

  if (index >= int_value(strlen_obj)) {
    destroy_object(strlen_obj);
    return make_error("Index is outside of string");
  }

  destroy_object(strlen_obj);
  char *result_string = "0";
  result_string[0] = string_value(obj)[index];
  return make_string(result_string);
}

object_t string_substring(object_t obj, size_t begin, size_t end) {
  if (!is_string(obj)) {
    return make_error("substring operand is not string");
  }

  object_t strlen_obj = string_length(obj);
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
  object_t result = make_string(substr);
  free(substr);
  return result;
}
