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

#include "variable.h"

#include <stdlib.h>
#include <string.h>

variable_t *new_variable(const char *name, object_t value) {
  variable_t *var = (variable_t *)malloc(sizeof(variable_t));
  var->name = strdup(name);
  var->value = clone_object(value);
  return var;
}

void delete_variable(variable_t *var) {
  free(var->name);
  destroy_object(var->value);
  free(var);
}

object_t variable_get_value(variable_t *var) { return clone_object(var->value); }

void variable_set_value(variable_t *var, object_t value) { var->value = value; }

char *variable_get_name(variable_t *var) { return var->name; }
