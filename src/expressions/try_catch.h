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

/// @file try_catch.h

#ifndef THEORYLISP_EXPRESSIONS_TRY_CATCH_H
#define THEORYLISP_EXPRESSIONS_TRY_CATCH_H

#include "expression.h"

#include "../types/object.h"
#include "../utils/list.h"
#include "../interpreter/interpreter.h"
#include "../scanner/scanner.h"

/* try-catch expression "new" operation */
exprptr new_try_catch_expr(exprptr body, const char *var_name, 
                           exprptr handler, tokenptr tkn);

/* try-catch expression "delete" operation */
void destroy_try_catch_expr(exprptr self);

/* try-catch expression tostring implementation */
char *try_catch_expr_tostring(exprptr self);

/* try-catch expression parser */
exprptr try_catch_expr_parse(tokenstreamptr tkns, stack_frame_ptr sf);

/* true if e is try-catch expression */
bool is_try_catch_expr(exprptr e);

/* evaluates try-catch expression */
objectptr interpret_try_catch_expr(exprptr self, stack_frame_ptr ptr);

#endif
