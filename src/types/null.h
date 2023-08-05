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

/// @file null.h

#ifndef THEORYLISP_TYPES_NULL_H
#define THEORYLISP_TYPES_NULL_H

#include "object.h"

#include "../utils/string.h"

/**
 * Creates a null object
 * No memory allocation occurs.
 */
object_t make_null(void);

/**
 * Destroys a null object
 * No memory deallocation occurs.
 */
void destroy_null(object_t obj);

/**
 * Copies a null object
 * No memory allocation occurs.
 */
object_t clone_null(object_t self);

/**
 * Returns malloc'ed string representation of null ("null")
 */
char *null_tostring(object_t obj);

/**
 * Compares two null values (always true)
 */
bool null_equals(object_t obj, object_t other);

/**
 * Checks whether the given object is null
 */
bool is_null(object_t obj);

#endif
