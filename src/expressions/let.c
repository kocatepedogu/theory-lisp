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

#include "let.h"

#include <assert.h>
#include <string.h>

#include "../scanner/scanner.h"
#include "../utils/string.h"
#include "../utils/list.h"
#include "expression.h"
#include "expression_base.h"
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

static const expr_vtable let_expr_vtable = {.destroy = destroy_let_expr,
                                            .to_string = let_expr_tostring,
					                                  .interpret = interpret_let};

static const char let_expr_name[] = "let_expr";

bool is_let_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, let_expr_name) == 0;
}

exprptr new_let_expr(exprptr body, tokenptr tkn) {
  let_expr *le = malloc(sizeof *le);
  le->body = body;
  le->declarations = new_list();

  return expr_base_new(le, &let_expr_vtable, let_expr_name, tkn);
}

void destroy_let_expr(exprptr self) {
  let_expr *expr = self->data;
  delete_expr(expr->body);
  for (size_t i = 0; i < list_size(expr->declarations); ++i) {
    var_declaration *decl = list_get(expr->declarations, i);
    free(decl->name);
    delete_expr(decl->value);
    free(decl);
  }
  delete_list(expr->declarations);
  free(expr);
}

void let_expr_add_declaration(exprptr self, const char *name, exprptr expr) {
  let_expr *le = self->data;
  var_declaration *decl = malloc(sizeof *decl);
  decl->name = strdup(name);
  decl->value = expr;
  list_add(le->declarations, decl);
}

char *let_expr_tostring(exprptr self) {
  let_expr *expr = self->data;
  char *declarations = NULL;
  for (size_t i = 0; i < list_size(expr->declarations); ++i) {
    var_declaration *decl = list_get(expr->declarations, i);
    char *value_str = expr_tostring(decl->value);
    char *new_decl_str = format("(%s %s)", decl->name, value_str);
    free(value_str);
    declarations = unique_append(declarations, new_decl_str);
  }

  char *body = expr_tostring(expr->body);
  if (declarations) {
    return unique_format("(let (%s) %s)", declarations, body);
  } else {
    return unique_format("(let () %s)", body);
  }
}

static bool var_declaration_parse(tokenstreamptr tkns, listptr var_names,
			          listptr var_values, stack_frame_ptr sf) {
  tokenptr left_p = next_tkn(tkns);
  if (left_p->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_p, ERR_NO_BEGINNING_PARENTHESIS);
  }
  tokenptr var_name = next_tkn(tkns);
  if (var_name->type != TOKEN_IDENTIFIER) {
    return parser_error(var_name, ERR_NAME_IS_NOT_ID);
  }
  exprptr var_value = expr_parse(tkns, sf);
  if (var_value == NULL) {
    return false;
  }
  tokenptr right_p = next_tkn(tkns);
  if (right_p->type != TOKEN_RIGHT_PARENTHESIS) {
    delete_expr(var_value);
    return parser_error(right_p, ERR_NO_END_PARENTHESIS);
  }

  list_add(var_names, var_name);
  list_add(var_values, var_value);
  return true;
}

static inline exprptr let_expr_parse_error(listptr names, listptr values) {
  delete_list(names);
  for (size_t i = 0; i < list_size(values); ++i) {
    delete_expr(list_get(values, i));
  }
  delete_list(values);
  return NULL;
}

exprptr let_expr_parse(tokenstreamptr tkns, stack_frame_ptr sf) {
  tokenptr let_token = next_tkn(tkns);
  assert(let_token->type == TOKEN_LET);

  tokenptr left_p = next_tkn(tkns);
  if (left_p->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_p, ERR_NO_LEFT_PARENTHESIS_IN_DECL_BLOCK);
  }

  listptr var_names = new_list();
  listptr var_values = new_list();
  while (current_tkn(tkns)->type != TOKEN_RIGHT_PARENTHESIS) {
    if (!var_declaration_parse(tkns, var_names, var_values, sf)) {
      return let_expr_parse_error(var_names, var_values);
    }
  }

  tokenptr right_p = next_tkn(tkns);
  if (right_p->type != TOKEN_RIGHT_PARENTHESIS) {
    return parser_error(right_p, ERR_NO_RIGHT_PARENTHESIS_IN_DECL_BLOCK);
  }

  exprptr body = expr_parse(tkns, sf);
  if (body == NULL) {
    return let_expr_parse_error(var_names, var_values);
  }

  exprptr le = new_let_expr(body, let_token);
  for (size_t i = 0; i < list_size(var_names); ++i) {
    tokenptr var_name = list_get(var_names, i);
    exprptr var_value = list_get(var_values, i);
    let_expr_add_declaration(le, var_name->value.character_sequence,
                             var_value);
  }
  delete_list(var_names);
  delete_list(var_values);

  return le;
}

objectptr interpret_let(exprptr self, stack_frame_ptr sf) {
  let_expr *le = self->data;

  stack_frame_ptr new_frame = new_stack_frame(sf);
  for (size_t i = 0; i < list_size(le->declarations); ++i) {
    var_declaration *decl = list_get(le->declarations, i);
    objectptr value = interpret_expr(decl->value, new_frame);
    if (is_error(value)) {
      delete_stack_frame(new_frame);
      return value;
    }

    stack_frame_set_local_variable(new_frame, decl->name, value);
    delete_object(value);
  }

  objectptr result = interpret_expr(le->body, new_frame);
  delete_stack_frame(new_frame);
  return result;
}
