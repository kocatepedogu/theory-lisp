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

/* lambda_expr "new" operation */
exprptr new_lambda_expr(exprptr body, bool variadic);

/* Lambda expression "delete" operation */
void delete_lambda_expr(exprptr self);

/* Lambda expression clone */
exprptr clone_lambda_expr(exprptr self);

/* Adds a formal parameter to the lambda function */
void lambda_expr_add_param(exprptr self, const char *name);

/* Adds a captured variable to the lambda function */
void lambda_expr_add_captured_var(exprptr self, const char *name);

/* Changes Polish Notation arity of the lambda function */
void lambda_expr_set_pn_arity(exprptr self, size_t arity);

/* Returns Polish Notation arity of the lambda function */
size_t lambda_expr_get_pn_arity(exprptr self);

/* Returns arity of the lambda function */
size_t lambda_expr_get_arity(exprptr self);

/* Returns whether PN arity is given */
bool lambda_expr_is_pn_arity_given(exprptr self);

/* Returns whether lambda is variadic */
bool lambda_expr_is_variadic(exprptr self);

/* Lambda expression tostring implementation */
char *lambda_expr_tostring(exprptr self);

/* lambda_expr parser */
exprptr lambda_expr_parse(listptr tokens, int *index);

/* true if e is lambda expression */
bool is_lambda_expr(exprptr e);

/* evaluates lambda expression to an object value */
object_t interpret_lambda(exprptr self, stack_frame_ptr sf);

/* evaluates a lambda with arguments */
object_t call_lambda(exprptr lambda, listptr closure, size_t number_of_args,
                         object_t *args, stack_frame_ptr sf);

#endif
