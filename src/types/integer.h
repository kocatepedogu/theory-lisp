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

/// @file integer.h

#ifndef INTEGER_H
#define INTEGER_H

#include "object.h"

/* Integer type 
 * All integers are currently implemented using signed long integers.
 */
typedef long int integer_t;

/**
 * Creates an instance of integer type 
 * The resulting object contains a reference to the value.
 */
object_t make_integer(integer_t value);

/**
 * Copies an instance of integer type.
 * The resulting object contains a reference to the value.
 */
object_t clone_integer(object_t self);

/**
 * Destroys an instance of integer type.
 * The memory used by the real number is free()'ed.
 * Given object is no longer valid after this function finishes.
 */
void destroy_integer(object_t val);

/**
 * Returns string representation of real number type.
 * The result is equivalent to the output produced by
 * printf("%ld", int_value(val);
 */
char *integer_tostring(object_t val);

/**
 * Compares two integers for equality.
 * 
 * If the other object is a real, self will be converted
 * to real before making the comparison.
 */
bool integer_equals(object_t self, object_t other);

/**
 * Less than operator (self < other).
 * The second operand can have a type different from integer
 * as long as it is a number type (currently, the only such type
 * is real). In case it is a real, self will be converted to real
 * before making the comparison.
 */
object_t integer_less(object_t self, object_t other);

/**
 * Addition operator (obj < other).
 * The second operand can have a type different from integer
 * as long as it is a number type (currently, the only such type
 * is real). In case it is a real, self will be converted to real
 * before making the addition.
 */
object_t integer_op_add(object_t self, object_t other);

/**
 * Multiplication operator (obj < other).
 * The second operand can have a type different from integer
 * as long as it is a number type (currently, the only such type
 * is real). In case it is a real, self will be converted to real
 * before making the multiplication.
 */
object_t integer_op_mul(object_t self, object_t other);

/**
 * Subtraction operator (obj < other).
 * The second operand can have a type different from integer
 * as long as it is a number type (currently, the only such type
 * is real). In case it is a real, self will be converted to real
 * before making the subtraction.
 */
object_t integer_op_sub(object_t self, object_t other);

/**
 * Division operator (obj < other).
 * The second operand can have a type different from integer
 * as long as it is a number type (currently, the only such type
 * is real). In case it is a real, self will be converted to real
 * before making the division.
 */
object_t integer_op_div(object_t self, object_t other);

/**
 * Returns true if and only if the given object is an integer.
 * Warning: This function checks exact type equality.
 * It returns false for real numbers with zero fractional part,
 * even though they are mathematically integers.
 */
bool is_integer(object_t obj);

/**
 * Returns the integer value stored in the given object (unboxing)
 */
integer_t int_value(object_t obj);

#endif
