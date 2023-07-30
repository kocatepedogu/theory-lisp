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

#include "definition_expr.h"
#include "expression.h"
#include <string.h>
#include <assert.h>
#include "../utils/heap-format.h"
#include "../scanner/scanner.h"
#include "../parser/parser.h"
#include "../types/void.h"
#include "../types/error.h"

#define ERR_DEF_NAME_IS_NOT_ID \
  "Name is not a valid identifier in define expression"
#define ERR_NOT_DEF_EXPR \
  "Not a define expression"

/* (define name expr) */
typedef struct {
  char *name;
  exprptr value;
} definition_expr;

static const char definition_expr_name[] = "definition_expr";

static const expr_vtable definition_expr_vtable = {
  .deallocate = delete_definition_expr,
  .clone = clone_definition_expr,
  .to_string = definition_expr_tostring,
  .interpret = interpret_definition
};

exprptr new_definition_expr(const char *name, exprptr body) {
  definition_expr *de = (definition_expr *)malloc(sizeof(definition_expr));
  de->name = strdup(name);
  de->value = body;

  expr_t *e = (expr_t *)malloc(sizeof(expr_t));
  e->data = de;
  e->vtable = &definition_expr_vtable;
  e->expr_name = definition_expr_name;
  return e;
}

void delete_definition_expr(exprptr self) {
  definition_expr *de = self->data;
  free(de->name);
  delete_expr(de->value);
  free(de);
  free(self);
}

exprptr clone_definition_expr(exprptr self) {
  definition_expr *self_de = self->data;
  definition_expr *new_de = (definition_expr *)malloc(sizeof(definition_expr));
  new_de->name = strdup(self_de->name);
  new_de->value = clone_expr(self_de->value);

  return base_clone(self, new_de);
}

char *definition_expr_tostring(exprptr self) {
  definition_expr *de = self->data;
  char *body_str = expr_tostring(de->value);
  char *result = heap_format("(define %s %s)", de->name, body_str);
  free(body_str);
  return result;
}

exprptr definition_expr_parse(listptr tokens, int *index) {
  token_t *define_token = list_get(tokens, (*index)++);
  assert(define_token->type == TOKEN_DEFINE);

  token_t *name_token = list_get(tokens, (*index)++);
  if (name_token->type != TOKEN_IDENTIFIER) {
    return parser_error(name_token->line, name_token->column, ERR_DEF_NAME_IS_NOT_ID);
  }

  exprptr value_expression = expr_parse(tokens, index);
  if (value_expression == NULL) {
    return NULL;
  }

  char *name = name_token->value.character_sequence;
  exprptr result =  new_definition_expr(name, value_expression);
  result->line_number = define_token->line;
  result->column_number = define_token->column;
  return result;
}

bool is_definition_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(definition_expr_name, e->expr_name) == 0; 
}

object_t interpret_definition(exprptr self, stack_frame_ptr sf) {
  definition_expr *de = self->data;
  object_t value = interpret_expr(de->value, sf);
  if (is_error(value)) {
    return value;
  }

  stack_frame_set_global_variable(sf, de->name, value);
  destroy_object(value);
  return make_void();
}
