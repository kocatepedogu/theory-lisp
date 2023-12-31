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

/// @file builtin.h

#ifndef THEORYLISP_BUILTIN_BUILTIN_H
#define THEORYLISP_BUILTIN_BUILTIN_H

#include "../types/object.h"

#include "arithmetic.h"
#include "io.h"
#include "object.h"
#include "boolean.h"
#include "list.h"
#include "string.h"
#include "eval.h"
#include "error.h"
#include "math.h"
#include "macro_utils.h"

typedef objectptr (*builtin_function_ptr)(size_t, objectptr *, stack_frame_ptr);

typedef struct {
  const char *name;
  builtin_function_ptr func;
  size_t arity;
  size_t pn_arity;
  bool variadic;
} builtin_function;

const builtin_function *find_builtin_function(const char *name);

bool is_builtin_name(const char *name);

void define_builtin_function_wrappers(stack_frame_ptr sf);

objectptr builtin_begin(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_begin0(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_void(size_t n, objectptr *args, stack_frame_ptr sf);

#endif
