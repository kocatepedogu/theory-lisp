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

#ifndef STACK_FRAME_H
#define STACK_FRAME_H

#include "../types/types.h"
#include "../utils/list.h"

typedef struct stack_frame {
  list local_variables;
  struct stack_frame *saved_frame_pointer;
} stack_frame_t;

typedef stack_frame_t *stack_frame_ptr;

void construct_stack_frame(stack_frame_ptr sf, stack_frame_ptr previous);
stack_frame_ptr new_stack_frame(stack_frame_ptr previous);
void destroy_stack_frame(stack_frame_ptr sf);
void delete_stack_frame(stack_frame_ptr sf);
void stack_frame_set_variable(stack_frame_ptr sf, char *name, object_t value);
void stack_frame_set_global_variable(stack_frame_ptr sf, char *name,
                                     object_t value);
object_t stack_frame_get_variable(stack_frame_ptr sf, char *name);

#endif
