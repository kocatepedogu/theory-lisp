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

/// @file identifier_expr.h

#ifndef IDENTIFIER_EXPR_H
#define IDENTIFIER_EXPR_H

#include <stdbool.h>

#include "../interpreter/interpreter.h"

struct expr;
typedef struct expr *exprptr;

/* identifier */
typedef struct {
  char *name;
} identifier_expr;

/* identifier_expr constructor */
void construct_identifier_expr(exprptr e, const char *name);

/* identifier_expr "new" operation */
exprptr new_identifier_expr(const char *name);

/* identifier_expr destructor */
void destroy_identifier_expr(exprptr e);

/* identifier_expr "delete" operation */
void delete_identifier_expr(exprptr e);

/* identifier_expr clone */
exprptr clone_identifier_expr(exprptr e);

/* identifier_expr tostring implementation */
char *identifier_expr_tostring(exprptr e);

/* true if e is identifier expression */
bool is_identifier_expr(exprptr e);

/* evaluates identifier expression */
object_t interpret_identifier(exprptr e, stack_frame_ptr sf);

#endif
