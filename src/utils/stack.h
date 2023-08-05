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

#ifndef THEORYLISP_UTILS_STACK_H
#define THEORYLISP_UTILS_STACK_H

#include <stdbool.h>
#include <stdlib.h>

struct stack;
typedef struct stack *stackptr;

stackptr new_stack(void);

void delete_stack(stackptr st);

void stack_push(stackptr st, void *element);

void *stack_pop(stackptr st);

void *stack_get(stackptr st, size_t index);

void *stack_peek(stackptr st);

size_t stack_size(stackptr st);

bool stack_is_empty(stackptr st);

#endif
