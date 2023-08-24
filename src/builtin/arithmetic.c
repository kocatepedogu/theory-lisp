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

#include "arithmetic.h"
#include <stdio.h>
#include <assert.h>
#include "../types/integer.h"
#include "../types/error.h"
#include "../interpreter/stack_frame.h"

objectptr builtin_add(size_t n, objectptr *args, stack_frame_ptr sf) {
  objectptr result = make_integer(0);
  for (size_t i = 0; i < n; ++i) {
    assign_object(&result, object_op_add(result, args[i]));
  }
  return result; 
}

objectptr builtin_mul(size_t n, objectptr *args, stack_frame_ptr sf) {
  objectptr result = make_integer(1);
  for (size_t i = 0; i < n; ++i) {
    assign_object(&result, object_op_mul(result, args[i]));
  }
  return result;
}

objectptr builtin_sub(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n >= 1);

  if (n == 1) {
    objectptr result = make_integer(0);
    assign_object(&result, object_op_sub(result, *args));
    return result;
  }

  if (n >= 2) {
    objectptr result = clone_object(*args);
    for (size_t i = 1; i < n; ++i) {
      assign_object(&result, object_op_sub(result, args[i]));
    }
    return result;
  }

  abort();
}

objectptr builtin_div(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n >= 1);

  if (n == 1) {
    objectptr result = make_integer(1);
    assign_object(&result, object_op_div(result, *args));
    return result;
  }

  if (n >= 2) {
    objectptr result = clone_object(*args);
    for (size_t i = 1; i < n; ++i) {
      assign_object(&result, object_op_div(result, args[i]));
    }
    return result;
  }

  abort();
}
