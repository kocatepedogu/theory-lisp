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

/// @file math.h

#ifndef THEORYLISP_BUILTIN_MATH_H
#define THEORYLISP_BUILTIN_MATH_H

#include "../interpreter/stack_frame.h"

objectptr builtin_cos(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_sin(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_tan(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_acos(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_asin(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_atan(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_atan2(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_cosh(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_sinh(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_tanh(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_acosh(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_asinh(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_atanh(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_exp(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_log(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_log10(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_pow(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_sqrt(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_cbrt(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_hypot(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_erf(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_erfc(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_tgamma(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_lgamma(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_ceil(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_floor(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_trunc(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_round(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_modulo(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_isfinite(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_isinf(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_isnan(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_isnormal(size_t n, objectptr *args, stack_frame_ptr sf);

objectptr builtin_random(size_t n, objectptr *args, stack_frame_ptr sf);

#endif
