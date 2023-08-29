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

#include "identifier.h"
#include "../utils/string.h"

#include <string.h>


#include "expression.h"
#include "expression_base.h"

/* identifier */
typedef struct {
  char *name;
} identifier_expr;

static const char identifier_expr_name[] = "identifier_expr";

static const expr_vtable identifier_expr_vtable = {
  .destroy = destroy_identifier_expr,
  .to_string = identifier_expr_tostring,
  .interpret = interpret_identifier
};

bool is_identifier_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, identifier_expr_name) == 0;
}

exprptr new_identifier_expr(const char *name, tokenptr tkn) {
  identifier_expr *ie = malloc(sizeof *ie);
  ie->name = strdup(name);
  
  return expr_base_new(ie, &identifier_expr_vtable, identifier_expr_name, tkn);
}

void destroy_identifier_expr(exprptr self) {
  identifier_expr *ie = self->data;
  free(ie->name);
  free(ie);
}

char *identifier_expr_tostring(exprptr self) {
  identifier_expr *ie = self->data;
  return format("%s", ie->name);
}

const char *identifier_expr_get_name(exprptr self) {
  identifier_expr *ie = self->data;
  return ie->name;
}

objectptr interpret_identifier(exprptr self, stack_frame_ptr sf) {
  identifier_expr *ie = self->data;
  objectptr result = stack_frame_get_variable(sf, ie->name);
  return result;
}
