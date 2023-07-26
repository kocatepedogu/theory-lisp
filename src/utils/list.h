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

/// @file list.h

#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H

#include <stdlib.h>

/**
 * \brief A generic list type that stores void* pointers.
 *
 * It uses an array to store the elements, and the capacity is
 * doubled whenever number of elements becomes equal to the
 * size of the array.
 *
 * The functions construct_list, new_list, destroy_list, delete_list,
 * list_add, list_add_all, list_size, list_get should be used to
 * manipulate list's.
 */
typedef struct {
  void **data;
  size_t number_of_elements;
  size_t capacity;
} list;

/** 
 * List constructor.
 * Initializes the given list. It must not be called on an
 * already initialized list.
 */
void construct_list(list *lst);

/** 
 * Returns a malloc'ed and initialized list.
 */
list *new_list(void);

/** 
 * List destructor.
 * It free()'s the internal array, and sets the capacity to 0.
 * The data pointed to by void* elements must be free'd by the
 * user.
 */
void destroy_list(list* lst);

/** 
 * Calls list destructor and then free()'s the given list pointer.
 */
void delete_list(list* lst);

/** 
 * Adds element to given list
 */
void list_add(list *lst, void *element);

/** 
 * Adds all elements of another list to given list
 */
void list_add_all(list *lst, list *other);

/** 
 * Returns the number of elements in the given list
 */
size_t list_size(list *lst);

/* 
 * Returns the element at the given index
 */
void *list_get(list *lst, size_t index);

#endif
