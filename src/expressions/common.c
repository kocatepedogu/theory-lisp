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

#include <string.h>

#include "common.h"
#include "../parser/parser.h"
#include "../utils/string.h"

listptr get_capture_list(tokenstreamptr tkns) {
  listptr captured_variables = new_list();

  tokenptr left_bracket = current_tkn(tkns);
  if (left_bracket->type == TOKEN_LEFT_SQUARE_BRACKET) {
    (void)next_tkn(tkns);
    
    tokenptr captured_token = NULL;
    while ((captured_token = current_tkn(tkns))->type == TOKEN_IDENTIFIER) {
      list_add(captured_variables, captured_token->value.character_sequence);
      (void)next_tkn(tkns);
    }

    tokenptr right_bracket = next_tkn(tkns);
    if (right_bracket->type != TOKEN_RIGHT_SQUARE_BRACKET) {
      delete_list(captured_variables);
      return parser_error(right_bracket, "Capture list is incomplete");
    }
  }

  return captured_variables;
}

char *capture_list_tostring(listptr lst) {
  char *captures = NULL;
  for (size_t i = 0; i < list_size(lst); i++) {
    char *str = list_get(lst, i);
    captures = unique_append_sep(captures, " ", strdup(str));
  }

  if (captures) {
    return unique_format("[%s]", captures);
  } else {
    return unique_format("");    
  }
}
