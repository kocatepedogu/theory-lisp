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

struct stack_frame {
  listptr local_variables;
  struct stack_frame *saved_frame_pointer;
};

stack_frame_ptr new_stack_frame(stack_frame_ptr previous) {
  stack_frame_ptr sf = (stack_frame_ptr)malloc(sizeof(struct stack_frame));
  sf->local_variables = new_list();
  sf->saved_frame_pointer = previous;
  return sf;
}

void delete_stack_frame(stack_frame_ptr sf) {
  for (size_t i = 0; i < list_size(sf->local_variables); i++) {
    variableptr var = list_get(sf->local_variables, i);
    delete_variable(var);
  }
  delete_list(sf->local_variables);
  sf->saved_frame_pointer = NULL;
  free(sf);
}

static variableptr find_variable_locally(stack_frame_ptr sf, const char *name) {
  listptr local_vars = sf->local_variables;
  for (size_t i = 0; i < list_size(local_vars); i++) {
    variableptr var = list_get(local_vars, i);
    if (strcmp(variable_get_name(var), name) == 0) {
      return var;
    }
  }
  return NULL;
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

void stack_frame_set_local_variable(stack_frame_ptr sf, const char *name, object_t value) {
  variableptr var = find_variable_locally(sf, name);
  if (var) {
    variable_set_value(var, value);
  } else {
    var = new_variable(name, value);
    list_add(sf->local_variables, var);
  }
}

void stack_frame_set_variable(stack_frame_ptr sf, const char *name, object_t value) {
  variableptr var = find_variable(sf, name);
  if (var) {
    variable_set_value(var, value);
  } else {
    var = new_variable(name, value);
    list_add(sf->local_variables, var);
  }
}

void stack_frame_set_global_variable(stack_frame_ptr sf, const char *name,
                                     object_t value) {
  while (sf->saved_frame_pointer != NULL) {
    sf = sf->saved_frame_pointer;
  }
  stack_frame_set_local_variable(sf, name, value);
}

object_t stack_frame_get_variable(stack_frame_ptr sf, const char *name) {
  variableptr var = find_variable(sf, name);
  if (var) {
    return variable_get_value(var);
  }
  return make_error("Variable %s does not exist", name);
}

bool stack_frame_defined(stack_frame_ptr sf, const char *name) {
  object_t value = stack_frame_get_variable(sf, name);
  bool defined = !is_error(value);
  destroy_object(value);
  return defined;
}
