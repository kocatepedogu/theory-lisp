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
#include "../interpreter/stack_frame.h"
#include "../interpreter/variable.h"
#include "expression.h"

static const char lambda_expr_name[] = "lambda_expr";

#define ERR_NO_PARAM \
  "A lambda expression must contain a formal parameter list"  

#define ERR_LAMBDA_SYNTAX                                       \
  "Formal parameters of a lambda expression must only contain " \
  "identifiers, and the list must end with a right parenthesis."

#define ERR_ARITY_MISMATCH \
  "Arity mismatch: Lambda %s takes %d parameters but %d given"

#define ERR_ARITY_AT_LEAST \
  "Arity mismatch: Lambda %s takes at least %d parameters but %d given"

static const expr_vtable lambda_expr_vtable = {
    .deallocate = delete_lambda_expr,
    .destructor = destroy_lambda_expr,
    .clone = clone_lambda_expr,
    .to_string = lambda_expr_tostring,
    .interpret = interpret_lambda};

void construct_lambda_expr(exprptr e, exprptr body, bool variadic) {
  lambda_expr *le = malloc(sizeof(lambda_expr));
  construct_list(&le->params);
  construct_list(&le->captured_vars);
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

  for (size_t i = 0; i < list_size(&expr->captured_vars); i++) {
    char *captured_var = list_get(&expr->captured_vars, i);
    free(captured_var);
  }
  destroy_list(&expr->captured_vars);

  free(expr);
}

void delete_lambda_expr(exprptr e) {
  destroy_lambda_expr(e);
  free(e);
}

exprptr clone_lambda_expr(exprptr e) {
  lambda_expr *self_le = e->data;
  lambda_expr *new_le = (lambda_expr *)malloc(sizeof(lambda_expr));
  new_le->variadic = self_le->variadic;
  new_le->body = clone_expr(self_le->body);

  construct_list(&new_le->params);
  for (size_t i = 0; i < list_size(&self_le->params); i++) {
    char *param = list_get(&self_le->params, i);
    list_add(&new_le->params, strdup(param));
  }

  construct_list(&new_le->captured_vars);
  for (size_t i = 0; i < list_size(&self_le->captured_vars); i++) {
    char *captured_var = list_get(&self_le->captured_vars, i);
    list_add(&new_le->captured_vars, strdup(captured_var));
  }

  exprptr new_expr = (exprptr)malloc(sizeof(expr_t));
  new_expr->data = new_le;
  new_expr->vtable = &lambda_expr_vtable;
  new_expr->expr_name = lambda_expr_name;
  new_expr->line_number = e->line_number;
  new_expr->column_number = e->column_number;
  return new_expr;
}

void lambda_expr_add_param(exprptr e, const char *name) {
  lambda_expr *le = e->data;
  list_add(&le->params, strdup(name));
}

void lambda_expr_add_captured_var(exprptr e, const char *name) {
  lambda_expr *le = e->data;
  list_add(&le->captured_vars, strdup(name));
}

char *lambda_expr_tostring_param_list(list *lst) {
  char *result = NULL;
  for (int i = 0; i < list_size(lst); i++) {
    char *str = list_get(lst, i);
    if (!result) {
      result = strdup(str);
    } else {
      char *new_result = heap_format("%s %s", result, str);
      free(result);
      result = new_result;
    }
  }
  return result;
}

char *lambda_expr_tostring(exprptr e) {
  lambda_expr *expr = e->data;

  char *captures_str = lambda_expr_tostring_param_list(&expr->captured_vars);
  char *params_str = lambda_expr_tostring_param_list(&expr->params);

  char *captures = NULL;
  if (captures_str) {
    char *fmt = "[%s] ";
    captures = heap_format(fmt, captures_str);
    free(captures_str);
  } else {
    char *fmt = "";
    captures = heap_format(fmt);
  }

  char *params = NULL;
  if (params_str) {
    char *fmt = expr->variadic ? "(%s ...) " : "(%s) ";
    params = heap_format(fmt, params_str);
    free(params_str);
  } else {
    char *fmt = expr->variadic ? "(...) " : "() ";
    params = heap_format(fmt);
  }

  char *body = expr_tostring(expr->body);
  char *result = heap_format("(lambda %s%s%s)", captures, params, body);

  free(captures);
  free(params);
  free(body);

  return result;
}

