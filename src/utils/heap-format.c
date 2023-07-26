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

#include "heap-format.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char *vheap_format(const char *format, va_list args) {
  if (format) {
    va_list copy_args;
    va_copy(copy_args, args);
    /* Get required length from vsnprintf */
    int required_length = vsnprintf(NULL, 0, format, copy_args) + 1;
    va_end(copy_args);
    /* Write the formatted string to the string allocated on the heap. */
    char *result = malloc(required_length);
    vsprintf(result, format, args);
    return result;
  }
  return NULL;
}

char *heap_format(const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *result = vheap_format(format, args);
  va_end(args);
  return result;
}

char *heap_concat(const char *first, const char *second) {
  return heap_format("%s%s", first, second);
}
