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

/// @file variable.h

#ifndef THEORYLISP_INTERPRETER_VARIABLE_H
#define THEORYLISP_INTERPRETER_VARIABLE_H



#include "../types/object.h"

/** Stores the name and values of a variable */
struct variable;
typedef struct variable *variableptr;

/**
 * Returns a malloc'ed variable with the given name and value.
 * A clone of the given parameter value is stored internally.
 */
variableptr new_variable(const char *name, objectptr value);

/** Deallocates the given variable */
void delete_variable(variableptr var);

/** Clones variable */
variableptr clone_variable(variableptr var);

/** Returns the value stored in the given variable */
objectptr variable_get_value(variableptr var);

/**
 * Sets the value of an existing variable.
 * A clone of the given parameter value is stored internally.
 */
void variable_set_value(variableptr var, objectptr value);

/** Returns the name of the given variable */
char *variable_get_name(variableptr var);

#endif
