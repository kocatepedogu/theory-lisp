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

/// @file object.h

#ifndef OBJECT_H
#define OBJECT_H

#include "../types/object.h"
#include "../interpreter/stack_frame.h"

object_t builtin_equals(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_not_equals(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_less(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_less_or_eq(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_greater(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_greater_or_eq(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_is_null(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_is_void(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_is_boolean(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_is_integer(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_is_real(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_is_number(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_is_string(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_is_pair(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_is_procedure(size_t n, object_t *args, stack_frame_ptr sf);

#endif
