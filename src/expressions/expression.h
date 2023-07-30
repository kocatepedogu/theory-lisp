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

/// @file expression.h

#ifndef PARSE_H
#define PARSE_H

#include "../utils/list.h"
#include "../types/object.h"

struct stack_frame;
typedef struct stack_frame *stack_frame_ptr;

struct expr;
typedef struct expr *exprptr;

/* Expression vtable */
typedef struct {
  exprptr (*clone)(exprptr e);
  void (*deallocate)(exprptr e);
  char *(*to_string)(exprptr e);
  object_t (*interpret)(exprptr e, stack_frame_ptr ptr);
} expr_vtable;

/* Expression */
typedef struct expr {
  void *data;
  const expr_vtable *vtable;
  const char *expr_name;
  size_t line_number;
  size_t column_number;
} expr_t;

typedef expr_t *exprptr;

/* Expression base constructor */
exprptr new_expr(void *data, const expr_vtable *vtable, const char *expr_name,
                 size_t line_number, size_t column_number);

/* Expression base copy constructor */
exprptr base_clone(exprptr other, void *new_data);

/* Expression clone */
exprptr clone_expr(exprptr self);

/* Expression "delete" operation */
void delete_expr(exprptr self);

/* Expression tostring */
char *expr_tostring(exprptr self);

/* Expression parser */
exprptr expr_parse(listptr tokens, int *index);

/* Expression interpreter */
object_t interpret_expr(exprptr self, stack_frame_ptr sf);

#endif
