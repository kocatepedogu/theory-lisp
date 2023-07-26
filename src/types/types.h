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

#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdbool.h>

/* Object with type and value */
typedef struct object {
  void *value;
  const struct object_vtable *vtable;
  const char *type;
  bool temporary;
} object_t;

typedef struct object_vtable {
  void (*destroy)(object_t);
  object_t (*copy)(object_t);
  char *(*tostring)(object_t);
  bool (*equals)(object_t, object_t);

  object_t (*less)(object_t, object_t);

  object_t (*op_add)(object_t, object_t);
  object_t (*op_mul)(object_t, object_t);
  object_t (*op_sub)(object_t, object_t);
  object_t (*op_div)(object_t, object_t);

  object_t (*op_and)(object_t, object_t);
  object_t (*op_or)(object_t, object_t);
  object_t (*op_xor)(object_t, object_t);
  object_t (*op_not)(object_t);
} object_vtable_t;

void destroy_object(object_t obj);
object_t copy_object(object_t other);
void assign_object(object_t *dest, object_t src);
object_t move(object_t obj);
char *object_tostring(object_t obj);
bool object_equals(object_t obj, object_t other);

object_t object_less(object_t obj, object_t other);

object_t object_op_add(object_t obj, object_t other);
object_t object_op_mul(object_t obj, object_t other);
object_t object_op_sub(object_t obj, object_t other);
object_t object_op_div(object_t obj, object_t other);

object_t object_op_and(object_t obj, object_t other);
object_t object_op_or(object_t obj, object_t other);
object_t object_op_xor(object_t obj, object_t other);
object_t object_op_not(object_t obj);

#endif
