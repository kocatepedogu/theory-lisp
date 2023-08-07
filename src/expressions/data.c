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

#include "data.h"
#include "expression.h"
#include "expression_base.h"
#include <string.h>


/* Data */
typedef struct {
  objectptr obj;
} data_expr;

static const char data_expr_name[] = "data_expr";

static const expr_vtable data_expr_vtable = {
  .destroy = destroy_data_expr,
  .to_string = data_expr_tostring,
  .interpret = interpret_data
};

bool is_data_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, data_expr_name) == 0;
}

exprptr new_data_expr(objectptr obj) {
  data_expr *de = malloc(sizeof *de);
  de->obj = clone_object(obj);

  return expr_base_new(de, &data_expr_vtable, data_expr_name, 0, 0);
}

void destroy_data_expr(exprptr self) {
  data_expr *de = self->data;
  delete_object(de->obj);
  free(de);
}

char *data_expr_tostring(exprptr self) {
  data_expr *de = self->data;
  return object_tostring(de->obj);
}

objectptr get_data_value(exprptr self) {
  data_expr *de = self->data;
  return de->obj;
}

objectptr interpret_data(exprptr self, stack_frame_ptr sf) {
  data_expr *de = self->data;
  return clone_object(de->obj);
}
