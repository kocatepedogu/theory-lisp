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

#include "try_catch.h"
#include "expression.h"
#include "expression_base.h"
#include <string.h>
#include <assert.h>

#include "../utils/string.h"
#include "../scanner/scanner.h"
#include "../parser/parser.h"
#include "../types/void.h"
#include "../types/error.h"
#include "../types/string.h"

#define ERR_SET_NAME_IS_NOT_ID \
  "Name is not a valid identifier in define expression"
#define ERR_NOT_SET_EXPR \
  "Not a try-catch expression"

/* (try
 *   code-that-may-throw-an-error
 *   (catch (e)
 *     error-handling-code)) */
typedef struct {
  exprptr body;
  char *exception_name;
  exprptr handler;
} try_catch_expr;

static const char try_catch_expr_name[] = "try_catch_expr";

bool is_try_catch_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(try_catch_expr_name, e->expr_name) == 0;
}

static const expr_vtable try_catch_expr_vtable = {
  .destroy = destroy_try_catch_expr,
  .to_string = try_catch_expr_tostring,
  .interpret = interpret_try_catch_expr
};

exprptr new_try_catch_expr(exprptr body, const char *name, exprptr handler, 
                           tokenptr tkn) {
  try_catch_expr *tce = malloc(sizeof *tce);
  tce->body = body;
  tce->handler = handler;
  tce->exception_name = strdup(name);

  return expr_base_new(tce, &try_catch_expr_vtable, try_catch_expr_name, tkn);
}

void destroy_try_catch_expr(exprptr self) {
  try_catch_expr *tce = self->data;  
  delete_expr(tce->body);
  delete_expr(tce->handler);
  free(tce->exception_name);
  free(tce);
}

char *try_catch_expr_tostring(exprptr self) {
  try_catch_expr *tce = self->data;
  char *body_str = expr_tostring(tce->body);
  char *handler_str = expr_tostring(tce->handler);
  char *result = format("(try %s (catch (%s) %s))", body_str, tce->exception_name,
                        handler_str);
  free(body_str);
  free(handler_str);
  return result;
}

exprptr try_catch_expr_parse(tokenstreamptr tkns, stack_frame_ptr sf) {
  tokenptr try_token = next_tkn(tkns);
  assert(try_token->type == TOKEN_TRY);

  exprptr body_expr = expr_parse(tkns, sf);
  if (body_expr == NULL) {
    return NULL;
  }

  tokenptr catch_leftp_tkn = next_tkn(tkns);
  if (catch_leftp_tkn->type != TOKEN_LEFT_PARENTHESIS) {
    delete_expr(body_expr);
    return parser_error(catch_leftp_tkn, "Left parenthesis excepted");
  }

  tokenptr catch_tkn = next_tkn(tkns);
  if (catch_tkn->type != TOKEN_CATCH) {
    delete_expr(body_expr);
    return parser_error(catch_tkn, "'catch' expected");
  }

  tokenptr name_leftp_tkn = next_tkn(tkns);
  if (name_leftp_tkn->type != TOKEN_LEFT_PARENTHESIS) {
    delete_expr(body_expr);
    return parser_error(name_leftp_tkn, "Left parenthesis expected");
  }

  tokenptr name_tkn = next_tkn(tkns);
  if (name_tkn->type != TOKEN_IDENTIFIER) {
    delete_expr(body_expr);
    return parser_error(name_tkn, "Exception name expected");
  }

  tokenptr name_rightp_tkn = next_tkn(tkns);
  if (name_rightp_tkn->type != TOKEN_RIGHT_PARENTHESIS) {
    delete_expr(body_expr);
    return parser_error(name_rightp_tkn, "Right parenthesis expected");
  }

  exprptr handler_expr = expr_parse(tkns, sf);
  if (handler_expr == NULL) {
    delete_expr(body_expr);
    return NULL;
  }

  tokenptr catch_rightp_tkn = next_tkn(tkns);
  if (catch_rightp_tkn->type != TOKEN_RIGHT_PARENTHESIS) {
    delete_expr(body_expr);
    return parser_error(catch_rightp_tkn, "Right parenthesis excepted");
  }

  return new_try_catch_expr(body_expr, name_tkn->value.character_sequence, 
                            handler_expr, try_token);
}

objectptr interpret_try_catch_expr(exprptr self, stack_frame_ptr sf) {
  try_catch_expr *tce = self->data;
  objectptr body_value = interpret_expr(tce->body, sf);
  if (!is_error(body_value)) {
    return body_value;
  }

  char *error_value = error_tostring(body_value);
  delete_object(body_value);
  objectptr error_object = make_string(error_value);
  free(error_value);

  stack_frame_ptr local_sf = new_stack_frame(sf);
  stack_frame_set_variable(local_sf, tce->exception_name, error_object);
  delete_object(error_object);

  objectptr handler_value = interpret_expr(tce->handler, local_sf);
  delete_stack_frame(local_sf);
  return handler_value;
}
