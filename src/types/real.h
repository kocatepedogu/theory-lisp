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

/// @file real.h

#ifndef REAL_H
#define REAL_H

#include "object.h"

/** 
 * Real number type.
 * All real numbers are currently implemented using double precision
 * floating point numbers.
 */
typedef double real_t;

/**
 * Creates an instance of real number type.
 * The resulting object contains a reference to the value.
 */
object_t make_real(real_t value);

/**
 * Copies an instance of real number type.
 * The resulting object contains a reference to the value.
 */
object_t clone_real(object_t self);

/**
 * Destroys an instance of real number type.
 * The memory used by the real number is free()'ed.
 * Given object is no longer valid after this function finishes.
 */
void destroy_real(object_t val);

/**
 * Returns string representation of real number type.
 * The result is equivalent to the output produced by
 * printf("%f", real_value(val));
 */
char *real_tostring(object_t val);

/**
 * Compares two real numbers for equality.
 * Warning: Real numbers are currently implemented using double
 * precision floating point numbers. Two expressions that produce
 * analytically equivalent values may not compare equal in double
 * precision arithmetic. This function exists because all subtypes of
 * object must have the equals function, but relying on this function
 * is prone to errors.
 *
 * If the other object has a different number type, it will be
 * converted to real before making the comparison.
 */
bool real_equals(object_t val, object_t other);

/**
 * Less than operator for real numbers (self < other).
 * The second operand can have a type different from real
 * as long as it is a number type (currently the only such type is integer)
 * In case it has a different type, it is converted to real and the comparison
 * is done with two real numbers.
 */
object_t real_less(object_t self, object_t other);

/**
 * Addition operator for real numbers.
 * The second operand can have a type different from real
 * as long as it is a number type (currently the only such type is integer)
 * In case it has a different type, it is converted to real and the addition
 * is done with two real numbers.
 */
object_t real_op_add(object_t self, object_t other);

/**
 * Multiplication operator for real numbers.
 * The second operand can have a type different from real
 * as long as it is a number type (currently the only such type is integer)
 * In case it has a different type, it is converted to real, and the 
 * multiplication is done with two real numbers.
 */
object_t real_op_mul(object_t self, object_t other);

/**
 * Subtraction operator for real numbers.
 * The second operand can have a type different from real
 * as long as it is a number type. (currently the only such type is integer)
 * In case it has a different type, it is converted to real, and the subtraction
 * is done with two real numbers.
 */
object_t real_op_sub(object_t self, object_t other);

/**
 * Division operator for real numbers.
 * The second operand can have a type different from real
 * as long as it is a number type. (currently the only such type is integer)
 * In case it has a different type, it is converted to real, and the division
 * is done with two real numbers.
 */
object_t real_op_div(object_t self, object_t other);

/**
 * Returns true if and only if the given object has the real type.
 * Warning: This function checks exact type equality.
 * It returns false for integer objects, even though integers are,
 * mathematically, real numbers.
 */
bool is_real(object_t obj);

/**
 * Returns the real number value stored in the given object (unboxing).
 */
real_t real_value(object_t obj);

#endif
