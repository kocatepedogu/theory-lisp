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

#ifndef THEORYLISP_EXPRESSIONS_EXPRESSION_BASE_H
#define THEORYLISP_EXPRESSIONS_EXPRESSION_BASE_H

/**
 * @file expression_base.h
 * This file is not part of the library interface.
 * This file should be included only by the subclasses of expression.
 */

#include <stdlib.h>

#include "../interpreter/stack_frame.h"
#include "../types/object.h"
#include "expression.h"

/* Expression vtable */
typedef struct {
  exprptr (*clone)(exprptr e);
  void (*destroy)(exprptr e);
  void (*deallocate)(exprptr e);
  char *(*to_string)(exprptr e);
  objectptr (*interpret)(exprptr e, stack_frame_ptr sf);
  objectptr (*call)(exprptr e, size_t nargs, objectptr *args, stack_frame_ptr sf);
  objectptr (*call_internal)(exprptr e, void *args, stack_frame_ptr sf);
} expr_vtable;

/* Expression */
typedef struct expr {
  void *data;
  const expr_vtable *vtable;
  const char *expr_name;
  size_t line_number;
  size_t column_number;
  size_t ref_count;
} expr_t;

typedef expr_t *exprptr;

/* Expression base constructor */
exprptr expr_base_new(void *data, const expr_vtable *vtable, const char *expr_name,
                 size_t line_number, size_t column_number);

/* Expression base copy constructor */
exprptr expr_base_clone(exprptr other, void *new_data);

#endif
