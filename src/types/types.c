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

#include "types.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../utils/heap-format.h"
#include "boolean.h"
#include "error.h"
#include "integer.h"
#include "pair.h"
#include "procedure.h"
#include "real.h"
#include "symbol.h"
#include "types.h"
#include "void.h"

#define ERR_UNSUPPORTED_OPERATION "Unsupported operation"

/* Object copy constructor */
object_t copy_object(object_t other) {
  assert(other.vtable->copy);

  if (other.temporary) {
    other.temporary = false;
    return other;
  }

  if (other.vtable->copy) {
    return other.vtable->copy(other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

/* Object destructor */
void destroy_object(object_t obj) {
  assert(obj.vtable->destroy);
  obj.vtable->destroy(obj);
}

/* Object assignment operator */
void assign_object(object_t *dest, object_t src) {
  destroy_object(*dest);
  *dest = src;
}

object_t move(object_t obj) {
  obj.temporary = true;
  return obj;
}

/* Object tostring */
char *object_tostring(object_t obj) {
  if (obj.vtable->tostring) {
    return obj.vtable->tostring(obj);
  }
  return NULL;
}

/* Object equals */
bool object_equals(object_t obj, object_t other) {
  assert(obj.vtable->equals);

  if (obj.vtable->equals) {
    return obj.vtable->equals(obj, other);
  }

  return false;
}

/* Object < operator */
object_t object_less(object_t obj, object_t other) {
  if (obj.vtable->less) {
    return obj.vtable->less(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

/* Object + operator */
object_t object_op_add(object_t obj, object_t other) {
  if (obj.vtable->op_add) {
    return obj.vtable->op_add(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

/* Object * operator */
object_t object_op_mul(object_t obj, object_t other) {
  if (obj.vtable->op_mul) {
    return obj.vtable->op_mul(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

/* Object - operator */
object_t object_op_sub(object_t obj, object_t other) {
  if (obj.vtable->op_sub) {
    return obj.vtable->op_sub(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

/* Object / operator */
object_t object_op_div(object_t obj, object_t other) {
  if (obj.vtable->op_div) {
    return obj.vtable->op_div(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

/* Object && operator */
object_t object_op_and(object_t obj, object_t other) {
  if (obj.vtable->op_and) {
    return obj.vtable->op_and(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

/* Object || operator */
object_t object_op_or(object_t obj, object_t other) {
  if (obj.vtable->op_or) {
    return obj.vtable->op_or(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

/* Object ^ operator */
object_t object_op_xor(object_t obj, object_t other) {
  if (obj.vtable->op_xor) {
    return obj.vtable->op_xor(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

/* Object ! operator */
object_t object_op_not(object_t obj) {
  if (obj.vtable->op_not) {
    return obj.vtable->op_not(obj);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}
