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

/// @file pair.h

#ifndef PAIR_H
#define PAIR_H

#include "object.h"
#include "../utils/list.h"

/** Cons pair type */
typedef struct pair {
  object_t first;
  object_t second;
} pair_t;

/**
 * Pair constructor.
 * Calls copy constructors of the given objects to make a deep copy.
 */
object_t make_pair(object_t first, object_t second);

/**
 * Pair clone.
 * Calls the clone functions of the objects in
 * the other pair to make a deep copy.
 */
object_t clone_pair(object_t self);

/**
 * Pair destructor.
 * Calls destructors of internal objects.
 */
void destroy_pair(object_t obj);

/**
 * Returns string representation of the pair.
 * It will be of the form (cons [object1] [object2]) to
 * satisfy the requirement that the output of tostring can be
 * used as input to scanner and then to parser to obtain exactly the
 * same expression.
 */
char *pair_tostring(object_t val);

/**
 * Returns true if and only if the first element of obj equals
 * the first element of other and the second element of obj equals
 * the second element of other.
 */
bool pair_equals(object_t obj, object_t other);

/**
 * Returns true if and only if the given object is a cons pair.
 */
bool is_pair(object_t obj);

/**
 * Returns the internally stored pair struct 
 */
pair_t pair_value(object_t obj);

/**
 * Theory Lisp programs use linked chains of cons pairs to form lists,
 * but the internal lists used by the interpreter are implemented using 
 * arrays for better performance. This function converts a cons list
 * to an internal list. If the given object is not a proper cons list,
 * it will return false.
 */
bool cons_list_to_internal_list(object_t list_object, list *output_list);

/**
 * Theory Lisp programs use linked chains of cons pairs to form lists,
 * but the internal lists used by the interpreter are implemented using arrays
 * for better performance. This function converts an internal list
 * to a cons list. Internal lists are always assumed to be well-formed,
 * so this function should always succeed.
 */
object_t internal_list_to_cons_list(list *input_list);

#endif
