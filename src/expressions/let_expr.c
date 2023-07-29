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

#include "let_expr.h"

#include <assert.h>
#include <string.h>

#include "../scanner/scanner.h"
#include "../utils/heap-format.h"
#include "../utils/list.h"
#include "expression.h"
#include "../parser/parser.h"
#include "../types/error.h"

static const char let_expr_name[] = "let_expr";

#define ERR_NO_BEGINNING_PARENTHESIS \
  "Left parenthesis missing in variable declaration"

#define ERR_NAME_IS_NOT_ID \
  "The variable name must be an identifier in a variable declaration"

#define ERR_NO_END_PARENTHESIS \
  "Right parenthesis missing in variable declaration"

#define ERR_NO_LEFT_PARENTHESIS_IN_DECL_BLOCK                 \
  "Left parenthesis missing at the beginning of the list of " \
  "variable declarations"

#define ERR_NO_RIGHT_PARENTHESIS_IN_DECL_BLOCK           \
  "Right parenthesis missing at the end of the list of " \
  "variable declarations"

static const expr_vtable let_expr_vtable = {.deallocate = delete_let_expr,
                                            .destructor = destroy_let_expr,
					    .clone = clone_let_expr,
                                            .to_string = let_expr_tostring,
					    .interpret = interpret_let};

void construct_let_expr(exprptr e, exprptr body) {
  let_expr *le = malloc(sizeof(let_expr));
  le->body = body;
  construct_list(&le->declarations);
  e->data = le;
  e->vtable = &let_expr_vtable;
  e->expr_name = let_expr_name;
}

exprptr new_let_expr(exprptr body) {
  expr_t *e = (expr_t *)malloc(sizeof(expr_t));
  construct_let_expr(e, body);
  return e;
}

void destroy_let_expr(exprptr e) {
  let_expr *expr = e->data;
  delete_expr(expr->body);
  for (int i = 0; i < list_size(&expr->declarations); i++) {
    var_declaration *decl = list_get(&expr->declarations, i);
    free(decl->name);
    delete_expr(decl->value);
    free(decl);
  }
  destroy_list(&expr->declarations);
  free(expr);
}

void delete_let_expr(exprptr e) {
  destroy_let_expr(e);
  free(e);
}

exprptr clone_let_expr(exprptr e) {
  let_expr *self_le = e->data;
  let_expr *new_le = (let_expr *)malloc(sizeof(let_expr));
  new_le->body = clone_expr(self_le->body);
  construct_list(&new_le->declarations);

  for (size_t i = 0; i < list_size(&self_le->declarations); i++) {
    var_declaration *self_decl = list_get(&self_le->declarations, i);
    var_declaration *new_decl = (var_declaration *)malloc(sizeof(var_declaration));
    new_decl->name = strdup(self_decl->name);
    new_decl->value = clone_expr(self_decl->value);
    list_add(&new_le->declarations, new_decl);
  }

  exprptr new_expr = (exprptr)malloc(sizeof(expr_t));
  new_expr->data = new_le;
  new_expr->vtable = &let_expr_vtable;
  new_expr->expr_name = let_expr_name;
  new_expr->line_number = e->line_number;
  new_expr->column_number = e->column_number;
  return new_expr;
}

void let_expr_add_declaration(exprptr e, const char *name, exprptr expr) {
  let_expr *le = e->data;
  var_declaration *decl = malloc(sizeof(var_declaration));
  decl->name = strdup(name);
  decl->value = expr;
  list_add(&le->declarations, decl);
}

char *let_expr_tostring(exprptr e) {
  let_expr *expr = e->data;
  char *declarations = NULL;
  for (int i = 0; i < list_size(&expr->declarations); i++) {
    var_declaration *decl = list_get(&expr->declarations, i);
    char *value_str = expr_tostring(decl->value);
    char *new_decl_str = heap_format("(%s %s)", decl->name, value_str);
    free(value_str);
    if (!declarations) {
      declarations = new_decl_str;
    } else {
      char *new_declarations = heap_concat(declarations, new_decl_str);
      free(declarations);
      free(new_decl_str);
      declarations = new_declarations;
    }
  }

  char *body_str = expr_tostring(expr->body);
  char *result = declarations
                     ? heap_format("(let (%s) %s)", declarations, body_str)
                     : heap_format("(let () %s)", body_str);

  free(body_str);
  free(declarations);
  return result;
}

