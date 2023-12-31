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

/// @file pair.h

#ifndef THEORYLISP_TYPES_PAIR_H
#define THEORYLISP_TYPES_PAIR_H

#include "../utils/list.h"
#include "object.h"
#include "../utils/string.h"

/**
 * Pair constructor.
 * Calls copy constructors of the given objects to make a deep copy.
 */
objectptr make_pair(objectptr first, objectptr second);

/**
 * Pair destructor.
 * Calls destructors of internal objects.
 */
void destroy_pair(objectptr obj);

/**
 * Returns string representation of the pair.
 * It will be of the form (cons [object1] [object2]) to
 * satisfy the requirement that the output of tostring can be
 * used as input to scanner and then to parser to obtain exactly the
 * same expression.
 */
char *pair_tostring(objectptr val);

/**
 * Returns true if and only if the first element of obj equals
 * the first element of other and the second element of obj equals
 * the second element of other.
 */
bool pair_equals(objectptr obj, objectptr other);

/**
 * Returns true if and only if the given object is a cons pair.
 */
bool is_pair(objectptr obj);

/**
 * Returns the first element in the pair
 */
objectptr pair_first(objectptr obj);

/**
 * Returns the second element in the pair
 */
objectptr pair_second(objectptr obj);

/**
 * Theory Lisp programs use linked chains of cons pairs to form lists,
 * but the internal lists used by the interpreter are implemented using
 * arrays for better performance. This function converts a cons list
 * to an internal list. If the given object is not a proper cons list,
 * it will return false.
 */
bool cons_list_to_internal_list(objectptr list_object, listptr output_list);

/**
 * Theory Lisp programs use linked chains of cons pairs to form lists,
 * but the internal lists used by the interpreter are implemented using arrays
 * for better performance. This function converts an internal list
 * to a cons list. Internal lists are always assumed to be well-formed,
 * so this function should always succeed.
 */
objectptr internal_list_to_cons_list(listptr input_list);

#endif
