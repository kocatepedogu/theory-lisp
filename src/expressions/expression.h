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

/// @file expression.h

#ifndef THEORYLISP_EXPRESSIONS_EXPRESSION_H
#define THEORYLISP_EXPRESSIONS_EXPRESSION_H

#include "../utils/list.h"
#include "../types/object.h"
#include "../scanner/scanner.h"

struct stack_frame;
typedef struct stack_frame *stack_frame_ptr;

struct expr;
typedef struct expr *exprptr;

/* Expression clone */
exprptr clone_expr(exprptr self);

/* Expression "delete" operation */
void delete_expr(exprptr self);

/* Expression tostring */
char *expr_tostring(exprptr self);

/* Expression parser */
exprptr expr_parse(tokenstreamptr tkns);

/* Expression interpreter */
objectptr interpret_expr(exprptr self, stack_frame_ptr sf);

/* Expression function call operator */
objectptr expr_call(exprptr e, size_t nargs,
                   objectptr *args, stack_frame_ptr sf);

/* Internal expression call operator */
objectptr expr_call_internal(exprptr e, void *args, stack_frame_ptr sf);

/* If the expression is callable, returns arity. */
size_t expr_get_arity(exprptr e);

/* If the expression is callable, returns PN arity */
size_t expr_get_pn_arity(exprptr e);

#endif
