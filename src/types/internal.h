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

/// @file internal.h

/**
 * Internal objects are used by parser to pass data to macros.
 * They can contain any data.
 */

#ifndef THEORYLISP_TYPES_INTERNAL_H
#define THEORYLISP_TYPES_INTERNAL_H

#include "object.h"
#include "../utils/string.h"

objectptr make_internal(void *ptr);

void destroy_internal(objectptr obj);

char *internal_tostring(objectptr obj);

bool internal_equals(objectptr obj, objectptr other);

bool is_internal(objectptr obj);

void *internal_get_raw_data(objectptr self);

#endif
