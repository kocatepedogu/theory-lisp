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

#ifndef PAIR_H
#define PAIR_H

#include "types.h"
#include "../utils/list.h"

/* Cons pair type */
typedef struct pair {
  object_t first;
  object_t second;
} pair_t;

object_t make_pair(object_t first, object_t second);
object_t copy_pair(object_t other);
void destroy_pair(object_t obj);
char *pair_tostring(object_t val);
bool pair_equals(object_t obj, object_t other);
bool is_pair(object_t obj);
pair_t pair_value(object_t obj);
bool cons_list_to_internal_list(object_t list_object, list *output_list);
object_t internal_list_to_cons_list(list *input_list);

#endif
