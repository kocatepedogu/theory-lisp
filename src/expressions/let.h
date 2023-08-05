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

/// @file let_expr.h

#ifndef THEORYLISP_EXPRESSIONS_LET_H
#define THEORYLISP_EXPRESSIONS_LET_H

#include "expression.h"
#include "../types/object.h"
#include "../utils/list.h"
#include "../interpreter/interpreter.h"
#include "../scanner/scanner.h"

/* let_expr "new" operation */
exprptr new_let_expr(exprptr body);

/* let_expr "delete" operation */
void delete_let_expr(exprptr self);

/* let_expr clone */
exprptr clone_let_expr(exprptr self);

/* adds a variable declaration to the let block */
void let_expr_add_declaration(exprptr let_expr, const char *name, 
    exprptr expr);

/* let_expr tostring implementation */
char *let_expr_tostring(exprptr self);

/* let_expr parser */
exprptr let_expr_parse(tokenstreamptr tkns);

/* true if e is let_expr */
bool is_let_expr(exprptr e);

/* evaluates let expression */
object_t interpret_let(exprptr self, stack_frame_ptr sf);

#endif
