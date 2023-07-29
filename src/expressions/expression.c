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

#include "expression.h"

#include <stdio.h>
#include <assert.h>

#include "../scanner/scanner.h"
#include "../utils/list.h"
#include "data_expr.h"
#include "definition_expr.h"
#include "evaluation_expr.h"
#include "expanded_expr.h"
#include "identifier_expr.h"
#include "if_expr.h"
#include "lambda_expr.h"
#include "let_expr.h"
#include "../parser/parser.h"

#include "../types/null.h"
#include "../types/boolean.h"
#include "../types/integer.h"
#include "../types/real.h"
#include "../types/string.h"
#include "../types/error.h"

/* Parser error messages */
#define ERR_UNEXPECTED_EOF "Unexpected end of file"
#define ERR_INVALID_TOKEN "Invalid token"
#define ERR_UNMATCHED_PARENTHESIS "Unmatched parenthesis"

/* destroys and expression */
void destroy_expr(exprptr e) { 
  if (e != NULL) {
    e->vtable->destructor(e); 
  }
}

/* destroys and deallocates an expression */
void delete_expr(exprptr e) {
  if (e != NULL) {
    e->vtable->deallocate(e); 
  }
}

/* clones an expression */
exprptr clone_expr(exprptr e) {
  if (e != NULL) {
    return e->vtable->clone(e);
  }
  return NULL;
}

/* Returns string representation of an expression */
char *expr_tostring(exprptr e) { return e->vtable->to_string(e); }

/* Parses an expression that starts with a left parenthesis */
static exprptr parenthesized_expr_parse(list *tokens, int *index) {
  token_t *next_tkn = list_get(tokens, *index);
  size_t line = next_tkn->line;
  size_t column = next_tkn->column;

  if (next_tkn->type == TOKEN_END_OF_FILE) {
    return parser_error(line, column, ERR_UNEXPECTED_EOF);
  }

  if (next_tkn->type == TOKEN_RIGHT_PARENTHESIS) {
    (*index)++;

    exprptr result = new_data_expr(make_null());
    result->line_number = line;
    result->column_number = column;
    return result;
  }

  exprptr subexpr = NULL;
  switch (next_tkn->type) {
    case TOKEN_LET:
      subexpr = let_expr_parse(tokens, index);
      break;
    case TOKEN_LAMBDA:
      subexpr = lambda_expr_parse(tokens, index);
      break;
    case TOKEN_IF:
      subexpr = if_expr_parse(tokens, index);
      break;
    case TOKEN_DEFINE:
      subexpr = definition_expr_parse(tokens, index);
      break;
    case TOKEN_COND:
      subexpr = cond_expr_parse(tokens, index);
      break;
    case TOKEN_LEFT_PARENTHESIS:
    case TOKEN_IDENTIFIER:
    case TOKEN_STRING:
    case TOKEN_INTEGER:
    case TOKEN_REAL:
      subexpr = evaluation_expr_parse(tokens, index);
      break;
    default:
      return parser_error(line, column, ERR_INVALID_TOKEN);
  }

  if (subexpr) {
    token_t *right_p = list_get(tokens, (*index)++);
    if (right_p->type != TOKEN_RIGHT_PARENTHESIS) {
      delete_expr(subexpr);
      return parser_error(line, column, ERR_UNMATCHED_PARENTHESIS);
    }
  }

  return subexpr;
}

/* Parses an arbitrary expression */
exprptr expr_parse(list *tokens, int *index) {
  token_t *tkn = list_get(tokens, (*index)++);
  size_t line = tkn->line;
  size_t column = tkn->column;

  if (tkn->type == TOKEN_END_OF_FILE) {
    return parser_error(line, column, ERR_UNEXPECTED_EOF);
  }
  if (tkn->type == TOKEN_RIGHT_PARENTHESIS) {
    return parser_error(line, column, ERR_UNMATCHED_PARENTHESIS);
  }
  if (tkn->type == TOKEN_LEFT_PARENTHESIS) {
    return parenthesized_expr_parse(tokens, index);
  }
  if (tkn->type == TOKEN_AMPERSAND) {
    exprptr inner = expr_parse(tokens, index);
    if (!inner) {
      return NULL;
    }

    return new_expanded_expr(inner);
  }

  object_t obj = make_error("uninitialized object");
  exprptr result = NULL;
  switch (tkn->type) {
    case TOKEN_NULL:
      assign_object(&obj, make_null());
      result = new_data_expr(obj);
      destroy_object(obj);
      break;
    case TOKEN_BOOLEAN:
      assign_object(&obj, make_boolean(tkn->value.boolean));
      result = new_data_expr(obj);
      destroy_object(obj);
      break;
    case TOKEN_INTEGER:
      assign_object(&obj, make_integer(tkn->value.integer));
      result = new_data_expr(obj);
      destroy_object(obj);
      break;
    case TOKEN_REAL:
      assign_object(&obj, make_real(tkn->value.real));
      result = new_data_expr(obj);
      destroy_object(obj);
      break;
    case TOKEN_IDENTIFIER:
      destroy_object(obj);
      result = new_identifier_expr(tkn->value.character_sequence);
      break;
    case TOKEN_STRING:
      assign_object(&obj, make_string(tkn->value.character_sequence));
      result = new_data_expr(obj);
      destroy_object(obj);
      break;
    default:
      destroy_object(obj);
      result = parser_error(line, column, ERR_INVALID_TOKEN);
      break;
  }

  if (result != NULL) {
    result->line_number = tkn->line;
    result->column_number = tkn->column;
  }

  return result;
}

/* interpretes an arbitrary expression */
object_t interpret_expr(exprptr e, stack_frame_ptr sf) {
  return e->vtable->interpret(e, sf);
}
