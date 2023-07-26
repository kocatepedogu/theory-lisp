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

#ifndef PROCEDURE_H
#define PROCEDURE_H

#include "types.h"

/* Procedure type */
typedef void *procedure_t;

object_t make_procedure(procedure_t proc);
object_t copy_procedure(object_t other);
void destroy_procedure(object_t procedure);
char *procedure_tostring(object_t obj);
bool procedure_equals(object_t obj, object_t other);
bool is_procedure(object_t obj);
void *procedure_value(object_t obj);

#endif
