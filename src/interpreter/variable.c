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

#include "variable.h"

#include <stdlib.h>
#include <string.h>


struct variable {
  char *name;
  objectptr value;
};

variableptr new_variable(const char *name, objectptr value) {
  variableptr var = malloc(sizeof *var);
  var->name = strdup(name);
  var->value = clone_object(value);
  return var;
}

variableptr clone_variable(variableptr var) {
  return new_variable(var->name, var->value);
}

void delete_variable(variableptr var) {
  free(var->name);
  delete_object(var->value);
  free(var);
}

objectptr variable_get_value(variableptr var) { return clone_object(var->value); }

void variable_set_value(variableptr var, objectptr value) { 
  assign_object(&var->value, clone_object(value));
}

char *variable_get_name(variableptr var) { return var->name; }
