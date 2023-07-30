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

/// @file data_expr.h

#ifndef DATA_EXPR_H
#define DATA_EXPR_H

#include "../types/object.h"
#include "../utils/list.h"
#include "../interpreter/interpreter.h"

#include <stdbool.h>

struct expr;
typedef struct expr *exprptr;

/* data_expr "new" operation */
exprptr new_data_expr(object_t obj);

/* data_expr "delete" operation */
void delete_data_expr(exprptr self);

/* data_expr clone */
exprptr clone_data_expr(exprptr self);

/* data_expr tostring implementation */
char *data_expr_tostring(exprptr self);

/* returns stored value */
object_t get_data_value(exprptr self); 

/* data_expr parser */
exprptr data_expr_parse(listptr tokens, int *index);

/* true if e is data expression */
bool is_data_expr(exprptr e);

/* evaluates data expression */
object_t interpret_data(exprptr self, stack_frame_ptr sf);

#endif
