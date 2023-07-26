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

#include "builtin.h"

#include <string.h>

#define VARIADIC 0, true

// TODO: Perfect hashing is possible

const builtin_function builtin_functions[] = {
    {"display", builtin_display, 1, true},
    {"=", builtin_equals, 1, true},
    {"<", builtin_less, 2},
    {"<=", builtin_less_or_eq, 2},
    {">", builtin_greater, 2},
    {">=", builtin_greater_or_eq, 2},
    {"void?", builtin_is_void, 1},
    {"boolean?", builtin_is_boolean, 1},
    {"integer?", builtin_is_integer, 1},
    {"real?", builtin_is_real, 1},
    {"number?", builtin_is_number, 1},
    {"symbol?", builtin_is_symbol, 1},
    {"pair?", builtin_is_pair, 1},
    {"procedure?", builtin_is_procedure, 1},
    {"+", builtin_add, 0, true},
    {"*", builtin_mul, 0, true},
    {"-", builtin_sub, 1, true},
    {"/", builtin_div, 1, true},
    {"and", builtin_and, 0, true},
    {"or", builtin_or, 0, true},
    {"xor", builtin_xor, 0, true},
    {"not", builtin_not, 1},
    {"cons", builtin_cons, 2},
    {"car", builtin_car, 1},
    {"cdr", builtin_cdr, 1},
    {"list", builtin_list, 0, true}};

const size_t number_of_builtin_functions =
    sizeof builtin_functions / sizeof *builtin_functions;

const builtin_function *find_builtin_function(const char *name) {
  for (size_t i = 0; i < number_of_builtin_functions; i++) {
    if (strcmp(builtin_functions[i].name, name) == 0) {
      return &builtin_functions[i];
    }
  }
  return NULL;
}

bool is_builtin_name(const char *name) {
  return find_builtin_function(name) != NULL;
}
