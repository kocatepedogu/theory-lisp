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

/// @file parser.h

#ifndef THEORYLISP_PARSER_PARSER_H
#define THEORYLISP_PARSER_PARSER_H

#include "../utils/list.h"
#include "../scanner/scanner.h"

#include "../expressions/definition.h"
#include "../expressions/evaluation.h"
#include "../expressions/lambda.h"
#include "../expressions/let.h"
#include "../expressions/cond.h"
#include "../expressions/data.h"
#include "../expressions/expression.h"

/**
 * A common error printing function for parser functions.
 * Displays the line and column numbers where the error has occured,
 * and prints the given formatted error message.
 */
void *parser_error(token_t *tkn, const char *format, ...);

/**
 * Takes a list of tokens and returns a list of expressions (parse tree).
 *
 * If an error occurs during parsing, the constructed portion of the
 * parse tree is deallocated and NULL is returned. It will print an error
 * message before returning to the caller.
 */
listptr parser(tokenstreamptr tkns);

/**
 * Recursively deallocates the given parse tree.
 */
void delete_parse_tree(listptr parse_tree);

#endif
