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

#include "interpreter.h"

#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "stack_frame.h"
#include "variable.h"
#include "../scanner/scanner.h"
#include "../parser/parser.h"
#include "../types/error.h"
#include "../types/void.h"

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

static exprptr get_expression(listptr parse_tree, size_t i, bool verbose) {
  exprptr expr = list_get(parse_tree, i);
  if (verbose) {
    char *expr_str = expr_tostring(expr);
    printf("Expression: %s\n", expr_str);
    free(expr_str);
  }

  return expr;
}

static object_t evaluate(exprptr e, stack_frame_ptr sf, bool verbose, bool quiet) {
  object_t result = interpret_expr(e, sf);
  char *result_str = object_tostring(result);

  if (verbose) {
    printf("Result: ");
  }

  bool show_result = !quiet && !is_void(result);
  if (!is_exit(result) && (is_error(result) || show_result || verbose)) {
    printf("%s\n", result_str);
  }

  free(result_str);
  return result;
}

object_t interpreter(listptr parse_tree, bool verbose, bool quiet, stack_frame_ptr sf) {
  object_t result = make_void();

  for (size_t i = 0; !is_error(result) && i < list_size(parse_tree); i++) {
    exprptr e = get_expression(parse_tree, i, verbose);
    assign_object(&result, evaluate(e, sf, verbose, quiet));
  }

  return result;
}

void repl(stack_frame_ptr sf) {
  char line[LINE_MAX];

  while (!feof(stdin)) {
    if (!fgets(line, LINE_MAX, stdin)) {
      if (!feof(stdin)) {
        fprintf(stderr, 
            "Cannot read the next line from stdin."
            "The length of a line must not exceed %d characters",
            LINE_MAX);
      }

      return;
    }

    tokenstreamptr tkns = scanner(line);
    listptr parse_tree = parser(tkns);
    delete_tokenstream(tkns);
    
    if (parse_tree) {
      object_t result = interpreter(parse_tree, false, false, sf);
      delete_parse_tree(parse_tree);

      bool exit = is_exit(result);
      destroy_object(result);
      if (exit) {
        return;
      }
    }
  }
}
