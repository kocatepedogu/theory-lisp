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

/// @file rational.h

#ifndef THEORYLISP_TYPES_RATIONAL_H
#define THEORYLISP_TYPES_RATIONAL_H

#include "integer.h"
#include "object.h"

#include "../utils/string.h"

typedef struct rational {
  integer_t x;
  integer_t y;
} rational_t;

/** x / y */
objectptr make_rational(integer_t x, integer_t y);

void destroy_rational(objectptr val);

char *rational_tostring(objectptr val);

bool rational_equals(objectptr self, objectptr other);

objectptr rational_less(objectptr self, objectptr other);

objectptr rational_op_add(objectptr self, objectptr other);

objectptr rational_op_mul(objectptr self, objectptr other);

objectptr rational_op_sub(objectptr self, objectptr other);

objectptr rational_op_div(objectptr self, objectptr other);

bool is_rational(objectptr obj);

rational_t rational_value(objectptr obj);

#endif
