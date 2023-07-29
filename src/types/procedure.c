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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../expressions/expression.h"
#include "../expressions/lambda_expr.h"
#include "../expressions/let_expr.h"
#include "../expressions/data_expr.h"
#include "../utils/heap-format.h"
#include "../interpreter/variable.h"
#include "../interpreter/stack_frame.h"



static const object_vtable_t procedure_vtable = {
    .clone = clone_procedure,
    .destroy = destroy_procedure,
    .tostring = procedure_tostring,
    .equals = procedure_equals,
};

static const char procedure_type_name[] = "procedure";

inline bool is_procedure(object_t obj) {
  return strcmp(procedure_type_name, obj.type) == 0;
}

inline proc_t *procedure_value(object_t obj) {
  assert(is_procedure(obj));
  return obj.value;
}

object_t make_procedure(procedure_t proc, stack_frame_ptr environment) {
  proc_t *p = (proc_t *)malloc(sizeof(proc_t));
  p->lambda = clone_expr(proc);
  construct_list(&p->closure);

  /* Capture variables from the environment */
  if (environment) {
    lambda_expr *le = ((exprptr)p->lambda)->data;
    for (size_t i = 0; i < list_size(&le->captured_vars); i++) {
      char *name = list_get(&le->captured_vars, i);
      object_t value = stack_frame_get_variable(environment, name);
      variable_t *var = new_variable(name, value);
      list_add(&p->closure, var);
      destroy_object(value);
    }
  }

  object_t obj;
  obj.value = p;
  obj.type = procedure_type_name;
  obj.vtable = &procedure_vtable;
  obj.temporary = false;
  return obj;
}

object_t clone_procedure(object_t self) {
  assert(is_procedure(self));
  proc_t *self_p = self.value;
  proc_t *new_p = (proc_t *)malloc(sizeof(proc_t));
  new_p->lambda = clone_expr(self_p->lambda);

  /* Clone values of captured variables */
  construct_list(&new_p->closure);
  for (size_t i = 0; i < list_size(&self_p->closure); i++) {
    variable_t *self_var = list_get(&self_p->closure, i);
    variable_t *new_var = new_variable(self_var->name, self_var->value);
    list_add(&new_p->closure, new_var);
  }

  object_t new_procedure;
  new_procedure.type = procedure_type_name;
  new_procedure.value = new_p;
  new_procedure.vtable = &procedure_vtable;
  new_procedure.temporary = false;
  return new_procedure;
}

void destroy_procedure(object_t self) {
  assert(is_procedure(self));
  proc_t *p = self.value;

  /* Delete captured variables */
  for (size_t i = 0; i < list_size(&p->closure); i++) {
    delete_variable(list_get(&p->closure, i));
  }
  destroy_list(&p->closure);

  /* Delete lambda */
  delete_expr(p->lambda);
  free(p);
}

char *procedure_tostring(object_t self) {
  assert(is_procedure(self));
  proc_t *p = self.value;

  /* If there are no captured variables, the string
   * representation of the lambda itself is enough
   * to describe the procedure object. 
   */
  if (list_size(&p->closure) == 0) {
    return expr_tostring(p->lambda);
  }

  /* Otherwise the captured variables are assigned values 
   * in a let expression whose body is the
   * lambda expression, so that evaluating the string representation
   * gives a procedure object with captured values.
   */
  exprptr le = new_let_expr(clone_expr(p->lambda));
  for (size_t i = 0; i < list_size(&p->closure); i++) {
    variable_t *var = list_get(&p->closure, i);
    let_expr_add_declaration(le, var->name, new_data_expr(var->value));
  }

  char *result = expr_tostring(le);
  delete_expr(le);
  return result;
}

bool procedure_equals(object_t self, object_t other) { 
  assert(false);
  return false; 
}

