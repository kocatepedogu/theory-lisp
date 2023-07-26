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

#include "interpreter.h"

#include <stdio.h>

#include "stack_frame.h"
#include "variable.h"
#include "../types/error.h"

static exprptr get_expression(list *parse_tree, size_t i, bool verbose) {
  exprptr expr = list_get(parse_tree, i);
  if (verbose) {
    char *expr_str = expr_tostring(expr);
    printf("Expression: %s\n", expr_str);
    free(expr_str);
  }

  return expr;
}

static bool evaluate(exprptr e, stack_frame_ptr sf, bool verbose) {
  object_t result = interpret_expr(e, sf);
  bool error = is_error(result);
  char *result_str = object_tostring(result);
  destroy_object(result);
  if (verbose && !error) {
    printf("Result: ");
  }
  puts(result_str);
  free(result_str);
  return error;
}

static void print_variables(stack_frame_ptr sf, bool verbose) {
  if (verbose) {
    list *vars = &sf->local_variables;
    size_t length = list_size(vars);

    if (length == 0) {
      puts("No global variables");
    } else {
      puts("Global variables:");
    }

    for (size_t i = 0; i < length; i++) {
      variable_t *var = list_get(vars, i);
      char *obj_str = object_tostring(var->value);
      printf("  %s: %s = %s\n", var->name, var->value.type, obj_str);
      free(obj_str);
    }

    puts("");
  }
}

void interpreter(list *parse_tree, bool verbose) {
  stack_frame_t global_frame;
  construct_stack_frame(&global_frame, NULL);

  bool error = false;
  for (size_t i = 0; !error && i < list_size(parse_tree); i++) {
    exprptr e = get_expression(parse_tree, i, verbose);
    error = evaluate(e, &global_frame, verbose);
    print_variables(&global_frame, verbose);
  }

  destroy_stack_frame(&global_frame);
}
