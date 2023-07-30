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

#define ERR_PN_MISMATCH \
  "Explicit PN arity does not match with the number of formal parameters in non-variadic function"

#define ERR_ARITY_FOLLOW_SLASH \
  "An integer arity value must follow backslash symbol in a lambda expression"

/* (lambda (p1 p2 .. pn) (expr)) */
typedef struct {
  bool variadic;
  size_t pn_arity;
  bool pn_given;
  listptr captured_vars; /* list of char*'s */
  listptr params; /* list of char*'s */
  exprptr body;
} lambda_expr;

static const expr_vtable lambda_expr_vtable = {
    .deallocate = delete_lambda_expr,
    .clone = clone_lambda_expr,
    .to_string = lambda_expr_tostring,
    .interpret = interpret_lambda};

exprptr new_lambda_expr(exprptr body, bool variadic) {
  lambda_expr *le = malloc(sizeof(lambda_expr));
  le->params = new_list();
  le->captured_vars = new_list();
  le->body = body;
  le->variadic = variadic;
  le->pn_arity = 0;
  le->pn_given = false;
  
  expr_t *e = (expr_t *)malloc(sizeof(expr_t));
  e->data = le;
  e->vtable = &lambda_expr_vtable;
  e->expr_name = lambda_expr_name;

  return e;
}

void delete_lambda_expr(exprptr self) {
  lambda_expr *expr = self->data;
  delete_expr(expr->body);

  for (size_t i = 0; i < list_size(expr->params); i++) {
    char *param = list_get(expr->params, i);
    free(param);
  }
  delete_list(expr->params);

  for (size_t i = 0; i < list_size(expr->captured_vars); i++) {
    char *captured_var = list_get(expr->captured_vars, i);
    free(captured_var);
  }
  delete_list(expr->captured_vars);

  free(expr);
  free(self);
}

exprptr clone_lambda_expr(exprptr self) {
  lambda_expr *self_le = self->data;
  lambda_expr *new_le = (lambda_expr *)malloc(sizeof(lambda_expr));
  new_le->variadic = self_le->variadic;
  new_le->pn_arity = self_le->pn_arity;
  new_le->pn_given = self_le->pn_given;
  new_le->body = clone_expr(self_le->body);

  new_le->params = new_list();
  for (size_t i = 0; i < list_size(self_le->params); i++) {
    char *param = list_get(self_le->params, i);
    list_add(new_le->params, strdup(param));
  }

  new_le->captured_vars = new_list();
  for (size_t i = 0; i < list_size(self_le->captured_vars); i++) {
    char *captured_var = list_get(self_le->captured_vars, i);
    list_add(new_le->captured_vars, strdup(captured_var));
  }

  return base_clone(self, new_le);
}

void lambda_expr_add_param(exprptr self, const char *name) {
  lambda_expr *le = self->data;
  list_add(le->params, strdup(name));
}

void lambda_expr_add_captured_var(exprptr self, const char *name) {
  lambda_expr *le = self->data;
  list_add(le->captured_vars, strdup(name));
}

void lambda_expr_set_pn_arity(exprptr self, size_t value) {
  lambda_expr *le = self->data;
  le->pn_arity = value;
  le->pn_given = true;
}

size_t lambda_expr_get_pn_arity(exprptr self) {
  lambda_expr *le = self->data;
  return le->pn_arity;
}

size_t lambda_expr_get_arity(exprptr self) {
  lambda_expr *le = self->data;
  return list_size(le->params);
}

bool lambda_expr_is_pn_arity_given(exprptr self) {
  lambda_expr *le = self->data;
  return le->pn_given;
}

bool lambda_expr_is_variadic(exprptr self) {
  lambda_expr *le = self->data;
  return le->variadic;
}

