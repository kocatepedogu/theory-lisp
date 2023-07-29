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

#include "list.h"
#include "../types/pair.h"
#include "../types/error.h"
#include "../types/null.h"

#include <assert.h>
#include <stdio.h>

object_t builtin_cons(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 2);
  return make_pair(args[0], args[1]);
}

object_t builtin_car(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);

  if (is_pair(args[0])) {
    pair_t p = pair_value(args[0]);
    return clone_object(p.first);
  }

  return make_error("car argument is not a pair.");
}

object_t builtin_cdr(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);

  if (is_pair(args[0])) {
    pair_t p = pair_value(args[0]);
    return clone_object(p.second);
  }

  return make_error("cdr argument is not a pair");
}

object_t builtin_list(size_t n, object_t *args, stack_frame_ptr sf) {
  if (n == 0) {
    return make_null();
  }

  object_t nil = make_null();
  object_t pair = make_pair(args[n-1], nil);
  if (n >= 2) {
    for (size_t i = n - 1; i != 0; i--) {
      assign_object(&pair, make_pair(args[i-1], pair));
    }
  }
  return pair;
}
