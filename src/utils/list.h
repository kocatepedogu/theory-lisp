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

/// @file list.h

#ifndef THEORYLISP_UTILS_LIST_H
#define THEORYLISP_UTILS_LIST_H

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
struct list;
typedef struct list *listptr;

/**
 * Returns a malloc'ed and initialized list.
 */
listptr new_list(void);

/**
 * Calls list destructor and then free()'s the given list pointer.
 */
void delete_list(listptr lst);

/**
 * Adds element to given list
 */
void list_add(listptr lst, void *element);

/**
 * Adds all elements of another list to given list
 */
void list_add_all(listptr lst, listptr other);

/**
 * Returns the number of elements in the given list
 */
size_t list_size(listptr lst);

/*
 * Returns the element at the given index
 */
void *list_get(listptr lst, size_t index);

#endif
