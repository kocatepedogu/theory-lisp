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

#include "eval.h"

#include <assert.h>

#include "../types/error.h"
#include "../types/string.h"
#include "../types/boolean.h"

#include "../scanner/scanner.h"
#include "../parser/parser.h"

object_t builtin_eval(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);
  if (!is_string(*args)) {
    return make_error("eval argument is not a string");
  }

  char *str = string_value(*args);
  list *tokens = scanner(str);
  list *expressions = parser(tokens);

  if (expressions == NULL) {
    delete_token_list(tokens);
    return make_error("Parse error has occured during eval operation");
  }

  if (list_size(expressions) > 1) {
    delete_token_list(tokens);
    delete_parse_tree(expressions);
    return make_error("Evaluated string yield multiple expressions");
  }

  exprptr e = list_get(expressions, 0);
  object_t result = interpret_expr(e, sf);

  delete_token_list(tokens);
  delete_parse_tree(expressions);

  return result;
}

object_t builtin_defined(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);
  if (!is_string(*args)) {
    return make_error("Identifier name is not a string");
  }

  char *str = string_value(*args);
  object_t value = stack_frame_get_variable(sf, str);
  bool answer = !is_error(value);
  destroy_object(value);
  return make_boolean(answer);
}
