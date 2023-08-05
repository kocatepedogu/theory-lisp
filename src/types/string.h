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

#ifndef THEORYLISP_TYPES_STRING_H
#define THEORYLISP_TYPES_STRING_H

#include "object.h"

#include "../utils/string.h"

/**
 * String type is implemented as an array of characters.
 */
typedef char *string_t;

/**
 * String constructor.
 * Stores a copy of the given string in an internal malloc'ed char array.
 */
object_t make_string(string_t value);

/**
 * String clone.
 * Duplicates the given string object.
 */
object_t clone_string(object_t self);

/**
 * String destructor.
 * Deallocates the internal storage allocated for the string value.
 */
void destroy_string(object_t val);

/** Returns the quoted version of the stored string */
char *string_tostring(object_t val);

/**
 * Returns true if and only if given objects are both strings, they both
 * have the same length, and they have the same characters in the same
 * order.
 */
bool string_equals(object_t val, object_t other);

/** Returns true if and only if the given object is a string */
bool is_string(object_t obj);

/** Returns stored string (unquoted) */
string_t string_value(object_t obj);

/** Returns string length */
object_t string_length(object_t obj);

/** Concatenates two strings */
object_t string_concat(object_t first, object_t second);

/** Returns character at given index */
object_t string_charat(object_t obj, size_t index);

/** Returns substring */
object_t string_substring(object_t obj, size_t begin, size_t end);

#endif
