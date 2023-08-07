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

/// @file hashtable.h

#ifndef THEORYLISP_UTILS_HASHTABLE_H
#define THEORYLISP_UTILS_HASHTABLE_H

#include <stdlib.h>

struct hashtable;
typedef struct hashtable *hashtableptr;

typedef void (*dict_value_destructor)(void *value);

hashtableptr new_hash_table(size_t capacity);

void delete_hash_table(hashtableptr table, dict_value_destructor destr);

void *hash_table_put(hashtableptr table, const char *key, void *value);

void *hash_table_get(hashtableptr table, const char *key);

#endif