char *lambda_expr_tostring_param_list(listptr lst) {
  char *result = NULL;
  for (size_t i = 0; i < list_size(lst); i++) {
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

char *lambda_expr_tostring(exprptr self) {
  lambda_expr *expr = self->data;

  char *captures_str = lambda_expr_tostring_param_list(expr->captured_vars);
  char *params_str = lambda_expr_tostring_param_list(expr->params);

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
  char *result = NULL;
  if (expr->variadic && expr->pn_given) {
    result = heap_format("(lambda\\%ld %s%s%s", expr->pn_arity, captures, params, body);
  } else {
    result = heap_format("(lambda %s%s%s)", captures, params, body);
  }

  free(captures);
  free(params);
  free(body);

  return result;
}

static bool lambda_expr_parse_get_pn_arity(listptr tokens, int *index, 
                                           size_t *pn_arity, bool *pn_given) {
  token_t *backslash = list_get(tokens, *index);

  if (backslash->type == TOKEN_BACKSLASH) {
    (*index)++;

    token_t *arity_token = list_get(tokens, (*index)++);
    if (arity_token->type == TOKEN_INTEGER) {
      *pn_arity = arity_token->value.integer;
      *pn_given = true;
    } else {
      parser_error(backslash->line, backslash->column, ERR_ARITY_FOLLOW_SLASH);
      return true;
    }
  }

  return false;
}

static listptr lambda_expr_parse_get_captures(listptr tokens, int *index) {
  listptr captured_variables = new_list();

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

static listptr lambda_expr_parse_get_params(listptr tokens, int *index, bool *variadic) {
  token_t *left_par = list_get(tokens, (*index)++);
  if (left_par->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_par->line, left_par->column, ERR_NO_PARAM);
  }

  listptr formal_parameters = new_list();
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

exprptr lambda_expr_parse(listptr tokens, int *index) {
  /* Read lambda token */
  token_t *lambda_token = list_get(tokens, (*index)++);
  assert(lambda_token->type == TOKEN_LAMBDA);

  /* Read PN arity if given */
  size_t pn_arity = 0;
  bool pn_given = false;
  if (lambda_expr_parse_get_pn_arity(tokens, index, &pn_arity, &pn_given)) {
    return NULL;
  }

  /* Read captured variables if given */
  listptr captured_variables = lambda_expr_parse_get_captures(tokens, index);
  if (!captured_variables) {
    return NULL;
  }

  /* Read formal parameters */
  bool variadic = false;
  listptr formal_parameters = lambda_expr_parse_get_params(tokens, index, &variadic);
  if (!formal_parameters) {
    delete_list(captured_variables);
    return NULL;
  }

  /* Check and correct PN arity according to the number of formal parameters*/
  if (!variadic) {
    if (pn_given) {
      if (pn_arity != list_size(formal_parameters)) {
        delete_list(captured_variables);
        delete_list(formal_parameters);
        return parser_error(lambda_token->line, lambda_token->column, ERR_PN_MISMATCH);
      }
    } else {
      pn_arity = list_size(formal_parameters);
    }
  }

  /* Read body */
  exprptr body = expr_parse(tokens, index);
  if (body == NULL) {
    delete_list(formal_parameters);
    delete_list(captured_variables);
    return NULL;
  }

  /* Construct lambda expression */
  exprptr lambda_expr = new_lambda_expr(body, variadic);

  lambda_expr->line_number = lambda_token->line;
  lambda_expr->column_number = lambda_token->column;

  if (pn_given) {
    lambda_expr_set_pn_arity(lambda_expr, pn_arity);
  }

  for (size_t i = 0; i < list_size(formal_parameters); i++) {
    lambda_expr_add_param(lambda_expr, list_get(formal_parameters, i));
  }

  for (size_t i = 0; i < list_size(captured_variables); i++) {
    lambda_expr_add_captured_var(lambda_expr, list_get(captured_variables, i));
  }

  /* Cleaning */
  delete_list(formal_parameters);
  delete_list(captured_variables);

  return lambda_expr;
}

bool is_lambda_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, lambda_expr_name) == 0;
}

object_t interpret_lambda(exprptr self, stack_frame_ptr sf) {
  lambda_expr *le = self->data;
  
  /* Capture variables from the environment */
  for (size_t i = 0; i < list_size(le->captured_vars); i++) {
    const char *name = list_get(le->captured_vars, i);
    if (!stack_frame_defined(sf, name)) {
      return make_error("Captured variable %s does not exist.", name);
    }
  }

  object_t procedure = make_procedure(self);
  for (size_t i = 0; i < list_size(le->captured_vars); i++) {
    const char *name = list_get(le->captured_vars, i);
    object_t value = stack_frame_get_variable(sf, name);
    assert(!is_error(value));
    procedure_add_closure_variable(procedure, name, value);
    destroy_object(value);
  }

  return procedure;
}

object_t call_lambda(exprptr lambda, listptr closure, 
                     size_t number_of_args, object_t *evaluated_args,
                     stack_frame_ptr sf) {
  lambda_expr *le = lambda->data;
  if (!le->variadic && list_size(le->params) != number_of_args) {
    char *lstr = lambda_expr_tostring(lambda);
    object_t err = make_error(ERR_ARITY_MISMATCH, lstr, list_size(le->params),
                      number_of_args);
    free(lstr);
    return err;
  }
  if (le->variadic && list_size(le->params) > number_of_args) {
    char *lstr = lambda_expr_tostring(lambda);
    object_t err = make_error(ERR_ARITY_AT_LEAST, lstr, list_size(le->params),
		      number_of_args);
    free(lstr);
    return err;
  }

  stack_frame_ptr new_frame = new_stack_frame(sf);
  for (size_t i = 0; i < list_size(le->params); i++) {
    object_t arg = evaluated_args[i];
    stack_frame_set_variable(new_frame, list_get(le->params, i), arg);
  }
  for (size_t i = 0; i < list_size(closure); i++) {
    variableptr var = list_get(closure, i);
    const char *name = variable_get_name(var);
    object_t value = variable_get_value(var);
    stack_frame_set_variable(new_frame, name, value);
  }

  if (le->variadic) {
    size_t fixed_param_size = list_size(le->params);
    object_t *varargs = evaluated_args + fixed_param_size;
    size_t varsize = number_of_args - fixed_param_size;
    object_t args_object = builtin_list(varsize, varargs, sf);
    stack_frame_set_variable(new_frame, "va_args", args_object);
    destroy_object(args_object); 
  }

  object_t result = interpret_expr(le->body, new_frame);
  delete_stack_frame(new_frame);
  return result;
}
