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

#include "eval.h"

#include <assert.h>


#include "../types/error.h"
#include "../types/string.h"
#include "../types/boolean.h"

#include "../scanner/scanner.h"
#include "../parser/parser.h"

objectptr builtin_eval(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  if (!is_string(*args)) {
    return make_error("eval argument is not a string");
  }

  char *str = string_value(*args);
  tokenstreamptr tkns = scanner(str);
  listptr expressions = parser(tkns, sf);

  if (expressions == NULL) {
    delete_tokenstream(tkns);
    return make_error("Parse error has occured during eval operation");
  }

  if (list_size(expressions) > 1) {
    delete_tokenstream(tkns);
    delete_parse_tree(expressions);
    return make_error("Evaluated string yield multiple expressions");
  }

  exprptr e = list_get(expressions, 0);
  objectptr result = interpret_expr(e, sf);

  delete_tokenstream(tkns);
  delete_parse_tree(expressions);

  return result;
}

objectptr builtin_defined(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert(n == 1);
  if (!is_string(*args)) {
    return make_error("Identifier name is not a string");
  }

  char *str = string_value(*args);
  objectptr value = stack_frame_get_variable(sf, str);
  bool answer = !is_error(value);
  delete_object(value);
  return make_boolean(answer);
}
