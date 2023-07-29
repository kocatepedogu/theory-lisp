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

/// @file lambda_expr.h

#ifndef LAMBDA_EXPR_H
#define LAMBDA_EXPR_H

#include "../types/object.h"
#include "../utils/list.h"
#include "../types/procedure.h"
#include "../interpreter/interpreter.h"

#include <stdbool.h>

struct expr;
typedef struct expr *exprptr;

/* (lambda (p1 p2 .. pn) (expr)) */
typedef struct {
  bool variadic;
  list captured_vars; /* list of char*'s */
  list params; /* list of char*'s */
  exprptr body;
} lambda_expr;

/* lambda_expr constructor */
void construct_lambda_expr(exprptr e, exprptr body, bool variadic);

/* lambda_expr "new" operation */
exprptr new_lambda_expr(exprptr body, bool variadic);

/* Lambda expression destructor */
void destroy_lambda_expr(exprptr e);

/* Lambda expression delete operation */
void delete_lambda_expr(exprptr e);

/* Lambda expression clone */
exprptr clone_lambda_expr(exprptr e);

/* Adds a formal parameter to the lambda function */
void lambda_expr_add_param(exprptr e, const char *name);

/* Lambda expression tostring implementation */
char *lambda_expr_tostring(exprptr e);

/* lambda_expr parser */
exprptr lambda_expr_parse(list *tokens, int *index);

/* true if e is lambda expression */
bool is_lambda_expr(exprptr e);

/* evaluates lambda expression to an object value */
object_t interpret_lambda(exprptr e, stack_frame_ptr sf);

/* evaluates a lambda with arguments */
object_t call_lambda(proc_t *proc, size_t number_of_args,
                         object_t *args, stack_frame_ptr sf);

#endif
