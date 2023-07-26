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

/// @file heap-format.h

#ifndef HEAP_FORMAT_H
#define HEAP_FORMAT_H

#include <stdarg.h>

/**
 * A string formatting function that returns a malloc'ed string.
 * The behavior is exactly the same as sprintf except that the
 * length of the resulting string is dynamically determined
 * using snprintf function.
 */
char *heap_format(const char *format, ...);

/** 
 * A string formatting function with va_list parameter
 * that returns a malloc'ed string.
 * The behavior is exactly the same as vsprintf except that the
 * length of the resulting string is dynamically determined
 * using snprintf function.
 */
char *vheap_format(const char *format, va_list args);

/**
 * Concatenates two strings and returns a pointer
 * to the malloc'ed result.
 */
char *heap_concat(const char* first, const char* second);

#endif
