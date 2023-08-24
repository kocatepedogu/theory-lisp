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

#include "io.h"

#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "../types/void.h"
#include "../types/string.h"
#include "../types/error.h"
#include "../types/integer.h"

objectptr builtin_system(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);

  if (!is_string(*args)) {
    return make_error("system argument is not a string");
  }

  return make_integer(system(string_value(*args)));
}

objectptr builtin_display(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n >= 1);

  for (size_t i = 0; i < n; ++i) {
    char *str = NULL;

    if (is_string(args[i])) {
      str = string_value(args[i]);
      printf("%s", str);
    } else {
      str = object_tostring(args[i]);
      printf("%s", str);
      free(str);
    }
  }

  return make_void();
}

objectptr builtin_getchar(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 0);
  if (feof(stdin)) {
    return make_void();
  }

  int result = getchar();
  if (result == EOF) {
    return make_error("getchar failed");
  }

  return make_integer(result);
}

objectptr builtin_putchar(size_t n, objectptr *args, stack_frame_ptr sf) {
  for (size_t i = 0; i < n; i++) {
    if (is_integer(args[i])) {
      int result = putchar((int)int_value(args[i]));
      if (result == EOF) {
        return make_error("Cannot write to standard output");
      }
    } else {
      return make_error("putchar argument is not an integer.");
    }
  }

  return make_void();
}

objectptr builtin_current_seconds(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 0);
  return make_integer(time(NULL));
}
