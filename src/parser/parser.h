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

#ifndef PARSER_H
#define PARSER_H

#include "../utils/list.h"
#include "../scanner/scanner.h"

#include "../expressions/definition_expr.h"
#include "../expressions/evaluation_expr.h"
#include "../expressions/lambda_expr.h"
#include "../expressions/let_expr.h"
#include "../expressions/cond_expr.h"
#include "../expressions/data_expr.h"
#include "../expressions/expression.h"

void *parser_error(size_t line, size_t column, char *format, ...);
list *parser(list *token_list);
void delete_parse_tree(list *parse_tree);

#endif
