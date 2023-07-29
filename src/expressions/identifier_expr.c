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

#include "identifier_expr.h"
#include "expression.h"
#include "../utils/heap-format.h"

#include <string.h>

static const char identifier_expr_name[] = "identifier_expr";

static const expr_vtable identifier_expr_vtable = {
  .deallocate = delete_identifier_expr,
  .destructor = destroy_identifier_expr,
  .clone = clone_identifier_expr,
  .to_string = identifier_expr_tostring,
  .interpret = interpret_identifier
};

void construct_identifier_expr(exprptr e, const char *name) {
  identifier_expr *ie = malloc(sizeof(identifier_expr));
  ie->name = strdup(name);
  e->data = ie;
  e->vtable = &identifier_expr_vtable;
  e->expr_name = identifier_expr_name;
}

exprptr new_identifier_expr(const char *name) {
  expr_t *e = malloc(sizeof(expr_t));
  construct_identifier_expr(e, name);
  return e;
}

void destroy_identifier_expr(exprptr e) {
  identifier_expr *ie = e->data;
  free(ie->name);
  free(ie);
}

void delete_identifier_expr(exprptr e) {
  destroy_identifier_expr(e);
  free(e);
}

exprptr clone_identifier_expr(exprptr e) {
  identifier_expr *new_ie = (identifier_expr *)malloc(sizeof(identifier_expr));
  identifier_expr *self_ie = e->data;
  new_ie->name = strdup(self_ie->name);

  exprptr new_expr = (exprptr)malloc(sizeof(expr_t));
  new_expr->data = new_ie;
  new_expr->vtable = &identifier_expr_vtable;
  new_expr->expr_name = identifier_expr_name;
  new_expr->line_number = e->line_number;
  new_expr->column_number = e->column_number;
  return new_expr;
}

char *identifier_expr_tostring(exprptr e) {
  identifier_expr *ie = e->data;
  return heap_format("%s", ie->name);
}

bool is_identifier_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, identifier_expr_name) == 0;
}

object_t interpret_identifier(exprptr e, stack_frame_ptr sf) {
  identifier_expr *ie = e->data;
  object_t result = stack_frame_get_variable(sf, ie->name);
  return result;
}
