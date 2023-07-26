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

#ifndef VARIABLE_H
#define VARIABLE_H

#include "../types/types.h"

typedef struct variable {
  char *name;
  object_t value;
} variable_t;

variable_t *new_variable(char *name, object_t value);
void delete_variable(variable_t *);
object_t variable_get_value(variable_t *var);
void variable_set_value(variable_t *var, object_t value);
char *variable_get_name(variable_t *var);

#endif
