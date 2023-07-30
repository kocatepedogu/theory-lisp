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

#include "data_expr.h"
#include "expression.h"
#include <string.h>

/* Data */
typedef struct {
  object_t obj;
} data_expr;

static const char data_expr_name[] = "data_expr";

static const expr_vtable data_expr_vtable = {
  .deallocate = delete_data_expr,
  .clone = clone_data_expr,
  .to_string = data_expr_tostring,
  .interpret = interpret_data
};

exprptr new_data_expr(object_t obj) {
  data_expr *de = (data_expr *)malloc(sizeof(data_expr));
  de->obj = clone_object(obj);

  expr_t *e = (expr_t *)malloc(sizeof(expr_t));
  e->data = de;
  e->vtable = &data_expr_vtable;
  e->expr_name = data_expr_name;
  return e;
}

void delete_data_expr(exprptr self) {
  data_expr *de = self->data;
  destroy_object(de->obj);
  free(de);
  free(self);
}

exprptr clone_data_expr(exprptr self) {
  data_expr *new_de = (data_expr *)malloc(sizeof(data_expr));
  data_expr *self_de = self->data;
  new_de->obj = clone_object(self_de->obj);

  return base_clone(self, new_de);
}

char *data_expr_tostring(exprptr self) {
  data_expr *de = self->data;
  return object_tostring(de->obj);
}

object_t get_data_value(exprptr self) {
  data_expr *de = self->data;
  return de->obj;
}

bool is_data_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, data_expr_name) == 0;
}

object_t interpret_data(exprptr self, stack_frame_ptr sf) {
  data_expr *de = self->data;
  return clone_object(de->obj);
}
