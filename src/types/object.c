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

#include "object.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


#include "object-base.h"
#include "../utils/string.h"
#include "boolean.h"
#include "error.h"
#include "object.h"

#define ERR_UNSUPPORTED_OPERATION "Unsupported operation"

objectptr object_base_new(void *value, const object_type_t *type_id) {
  objectptr obj = malloc(sizeof *obj);
  obj->value = value;
  obj->type_id = type_id;
  obj->temporary = false;
  obj->ref_count = 1;
  return obj;
}

objectptr clone_object(objectptr other) {
  if (other->temporary) {
    other->temporary = false;
    return other;
  }

  if (other->type_id->vtable.clone) {
    /* If object explicitly provides a clone method,
     * call that specific clone method. */
    return other->type_id->vtable.clone(other);
  } else {
    /* If object does not provide a clone method,
     * it is assumed to be immutable, and there is no
     * need for deep copy. Just increase the reference
     * count, and return the same object. */
    ++other->ref_count;
    return other;
  }
}

void delete_object(objectptr obj) {
  /* If object explicitly provides a delete method,
   * call that specific delete method */
  if (obj->type_id->vtable.delete_obj) {
    obj->type_id->vtable.delete_obj(obj);
  } else {
    /* If object does not provide a delete method,
     * decrease the reference count and delete object
     * if the count reaches 0. */
    assert(obj->type_id->vtable.destroy);
    if (--obj->ref_count == 0)
    {
      obj->type_id->vtable.destroy(obj);
      free(obj);
    }
  }
}

void assign_object(objectptr *dest, objectptr src) {
  delete_object(*dest);
  *dest = src;
}

objectptr move(objectptr obj) {
  obj->temporary = true;
  return obj;
}

char *object_tostring(objectptr obj) {
  if (obj->type_id->vtable.tostring) {
    return obj->type_id->vtable.tostring(obj);
  }
  return NULL;
}

bool object_equals(objectptr obj, objectptr other) {
  assert(obj->type_id->vtable.equals);

  if (obj->type_id->vtable.equals) {
    return obj->type_id->vtable.equals(obj, other);
  }

  return false;
}

objectptr object_less(objectptr obj, objectptr other) {
  if (obj->type_id->vtable.less) {
    return obj->type_id->vtable.less(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_add(objectptr obj, objectptr other) {
  if (obj->type_id->vtable.op_add) {
    return obj->type_id->vtable.op_add(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_mul(objectptr obj, objectptr other) {
  if (obj->type_id->vtable.op_mul) {
    return obj->type_id->vtable.op_mul(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_sub(objectptr obj, objectptr other) {
  if (obj->type_id->vtable.op_sub) {
    return obj->type_id->vtable.op_sub(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_div(objectptr obj, objectptr other) {
  if (obj->type_id->vtable.op_div) {
    return obj->type_id->vtable.op_div(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_and(objectptr obj, objectptr other) {
  if (obj->type_id->vtable.op_and) {
    return obj->type_id->vtable.op_and(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_or(objectptr obj, objectptr other) {
  if (obj->type_id->vtable.op_or) {
    return obj->type_id->vtable.op_or(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_xor(objectptr obj, objectptr other) {
  if (obj->type_id->vtable.op_xor) {
    return obj->type_id->vtable.op_xor(obj, other);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_not(objectptr obj) {
  if (obj->type_id->vtable.op_not) {
    return obj->type_id->vtable.op_not(obj);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_call(objectptr obj, size_t nargs, objectptr *args,
                        void *sf) {
  if (obj->type_id->vtable.op_call) {
    return obj->type_id->vtable.op_call(obj, nargs, args, sf);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

objectptr object_op_call_internal(objectptr obj, void *args, void *sf) {
  if (obj->type_id->vtable.op_call_internal) {
    return obj->type_id->vtable.op_call_internal(obj, args, sf);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}

void *object_get_raw_data(objectptr obj) {
  if (obj->type_id->vtable.get_raw_data) {
    return obj->type_id->vtable.get_raw_data(obj);
  }

  return make_error(ERR_UNSUPPORTED_OPERATION);
}
