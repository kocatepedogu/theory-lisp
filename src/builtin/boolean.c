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

#include "boolean.h"

#include <assert.h>

#include "../types/boolean.h"

object_t builtin_and(size_t n, object_t *args, stack_frame_ptr sf) {
  object_t result = make_boolean(true);
  for (size_t i = 0; i < n; i++) {
    assign_object(&result, object_op_and(result, args[i]));
  }
  return result;
}

object_t builtin_or(size_t n, object_t *args, stack_frame_ptr sf) {
  object_t result = make_boolean(false);
  for (size_t i = 0; i < n; i++) {
    assign_object(&result, object_op_or(result, args[i]));  
  }
  return result;
}

object_t builtin_xor(size_t n, object_t *args, stack_frame_ptr sf) {
  object_t result = make_boolean(false);
  for (size_t i = 0; i < n; i++) {
    assign_object(&result, object_op_xor(result, args[i]));
  }
  return result;
}

object_t builtin_not(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);
  return object_op_not(*args);
}
