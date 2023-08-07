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

/// @file object.h

#ifndef THEORYLISP_TYPES_OBJECT_H
#define THEORYLISP_TYPES_OBJECT_H

#include <stdbool.h>
#include <stdlib.h>

#include "../utils/string.h"

struct object;
typedef struct object *objectptr;

/** Object destructor (Must be implemented) */
void delete_object(objectptr obj);

/** Object clone (must be implemented as a deep copy) */
objectptr clone_object(objectptr other);

/**
 * Object assignment operator.
 * It calls destructor of the destination object and makes a shallow copy.
 * This is not a function that can be overriden by implementations.
 */
void assign_object(objectptr *dest, objectptr src);

/**
 * Marks an object as temporary. Applying clone_object to a temporary object
 * will make a shallow copy.
 */
objectptr move(objectptr obj);

/** Returns string representation of object (Must be implemented) */
char *object_tostring(objectptr obj);

/** Returns true if and only if given objects are equal to each other (Must be
 * implemented) */
bool object_equals(objectptr obj, objectptr other);

/** Returns true if obj is less than other (Optional operation) */
objectptr object_less(objectptr obj, objectptr other);

/** Addition operator (Optional operation) */
objectptr object_op_add(objectptr obj, objectptr other);

/** Multiplication operator (Optional operation) */
objectptr object_op_mul(objectptr obj, objectptr other);

/** Subtraction operator (Optional operation) */
objectptr object_op_sub(objectptr obj, objectptr other);

/** Division operator (Optional operation) */
objectptr object_op_div(objectptr obj, objectptr other);

/** Boolean AND operator (Optional operation) */
objectptr object_op_and(objectptr obj, objectptr other);

/** Boolean OR operator (Optional operation) */
objectptr object_op_or(objectptr obj, objectptr other);

/** Boolean XOR operator (Optional operation) */
objectptr object_op_xor(objectptr obj, objectptr other);

/** Boolean NOT operator (Optional operation) */
objectptr object_op_not(objectptr obj);

objectptr object_op_call(objectptr obj, size_t nargs, objectptr *args,
                        void *sf); 

objectptr object_op_call_internal(objectptr obj, void *args, void *sf);

#endif
