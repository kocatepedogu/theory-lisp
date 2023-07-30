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

#include "pair.h"

#include <string.h>
#include <assert.h>

#include "../utils/list.h"
#include "../utils/heap-format.h"
#include "../types/null.h"
#include "object.h"

/** Cons pair type */
typedef struct pair {
  object_t first;
  object_t second;
} pair_t;

static const object_vtable_t pair_vtable = {
    .clone = clone_pair,
    .destroy = destroy_pair,
    .equals = pair_equals,
    .tostring = pair_tostring,
};

static const char pair_typename[] = "pair";

inline bool is_pair(object_t obj) { 
  return strcmp(pair_typename, obj.type) == 0;
}

inline object_t pair_first(object_t obj) { 
  assert(is_pair(obj));
  return (*(pair_t *)obj.value).first; 
}

inline object_t pair_second(object_t obj) {
  assert(is_pair(obj));
  return (*(pair_t*)obj.value).second;
}

object_t make_pair(object_t first, object_t second) {
  pair_t *pair_value = (pair_t *)malloc(sizeof(pair_t));
  pair_value->first = clone_object(first);
  pair_value->second = clone_object(second);

  object_t obj;
  obj.value = pair_value;
  obj.type = pair_typename;
  obj.vtable = &pair_vtable;
  obj.temporary = false;
  return obj;
}

object_t clone_pair(object_t self) {
  assert(is_pair(self));
  pair_t *pair_value = (pair_t *)malloc(sizeof(pair_t));
  pair_t *other_pair_value = self.value;
  pair_value->first = clone_object(other_pair_value->first);
  pair_value->second = clone_object(other_pair_value->second);

  object_t new_pair;
  new_pair.value = pair_value;
  new_pair.type = pair_typename;
  new_pair.vtable = &pair_vtable;
  new_pair.temporary = false;
  return new_pair;
}

void destroy_pair(object_t self) {
  assert(is_pair(self));
  pair_t *pair_value = self.value;
  destroy_object(pair_value->first);
  destroy_object(pair_value->second);
  free(pair_value);
}

char *pair_tostring(object_t self) {
  assert(is_pair(self));
  pair_t *pair_value = self.value;
  char *first_string = object_tostring(pair_value->first);
  char *second_string = object_tostring(pair_value->second);
  char *result = heap_format("(cons %s %s)", first_string, second_string);
  free(first_string);
  free(second_string);
  return result;
}

bool pair_equals(object_t self, object_t other) {
  assert(is_pair(self));
  if (!is_pair(other)) {
    return false;
  }

  pair_t *self_value = self.value;
  pair_t *other_value = other.value;
  return object_equals(self_value->first, other_value->first) &&
         object_equals(self_value->second, other_value->second);
}

bool cons_list_to_internal_list(object_t list_object, listptr output_list) {
  if (is_null(list_object)) {
    return true;
  }

  if (is_pair(list_object)) {
    pair_t *pair = list_object.value;

    while (true) {
      object_t *obj = (object_t *)malloc(sizeof(object_t));
      *obj = clone_object(pair->first);
      list_add(output_list, obj);

      object_t next = pair->second;
      if (is_pair(next)) {
        pair = next.value;
      } else if (is_null(next)) {
        break;
      } else {
        return false;
      }
    }

    return true;
  }

  return false;
}

object_t internal_list_to_cons_list(listptr input_list) {
  size_t len = list_size(input_list);

  if (len == 0) {
    return make_null();
  }

  object_t nil = make_null();
  object_t *arg = list_get(input_list, len-1);
  object_t pair = make_pair(*arg, nil);
  if (len >= 2) {
    for (size_t i = len - 1; i != 0; i--) {
      arg = list_get(input_list, i-1);
      assign_object(&pair, make_pair(*arg, pair));
    }
  }

  return pair;
}


