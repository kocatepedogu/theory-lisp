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

/// @file definition.h

#ifndef THEORYLISP_EXPRESSIONS_DEFINITION_H
#define THEORYLISP_EXPRESSIONS_DEFINITION_H

#include "expression.h"

#include "../types/object.h"
#include "../utils/list.h"
#include "../interpreter/interpreter.h"
#include "../scanner/scanner.h"

/* definition_expr "new" operation */
exprptr new_definition_expr(const char *name, exprptr body, tokenptr tkn);

/* definiton_expr "delete" operation */
void destroy_definition_expr(exprptr self);

/* definition_expr tostring implementation */
char *definition_expr_tostring(exprptr self);

/* definition_expr parser */
exprptr definition_expr_parse(tokenstreamptr tkns);

/* true if e is definition expression */
bool is_definition_expr(exprptr e);

/* evaluates definition expression */
objectptr interpret_definition(exprptr self, stack_frame_ptr ptr);

#endif
