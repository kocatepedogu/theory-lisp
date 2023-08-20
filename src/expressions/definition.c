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

#include "definition.h"
#include "expression.h"
#include "expression_base.h"
#include <string.h>
#include <assert.h>

#include "../utils/string.h"
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

bool is_definition_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(definition_expr_name, e->expr_name) == 0; 
}

static const expr_vtable definition_expr_vtable = {
  .destroy = destroy_definition_expr,
  .to_string = definition_expr_tostring,
  .interpret = interpret_definition
};

exprptr new_definition_expr(const char *name, exprptr body, tokenptr tkn) {
  definition_expr *de = malloc(sizeof *de);
  de->name = strdup(name);
  de->value = body;

  return expr_base_new(de, &definition_expr_vtable, definition_expr_name, tkn);
}

void destroy_definition_expr(exprptr self) {
  definition_expr *de = self->data;
  free(de->name);
  delete_expr(de->value);
  free(de);
}

char *definition_expr_tostring(exprptr self) {
  definition_expr *de = self->data;
  char *body_str = expr_tostring(de->value);
  char *result = format("(define %s %s)", de->name, body_str);
  free(body_str);
  return result;
}

exprptr definition_expr_parse(tokenstreamptr tkns) {
  tokenptr define_token = next_tkn(tkns);
  assert(define_token->type == TOKEN_DEFINE);

  tokenptr name_token = next_tkn(tkns);
  if (name_token->type != TOKEN_IDENTIFIER) {
    return parser_error(name_token, ERR_DEF_NAME_IS_NOT_ID);
  }

  exprptr value_expression = expr_parse(tkns);
  if (value_expression == NULL) {
    return NULL;
  }

  char *name = name_token->value.character_sequence;
  return new_definition_expr(name, value_expression, define_token);
}

objectptr interpret_definition(exprptr self, stack_frame_ptr sf) {
  definition_expr *de = self->data;
  objectptr value = interpret_expr(de->value, sf);
  if (is_error(value)) {
    return value;
  }

  stack_frame_set_global_variable(sf, de->name, value);
  delete_object(value);
  return make_void();
}
