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

#include "lambda_expr.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/error.h"
#include "../types/procedure.h"
#include "../utils/heap-format.h"
#include "../utils/list.h"
#include "../builtin/list.h"
#include "expression.h"

static const char lambda_expr_name[] = "lambda_expr";

#define ERR_PARENTHESIZE_MUST_FOLLOW_LAMBDA                   \
  "A parenthesized list must follow the lambda keyword in a " \
  "lambda declaration"

#define ERR_LAMBDA_SYNTAX                                       \
  "Formal parameters of a lambda expression must only contain " \
  "identifiers, and the list must end with a right parenthesis."

#define ERR_ARITY_MISMATCH \
  "Arity mismatch: Lambda takes %d parameters but %d given"

#define ERR_ARITY_AT_LEAST \
  "Arity mismatch: Lambda takes at least %d parameters but %d given"

static const expr_vtable lambda_expr_vtable = {
    .deallocate = delete_lambda_expr,
    .destructor = destroy_lambda_expr,
    .to_string = lambda_expr_tostring,
    .interpret = interpret_lambda};

void construct_lambda_expr(exprptr e, exprptr body, bool variadic) {
  lambda_expr *le = malloc(sizeof(lambda_expr));
  construct_list(&le->params);
  le->body = body;
  le->variadic = variadic;
  e->data = le;
  e->vtable = &lambda_expr_vtable;
  e->expr_name = lambda_expr_name;
}

exprptr new_lambda_expr(exprptr body, bool variadic) {
  expr_t *e = (expr_t *)malloc(sizeof(expr_t));
  construct_lambda_expr(e, body, variadic);
  return e;
}

void destroy_lambda_expr(exprptr e) {
  lambda_expr *expr = e->data;
  delete_expr(expr->body);
  for (int i = 0; i < list_size(&expr->params); i++) {
    char *param = list_get(&expr->params, i);
    free(param);
  }
  destroy_list(&expr->params);
  free(expr);
}

void delete_lambda_expr(exprptr e) {
  destroy_lambda_expr(e);
  free(e);
}

void lambda_expr_add_param(exprptr e, const char *name) {
  lambda_expr *le = e->data;
  list_add(&le->params, strdup(name));
}

char *lambda_expr_tostring(exprptr e) {
  lambda_expr *expr = e->data;
  char *params = NULL;
  for (int i = 0; i < list_size(&expr->params); i++) {
    char *str = list_get(&expr->params, i);
    if (!params) {
      params = strdup(str);
    } else {
      char *new_params = heap_format("%s %s", params, str);
      free(params);
      params = new_params;
    }
  }

  char *body_str = expr_tostring(expr->body);
  char *result = NULL;
  if (params) {
    if (expr->variadic) {
      result = heap_format("(lambda (%s...) %s)", params, body_str);
    } else {
      result = heap_format("(lambda (%s) %s)", params, body_str);
    }
  } else {
    if (expr->variadic) {
      result = heap_format("(lambda (...) %s)", body_str);
    } else {
      result = heap_format("(lambda () %s)", body_str);
    }
  }

  free(body_str);
  free(params);
  return result;
}

exprptr lambda_expr_parse(list *tokens, int *index) {
  token_t *lambda_token = list_get(tokens, (*index)++);
  assert(lambda_token->type == TOKEN_LAMBDA);

  token_t *left_parenthesis_token = list_get(tokens, (*index)++);
  if (left_parenthesis_token->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_parenthesis_token->line,
                        left_parenthesis_token->column,
                        ERR_PARENTHESIZE_MUST_FOLLOW_LAMBDA);
  }

  list *formal_parameters = new_list();
  bool variadic = false;
  token_t *param_token = NULL;
  while ((param_token = list_get(tokens, *index))->type == TOKEN_IDENTIFIER &&
         !variadic) {
    if (strcmp(param_token->value.character_sequence, "...") == 0) {
      variadic = true;
    } else {
      list_add(formal_parameters, param_token->value.character_sequence);
    }

    *index += 1;
  }

  token_t *right_parenthesis_token = list_get(tokens, (*index)++);
  if (right_parenthesis_token->type != TOKEN_RIGHT_PARENTHESIS) {
    return parser_error(right_parenthesis_token->line,
                        right_parenthesis_token->column, ERR_LAMBDA_SYNTAX);
  }

  exprptr body = expr_parse(tokens, index);
  if (body == NULL) {
    delete_list(formal_parameters);
    return NULL;
  }

  exprptr lambda_expr = new_lambda_expr(body, variadic);
  for (size_t i = 0; i < list_size(formal_parameters); i++) {
    lambda_expr_add_param(lambda_expr, list_get(formal_parameters, i));
  }
  delete_list(formal_parameters);

  lambda_expr->line_number = lambda_token->line;
  lambda_expr->column_number = lambda_token->column;
  return lambda_expr;
}

bool is_lambda_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, lambda_expr_name) == 0;
}

object_t interpret_lambda(exprptr e, stack_frame_ptr sf) {
  object_t procedure = make_procedure(e);
  return procedure;
}

object_t call_lambda(exprptr e, size_t number_of_args, object_t *evaluated_args,
                     stack_frame_ptr sf) {
  lambda_expr *le = e->data;
  if (!le->variadic && list_size(&le->params) != number_of_args) {
    return make_error(ERR_ARITY_MISMATCH, list_size(&le->params),
                      number_of_args);
  }
  if (le->variadic && list_size(&le->params) > number_of_args) {
    return make_error(ERR_ARITY_AT_LEAST, list_size(&le->params),
		      number_of_args);
  }

  stack_frame_t new_frame;
  construct_stack_frame(&new_frame, sf);
  for (size_t i = 0; i < list_size(&le->params); i++) {
    object_t arg = evaluated_args[i];
    stack_frame_set_variable(&new_frame, list_get(&le->params, i), arg);
  }

  if (le->variadic) {
    size_t fixed_param_size = list_size(&le->params);
    object_t *varargs = evaluated_args + fixed_param_size;
    size_t varsize = number_of_args - fixed_param_size;
    object_t args_object = builtin_list(varsize, varargs, sf);
    stack_frame_set_variable(&new_frame, "va_args", args_object);
    destroy_object(args_object); 
  }

  object_t result = interpret_expr(le->body, &new_frame);
  destroy_stack_frame(&new_frame);
  return result;
}
