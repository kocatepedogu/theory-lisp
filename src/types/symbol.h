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

#ifndef SYMBOL_H
#define SYMBOL_H

#include "types.h"

/* Symbol type */
typedef char *symbol_t;

object_t make_symbol(symbol_t value);
object_t copy_symbol(object_t other);
void destroy_symbol(object_t val);
char *symbol_tostring(object_t val);
bool symbol_equals(object_t val, object_t other);
bool is_symbol(object_t obj);
symbol_t symbol_value(object_t obj);

#endif
