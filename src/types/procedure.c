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

#include "procedure.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "object-base.h"
#include "../expressions/data.h"
#include "../expressions/expression.h"
#include "../expressions/lambda.h"
#include "../expressions/let.h"
#include "../interpreter/stack_frame.h"
#include "../interpreter/variable.h"
#include "../types/error.h"
#include "../utils/string.h"

typedef struct {
  /// Expression obtained from parse tree
  void *lambda;
  /// List of variables that existed in the original environment
  listptr closure;
} proc_t;

static const object_type_t procedure_type_id = {{
    .destroy = destroy_procedure,
    .tostring = procedure_tostring,
    .equals = procedure_equals,
    .op_call = procedure_op_call,
    .op_call_internal = procedure_op_call_internal},
    "procedure"};

inline bool is_procedure(objectptr obj) {
  return strcmp(procedure_type_id.type_name, obj->type_id->type_name) == 0;
}

objectptr make_procedure(lambda_t proc, listptr captures, stack_frame_ptr sf) {
  /* Check that the captured variables declared in the capture list 
   * are defined in the environment where the procedure object is created. */
  if (captures) {
    for (size_t i = 0; i < list_size(captures); ++i) {
      const char *name = list_get(captures, i);
      if (!stack_frame_defined(sf, name)) {
        return make_error("Captured variable %s does not exist.", name);
      }
    }
  }

  /* Create the procedure object */
  proc_t *p = malloc(sizeof *p);
  p->lambda = clone_expr(proc);
  p->closure = new_list();

  /* Add captured variables to the closure of the procedure object */
  if (captures) {
    for (size_t i = 0; i < list_size(captures); ++i) {
      const char *name = list_get(captures, i);
      objectptr value = stack_frame_get_variable(sf, name);
      list_add(p->closure, new_variable(name, value));
      delete_object(value);
    }
  }

  return object_base_new(p, &procedure_type_id);
}

void destroy_procedure(objectptr self) {
  assert(is_procedure(self));
  proc_t *p = self->value;

  /* Delete captured variables */
  for (size_t i = 0; i < list_size(p->closure); ++i) {
    delete_variable(list_get(p->closure, i));
  }
  delete_list(p->closure);

  /* Delete lambda */
  delete_expr(p->lambda);
  free(p);
}

char *procedure_tostring(objectptr self) {
  assert(is_procedure(self));
  proc_t *p = self->value;

  /* If there are no captured variables, the string
   * representation of the lambda itself is enough
   * to describe the procedure object.
   */
  if (list_size(p->closure) == 0) {
    return expr_tostring(p->lambda);
  }

  /* Otherwise the captured variables are assigned values
   * in a let expression whose body is the
   * lambda expression, so that evaluating the string representation
   * gives a procedure object with captured values.
   */
  exprptr le = new_let_expr(clone_expr(p->lambda), NULL);
  for (size_t i = 0; i < list_size(p->closure); ++i) {
    variableptr var = list_get(p->closure, i);
    objectptr value = variable_get_value(var);
    exprptr de = new_data_expr(value, NULL);
    let_expr_add_declaration(le, variable_get_name(var), de);
    delete_object(value);
  }

  char *result = expr_tostring(le);
  delete_expr(le);
  return result;
}

bool procedure_equals(objectptr self, objectptr other) {
  assert(false);
  return false;
}

size_t procedure_get_pn_arity(objectptr self) {
  proc_t *p = self->value;
  return lambda_expr_get_pn_arity(p->lambda);
}

size_t procedure_get_arity(objectptr self) {
  proc_t *p = self->value;
  return lambda_expr_get_arity(p->lambda);
}

bool procedure_is_pn_arity_given(objectptr self) {
  proc_t *p = self->value;
  return lambda_expr_is_pn_arity_given(p->lambda);
}

bool procedure_is_variadic(objectptr self) {
  proc_t *p = self->value;
  return lambda_expr_is_variadic(p->lambda);
}

static stack_frame_ptr construct_stack_frame(listptr closure, stack_frame_ptr sf) {
  stack_frame_ptr local_frame = new_stack_frame(sf);

  for (size_t i = 0; i < list_size(closure); ++i) {
    variableptr var = list_get(closure, i);
    const char *name = variable_get_name(var);
    objectptr value = variable_get_value(var);
    stack_frame_set_local_variable(local_frame, name, value);
    delete_object(value);
  }

  return local_frame;
}

objectptr procedure_op_call(objectptr self, size_t nargs, objectptr *args,
                           void *sf) {
  proc_t *p = self->value;
  stack_frame_ptr local_frame = construct_stack_frame(p->closure, sf); 
  objectptr result = expr_call(p->lambda, nargs, args, local_frame);
  delete_stack_frame(local_frame);
  return result;
}

objectptr procedure_op_call_internal(objectptr self, void *args, void *sf) {
  proc_t *p = self->value;
  stack_frame_ptr local_frame = construct_stack_frame(p->closure, sf);
  objectptr result = expr_call_internal(p->lambda, args, local_frame);
  delete_stack_frame(local_frame);
  return result;
}
