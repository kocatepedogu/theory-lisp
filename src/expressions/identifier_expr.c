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

/* identifier */
typedef struct {
  char *name;
} identifier_expr;

static const char identifier_expr_name[] = "identifier_expr";

static const expr_vtable identifier_expr_vtable = {
  .deallocate = delete_identifier_expr,
  .clone = clone_identifier_expr,
  .to_string = identifier_expr_tostring,
  .interpret = interpret_identifier
};

exprptr new_identifier_expr(const char *name) {
  identifier_expr *ie = malloc(sizeof(identifier_expr));
  ie->name = strdup(name);
  
  expr_t *e = malloc(sizeof(expr_t));
  e->data = ie;
  e->vtable = &identifier_expr_vtable;
  e->expr_name = identifier_expr_name;
  return e;
}

void delete_identifier_expr(exprptr self) {
  identifier_expr *ie = self->data;
  free(ie->name);
  free(ie);
  free(self);
}

exprptr clone_identifier_expr(exprptr self) {
  identifier_expr *new_ie = (identifier_expr *)malloc(sizeof(identifier_expr));
  identifier_expr *self_ie = self->data;
  new_ie->name = strdup(self_ie->name);

  return base_clone(self, new_ie);
}

char *identifier_expr_tostring(exprptr self) {
  identifier_expr *ie = self->data;
  return heap_format("%s", ie->name);
}

const char *identifier_expr_get_name(exprptr self) {
  identifier_expr *ie = self->data;
  return ie->name;
}

bool is_identifier_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, identifier_expr_name) == 0;
}

object_t interpret_identifier(exprptr self, stack_frame_ptr sf) {
  identifier_expr *ie = self->data;
  object_t result = stack_frame_get_variable(sf, ie->name);
  return result;
}
