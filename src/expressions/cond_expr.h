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

/// @file cond_expr.h

#ifndef COND_EXPR_H
#define COND_EXPR_H

#include "../interpreter/interpreter.h"
#include "../utils/list.h"
#include <stdbool.h>

struct expr;
typedef struct expr *exprptr;

/* ((cond) expr-if-cond) */
typedef struct {
  exprptr condition;
  exprptr true_case;
} cond_case;

/* (cond ((cond1) expr-if-cond1) ((cond2) expr-if-cond2) ... ) */
typedef struct {
  list cases; /* list of cond_case*'s */
} cond_expr; 

/* cond_expr constructor */
void construct_cond_expr(exprptr e);

/* "new" operation for cond_expr */
exprptr new_cond_expr(void);

/* cond_expr destructor */
void destroy_cond_expr(exprptr e);

/* "delete" operation for cond_expr */
void delete_cond_expr(exprptr e);

/* clone for cond_expr */
exprptr clone_cond_expr(exprptr e);

/* adds a new case to cond_expr */
void cond_expr_add_case(exprptr e, exprptr condition,
    exprptr true_case);

/* cond_expr tostring implementation */
char *cond_expr_tostring(exprptr e);

/* cond_expr parser */
exprptr cond_expr_parse(list *tokens, int *index);

/* true if e is cond_expr */
bool is_cond_expr(exprptr e);

/* evaluates cond expression */
object_t interpret_cond(exprptr e, stack_frame_ptr sf);

#endif
