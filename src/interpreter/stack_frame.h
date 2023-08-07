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

/// @file stack_frame.h

#ifndef THEORYLISP_INTERPRETER_STACK_FRAME_H
#define THEORYLISP_INTERPRETER_STACK_FRAME_H



#include "../types/object.h"
#include "../utils/list.h"

/**
 * A container for local variables
 */

struct stack_frame;
typedef struct stack_frame *stack_frame_ptr;

/**
 * Allocates a stack frame.
 * The member saved_frame_pointer is set to the given previous frame.
 * Previous should be NULL if a global stack frame is constructed.
 */
stack_frame_ptr new_stack_frame(stack_frame_ptr previous);

/**
 * Deallocates a stack frame.
 * Local variables in the stack frame are deallocated.
 * It has no effect on the variables of previous stack frames.
 */
void delete_stack_frame(stack_frame_ptr sf);

/**
 * Sets the value of a local variable in the given stack frame.
 *
 * If the variable does not exist in the local stack frame sf, it is created locally and then assigned the given value.
 * If the variable exists in the local stack frame sf, its value is changed to the given value.
 *
 * Variables with the same name in outer scopes/stack frames are not affected.
 */
void stack_frame_set_local_variable(stack_frame_ptr sf, const char *name, objectptr value);

/**
 * Sets the value of a global variable.
 *
 * By following the saved_frame_pointer's, this function recursively tries
 * to find the bottom stack frame, and then creates a variable with the given name,
 * or changes the value of the variable if it already exists.
 *
 */
void stack_frame_set_global_variable(stack_frame_ptr sf, const char *name,
                                     objectptr value);

/**
 * Sets the value of a variable.
 *
 * If the variable does not exist, it is created locally and then assigned the given value.
 * If the variable exists in some stack frame (it may not be the local one), modifies its value.
 */

void stack_frame_set_variable(stack_frame_ptr sf, const char *name,
                              objectptr value);

/**
 * Returns the value of the variable with the given name.
 *
 * Firstly, the local stack frame sf is searched for the name. If it is not
 * found, increasingly outer stack frames are searched for the variable.
 *
 * When it is found, the value is returned. If it does not exist,
 * the function returns an error object.
 *
 */
objectptr stack_frame_get_variable(stack_frame_ptr sf, const char *name);

/**
 * Returns true if the given variable was previously defined
 */
bool stack_frame_defined(stack_frame_ptr sf, const char *name);

#endif
