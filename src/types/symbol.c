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

#include "symbol.h"

#include <string.h>

#include "../utils/heap-format.h"

static const object_vtable_t symbol_vtable = {
    .copy = copy_symbol,
    .destroy = destroy_symbol,
    .equals = symbol_equals,
    .tostring = symbol_tostring,
};

static const char symbol_type_name[] = "symbol";

/*  constructor */
object_t make_symbol(const symbol_t value) {
  char *copy = strdup(value);
  object_t obj;
  obj.value = copy;
  obj.type = symbol_type_name;
  obj.vtable = &symbol_vtable;
  obj.temporary = false;
  return obj;
}

object_t copy_symbol(object_t other) {
  char *copy = strdup(other.value);
  object_t obj;
  obj.value = copy;
  obj.type = symbol_type_name;
  obj.vtable = &symbol_vtable;
  obj.temporary = false;
  return obj;
}

/* symbol destructor */
void destroy_symbol(object_t obj) { free(obj.value); }

/* Symbol tostring implementation */
char *symbol_tostring(object_t val) {
  return heap_format("'%s", (char *)val.value);
}

/* symbol equals implementation */
bool symbol_equals(object_t obj, object_t other) {
  if (!is_symbol(other)) {
    return false;
  }

  return strcmp(obj.value, other.value) == 0;
}

bool is_symbol(object_t obj) { return strcmp(symbol_type_name, obj.type) == 0; }

symbol_t symbol_value(object_t obj) { return obj.value; }
