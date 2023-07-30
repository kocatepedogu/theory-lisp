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

/* (var val) */
typedef struct {
  char *name;
  exprptr value;
} var_declaration;

/* (let ((var1 val1) (var2 val2) ...) (body)) */
typedef struct {
  listptr declarations; /* list of var_declaration*'s */
  exprptr body;
} let_expr;

static const expr_vtable let_expr_vtable = {.deallocate = delete_let_expr,
					                                  .clone = clone_let_expr,
                                            .to_string = let_expr_tostring,
					                                  .interpret = interpret_let};

static const char let_expr_name[] = "let_expr";

exprptr new_let_expr(exprptr body) {
  let_expr *le = malloc(sizeof(let_expr));
  le->body = body;
  le->declarations = new_list();

  expr_t *e = (expr_t *)malloc(sizeof(expr_t));
  e->data = le;
  e->vtable = &let_expr_vtable;
  e->expr_name = let_expr_name;
  return e;
}

void delete_let_expr(exprptr self) {
  let_expr *expr = self->data;
  delete_expr(expr->body);
  for (size_t i = 0; i < list_size(expr->declarations); i++) {
    var_declaration *decl = list_get(expr->declarations, i);
    free(decl->name);
    delete_expr(decl->value);
    free(decl);
  }
  delete_list(expr->declarations);
  free(expr);
  free(self);
}

exprptr clone_let_expr(exprptr self) {
  let_expr *self_le = self->data;
  let_expr *new_le = (let_expr *)malloc(sizeof(let_expr));
  new_le->body = clone_expr(self_le->body);
  new_le->declarations = new_list();

  for (size_t i = 0; i < list_size(self_le->declarations); i++) {
    var_declaration *self_decl = list_get(self_le->declarations, i);
    var_declaration *new_decl = (var_declaration *)malloc(sizeof(var_declaration));
    new_decl->name = strdup(self_decl->name);
    new_decl->value = clone_expr(self_decl->value);
    list_add(new_le->declarations, new_decl);
  }

  return base_clone(self, new_le);
}

void let_expr_add_declaration(exprptr self, const char *name, exprptr expr) {
  let_expr *le = self->data;
  var_declaration *decl = malloc(sizeof(var_declaration));
  decl->name = strdup(name);
  decl->value = expr;
  list_add(le->declarations, decl);
}

char *let_expr_tostring(exprptr self) {
  let_expr *expr = self->data;
  char *declarations = NULL;
  for (size_t i = 0; i < list_size(expr->declarations); i++) {
    var_declaration *decl = list_get(expr->declarations, i);
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

static bool var_declaration_parse(listptr tokens, int *index, listptr var_names,
			          listptr var_values) {
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

static inline exprptr let_expr_parse_error(listptr names, listptr values) {
  delete_list(names);
  for (size_t i = 0; i < list_size(values); i++) {
    delete_expr(list_get(values, i));
  }
  delete_list(values);
  return NULL;
}

exprptr let_expr_parse(listptr tokens, int *index) {
  token_t *let_token = list_get(tokens, (*index)++);
  assert(let_token->type == TOKEN_LET);

  token_t *left_p = list_get(tokens, (*index)++);
  if (left_p->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_p->line, left_p->column, ERR_NO_LEFT_PARENTHESIS_IN_DECL_BLOCK);
  }

  listptr var_names = new_list();
  listptr var_values = new_list();
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

  exprptr le = new_let_expr(body);
  for (size_t i = 0; i < list_size(var_names); i++) {
    token_t *var_name = list_get(var_names, i);
    exprptr var_value = list_get(var_values, i);
    let_expr_add_declaration(le, var_name->value.character_sequence,
                             var_value);
  }
  delete_list(var_names);
  delete_list(var_values);

  le->line_number = let_token->line;
  le->column_number = let_token->column;
  return le;
}

bool is_let_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, let_expr_name) == 0;
}

object_t interpret_let(exprptr self, stack_frame_ptr sf) {
  let_expr *le = self->data;

  stack_frame_ptr new_frame = new_stack_frame(sf);
  for (size_t i = 0; i < list_size(le->declarations); i++) {
    var_declaration *decl = list_get(le->declarations, i);
    object_t value = interpret_expr(decl->value, new_frame);
    if (is_error(value)) {
      delete_stack_frame(new_frame);
      return value;
    }

    stack_frame_set_variable(new_frame, decl->name, value);
    destroy_object(value);
  }

  object_t result = interpret_expr(le->body, new_frame);
  delete_stack_frame(new_frame);
  return result;
}
