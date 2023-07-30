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

/* "new" operation for cond_expr */
exprptr new_cond_expr(void);

/* "delete" operation for cond_expr */
void delete_cond_expr(exprptr self);

/* clone for cond_expr */
exprptr clone_cond_expr(exprptr self);

/* adds a new case to cond_expr */
void cond_expr_add_case(exprptr self, exprptr condition,
    exprptr true_case);

/* cond_expr tostring implementation */
char *cond_expr_tostring(exprptr self);

/* cond_expr parser */
exprptr cond_expr_parse(listptr tokens, int *index);

/* true if e is cond_expr */
bool is_cond_expr(exprptr e);

/* evaluates cond expression */
object_t interpret_cond(exprptr self, stack_frame_ptr sf);

#endif
