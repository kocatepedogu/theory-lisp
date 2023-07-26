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

#include "procedure.h"

#include <string.h>
#include <assert.h>

#include "../expressions/expression.h"
#include "../utils/heap-format.h"

static const object_vtable_t procedure_vtable = {
    .copy = copy_procedure,
    .destroy = destroy_procedure,
    .tostring = procedure_tostring,
    .equals = procedure_equals,
};

static const char procedure_type_name[] = "procedure";

/* Procedure constructor */
object_t make_procedure(procedure_t proc) {
  object_t obj;
  obj.value = proc;
  obj.type = procedure_type_name;
  obj.vtable = &procedure_vtable;
  obj.temporary = false;
  return obj;
}

object_t copy_procedure(object_t other) {
  object_t new_procedure;
  new_procedure.type = procedure_type_name;
  new_procedure.value = other.value;
  new_procedure.vtable = &procedure_vtable;
  new_procedure.temporary = false;
  return new_procedure;
}

/* Procedure destructor */
void destroy_procedure(object_t procedure) {}

/* Procedure tostring implementation */
char *procedure_tostring(object_t obj) {
  procedure_t proc = obj.value;
  return expr_tostring(proc);
}

/* Procedure equals implementation */
bool procedure_equals(object_t obj, object_t other) { 
  assert(false);
  return false; 
}

bool is_procedure(object_t obj) {
  return strcmp(procedure_type_name, obj.type) == 0;
}

void *procedure_value(object_t obj) {
  return obj.value;
}
