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

/// @file scanner.h

#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include <stdbool.h>
#include "../utils/list.h"

#define WORD_SIZE 128
#define TOKEN_SIZE 128
#define CHARSEQ_SIZE 128

typedef enum {
  /// null
  TOKEN_NULL,
  /// &
  TOKEN_AMPERSAND,
  /// Unquoted sequence of characters that does not begin with a special character or digit.
  TOKEN_IDENTIFIER,
  /// A quoted string
  TOKEN_STRING,
  /// #t and #f
  TOKEN_BOOLEAN,
  /// C integers
  TOKEN_INTEGER,
  /// C doubles
  TOKEN_REAL,
  /// (
  TOKEN_LEFT_PARENTHESIS,
  /// )
  TOKEN_RIGHT_PARENTHESIS,
  /// [
  TOKEN_LEFT_SQUARE_BRACKET,
  /// ]
  TOKEN_RIGHT_SQUARE_BRACKET,
  /// cond
  TOKEN_COND,
  /// define
  TOKEN_DEFINE,
  /// if
  TOKEN_IF,
  /// lambda
  TOKEN_LAMBDA,
  /// let
  TOKEN_LET,
  /// Placed at the end of token list by scanner
  TOKEN_END_OF_FILE
} token_type_t;

typedef union {
  char character_sequence[CHARSEQ_SIZE];
  bool boolean;
  long integer;
  double real;
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

/**
 * Takes a char array and returns a list of tokens
 *
 * If an error occurs during scanning, the constructed portion of the list is
 * deallocated and NULL is returned.
 */
list *scanner(const char *input);

/**
 * Returns the string representation of a token.
 * Substituting the result into the scanner must give the same token
 * except the end of file token.
 */
char *token_tostring(token_t *token);

/** Deallocates all tokens from the given token list and then deletes the list itself. */
void delete_token_list(list* token_list);

#endif
