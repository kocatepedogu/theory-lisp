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

#include "evaluation.h"

#include <stdio.h>
#include <string.h>


#include "../builtin/arithmetic.h"
#include "../builtin/builtin.h"
#include "../builtin/display.h"
#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/error.h"
#include "../types/procedure.h"
#include "../types/null.h"
#include "../types/pair.h"
#include "../utils/string.h"
#include "../utils/list.h"
#include "expanded.h"
#include "expression.h"
#include "expression_base.h"
#include "identifier.h"
#include "lambda.h"

#define ERR_PROC_NAME_IS_NOT_ID "Procedure name must be an identifier"
#define ERR_UNEXPECTED_EOF "Unexpected end of file"
#define ERR_INT_AS_PROCEDURE "%ld cannot be used as a procedure name"
#define ERR_REAL_AS_PROCEDURE "%f cannot be used as a procedure name"
#define ERR_SYM_AS_PROCEDURE "'%s cannot be used as a procedure name"
#define ERR_ARITY "Arity mismatch: %s takes %d arguments, but %d given"
#define ERR_ARITY_AT_LEAST "Arity mismatch: %s takes at least %d arguments, but %d given"
#define ERR_NOT_PROCEDURE "%s is not a procedure"
#define ERR_EXPANDED_NOT_LIST "Expanded expression does not yield a list"

/* (proc arg1 arg2 arg3 ...) */
typedef struct {
  exprptr procexpr;
  listptr arguments; /* list of exprptr's */
} evaluation_expr;

static const char evaluation_expr_name[] = "evaluation_expr";

static const expr_vtable evaluation_expr_vtable = {
    .destroy = destroy_evaluation_expr,
    .to_string = evaluation_expr_tostring,
    .interpret = interpret_evaluation};

inline bool is_evaluation_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, evaluation_expr_name) == 0;
}

exprptr new_evaluation_expr(exprptr proc) {
  evaluation_expr *ee = malloc(sizeof *ee);
  ee->procexpr = proc;
  ee->arguments = new_list();
  
  return expr_base_new(ee, &evaluation_expr_vtable, evaluation_expr_name, 0, 0);
}

void destroy_evaluation_expr(exprptr self) {
  evaluation_expr *expr = self->data;
  delete_expr(expr->procexpr);
  for (size_t i = 0; i < list_size(expr->arguments); i++) {
    delete_expr(list_get(expr->arguments, i));
  }
  delete_list(expr->arguments);
  free(expr);
}

void evaluation_expr_add_arg(exprptr self, exprptr argument) {
  evaluation_expr *ee = self->data;
  list_add(ee->arguments, argument);
}

char *evaluation_expr_tostring(exprptr self) {
  evaluation_expr *expr = self->data;
  char *args = NULL;
  for (size_t i = 0; i < list_size(expr->arguments); ++i) {
    char *str = expr_tostring(list_get(expr->arguments, i));
    args = unique_append_sep(args, " ", str);
  }

  char *proc = expr_tostring(expr->procexpr);
  if (args) {
    return unique_format("(%s %s)", proc, args);
  } else {
    return unique_format("(%s)", proc);
  }
}

exprptr evaluation_expr_parse(tokenstreamptr tkns) {
  tokenptr tkn = current_tkn(tkns);
  if (tkn->type == TOKEN_INTEGER) {
    return parser_error(tkn, ERR_INT_AS_PROCEDURE, tkn->value.integer);
  }
  if (tkn->type == TOKEN_REAL) {
    return parser_error(tkn, ERR_REAL_AS_PROCEDURE, tkn->value.real);
  }
  if (tkn->type == TOKEN_STRING) {
    return parser_error(tkn, ERR_SYM_AS_PROCEDURE, tkn->value.character_sequence);
  }

  exprptr proc_expr = expr_parse(tkns);
  if (proc_expr == NULL) {
    return NULL;
  }

  exprptr eval_expr = new_evaluation_expr(proc_expr);
  while (current_tkn(tkns)->type != TOKEN_RIGHT_PARENTHESIS) {
    exprptr arg = expr_parse(tkns);
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

static objectptr interpret_args(listptr arglist, size_t *argsize,
                               stack_frame_ptr sf, objectptr **args) {
  objectptr error = make_null();

  listptr evaluated_arglist = new_list();
  for (size_t i = 0; i < list_size(arglist); ++i) {
    exprptr expr = list_get(arglist, i);

    if (is_expanded_expression(expr)) {
      objectptr list_object = interpret_expr(expr->data, sf);
      if (!is_error(error) && is_error(list_object)) {
        assign_object(&error, clone_object(list_object));
      }

      if (!cons_list_to_internal_list(list_object, evaluated_arglist)) {
        if (!is_error(error)) {
          assign_object(&error, make_error(ERR_EXPANDED_NOT_LIST));
        }
      }

      delete_object(list_object);
    } else {
      objectptr value = interpret_expr(expr, sf);
      if (!is_error(error) && is_error(value)) {
        assign_object(&error, clone_object(value));
      }

      list_add(evaluated_arglist, value);
    }
  }

  *argsize = list_size(evaluated_arglist);
  *args = malloc(*argsize * sizeof(objectptr));

  for (size_t i = 0; i < *argsize; ++i) {
    (*args)[i] = list_get(evaluated_arglist, i);
  }

  delete_list(evaluated_arglist);
  return error;
}

static bool interpret_builtin_call(exprptr procexpr, stack_frame_ptr sf,
                                   size_t argsize, objectptr *evaluated_args,
                                   objectptr *result) {
  bool is_builtin_call = false;

  if (is_identifier_expr(procexpr)) {
    const char *name = identifier_expr_get_name(procexpr);
    const builtin_function *func = find_builtin_function(name);
    if (func) {
      is_builtin_call = true;

      if (func->variadic && func->arity > argsize) {
        objectptr error = make_error(ERR_ARITY_AT_LEAST, name, func->arity, argsize);
        assign_object(result, error);
      } else if (!func->variadic && func->arity != argsize) {
        objectptr error = make_error(ERR_ARITY, name, func->arity, argsize);
        assign_object(result, error);
      } else {
        assign_object(result, func->func(argsize, evaluated_args, sf));
      }
    }
  }

  return is_builtin_call;
}

static objectptr interpret_proc_call(exprptr procexpr, stack_frame_ptr sf,
                                      size_t args_size, objectptr *args) {
  objectptr proc = interpret_expr(procexpr, sf);
  if (is_error(proc)) {
    return proc;
  }

  objectptr result = object_op_call(proc, args_size, args, sf);
  delete_object(proc);
  return result;
}

objectptr interpret_evaluation(exprptr self, stack_frame_ptr sf) {
  objectptr result = NULL;

  evaluation_expr *evaluation_expr = self->data;
  exprptr proc = evaluation_expr->procexpr;
  listptr arglist = evaluation_expr->arguments;
  size_t argsize = 0;

  objectptr *args = NULL;
  if (!is_error(result = interpret_args(arglist, &argsize, sf, &args))) {
    if (!interpret_builtin_call(proc, sf, argsize, args, &result)) {
      assign_object(&result, interpret_proc_call(proc, sf, argsize, args));
    }
  }

  for (size_t i = 0; i < argsize; ++i) {
    delete_object(args[i]);
  }
  free(args);
  return result;

  return NULL;
}
