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

/// @file string.h

#ifndef THEORYLISP_BUILTIN_STRING_H
#define THEORYLISP_BUILTIN_STRING_H

#include "../types/string.h"
#include "../interpreter/stack_frame.h"

objectptr builtin_strlen(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_strcat(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_charat(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_substr(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_strcar(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_strcdr(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_newline(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_tab(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_backspace(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_quotation_mark(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_i2s(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_s2i(size_t n, objectptr *args, stack_frame_ptr sf);

#endif
