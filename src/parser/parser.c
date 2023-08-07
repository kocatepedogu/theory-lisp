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

#include "parser.h"

#include <stdarg.h>
#include <stdio.h>

#include "../utils/string.h"
#include "../scanner/scanner.h"

void *parser_error(token_t *tkn, const char *format, ...) {
  va_list args;
  va_start(args, format);
  fprintf(stderr, "Error at line %ld, column %ld: ", tkn->line, tkn->column);
  vfprintf(stderr, format, args);
  fputc(L'\n', stderr);
  va_end(args);
  return NULL;
}

listptr parser(tokenstreamptr tkns) {
  listptr parse_tree = new_list();
  while (current_tkn(tkns)->type != TOKEN_END_OF_FILE) {
    exprptr e = expr_parse(tkns);
    if (e == NULL) {
      delete_parse_tree(parse_tree);
      return NULL;
    }
    list_add(parse_tree, e);
  }

  return parse_tree;
}

void delete_parse_tree(listptr parse_tree) {
  if (parse_tree) {
    for (size_t i = 0; i < list_size(parse_tree); ++i) {
      delete_expr(list_get(parse_tree, i));
    }
    delete_list(parse_tree);
  }
}
