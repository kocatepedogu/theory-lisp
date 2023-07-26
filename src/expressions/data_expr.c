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

static const char data_expr_name[] = "data_expr";

static const expr_vtable data_expr_vtable = {
  .deallocate = delete_data_expr,
  .destructor = destroy_data_expr,
  .to_string = data_expr_tostring,
  .interpret = interpret_data
};

void construct_data_expr(exprptr e, object_t obj) {
  data_expr *de = (data_expr *)malloc(sizeof(data_expr));
  de->obj = copy_object(obj);
  e->data = de;
  e->vtable = &data_expr_vtable;
  e->expr_name = data_expr_name;
}

exprptr new_data_expr(object_t obj) {
  expr_t *e = (expr_t *)malloc(sizeof(expr_t));
  construct_data_expr(e, obj);
  return e;
}

void destroy_data_expr(exprptr e) {
  data_expr *de = e->data;
  destroy_object(de->obj);
  free(de);
}

void delete_data_expr(exprptr e) {
  destroy_data_expr(e);
  free(e);
}

char *data_expr_tostring(exprptr e) {
  data_expr *de = e->data;
  return object_tostring(de->obj);
}

bool is_data_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, data_expr_name) == 0;
}

object_t interpret_data(exprptr e, stack_frame_ptr sf) {
  data_expr *de = e->data;
  return copy_object(de->obj);
}
