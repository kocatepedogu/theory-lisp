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

/// @file evaluation.h

#ifndef THEORYLISP_EXPRESSIONS_EVALUATION_H
#define THEORYLISP_EXPRESSIONS_EVALUATION_H

#include "expression.h"
#include "../utils/list.h"
#include "../interpreter/interpreter.h"
#include "../scanner/scanner.h"

#include <stdbool.h>


/* evaluation_expr "new" operation */
exprptr new_evaluation_expr(exprptr proc, tokenptr tkn);

/* evaluation_expr "delete" operation */
void destroy_evaluation_expr(exprptr self);

/* Adds an argument to an evulation expression */
void evaluation_expr_add_arg(exprptr self, exprptr argument);

/* evalation_expr tostring implementation */
char *evaluation_expr_tostring(exprptr self);

/* evaluation_expr parser */
exprptr evaluation_expr_parse(tokenstreamptr tkns, stack_frame_ptr sf);

/* true if e is evaluation expression */
bool is_evaluation_expr(exprptr e);

/* evaluates evaluation expression */
objectptr interpret_evaluation(exprptr self, stack_frame_ptr sf);

#endif
