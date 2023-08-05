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

#ifndef THEORYLISP_BUILTIN_ERROR_H
#define THEORYLISP_BUILTIN_ERROR_H

#include "../types/object.h"
#include "../interpreter/stack_frame.h"

object_t builtin_error(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_exit(size_t n, object_t *args, stack_frame_ptr sf);

#endif
