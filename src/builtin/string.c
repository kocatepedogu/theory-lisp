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
#include <limits.h>

#include "../types/integer.h"
#include "../types/string.h"
#include "../types/error.h"
#include "../types/pair.h"

objectptr builtin_strlen(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return string_length(*args);
}

objectptr builtin_strcat(size_t n, objectptr *args, stack_frame_ptr sf) {
  objectptr result = make_string("");
  for (size_t i = 0; i < n; ++i) {
    assign_object(&result, string_concat(result, args[i]));
  } 

  return result;
}

objectptr builtin_charat(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 2);
  objectptr str = args[0];
  size_t index = int_value(args[1]);
  return string_charat(str, index);
}

objectptr builtin_substr(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 3);
  objectptr str = args[0];
  size_t begin = int_value(args[1]);
  size_t end = int_value(args[2]);

  return string_substring(str, begin, end);
}

objectptr builtin_strcar(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return string_charat(*args, 0);
}

objectptr builtin_strcdr(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  return make_string(&string_value(*args)[1]);
}

objectptr builtin_newline(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 0);
  return make_string("\n");
}

objectptr builtin_tab(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 0);
  return make_string("\t");
}

objectptr builtin_backspace(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 0);
  return make_string("\b");
}

objectptr builtin_quotation_mark(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 0);
  return make_string("\"");
}

objectptr builtin_i2s(size_t n, objectptr *args, stack_frame_ptr sf) {
  /* Only ASCII characters are allowed, and '\0' is forbidden
   * as it may cause problems in internal string operations */
  for (size_t i = 0; i < n; ++i) {
    objectptr intobj = args[i];
    if (!is_integer(intobj)) {
      return make_error("i2s arguments must be integers");
    }

    integer_t intval = int_value(intobj);
    if (intval < 0 || intval > CHAR_MAX) {
      return make_error("i2s arguments must be representable with a C char");
    }

    if (intval == 0) {
      return make_error("i2s arguments cannot be 0.");
    }
  }

  /* Convert array of integer objects to an array of C characters. */
  char *result_array = malloc(n + 1);
  for (size_t i = 0; i < n; ++i) {
    result_array[i] = (char)int_value(args[i]);
  }
  result_array[n] = '\0';

  /* Construct string object from the char array */
  objectptr result = make_string(result_array);
  free(result_array);
  return result;
}

objectptr builtin_s2i(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);

  if (!is_string(*args)) {
    return make_error("s2i argument must be a string");
  }

  /* Convert internally stored char array in the string object
   * to a list of integer objects */
  listptr integers = new_list(); 
  const char *strval = string_value(*args);
  for (size_t i = 0; i < strlen(strval); ++i) {
    list_add(integers, make_integer(strval[i]));
  }

  /* Convert list of integer objects to a cons list of integer objects */
  objectptr result = internal_list_to_cons_list(integers);
  for (size_t i = 0; i < list_size(integers); ++i) {
    delete_object(list_get(integers, i));
  }
  delete_list(integers);

  /* Result is a cons list of integers */
  return result;
}
