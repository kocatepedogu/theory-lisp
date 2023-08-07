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

/// @file boolean.h

#ifndef THEORYLISP_TYPES_BOOLEAN_H
#define THEORYLISP_TYPES_BOOLEAN_H

#include "object.h"

/* Boolean type */
typedef bool boolean_t;

/**
 * Creates a boolean object.
 * Any value other than true and false is undefined behavior.
 * (true and false from stdbool.h)
 */
objectptr make_boolean(boolean_t value);

/** Boolean destructor */
void destroy_boolean(objectptr val);

/** Returns "#t" for true, and "#f" for false. */
char *boolean_tostring(objectptr val);

/** Tests equality of two boolean values. */
bool boolean_equals(objectptr val, objectptr other);

/** True if and only if given object is a boolean. */
bool is_boolean(objectptr obj);

/** Boolean && operator */
objectptr boolean_op_and(objectptr obj, objectptr other);

/** Boolean || operator */
objectptr boolean_op_or(objectptr obj, objectptr other);

/** Boolean ^ operator */
objectptr boolean_op_xor(objectptr obj, objectptr other);

/** Boolean ! operator */
objectptr boolean_op_not(objectptr obj);

/** Returns the internally stored boolean value */
boolean_t boolean_value(objectptr obj);

#endif
