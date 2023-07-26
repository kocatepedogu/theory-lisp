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

#ifndef SCANNER_H
#define SCANNER_H

#include <stdlib.h>
#include <stdbool.h>
#include "../utils/list.h"

#define WORD_SIZE 128
#define TOKEN_SIZE 128
#define CHARSEQ_SIZE 128

typedef enum {
  TOKEN_NULL,
  TOKEN_AMPERSAND,
  TOKEN_IDENTIFIER,
  TOKEN_SYMBOL,
  TOKEN_BOOLEAN,
  TOKEN_INTEGER,
  TOKEN_REAL,
  TOKEN_LEFT_PARENTHESIS,
  TOKEN_RIGHT_PARENTHESIS,
  TOKEN_COND,
  TOKEN_DEFINE,
  TOKEN_IF,
  TOKEN_LAMBDA,
  TOKEN_LET,
  TOKEN_END_OF_FILE
} token_type_t;

typedef union {
  char character_sequence[CHARSEQ_SIZE];
  bool boolean;
  long integer;
  double real;
} token_value_t;

typedef struct {
  token_type_t type;
  token_value_t value;
  size_t line;
  size_t column;
} token_t;

list *scanner(const char *input);
char *token_tostring(token_t *token);
void delete_token_list(list* token_list);

#endif
