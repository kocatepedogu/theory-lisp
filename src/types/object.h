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

/**
 * object_t contains the type and value information of objects in
 * Theory Lisp programs.
 */
typedef struct object {
  /// Data contained in the object.
  void *value;
  /// Operations that can operate on the data
  const struct object_vtable *vtable;
  /// Runtime type name of the object
  const char *type;
  /// Whether the object is a temporary value or not.
  bool temporary;
} object_t;

/**
 * object_vtable_t contains operations that are supported by an object.
 * All objects must implement destroy, clone, tostring and equals functions.
 * The remaining are optional. If an operation is not implemented, the
 * member corresponding to it must be set to NULL. If an unsupported operation
 * is called, it will return an error object containing "unsupported operation"
 * message.
 */
typedef struct object_vtable {
  void (*destroy)(object_t);
  object_t (*clone)(object_t);
  char *(*tostring)(object_t);
  bool (*equals)(object_t, object_t);
  object_t (*less)(object_t, object_t);

  object_t (*op_add)(object_t, object_t);
  object_t (*op_mul)(object_t, object_t);
  object_t (*op_sub)(object_t, object_t);
  object_t (*op_div)(object_t, object_t);

  object_t (*op_and)(object_t, object_t);
  object_t (*op_or)(object_t, object_t);
  object_t (*op_xor)(object_t, object_t);
  object_t (*op_not)(object_t);

  object_t (*op_call)(object_t, size_t, object_t *, void *);
  object_t (*op_call_internal)(object_t, void *, void *);
} object_vtable_t;

/** Object destructor (Must be implemented) */
void destroy_object(object_t obj);

/** Object clone (must be implemented as a deep copy) */
object_t clone_object(object_t other);

/**
 * Object assignment operator.
 * It calls destructor of the destination object and makes a shallow copy.
 * This is not a function that can be overriden by implementations.
 */
void assign_object(object_t *dest, object_t src);

/**
 * Marks an object as temporary. Applying clone_object to a temporary object
 * will make a shallow copy.
 */
object_t move(object_t obj);

/** Returns string representation of object (Must be implemented) */
char *object_tostring(object_t obj);

/** Returns true if and only if given objects are equal to each other (Must be
 * implemented) */
bool object_equals(object_t obj, object_t other);

/** Returns true if obj is less than other (Optional operation) */
object_t object_less(object_t obj, object_t other);

/** Addition operator (Optional operation) */
object_t object_op_add(object_t obj, object_t other);

/** Multiplication operator (Optional operation) */
object_t object_op_mul(object_t obj, object_t other);

/** Subtraction operator (Optional operation) */
object_t object_op_sub(object_t obj, object_t other);

/** Division operator (Optional operation) */
object_t object_op_div(object_t obj, object_t other);

/** Boolean AND operator (Optional operation) */
object_t object_op_and(object_t obj, object_t other);

/** Boolean OR operator (Optional operation) */
object_t object_op_or(object_t obj, object_t other);

/** Boolean XOR operator (Optional operation) */
object_t object_op_xor(object_t obj, object_t other);

/** Boolean NOT operator (Optional operation) */
object_t object_op_not(object_t obj);

object_t object_op_call(object_t obj, size_t nargs, object_t *args,
                        void *sf); 

object_t object_op_call_internal(object_t obj, void *args, void *sf);

#endif
