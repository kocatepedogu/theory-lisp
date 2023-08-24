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

#include "../scanner/scanner.h"
#include "../parser/parser.h"
#include "../types/void.h"
#include "../types/string.h"
#include "../types/error.h"
#include "../types/integer.h"
#include "../utils/file.h"
#include "../utils/string.h"

/* 
 * This directory is normally given by autotools depending 
 * on the actual installation prefix
 */
#ifndef LIBRARY_DIR
#define LIBRARY_DIR "/usr/local/lib/tlisp"
#endif

char *read_code(char *file_name) {
  /* Try to read the file at the given path */
  char *code = read_file(file_name);
  if (code) {
    return code;
  }

  /* If the file at the given path is not accessible, search
   * for the file in the Theory Lisp library directory */
  char *library_file_name = format("%s/%s", LIBRARY_DIR, file_name);
  code = read_file(library_file_name);
  if (code) {
    free(library_file_name);
    return code;
  }

  /* If the file is not found, return NULL */
  free(library_file_name);
  return NULL;
}

objectptr builtin_load(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);

  /* Obtain file name */
  objectptr file_name_obj = *args;
  if (!is_string(file_name_obj)) {
    return make_error("File name must be a string"); 
  }

  char *file_name = string_value(file_name_obj);

  /* Check if the file is already included */
  char *include_guard = format("%s_included", file_name);
  if (stack_frame_defined(sf, include_guard)) {
    free(include_guard);
    return make_void();
  }

  /* Read file */
  char *code = read_code(file_name);
  if (!code) {
    free(include_guard);
    return make_error("%s is not accessible.", file_name);
  }

  /* Define an include guard variable in global scope to prevent
   * multiple inclusions of the same file */
  stack_frame_set_global_variable(sf, include_guard, move(make_void()));
  free(include_guard);

  /* Parse included file */
  tokenstreamptr tkns = scanner(code);
  listptr parse_tree = parser(tkns, sf);
  if (!parse_tree) {
    delete_tokenstream(tkns);
    free(code);
    return make_error("An error occured in the included file");
  }

  /* Execute included file in the same global scope */
  objectptr result = interpreter(parse_tree, false, true, sf);
  delete_object(result);

  delete_tokenstream(tkns);
  delete_parse_tree(parse_tree);
  free(code);

  return make_void();
}

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
