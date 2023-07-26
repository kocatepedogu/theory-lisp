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

#include "expanded_expr.h"

#include <string.h>

#include "../utils/heap-format.h"
#include "../types/error.h"
#include "../types/pair.h"
#include "expression.h"

static const char expanded_expr_name[] = "expanded_expr";

static const expr_vtable expanded_expr_vtable = {
  .destructor = destroy_expanded_expr,
  .deallocate = delete_expanded_expr,
  .to_string = expanded_expr_tostring,
  .interpret = interpret_expanded_expr
};

void construct_expanded_expr(exprptr e, exprptr inner) {
  e->data = inner;
  e->vtable = &expanded_expr_vtable;
  e->expr_name = expanded_expr_name;
}

exprptr new_expanded_expr(exprptr inner) {
  expr_t *ee = (expr_t *)malloc(sizeof(expr_t));
  construct_expanded_expr(ee, inner);
  return ee;
}

void destroy_expanded_expr(exprptr e) {
  delete_expr(e->data);  
}

void delete_expanded_expr(exprptr e) {
  destroy_expanded_expr(e);
  free(e);
}

char *expanded_expr_tostring(exprptr e) {
  char *inner = expr_tostring(e->data);
  char *result = heap_format("&%s", inner);
  free(inner);
  return result;
}

bool is_expanded_expression(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, expanded_expr_name) == 0;
}

object_t interpret_expanded_expr(exprptr e, stack_frame_ptr sf) {
  return make_error("An expanded expression can be evaluated only "
                    "as an argument in a function evaluation expression");
}


