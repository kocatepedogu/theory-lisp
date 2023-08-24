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

/// @file lambda.h

#ifndef THEORYLISP_EXPRESSIONS_LAMBDA_H
#define THEORYLISP_EXPRESSIONS_LAMBDA_H

#include "expression.h"
#include "../types/object.h"
#include "../utils/list.h"
#include "../types/procedure.h"
#include "../interpreter/interpreter.h"
#include "../scanner/scanner.h"

#include <stdbool.h>

/* lambda_expr "new" operation */
exprptr new_lambda_expr(exprptr body, bool variadic, tokenptr tkn);

/* Lambda expression "delete" operation */
void destroy_lambda_expr(exprptr self);

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

/* Returns whether lambda is variadic */
bool lambda_expr_is_variadic(exprptr self);

/* Lambda expression tostring implementation */
char *lambda_expr_tostring(exprptr self);

/* lambda_expr parser */
exprptr lambda_expr_parse(tokenstreamptr tkns);

/* true if e is lambda expression */
bool is_lambda_expr(exprptr e);

/* evaluates lambda expression to an object value */
objectptr lambda_interpret(exprptr self, stack_frame_ptr sf);

/* evaluates a lambda with arguments */
objectptr lambda_call(exprptr lambda, size_t nargs,
                     objectptr *args, stack_frame_ptr sf);

#endif
