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

#include "evaluation_expr.h"

#include <stdio.h>
#include <string.h>

#include "../builtin/arithmetic.h"
#include "../builtin/builtin.h"
#include "../builtin/display.h"
#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/error.h"
#include "../types/procedure.h"
#include "../types/void.h"
#include "../types/pair.h"
#include "../utils/heap-format.h"
#include "../utils/list.h"
#include "expanded_expr.h"
#include "expression.h"
#include "identifier_expr.h"
#include "lambda_expr.h"

#define ERR_PROC_NAME_IS_NOT_ID "Procedure name must be an identifier"
#define ERR_UNEXPECTED_EOF "Unexpected end of file"
#define ERR_INT_AS_PROCEDURE "%ld cannot be used as a procedure name"
#define ERR_REAL_AS_PROCEDURE "%f cannot be used as a procedure name"
#define ERR_SYM_AS_PROCEDURE "'%s cannot be used as a procedure name"
#define ERR_ARITY "Arity mismatch: %s takes %d arguments, but %d given"
#define ERR_ARITY_AT_LEAST "Arity mismatch: %s takes at least %d arguments, but %d given"
#define ERR_NOT_PROCEDURE "%s is not a procedure"
#define ERR_EXPANDED_NOT_LIST "Expanded expression does not yield a list"

static const char evaluation_expr_name[] = "evaluation_expr";

static const expr_vtable evaluation_expr_vtable = {
    .deallocate = delete_evaluation_expr,
    .destructor = destroy_evaluation_expr,
    .to_string = evaluation_expr_tostring,
    .interpret = interpret_evaluation};

void construct_evaluation_expr(exprptr e, exprptr proc) {
  evaluation_expr *ee = (evaluation_expr *)malloc(sizeof(evaluation_expr));
  ee->procexpr = proc;
  construct_list(&ee->arguments);
  e->data = ee;
  e->vtable = &evaluation_expr_vtable;
  e->expr_name = evaluation_expr_name;
}

exprptr new_evaluation_expr(exprptr proc) {
  expr_t *e = (expr_t *)malloc(sizeof(expr_t));
  construct_evaluation_expr(e, proc);
  return e;
}

void destroy_evaluation_expr(exprptr e) {
  evaluation_expr *expr = e->data;
  delete_expr(expr->procexpr);
  for (size_t i = 0; i < list_size(&expr->arguments); i++) {
    delete_expr(list_get(&expr->arguments, i));
  }
  destroy_list(&expr->arguments);
  free(expr);
}

void delete_evaluation_expr(exprptr e) {
  destroy_evaluation_expr(e);
  free(e);
}

void evaluation_expr_add_arg(exprptr e, exprptr argument) {
  evaluation_expr *ee = e->data;
  list_add(&ee->arguments, argument);
}

char *evaluation_expr_tostring(exprptr e) {
  evaluation_expr *expr = e->data;
  char *args = NULL;
  for (int i = 0; i < list_size(&expr->arguments); i++) {
    char *str = expr_tostring((exprptr)list_get(&expr->arguments, i));
    if (!args) {
      args = str;
    } else {
      char *new_args = heap_format("%s %s", args, str);
      free(args);
      free(str);
      args = new_args;
    }
  }

  char *proc = expr_tostring(expr->procexpr);
  char *result =
      args ? heap_format("(%s %s)", proc, args) : heap_format("(%s)", proc);

  free(args);
  free(proc);
  return result;
}

exprptr evaluation_expr_parse(list *tokens, int *index) {
  token_t *tkn = list_get(tokens, *index);
  if (tkn->type == TOKEN_INTEGER) {
    return parser_error(tkn->line, tkn->column, ERR_INT_AS_PROCEDURE,
                        tkn->value.integer);
  }
  if (tkn->type == TOKEN_REAL) {
    return parser_error(tkn->line, tkn->column, ERR_REAL_AS_PROCEDURE,
                        tkn->value.real);
  }
  if (tkn->type == TOKEN_SYMBOL) {
    return parser_error(tkn->line, tkn->column, ERR_SYM_AS_PROCEDURE,
                        tkn->value.character_sequence);
  }

  exprptr proc_expr = expr_parse(tokens, index);
  if (proc_expr == NULL) {
    return NULL;
  }

  exprptr eval_expr = new_evaluation_expr(proc_expr);
  while (((token_t *)list_get(tokens, *index))->type !=
         TOKEN_RIGHT_PARENTHESIS) {
    exprptr arg = expr_parse(tokens, index);
    if (arg == NULL) {
      delete_expr(eval_expr);
      return NULL;
    }
    evaluation_expr_add_arg(eval_expr, arg);
  }

  eval_expr->line_number = tkn->line;
  eval_expr->column_number = tkn->column;
  return eval_expr;
}

