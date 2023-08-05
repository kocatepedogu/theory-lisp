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

#include "string.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define ERR_FORMAT "Unknown format specifier"
#define INITIAL_BUFFER_LENGTH 1
#define MAX_TRIES 16

#define unique

static void vfree(const char *format, va_list args) {
  char c = '\0';
  while (*format) {
    if (*format++ == '%') {
      c = *format++;
      switch (c) {
        case 'l':
          c = *format++;
          switch (c) {
            case 'd':
              va_arg(args, long);
              break;
            default:
              fprintf(stderr, ERR_FORMAT);
              abort();
          }
          break;
        case 's':
          free(va_arg(args, char*));
          break;
        case 'f':
          va_arg(args, double);
          break;
        case 'd':
          va_arg(args, int);
          break;
        default:
          fprintf(stderr, ERR_FORMAT);
          abort();
      }
    }
  }
}

char *vformat(const char *format, va_list args) {
  if (format) {
    va_list copy_args;
    va_copy(copy_args, args);
    int required_length = vsnprintf(NULL, 0, format, copy_args) + 1;
    va_end(copy_args);
    char *result = malloc(required_length);
    vsprintf(result, format, args);
    return result;
  }

  return NULL;
}

char *format(const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *result = vformat(format, args);
  va_end(args);
  return result;
}

char *append(const char *first, const char *second) {
  if (first == NULL) {
    return strdup(second);
  }

  return format("%s%s", first, second);
}

char *append_sep(const char *first, const char *space,
                 const char *second) {
  if (first == NULL) {
    return strdup(second);
  }

  return format("%s%s%s", first, space, second);
}

char *unique_vformat(const char *fmt, va_list args) {
  va_list copy_args_1;
  va_copy(copy_args_1, args);
  char *result = vformat(fmt, copy_args_1);
  va_end(copy_args_1);

  va_list copy_args_2;
  va_copy(copy_args_2, args);
  vfree(fmt, copy_args_2);
  va_end(copy_args_2);

  return result;
}

char *unique_format(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char *result = unique_vformat(fmt, args);
  va_end(args);
  return result;
}

char *unique_append(char *first, char *second) {
  if (first == NULL) {
    return second;
  }

  char *result = format("%s%s", first, second);
  free(first);
  free(second);
  return result;
}

char *unique_append_sep(char *first,  const char *space,
                        char *second) {
  if (first == NULL) {
    return second;
  }

  char *result = format("%s%s%s", first, space, second);
  free(first);
  free(second);
  return result;
}
