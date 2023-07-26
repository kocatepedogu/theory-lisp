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

#ifndef INTEGER_H
#define INTEGER_H

#include "types.h"

/* Integer type */
typedef long int integer_t;

object_t make_integer(integer_t value);
object_t copy_integer(object_t other);
void destroy_integer(object_t val);
char *integer_tostring(object_t val);
bool integer_equals(object_t val, object_t other);
object_t integer_less(object_t obj, object_t other);
object_t integer_op_add(object_t self, object_t other);
object_t integer_op_mul(object_t self, object_t other);
object_t integer_op_sub(object_t self, object_t other);
object_t integer_op_div(object_t self, object_t other);
bool is_integer(object_t obj);
integer_t int_value(object_t obj);

#endif