static list *lambda_expr_parse_get_captures(list *tokens, int *index) {
  list *captured_variables = new_list();

  token_t *left_bracket = list_get(tokens, *index);
  if (left_bracket->type == TOKEN_LEFT_SQUARE_BRACKET) {
    (*index)++;
    
    token_t *captured_token = NULL;
    while ((captured_token = list_get(tokens, *index))->type == TOKEN_IDENTIFIER) {
      list_add(captured_variables, captured_token->value.character_sequence);
      (*index) += 1;
    }

    token_t *right_bracket = list_get(tokens, (*index)++);
    if (right_bracket->type != TOKEN_RIGHT_SQUARE_BRACKET) {
      delete_list(captured_variables);
      return parser_error(right_bracket->line,
			  right_bracket->column, "Capture list is incomplete");
    }
  }

  return captured_variables;
}

static list *lambda_expr_parse_get_params(list *tokens, int *index, bool *variadic) {
  token_t *left_par = list_get(tokens, (*index)++);
  if (left_par->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_par->line, left_par->column, ERR_NO_PARAM);
  }

  list *formal_parameters = new_list();
  token_t *param_token = NULL;
  while ((param_token = list_get(tokens, *index))->type == TOKEN_IDENTIFIER) {
    if (strcmp(param_token->value.character_sequence, "...") == 0) {
      *variadic = true;
      *index += 1;
      break;
    }

    list_add(formal_parameters, param_token->value.character_sequence);
    *index += 1;
  }

  token_t *right_par = list_get(tokens, (*index)++);
  if (right_par->type != TOKEN_RIGHT_PARENTHESIS) {
    delete_list(formal_parameters);
    return parser_error(right_par->line, right_par->column, ERR_LAMBDA_SYNTAX);
  }

  return formal_parameters;
}

exprptr lambda_expr_parse(list *tokens, int *index) {
  token_t *lambda_token = list_get(tokens, (*index)++);
  assert(lambda_token->type == TOKEN_LAMBDA);

  list *captured_variables = lambda_expr_parse_get_captures(tokens, index);
  if (!captured_variables) {
    return NULL;
  }

  bool variadic = false;
  list *formal_parameters = lambda_expr_parse_get_params(tokens, index, &variadic);
  if (!formal_parameters) {
    delete_list(captured_variables);
    return NULL;
  }

  exprptr body = expr_parse(tokens, index);
  if (body == NULL) {
    delete_list(formal_parameters);
    delete_list(captured_variables);
    return NULL;
  }

  exprptr lambda_expr = new_lambda_expr(body, variadic);

  for (size_t i = 0; i < list_size(formal_parameters); i++) {
    lambda_expr_add_param(lambda_expr, list_get(formal_parameters, i));
  }

  for (size_t i = 0; i < list_size(captured_variables); i++) {
    lambda_expr_add_captured_var(lambda_expr, list_get(captured_variables, i));
  }

  delete_list(formal_parameters);
  delete_list(captured_variables);

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
  object_t procedure = make_procedure(e, sf);
  return procedure;
}

object_t call_lambda(proc_t *proc, size_t number_of_args, object_t *evaluated_args,
                     stack_frame_ptr sf) {
  exprptr e = proc->lambda;
  lambda_expr *le = e->data;
  if (!le->variadic && list_size(&le->params) != number_of_args) {
    char *lstr = lambda_expr_tostring(proc->lambda);
    object_t err = make_error(ERR_ARITY_MISMATCH, lstr, list_size(&le->params),
                      number_of_args);
    free(lstr);
    return err;
  }
  if (le->variadic && list_size(&le->params) > number_of_args) {
    char *lstr = lambda_expr_tostring(proc->lambda);
    object_t err = make_error(ERR_ARITY_AT_LEAST, lstr, list_size(&le->params),
		      number_of_args);
    free(lstr);
    return err;
  }

  stack_frame_t new_frame;
  construct_stack_frame(&new_frame, sf);
  for (size_t i = 0; i < list_size(&le->params); i++) {
    object_t arg = evaluated_args[i];
    stack_frame_set_variable(&new_frame, list_get(&le->params, i), arg);
  }
  for (size_t i = 0; i < list_size(&proc->closure); i++) {
    variable_t *var = list_get(&proc->closure, i);
    stack_frame_set_variable(&new_frame, var->name, var->value);
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
