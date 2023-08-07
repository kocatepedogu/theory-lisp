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

#include "stack.h"

#include <assert.h>

#define DEFAULT_STACK_CAPACITY 8

struct stack {
  void **data;
  size_t number_of_elements;
  size_t capacity;
};

stackptr new_stack(void) {
  stackptr st = malloc(sizeof *st);
  st->capacity = DEFAULT_STACK_CAPACITY;
  st->data = malloc(st->capacity * sizeof(void *));
  st->number_of_elements = 0;
  return st;
}

void delete_stack(stackptr st) {
  free(st->data);
  st->capacity = 0;
  st->number_of_elements = 0;
  free(st);
}

void stack_push(stackptr st, void *element) {
  if (st->number_of_elements == st->capacity) {
    st->capacity *= 2;
    st->data = realloc(st->data, st->capacity);
  }

  st->data[st->number_of_elements++] = element;
}

void *stack_pop(stackptr st) {
  void *result = st->data[--st->number_of_elements];

  if (st->number_of_elements < st->capacity / 2) {
    size_t new_capacity = st->capacity / 2;
    if (new_capacity >= DEFAULT_STACK_CAPACITY) {
      st->capacity = new_capacity;
      st->data = realloc(st->data, st->capacity);
    }
  }

  return result;
}

void *stack_get(stackptr st, size_t index) {
  assert(index < stack_size(st));
  return st->data[index];
}

void *stack_peek(stackptr st) { return stack_get(st, stack_size(st) - 1); }

size_t stack_size(stackptr st) { return st->number_of_elements; }

bool stack_is_empty(stackptr st) { return stack_size(st) == 0; }
