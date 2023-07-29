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

/// @file let_expr.h

#ifndef LET_EXPR_H
#define LET_EXPR_H

#include "../types/object.h"
#include "../utils/list.h"
#include "../interpreter/interpreter.h"

struct expr;
typedef struct expr *exprptr;

/* (var val) */
typedef struct {
  char *name;
  exprptr value;
} var_declaration;

/* (let ((var1 val1) (var2 val2) ...) (body)) */
typedef struct {
  list declarations; /* list of var_declaration*'s */
  exprptr body;
} let_expr;

/* let_expr constructor */
void construct_let_expr(exprptr e, exprptr body);

/* let_expr "new" operation */
exprptr new_let_expr(exprptr body);

/* let_expr destructor */
void destroy_let_expr(exprptr e);

/* let_expr "delete" operation */
void delete_let_expr(exprptr e);

/* let_expr clone */
exprptr clone_let_expr(exprptr e);

/* adds a variable declaration to the let block */
void let_expr_add_declaration(exprptr let_expr, const char *name, 
    exprptr expr);

/* let_expr tostring implementation */
char *let_expr_tostring(exprptr e);

/* let_expr parser */
exprptr let_expr_parse(list *tokens, int *index);

/* true if e is let_expr */
bool is_let_expr(exprptr e);

/* evaluates let expression */
object_t interpret_let(exprptr e, stack_frame_ptr sf);

#endif
