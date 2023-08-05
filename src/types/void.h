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

/// @file void.h

#ifndef THEORYLISP_TYPES_VOID_H
#define THEORYLISP_TYPES_VOID_H

#include "object.h"
#include "../utils/string.h"

/**
 * Creates an instance of void type.
 * No memory allocation occurs.
 */
object_t make_void(void);

/**
 * Destroys an instance of void type.
 * No memory deallocation occurs.
 */
void destroy_void(object_t obj);

/**
 * Copies an instance of void object.
 * No memory allocation occurs.
 */
object_t clone_void(object_t self);

/**
 * Returns malloc'ed string representation of void object.
 * All void objects are represented with "null" string.
 */
char *void_tostring(object_t obj);

/**
 * Compares two instances of void type.
 * All instances of void type are equal.
 * The value space of the void type is the singleton set {null}.
 */
bool void_equals(object_t obj, object_t other);

/**
 * Checks whether the given object is of void type.
 */
bool is_void(object_t obj);

#endif
