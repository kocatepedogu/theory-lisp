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

/// @file evaluation_expr.h

#ifndef EVALUATION_EXPR_H
#define EVALUATION_EXPR_H

#include "../utils/list.h"
#include "../interpreter/interpreter.h"
#include <stdbool.h>

struct expr;
typedef struct expr *exprptr;

/* (proc arg1 arg2 arg3 ...) */
typedef struct {
  exprptr procexpr;
  list arguments; /* list of exprptr's */
} evaluation_expr;

/* evaluation_expr constructor */
void construct_evaluation_expr(exprptr e, exprptr proc);

/* evaluation_expr "new" operation */
exprptr new_evaluation_expr(exprptr proc);

/* evaluation_expr destructor */
void destroy_evaluation_expr(exprptr e);

/* evaluation_expr "delete" operation */
void delete_evaluation_expr(exprptr e);

/* evaluation_expr clone */
exprptr clone_evaluation_expr(exprptr e);

/* Adds an argument to an evulation expression */
void evaluation_expr_add_arg(exprptr e, exprptr argument);

/* evalation_expr tostring implementation */
char *evaluation_expr_tostring(exprptr e);

/* evaluation_expr parser */
exprptr evaluation_expr_parse(list *tokens, int *index);

/* true if e is evaluation expression */
bool is_evaluation_expr(exprptr e);

/* evaluates evaluation expression */
object_t interpret_evaluation(exprptr e, stack_frame_ptr sf);

#endif
