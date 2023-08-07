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

#include "object.h"
#include "../types/error.h"
#include "../types/null.h"
#include "../types/void.h"
#include "../types/integer.h"
#include "../types/real.h"
#include "../types/string.h"
#include "../types/pair.h"
#include "../types/boolean.h"
#include "../types/procedure.h"

#include <assert.h>
#include <stdio.h>

objectptr builtin_equals(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n >= 1);
  for (size_t i = 1; i < n; ++i) {
    if (!object_equals(args[0], args[i])) {
      return make_boolean(false);
    }
  }
  return make_boolean(true);
}

objectptr builtin_not_equals(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n >= 1);
  objectptr eq = builtin_equals(n, args, sf);
  objectptr result = make_boolean(!boolean_value(eq));
  delete_object(eq);
  return result;
}

objectptr builtin_less(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 2);
  return object_less(args[0], args[1]);
}

objectptr builtin_less_or_eq(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 2);
  objectptr less = object_less(args[0], args[1]);
  objectptr less_or_equal = make_boolean(boolean_value(less) || object_equals(args[0], args[1]));
  delete_object(less);
  return less_or_equal;
}

objectptr builtin_greater(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 2);
  objectptr less_or_equal = builtin_less_or_eq(n, args, sf);
  objectptr greater = object_op_not(less_or_equal);
  delete_object(less_or_equal);
  return greater;
}

objectptr builtin_greater_or_eq(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 2);
  objectptr less = builtin_less(n, args, sf);
  objectptr greater_or_eq = object_op_not(less);
  delete_object(less);
  return greater_or_eq;
}

objectptr builtin_is_null(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_boolean(is_null(*args));
}

objectptr builtin_is_void(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_boolean(is_void(*args));
}

objectptr builtin_is_boolean(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_boolean(is_boolean(*args));
}

objectptr builtin_is_integer(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_boolean(is_integer(*args));
}

objectptr builtin_is_real(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_boolean(is_real(*args));
}

objectptr builtin_is_number(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_boolean(is_integer(*args) || is_real(*args));
}

objectptr builtin_is_string(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_boolean(is_string(*args));
}

objectptr builtin_is_pair(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_boolean(is_pair(*args));
}

objectptr builtin_is_procedure(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_boolean(is_procedure(*args));
}

