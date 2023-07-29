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

/// @file boolean.h

#ifndef BOOLEAN_H
#define BOOLEAN_H

#include "object.h"

/* Boolean type */
typedef bool boolean_t;

/**
 * Creates a boolean object.
 * Any value other than true and false is undefined behavior.
 * (true and false from stdbool.h)
 */
object_t make_boolean(boolean_t value);

/** Clones a boolean object. */
object_t clone_boolean(object_t self);

/** Boolean destructor */
void destroy_boolean(object_t val);

/** Returns "#t" for true, and "#f" for false. */
char *boolean_tostring(object_t val);

/** Tests equality of two boolean values. */
bool boolean_equals(object_t val, object_t other);

/** True if and only if given object is a boolean. */
bool is_boolean(object_t obj);

/** Boolean && operator */
object_t boolean_op_and(object_t obj, object_t other);

/** Boolean || operator */
object_t boolean_op_or(object_t obj, object_t other);

/** Boolean ^ operator */
object_t boolean_op_xor(object_t obj, object_t other);

/** Boolean ! operator */
object_t boolean_op_not(object_t obj);

/** Returns the internally stored boolean value */
boolean_t boolean_value(object_t obj);

#endif
