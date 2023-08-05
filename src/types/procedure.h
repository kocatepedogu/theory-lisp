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

/// @file procedure.h

#ifndef THEORYLISP_TYPES_PROCEDURE_H
#define THEORYLISP_TYPES_PROCEDURE_H

#include "../interpreter/stack_frame.h"
#include "object.h"
#include "../utils/string.h"

/* Procedure type */
typedef void *lambda_t;

/**
 * Procedure constructor.
 * No memory allocation occurs. Created object refers to the given lambda
 * expression in the parse tree.
 */
object_t make_procedure(lambda_t proc, listptr captures, stack_frame_ptr sf);

/**
 * Procedure clone.
 * No memory allocation occurs. The new object still points to the same
 * lambda expression in the parse tree. This is not a violation of the
 * rule that all objects must do a deep copy, because the lambda is
 * never mutated or free()'d during program execution.
 */
object_t clone_procedure(object_t self);

/**
 * Procedure destructor.
 * No memory deallocation occurs. Lambda expressions, like other expessions,
 * are removed from the memory when the program finishes. Managing their
 * memory is not the responsibility of procedure objects.
 */
void destroy_procedure(object_t procedure);

/**
 * Returns the string representation of the procedure.
 * If the returned string is substituted as input to scanner, and then
 * to the parser, the resulting lambda expression must be
 * exactly the same as the lambda expression pointed to by the procedure
 * object. If this constraint is violated, it should be reported as a bug.
 */
char *procedure_tostring(object_t obj);

/**
 * Calling procedure_equals will throw an assertion error if
 * assertions are enabled in compile options, otherwise it will return false.
 *
 * The reasons for this implementation are:
 * 1. Testing whether two procedures do the same thing is
 * an undecidable problem.
 * 2. Testing whether two objects point to the same lambda
 * expression is useless.
 */
bool procedure_equals(object_t obj, object_t other);

/**
 * Returns true if and only if the given object is a procedure object.
 */
bool is_procedure(object_t obj);

/**
 * Returns PN arity of the lambda
 */
size_t procedure_get_pn_arity(object_t self);

/**
 * Returns arity of the lambda
 */
size_t procedure_get_arity(object_t self);

/**
 * Returns whether explicit PN arity is given
 */
bool procedure_is_pn_arity_given(object_t self);

/**
 * Returns whether the lambda is variadic
 */
bool procedure_is_variadic(object_t self);

/**
 * Calls the procedure
 */
object_t procedure_op_call(object_t self, size_t nargs, object_t *args,
                           void *sf);

/**
 * Calls the procedure (used internally)
 */

object_t procedure_op_call_internal(object_t self, void *args, void *sf);

#endif
