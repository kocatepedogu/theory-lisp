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

/// @file if_expr.h

#ifndef IF_EXPR_H
#define IF_EXPR_H

#include "../utils/list.h"
#include <stdbool.h>
#include "../interpreter/interpreter.h"

struct expr;
typedef struct expr *exprptr;

/* (if (cond) true-case false-case) */
typedef struct {
  exprptr condition;
  exprptr true_case;
  exprptr false_case;
} if_expr;

/* if_expr constructor */
void construct_if_expr(exprptr e, exprptr condition, exprptr true_case,
                           exprptr false_case);

/* if_expr "new" operation */
exprptr new_if_expr(exprptr condition, exprptr true_case, exprptr false_case);

/* if_expr destructor */
void destroy_if_expr(exprptr e);

/* if_expr "delete" operation */
void delete_if_expr(exprptr e);

/* if_expr clone */
exprptr clone_if_expr(exprptr e);

/* if_expr tostring */
char *if_expr_tostring(exprptr e); 

/* if_expr parser */
exprptr if_expr_parse(list *tokens, int *index);

/* true if e is if_expr */
bool is_if_expr(exprptr e);

/* evaluates if expression */
object_t interpret_if(exprptr e, stack_frame_ptr sf);

#endif