static bool var_declaration_parse(list *tokens, int *index, list *var_names,
			          list *var_values) {
  token_t *left_p = list_get(tokens, (*index)++);
  if (left_p->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_p->line, left_p->column, ERR_NO_BEGINNING_PARENTHESIS);
  }
  token_t *var_name = list_get(tokens, (*index)++);
  if (var_name->type != TOKEN_IDENTIFIER) {
    return parser_error(var_name->line, var_name->column, ERR_NAME_IS_NOT_ID);
  }
  exprptr var_value = expr_parse(tokens, index);
  if (var_value == NULL) {
    return false;
  }
  token_t *right_p = list_get(tokens, (*index)++);
  if (right_p->type != TOKEN_RIGHT_PARENTHESIS) {
    delete_expr(var_value);
    return parser_error(right_p->line, right_p->column, ERR_NO_END_PARENTHESIS);
  }

  list_add(var_names, var_name);
  list_add(var_values, var_value);
  return true;
}

static inline exprptr let_expr_parse_error(list *names, list *values) {
  delete_list(names);
  for (size_t i = 0; i < list_size(values); i++) {
    delete_expr(list_get(values, i));
  }
  delete_list(values);
  return NULL;
}

exprptr let_expr_parse(list *tokens, int *index) {
  token_t *let_token = list_get(tokens, (*index)++);
  assert(let_token->type == TOKEN_LET);

  token_t *left_p = list_get(tokens, (*index)++);
  if (left_p->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_p->line, left_p->column, ERR_NO_LEFT_PARENTHESIS_IN_DECL_BLOCK);
  }

  list *var_names = new_list();
  list *var_values = new_list();
  while (((token_t *)list_get(tokens, *index))->type !=
         TOKEN_RIGHT_PARENTHESIS) {
    if (!var_declaration_parse(tokens, index, var_names, var_values)) {
      return let_expr_parse_error(var_names, var_values);
    }
  }

  token_t *right_p = list_get(tokens, (*index)++);
  if (right_p->type != TOKEN_RIGHT_PARENTHESIS) {
    return parser_error(right_p->line, right_p->column, ERR_NO_RIGHT_PARENTHESIS_IN_DECL_BLOCK);
  }

  exprptr body = expr_parse(tokens, index);
  if (body == NULL) {
    return let_expr_parse_error(var_names, var_values);
  }

  exprptr let_expr = new_let_expr(body);
  for (size_t i = 0; i < list_size(var_names); i++) {
    token_t *var_name = list_get(var_names, i);
    exprptr var_value = list_get(var_values, i);
    let_expr_add_declaration(let_expr, var_name->value.character_sequence,
                             var_value);
  }
  delete_list(var_names);
  delete_list(var_values);

  let_expr->line_number = let_token->line;
  let_expr->column_number = let_token->column;
  return let_expr;
}

bool is_let_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, let_expr_name) == 0;
}

object_t interpret_let(exprptr e, stack_frame_ptr sf) {
  let_expr *le = e->data;

  stack_frame_t new_frame;
  construct_stack_frame(&new_frame, sf);
  for (size_t i = 0; i < list_size(&le->declarations); i++) {
    var_declaration *decl = list_get(&le->declarations, i);
    object_t value = interpret_expr(decl->value, &new_frame);
    if (is_error(value)) {
      destroy_stack_frame(&new_frame);
      return value;
    }

    stack_frame_set_variable(&new_frame, decl->name, value);
    destroy_object(value);
  }

  object_t result = interpret_expr(le->body, &new_frame);
  destroy_stack_frame(&new_frame);
  return result;
}
