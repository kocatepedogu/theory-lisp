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

#include "set.h"
#include "expression.h"
#include "expression_base.h"
#include <string.h>
#include <assert.h>

#include "../utils/string.h"
#include "../scanner/scanner.h"
#include "../parser/parser.h"
#include "../types/void.h"
#include "../types/error.h"

#define ERR_SET_NAME_IS_NOT_ID \
  "Name is not a valid identifier in define expression"
#define ERR_NOT_SET_EXPR \
  "Not a set expression"

/* (set! name expr) */
typedef struct {
  char *name;
  exprptr value;
} set_expr;

static const char set_expr_name[] = "set_expr";

bool is_set_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(set_expr_name, e->expr_name) == 0;
}

static const expr_vtable set_expr_vtable = {
  .deallocate = delete_set_expr,
  .clone = clone_set_expr,
  .to_string = set_expr_tostring,
  .interpret = interpret_set
};

exprptr new_set_expr(const char *name, exprptr body) {
  set_expr *se = (set_expr *)malloc(sizeof(set_expr));
  se->name = strdup(name);
  se->value = body;

  return base_new(se, &set_expr_vtable, set_expr_name, 0, 0);
}

void delete_set_expr(exprptr self) {
  set_expr *se = self->data;
  free(se->name);
  delete_expr(se->value);
  free(se);
  free(self);
}

exprptr clone_set_expr(exprptr self) {
  set_expr *self_se = self->data;
  set_expr *new_se = (set_expr *)malloc(sizeof(set_expr));
  new_se->name = strdup(self_se->name);
  new_se->value = clone_expr(self_se->value);

  return base_clone(self, new_se);
}

char *set_expr_tostring(exprptr self) {
  set_expr *se = self->data;
  char *body_str = expr_tostring(se->value);
  char *result = format("(set! %s %s)", se->name, body_str);
  free(body_str);
  return result;
}

exprptr set_expr_parse(tokenstreamptr tkns) {
  tokenptr set_token = next_tkn(tkns);
  assert(set_token->type == TOKEN_SET);

  tokenptr name_token = next_tkn(tkns);
  if (name_token->type != TOKEN_IDENTIFIER) {
    return parser_error(name_token, ERR_SET_NAME_IS_NOT_ID);
  }

  exprptr value_expression = expr_parse(tkns);
  if (value_expression == NULL) {
    return NULL;
  }

  char *name = name_token->value.character_sequence;
  exprptr result =  new_set_expr(name, value_expression);
  result->line_number = set_token->line;
  result->column_number = set_token->column;
  return result;
}

object_t interpret_set(exprptr self, stack_frame_ptr sf) {
  set_expr *se = self->data;
  object_t value = interpret_expr(se->value, sf);
  if (is_error(value)) {
    return value;
  }

  stack_frame_set_variable(sf, se->name, value);
  return value;
}
