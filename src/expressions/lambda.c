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

#include "lambda.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/error.h"
#include "../types/procedure.h"
#include "../utils/string.h"
#include "../utils/list.h"
#include "../builtin/list.h"
#include "../interpreter/stack_frame.h"
#include "../interpreter/variable.h"
#include "expression.h"
#include "expression_base.h"
#include "common.h"

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
  listptr captured_vars; /* list of char*'s */
  listptr params; /* list of char*'s */
  exprptr body;
} lambda_expr;

static const expr_vtable lambda_expr_vtable = {
    .destroy = destroy_lambda_expr,
    .to_string = lambda_expr_tostring,
    .interpret = lambda_interpret,
    .call = lambda_call,
    .get_arity = lambda_expr_get_arity,
    .get_pn_arity = lambda_expr_get_pn_arity
};

inline bool is_lambda_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, lambda_expr_name) == 0;
}

exprptr new_lambda_expr(exprptr body, bool variadic, tokenptr tkn) {
  lambda_expr *le = malloc(sizeof *le);
  le->params = new_list();
  le->captured_vars = new_list();
  le->body = body;
  le->variadic = variadic;
  le->pn_arity = 0;

  return expr_base_new(le, &lambda_expr_vtable, lambda_expr_name, tkn);
}

void destroy_lambda_expr(exprptr self) {
  lambda_expr *expr = self->data;
  delete_expr(expr->body);

  for (size_t i = 0; i < list_size(expr->params); ++i) {
    char *param = list_get(expr->params, i);
    free(param);
  }
  delete_list(expr->params);

  for (size_t i = 0; i < list_size(expr->captured_vars); ++i) {
    char *captured_var = list_get(expr->captured_vars, i);
    free(captured_var);
  }
  delete_list(expr->captured_vars);

  free(expr);
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
}

size_t lambda_expr_get_pn_arity(exprptr self) {
  lambda_expr *le = self->data;
  return le->pn_arity;
}

size_t lambda_expr_get_arity(exprptr self) {
  lambda_expr *le = self->data;
  return list_size(le->params);
}

bool lambda_expr_is_variadic(exprptr self) {
  lambda_expr *le = self->data;
  return le->variadic;
}

/**
 * Helper function of lambda_expr_tostring to print a list of strings
 * with spaces between them.
 */
char *lambda_expr_tostring_param_list(listptr lst) {
  char *result = NULL;
  for (size_t i = 0; i < list_size(lst); ++i) {
    char *str = list_get(lst, i);
    result = unique_append_sep(result, " ", strdup(str));
  }
  return result;
}

/**
 * Returns a string representation of lambda that can be
 * parsed to obtain the same lambda expression.
 */
char *lambda_expr_tostring(exprptr self) {
  lambda_expr *expr = self->data;

  char *captures = capture_list_tostring(expr->captured_vars);
  char *params = lambda_expr_tostring_param_list(expr->params);
  if (params) {
    char *fmt = expr->variadic ? "(%s ...) " : "(%s) ";
    params = unique_format(fmt, params);
  } else {
    char *fmt = expr->variadic ? "(...) " : "() ";
    params = unique_format(fmt);
  }

  char *body = expr_tostring(expr->body);
  char *result = NULL;
  if (expr->variadic) {
    result = unique_format("(lambda\\%ld %s%s%s", expr->pn_arity, captures, params, body);
  } else {
    result = unique_format("(lambda %s%s%s)", captures, params, body);
  }

  return result;
}

/**
 * Helper function of lambda_expr_parse to parse explicit PN arity 
 * (optional in lambda syntax)
 */
static bool lambda_expr_parse_get_pn_arity(tokenstreamptr tkns, 
                                           size_t *pn_arity, bool *pn_given) {
  tokenptr backslash = current_tkn(tkns);

  if (backslash->type == TOKEN_BACKSLASH) {
    (void)next_tkn(tkns);

    tokenptr arity_token = next_tkn(tkns);
    if (arity_token->type == TOKEN_INTEGER) {
      *pn_arity = arity_token->value.integer;
      *pn_given = true;
    } else {
      parser_error(backslash, ERR_ARITY_FOLLOW_SLASH);
      return true;
    }
  }

  return false;
}

/**
 * Helper function of lambda_expr_parse to parse formal parameter list.
 */
static listptr lambda_expr_parse_get_params(tokenstreamptr tkns, bool *variadic) {
  tokenptr left_par = next_tkn(tkns);
  if (left_par->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_par, ERR_NO_PARAM);
  }

  listptr formal_parameters = new_list();
  tokenptr param_token = NULL;
  while ((param_token = current_tkn(tkns))->type == TOKEN_IDENTIFIER) {
    if (strcmp(param_token->value.character_sequence, "...") == 0) {
      *variadic = true;
      (void)next_tkn(tkns);
      break;
    }

    list_add(formal_parameters, param_token->value.character_sequence);
    (void)next_tkn(tkns);
  }

  tokenptr right_par = next_tkn(tkns);
  if (right_par->type != TOKEN_RIGHT_PARENTHESIS) {
    delete_list(formal_parameters);
    return parser_error(right_par, ERR_LAMBDA_SYNTAX);
  }

  return formal_parameters;
}

