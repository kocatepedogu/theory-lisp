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

/// @file scanner.h

#ifndef THEORYLISP_SCANNER_SCANNER_H
#define THEORYLISP_SCANNER_SCANNER_H

#include <stdbool.h>
#include <stdlib.h>

#include "../utils/list.h"
#include "../utils/string.h"

typedef enum {
  /// include
  TOKEN_INCLUDE,
  /// null
  TOKEN_NULL,
  /// %
  TOKEN_PERCENT,
  /** Unquoted sequence of characters that does not begin
   * with a special character or digit. */
  TOKEN_IDENTIFIER,
  /// A quoted string
  TOKEN_STRING,
  /// #t and #f
  TOKEN_BOOLEAN,
  /// C integers
  TOKEN_INTEGER,
  /// C doubles
  TOKEN_REAL,
  /// Rational numbers
  TOKEN_RATIONAL,
  /// (
  TOKEN_LEFT_PARENTHESIS,
  /// )
  TOKEN_RIGHT_PARENTHESIS,
  /// [
  TOKEN_LEFT_SQUARE_BRACKET,
  /// ]
  TOKEN_RIGHT_SQUARE_BRACKET,
  /// {
  TOKEN_LEFT_CURLY_BRACKET,
  /// }
  TOKEN_RIGHT_CURLY_BRACKET,
  /// "\"
  TOKEN_BACKSLASH,
  /// cond
  TOKEN_COND,
  /// define
  TOKEN_DEFINE,
  /// define-syntax
  TOKEN_DEFINE_SYNTAX,
  /// try
  TOKEN_TRY,
  /// catch
  TOKEN_CATCH,
  /// set!
  TOKEN_SET,
  /// if
  TOKEN_IF,
  /// lambda
  TOKEN_LAMBDA,
  /// let
  TOKEN_LET,
  /// automaton
  TOKEN_AUTOMATON,
  /// ->
  TOKEN_MOVE_RIGHT,
  /// <-
  TOKEN_MOVE_LEFT,
  /// .
  TOKEN_NOP,
  /// :
  TOKEN_COLON,
  /// Placed at the end of token list by scanner
  TOKEN_END_OF_FILE
} token_type_t;

typedef union {
  char *character_sequence;
  bool boolean;
  long integer;
  double real;
  long rational[2];
} token_value_t;

typedef struct {
  /// Kind of token
  token_type_t type;
  /// Value of the token (not all tokens store value in this field)
  token_value_t value;
  /// Line where the token is found
  size_t line;
  /// Column number of the first character of the token
  size_t column;
} token_t;

typedef token_t *tokenptr;

typedef struct tokenstream{
  listptr tokens;
  size_t index;
} tokenstream_t;

typedef tokenstream_t *tokenstreamptr;

/**
 * Takes a char array and returns a list of tokens
 *
 * If an error occurs during scanning, the constructed portion of the list is
 * deallocated and NULL is returned.
 */
tokenstreamptr scanner(const char *input);

/**
 * Returns the string representation of a token.
 * Substituting the result into the scanner must give the same token
 * except the end of file token.
 */
char *token_tostring(token_t *token);

void delete_tokenstream(tokenstreamptr tkns);

tokenptr next_tkn(tokenstreamptr tkns);

tokenptr current_tkn(tokenstreamptr tkns);

tokenptr ahead_tkn(tokenstreamptr tkns);

tokenptr prev_tkn(tokenstreamptr tkns);

#endif
