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

#include "pair.h"

#include <assert.h>
#include <string.h>

#include "object-base.h"
#include "../types/null.h"
#include "../utils/string.h"
#include "../utils/list.h"
#include "object.h"

/** Cons pair type */
typedef struct pair {
  objectptr first;
  objectptr second;
} pair_t;

static const object_type_t pair_type_id = {{
    .destroy = destroy_pair,
    .equals = pair_equals,
    .tostring = pair_tostring,
}, "pair"};

inline bool is_pair(objectptr obj) {
  return strcmp(pair_type_id.type_name, obj->type_id->type_name) == 0;
}

inline objectptr pair_first(objectptr obj) {
  assert(is_pair(obj));
  return ((pair_t *)(obj->value))->first;
}

inline objectptr pair_second(objectptr obj) {
  assert(is_pair(obj));
  return ((pair_t *)(obj->value))->second;
}

objectptr make_pair(objectptr first, objectptr second) {
  pair_t *pair_value = malloc(sizeof *pair_value);
  pair_value->first = clone_object(first);
  pair_value->second = clone_object(second);

  return object_base_new(pair_value, &pair_type_id);
}

void destroy_pair(objectptr self) {
  assert(is_pair(self));
  pair_t *pair_value = self->value;
  delete_object(pair_value->first);
  delete_object(pair_value->second);
  free(pair_value);
}

char *pair_tostring(objectptr self) {
  assert(is_pair(self));
  pair_t *pair_value = self->value;
  char *first_string = object_tostring(pair_value->first);
  char *second_string = object_tostring(pair_value->second);
  char *result = format("(cons %s %s)", first_string, second_string);
  free(first_string);
  free(second_string);
  return result;
}

bool pair_equals(objectptr self, objectptr other) {
  assert(is_pair(self));
  if (!is_pair(other)) {
    return false;
  }

  pair_t *self_value = self->value;
  pair_t *other_value = other->value;
  return object_equals(self_value->first, other_value->first) &&
         object_equals(self_value->second, other_value->second);
}

bool cons_list_to_internal_list(objectptr list_object, listptr output_list) {
  if (is_null(list_object)) {
    return true;
  }

  if (is_pair(list_object)) {
    pair_t *pair = list_object->value;

    while (true) {
      list_add(output_list, clone_object(pair->first));

      objectptr next = pair->second;
      if (is_pair(next)) {
        pair = next->value;
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

objectptr internal_list_to_cons_list(listptr input_list) {
  size_t len = list_size(input_list);

  if (len == 0) {
    return make_null();
  }

  objectptr pair = make_null();
  if (len >= 2) {
    for (size_t i = len; i != 0; --i) {
      objectptr arg = list_get(input_list, i - 1);
      assign_object(&pair, make_pair(arg, pair));
    }
  }

  return pair;
}
