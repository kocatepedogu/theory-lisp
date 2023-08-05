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

/// @file if_expr.h

#ifndef THEORYLISP_EXPRESSIONS_IF_H
#define THEORYLISP_EXPRESSIONS_IF_H

#include "expression.h"
#include "../utils/list.h"

#include <stdbool.h>

#include "../interpreter/interpreter.h"
#include "../scanner/scanner.h"

/* if_expr "new" operation */
exprptr new_if_expr(exprptr condition, exprptr true_case, exprptr false_case);

/* if_expr "delete" operation */
void delete_if_expr(exprptr self);

/* if_expr clone */
exprptr clone_if_expr(exprptr self);

/* if_expr tostring */
char *if_expr_tostring(exprptr self); 

/* if_expr parser */
exprptr if_expr_parse(tokenstreamptr tkns);

/* true if e is if_expr */
bool is_if_expr(exprptr e);

/* evaluates if expression */
object_t interpret_if(exprptr self, stack_frame_ptr sf);

#endif
