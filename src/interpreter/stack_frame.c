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

#include "stack_frame.h"

#include <stdio.h>
#include <string.h>


#include "../types/error.h"
#include "variable.h"
#include "../builtin/builtin.h"
#include "../utils/hashtable.h"

struct stack_frame {
  hashtableptr local_variables;
  struct stack_frame *saved_frame_pointer;
};

stack_frame_ptr new_stack_frame(stack_frame_ptr previous) {
  stack_frame_ptr sf = malloc(sizeof *sf);
  sf->local_variables = new_hash_table(1);
  sf->saved_frame_pointer = previous;
  return sf;
}

static void variable_destructor(void *variable) {
  delete_variable(variable);
}

void delete_stack_frame(stack_frame_ptr sf) {
  delete_hash_table(sf->local_variables, variable_destructor);
  sf->saved_frame_pointer = NULL;
  free(sf);
}

static variableptr find_variable_locally(stack_frame_ptr sf, const char *name) {
  return hash_table_get(sf->local_variables, name);
}

static variableptr find_variable(stack_frame_ptr sf, const char *name) {
  while (sf) {
    variableptr var = find_variable_locally(sf, name);
    if (var) {
      return var;
    }

    sf = sf->saved_frame_pointer;
  }

  return NULL;
}

void stack_frame_set_local_variable(stack_frame_ptr sf, const char *name, objectptr value) {
  variableptr var = find_variable_locally(sf, name);
  if (var) {
    variable_set_value(var, value);
  } else {
    var = new_variable(name, value);
    hash_table_put(sf->local_variables, name, var);
  }
}

void stack_frame_set_variable(stack_frame_ptr sf, const char *name, objectptr value) {
  variableptr var = find_variable(sf, name);
  if (var) {
    variable_set_value(var, value);
  } else {
    var = new_variable(name, value);
    hash_table_put(sf->local_variables, name, var);
  }
}

void stack_frame_set_global_variable(stack_frame_ptr sf, const char *name,
                                     objectptr value) {
  while (sf->saved_frame_pointer != NULL) {
    sf = sf->saved_frame_pointer;
  }
  stack_frame_set_local_variable(sf, name, value);
}

objectptr stack_frame_get_variable(stack_frame_ptr sf, const char *name) {
  variableptr var = find_variable(sf, name);
  if (var) {
    return variable_get_value(var);
  }
  return make_error("Variable %s does not exist", name);
}

bool stack_frame_defined(stack_frame_ptr sf, const char *name) {
  objectptr value = stack_frame_get_variable(sf, name);
  bool defined = !is_error(value);
  delete_object(value);
  return defined;
}
