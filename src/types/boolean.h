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

#ifndef BOOLEAN_H
#define BOOLEAN_H

#include "types.h"

/* Boolean type */
typedef bool boolean_t;

object_t make_boolean(boolean_t value);
object_t copy_boolean(object_t other);
void destroy_boolean(object_t val);
char *boolean_tostring(object_t val);
bool boolean_equals(object_t val, object_t other);
bool is_boolean(object_t obj);
object_t boolean_op_and(object_t obj, object_t other);
object_t boolean_op_or(object_t obj, object_t other);
object_t boolean_op_xor(object_t obj, object_t other);
object_t boolean_op_not(object_t obj);
boolean_t boolean_value(object_t obj);

#endif
