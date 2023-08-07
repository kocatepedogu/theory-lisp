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

/// @file object-base.h

#ifndef THEORYLISP_TYPES_OBJECT_BASE_H
#define THEORYLISP_TYPES_OBJECT_BASE_H

#include <stdbool.h>
#include <stdlib.h>

#include "object.h"

/**
 * object_vtable_t contains operations that are supported by an object.
 * All objects must implement destroy, tostring and equals functions.
 * The remaining are optional. If an operation is not implemented, the
 * member corresponding to it must be set to NULL. If an unsupported operation
 * is called, it will return an error object containing "unsupported operation"
 * message.
 */
typedef struct object_vtable {
  void (*destroy)(objectptr);
  objectptr (*clone)(objectptr);
  char *(*tostring)(objectptr);
  bool (*equals)(objectptr, objectptr);
  objectptr (*less)(objectptr, objectptr);
  void (*delete_obj)(objectptr);

  objectptr (*op_add)(objectptr, objectptr);
  objectptr (*op_mul)(objectptr, objectptr);
  objectptr (*op_sub)(objectptr, objectptr);
  objectptr (*op_div)(objectptr, objectptr);

  objectptr (*op_and)(objectptr, objectptr);
  objectptr (*op_or)(objectptr, objectptr);
  objectptr (*op_xor)(objectptr, objectptr);
  objectptr (*op_not)(objectptr);

  objectptr (*op_call)(objectptr, size_t, objectptr *, void *);
  objectptr (*op_call_internal)(objectptr, void *, void *);
} object_vtable_t;

/**
 * Contains type information of a Theory Lisp object.
 */
typedef struct tltype {
  const object_vtable_t vtable;
  const char *type_name;
} object_type_t;

/**
 * All objects in Theory Lisp are represented with an object_t.
 */
typedef struct object {
  void *value;
  const object_type_t *type_id;
  bool temporary;
  size_t ref_count;
} object_t;


/* Base constructor */
objectptr object_base_new(void *value, const object_type_t *type_id);

#endif
