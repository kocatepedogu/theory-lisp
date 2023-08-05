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

#ifndef THEORYLISP_UTILS_HEAP_STRING_H
#define THEORYLISP_UTILS_HEAP_STRING_H

#include <stdarg.h>

char *format(const char *format, ...);

char *vformat(const char *format, va_list args);

char *append(const char *first, const char *second);

char *append_sep(const char *first, const char *space, const char *second);

char *unique_vformat(const char *fmt, va_list args);

char *unique_format(const char *fmt, ...);

char *unique_append(char *first, char *second);

char *unique_append_sep(char *first, const char *space, char *second);

#endif
