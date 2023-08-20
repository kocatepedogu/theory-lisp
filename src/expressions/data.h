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

/// @file data.h

#ifndef THEORYLISP_EXPRESSIONS_DATA_H
#define THEORYLISP_EXPRESSIONS_DATA_H

#include "expression.h"
#include "../types/object.h"
#include "../utils/list.h"
#include "../interpreter/interpreter.h"

#include <stdbool.h>


/* data_expr "new" operation */
exprptr new_data_expr(objectptr obj, tokenptr tkn);

/* data_expr "delete" operation */
void destroy_data_expr(exprptr self);

/* data_expr tostring implementation */
char *data_expr_tostring(exprptr self);

/* returns stored value */
objectptr get_data_value(exprptr self); 

/* true if e is data expression */
bool is_data_expr(exprptr e);

/* evaluates data expression */
objectptr interpret_data(exprptr self, stack_frame_ptr sf);

#endif
