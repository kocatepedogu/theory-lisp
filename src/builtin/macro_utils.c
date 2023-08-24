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

#include "io.h"

#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "../scanner/scanner.h"
#include "../expressions/expression.h"
#include "../types/void.h"
#include "../types/string.h"
#include "../types/error.h"
#include "../types/integer.h"
#include "../types/internal.h"

objectptr builtin_peek_tkn(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  if (!is_internal(*args)) {
    return make_error("peek-tkn requires an internal object.");
  }

  tokenstreamptr tkns = internal_get_raw_data(*args);
  char *str = token_tostring(current_tkn(tkns));
  objectptr result = make_string(str);
  free(str);
  return result;
}

objectptr builtin_pop_tkn(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  if (!is_internal(*args)) {
    return make_error("pop-tkn requires an internal object.");
  }

  tokenstreamptr tkns = internal_get_raw_data(*args);
  char *str = token_tostring(next_tkn(tkns));
  objectptr result = make_string(str);
  free(str);
  return result;
}

objectptr builtin_parse(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  if (!is_internal(*args)) {
    return make_error("parse requires an internal object.");
  }

  exprptr expression = expr_parse(internal_get_raw_data(*args), sf);
  if (expression == NULL) {
    return make_error("A parser error has occured");
  }

  char *expression_str = expr_tostring(expression);
  delete_expr(expression);
  objectptr result = make_string(expression_str);
  free(expression_str);
  return result;
}
