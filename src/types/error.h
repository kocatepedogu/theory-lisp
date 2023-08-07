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

/// @file error.h

#ifndef THEORYLISP_TYPES_ERROR_H
#define THEORYLISP_TYPES_ERROR_H

#include "object.h"

#include "../utils/string.h"

/** Error type */
typedef char *error_t;

/**
 * Creates an error object with the given error mesage.
 * The format string and parameters for creating the error message
 * are exactly the same as printf
 */
objectptr make_error(error_t fmt, ...);

/**
 * Creates an error object that causes the program to exit
 * normally without printing an error message.
 */
objectptr make_exit(void);

/** Error destructor */
void destroy_error(objectptr self);

/** Returns true if and only if both errors have the same error message */
bool error_equals(objectptr self, objectptr other);

/** Returns the stored error message */
char *error_tostring(objectptr self);

/** Returns true if and only if obj is an error. */
bool is_error(objectptr obj);

/** Returns true if and only if obj is a normal exit */
bool is_exit(objectptr obj);

#endif
