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

#include "list.h"

#include <assert.h>
#include <string.h>

struct list {
  void **data;
  size_t number_of_elements;
  size_t capacity;
};

static const size_t default_list_size = 4;

/* List "new" operator. */
listptr new_list(void) {
  listptr lst = malloc(sizeof *lst);
  lst->data = malloc(default_list_size * sizeof(void *));
  lst->capacity = default_list_size;
  lst->number_of_elements = 0;
  return lst;
}

/* list "delete" operation */
void delete_list(listptr lst) {
  free(lst->data);
  lst->capacity = 0;
  lst->number_of_elements = 0;
  free(lst);
}

/* adds element to given list */
void list_add(listptr lst, void *element) {
  if (lst->number_of_elements == lst->capacity) {
    lst->capacity *= 2;
    void **new_data = malloc(lst->capacity * sizeof(void *));
    memcpy(new_data, lst->data, lst->number_of_elements * sizeof(void *));
    free(lst->data);
    lst->data = new_data;
  }

  lst->data[lst->number_of_elements++] = element;
}

/* adds all elements of other list to given list */
void list_add_all(listptr lst, listptr other) {
  for (size_t i = 0; i < list_size(other); ++i) {
    list_add(lst, list_get(other, i));
  }
}

/* returns the number of elements in the given list */
size_t list_size(listptr lst) { return lst->number_of_elements; }

/* returns the element at the given index */
void *list_get(listptr lst, size_t index) {
  assert(index < list_size(lst));
  return lst->data[index];
}

/* modifies an element, returns old value*/
void *list_set(listptr lst, size_t index, void *new_value) {
  void *old_value = list_get(lst, index);
  lst->data[index] = new_value;
  return old_value;
}
