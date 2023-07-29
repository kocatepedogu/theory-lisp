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

#include "string.h"

#include <assert.h>

#include "../types/integer.h"
#include "../types/string.h"
#include "../types/error.h"

object_t builtin_strlen(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);
  return string_length(*args);
}

object_t builtin_strcat(size_t n, object_t *args, stack_frame_ptr sf) {
  object_t result = make_string("");
  for (size_t i = 0; i < n; i++) {
    assign_object(&result, string_concat(result, args[i]));
  } 

  return result;
}

object_t builtin_charat(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 2);
  object_t str = args[0];
  size_t index = int_value(args[1]);
  return string_charat(str, index);
}

object_t builtin_substr(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 3);
  object_t str = args[0];
  size_t begin = int_value(args[1]);
  size_t end = int_value(args[2]);

  return string_substring(str, begin, end);
}

object_t builtin_strcar(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);
  return string_charat(*args, 0);
}

object_t builtin_strcdr(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_string(&string_value(*args)[1]);
}
