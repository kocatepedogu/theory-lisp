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

/// @file identifier.h

#ifndef THEORYLISP_EXPRESSIONS_IDENTIFIER_H
#define THEORYLISP_EXPRESSIONS_IDENTIFIER_H

#include "expression.h"

#include <stdbool.h>

#include "../interpreter/interpreter.h"

/* identifier_expr "new" operation */
exprptr new_identifier_expr(const char *name, tokenptr tkn);

/* identifier_expr "delete" operation */
void destroy_identifier_expr(exprptr self);

/* identifier_expr tostring implementation */
char *identifier_expr_tostring(exprptr self);

/* returns name of the identifier */
const char *identifier_expr_get_name(exprptr self);

/* true if e is identifier expression */
bool is_identifier_expr(exprptr e);

/* evaluates identifier expression */
objectptr interpret_identifier(exprptr self, stack_frame_ptr sf);

#endif
