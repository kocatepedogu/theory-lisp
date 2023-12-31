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

/// @file expanded.h

#ifndef THEORYLISP_EXPRESSIONS_EXPANDED_H
#define THEORYLISP_EXPRESSIONS_EXPANDED_H

#include "expression.h"

#include <stdbool.h>


#include "../utils/list.h"
#include "../interpreter/interpreter.h"

/* expanded expression "new" operation */
exprptr new_expanded_expr(exprptr proc, tokenptr tkn);

/* expanded expression "delete" operation */
void destroy_expanded_expr(exprptr self);

/* expanded expression tostring implementation */
char *expanded_expr_tostring(exprptr self);

/* true if e is expanded expression */
bool is_expanded_expression(exprptr e);

/* expanded expression interpreter */
objectptr interpret_expanded_expr(exprptr self, stack_frame_ptr sf);

#endif
