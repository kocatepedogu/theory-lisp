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

#ifndef DEFINITION_EXPR_H
#define DEFINITION_EXPR_H

#include "../types/types.h"
#include "../utils/list.h"
#include "../interpreter/interpreter.h"

struct expr;
typedef struct expr *exprptr;

/* (define name expr) */
typedef struct {
  char *name;
  exprptr value;
} definition_expr;

/* definition_expr constructor */
void construct_definition_expr(exprptr e, const char *name, exprptr body);

/* definition_expr "new" operation */
exprptr new_definition_expr(const char *name, exprptr body);

/* definition_expr destructor */
void destroy_definition_expr(exprptr e);

/* definiton_expr "delete" operation */
void delete_definition_expr(exprptr e);

/* definition_expr tostring implementation */
char *definition_expr_tostring(exprptr e);

/* definition_expr parser */
exprptr definition_expr_parse(list *tokens, int *index);

/* true if e is definition expression */
bool is_definition_expr(exprptr e);

/* evaluates definition expression */
object_t interpret_definition(exprptr e, stack_frame_ptr ptr);

#endif
