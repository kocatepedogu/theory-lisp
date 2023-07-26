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

#include "list.h"

#include <string.h>
#include <assert.h>

static const size_t default_list_size = 4;

/* List constructor */
void construct_list(list *lst) {
  lst->data = malloc(default_list_size * sizeof(void *));
  lst->capacity = default_list_size;
  lst->number_of_elements = 0;
}

/* List "new" operator. */
list *new_list(void) {
  list *lst = (list *)malloc(sizeof(list));
  construct_list(lst);
  return lst;
}

/* list destructor */
void destroy_list(list* lst) {
  free(lst->data);
  lst->capacity = 0;
  lst->number_of_elements = 0;
}

/* list "delete" operation */
void delete_list(list* lst) {
  destroy_list(lst);
  free(lst);
}

/* adds element to given list */
void list_add(list *lst, void *element) {
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
void list_add_all(list *lst, list *other) {
  for (size_t i = 0; i < list_size(other); i++) {
    list_add(lst, list_get(other, i));
  }
}

/* returns the number of elements in the given list */
size_t list_size(list *lst) { return lst->number_of_elements; }

/* returns the element at the given index */
void *list_get(list *lst, size_t index) {
  assert (index < list_size(lst));
  return lst->data[index];
}