bool is_evaluation_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, evaluation_expr_name) == 0;
}

static object_t interpret_args(list *arglist, size_t *argsize,
                               stack_frame_ptr sf, object_t **args) {
  object_t error = make_void();

  list *evaluated_arglist = new_list();
  for (size_t i = 0; i < list_size(arglist); i++) {
    exprptr expr = list_get(arglist, i);

    if (is_expanded_expression(expr)) {
      object_t list_object = interpret_expr(expr->data, sf);
      if (!is_error(error) && is_error(list_object)) {
	assign_object(&error, copy_object(list_object));
      }

      if (!cons_list_to_internal_list(list_object, evaluated_arglist)) {
	if (!is_error(error)) {
	  assign_object(&error, make_error(ERR_EXPANDED_NOT_LIST));
	}
      }

      destroy_object(list_object);

    } else {
      object_t *value = (object_t *)malloc(sizeof(object_t));
      *value = interpret_expr(expr, sf);
      if (!is_error(error) && is_error(*value)) {
	assign_object(&error, copy_object(*value));
      }

      list_add(evaluated_arglist, value);
    }
  }

  *argsize = list_size(evaluated_arglist);
  *args = (object_t *)malloc(*argsize * sizeof(object_t));

  for (size_t i = 0; i < *argsize; i++) {
    object_t *ptr = list_get(evaluated_arglist, i);
    (*args)[i] = *ptr;
    free(ptr);
  }

  delete_list(evaluated_arglist);
  return error;
}

static bool interpret_builtin_call(exprptr procexpr, stack_frame_ptr sf,
                                   size_t argsize, object_t *evaluated_args,
                                   object_t *result) {
  bool is_builtin_call = false;

  if (is_identifier_expr(procexpr)) {
    identifier_expr *ie = procexpr->data;

    const builtin_function *func = find_builtin_function(ie->name);
    if (func) {
      is_builtin_call = true;

      if (func->variadic && func->arity > argsize) {
        object_t error = make_error(ERR_ARITY_AT_LEAST, ie->name, func->arity, argsize);
        assign_object(result, error);
      } else if (!func->variadic && func->arity != argsize) {
	object_t error = make_error(ERR_ARITY, ie->name, func->arity, argsize);
	assign_object(result, error);
      } else {
        assign_object(result, func->func(argsize, evaluated_args, sf));
      }
    }
  }

  return is_builtin_call;
}

static object_t interpret_lambda_call(exprptr procexpr, stack_frame_ptr sf,
                                      size_t args_size, object_t *args) {
  object_t proc = interpret_expr(procexpr, sf);
  if (is_error(proc)) {
    return proc;
  }

  if (is_procedure(proc) == false) {
    char *str = object_tostring(proc);
    object_t result = make_error(ERR_NOT_PROCEDURE, str);
    free(str);
    return result;
  }

  return call_lambda(procedure_value(proc), args_size, args, sf);
}

object_t interpret_evaluation(exprptr e, stack_frame_ptr sf) {
  object_t result = make_void();

  evaluation_expr *evaluation_expr = e->data;
  exprptr proc = evaluation_expr->procexpr;
  list *arglist = &evaluation_expr->arguments;
  size_t argsize = 0;

  object_t *args = NULL;
  if (!is_error(result = interpret_args(arglist, &argsize, sf, &args))) {
    if (!interpret_builtin_call(proc, sf, argsize, args, &result)) {
      assign_object(&result, interpret_lambda_call(proc, sf, argsize, args));
    }
  }

  for (size_t i = 0; i < argsize; i++) {
    destroy_object(args[i]);
  }
  free(args);
  return result;
}