/**
 * Parses lambda expression.
 */
exprptr lambda_expr_parse(tokenstreamptr tkns) {
  /* Read lambda token */
  tokenptr lambda_token = next_tkn(tkns);
  assert(lambda_token->type == TOKEN_LAMBDA);

  /* Read PN arity if given */
  size_t pn_arity = 0;
  bool pn_given = false;
  if (lambda_expr_parse_get_pn_arity(tkns, &pn_arity, &pn_given)) {
    return NULL;
  }

  /* Read captured variables if given */
  listptr captured_variables = get_capture_list(tkns);
  if (!captured_variables) {
    return NULL;
  }

  /* Read formal parameters */
  bool variadic = false;
  listptr formal_parameters = lambda_expr_parse_get_params(tkns, &variadic);
  if (!formal_parameters) {
    delete_list(captured_variables);
    return NULL;
  }

  /* Check and correct PN arity according to the number of formal parameters*/
  if (pn_given) {
    if (variadic) {
      if (pn_arity < list_size(formal_parameters)) {
        delete_list(captured_variables);
        delete_list(formal_parameters);
        return parser_error(lambda_token, "PN arity must be greater than or equal to "
                                          "the number of fixed parameters");
      }
    } else {
      if (pn_arity != list_size(formal_parameters)) {
        delete_list(captured_variables);
        delete_list(formal_parameters);
        return parser_error(lambda_token, ERR_PN_MISMATCH);
      }
    }
  } else {
    pn_arity = list_size(formal_parameters);
  }

  /* Read body */
  exprptr body = expr_parse(tkns);
  if (body == NULL) {
    delete_list(formal_parameters);
    delete_list(captured_variables);
    return NULL;
  }

  /* Construct lambda expression */
  exprptr lambda_expr = new_lambda_expr(body, variadic, lambda_token);
  lambda_expr_set_pn_arity(lambda_expr, pn_arity);

  for (size_t i = 0; i < list_size(formal_parameters); ++i) {
    lambda_expr_add_param(lambda_expr, list_get(formal_parameters, i));
  }

  for (size_t i = 0; i < list_size(captured_variables); ++i) {
    lambda_expr_add_captured_var(lambda_expr, list_get(captured_variables, i));
  }

  /* Cleaning */
  delete_list(formal_parameters);
  delete_list(captured_variables);

  return lambda_expr;
}

/** 
 * Evaluates the lambda function to a procedure object.
 */
objectptr lambda_interpret(exprptr self, stack_frame_ptr sf) {
  lambda_expr *le = self->data;
  return make_procedure(self, le->captured_vars, sf);
}

/** 
 * Calls a lambda.
 */
objectptr lambda_call(exprptr self, size_t nargs, objectptr *args,
                     stack_frame_ptr local_frame) {
  lambda_expr *le = self->data;

  /* Number of fixed parameters */
  size_t nparams = list_size(le->params);
  
  /* If the function has a fixed number of parameters, the number of actual
   * arguments must match the number of formal parameters */
    if (!le->variadic && list_size(le->params) != nargs) {
    char *lstr = lambda_expr_tostring(self);
    objectptr err = make_error(ERR_ARITY_MISMATCH, lstr, nparams, nargs);
    free(lstr);
    return err;
  }

  /* If the function is variadic, but it also has some fixed parameters at the 
   * beginning of the formal parameter list, the number of actual arguments must 
   * be greater than or equal to the number of fixed parameters in the formal 
   * parameter list.*/
  if (le->variadic && list_size(le->params) > nargs) {
    char *lstr = lambda_expr_tostring(self);
    objectptr err = make_error(ERR_ARITY_AT_LEAST, lstr, nparams, nargs);
    free(lstr);
    return err;
  }

  /* Create local variables from actual arguments */
  for (size_t i = 0; i < nparams; ++i) {
    objectptr arg = args[i];
    stack_frame_set_local_variable(local_frame, list_get(le->params, i), arg);
  }

  /* If the function is variadic, create a variable named "va_args" in the local 
   * stack frame. This is a list formed using linked cons pairs containing 
   * actual arguments of the function */
  if (le->variadic) {
    objectptr *va_args = args + nparams;
    objectptr args_object = builtin_list(nargs - nparams, va_args, local_frame);
    stack_frame_set_local_variable(local_frame, "va_args", args_object);
    delete_object(args_object); 
  }

  /* Compute the result */
  return interpret_expr(le->body, local_frame);
}
