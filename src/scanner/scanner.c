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

#include "scanner.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../utils/heap-format.h"
#include "../utils/list.h"

static const struct {
  const char *keyword;
  token_type_t type;
} keywords[] = {{"let", TOKEN_LET},
                {"lambda", TOKEN_LAMBDA},
                {"if", TOKEN_IF},
                {"define", TOKEN_DEFINE},
                {"cond", TOKEN_COND},
                {")", TOKEN_RIGHT_PARENTHESIS},
                {"(", TOKEN_LEFT_PARENTHESIS},
                {"]", TOKEN_RIGHT_SQUARE_BRACKET},
                {"[", TOKEN_LEFT_SQUARE_BRACKET},
                {"}", TOKEN_RIGHT_CURLY_BRACKET},
                {"{", TOKEN_LEFT_CURLY_BRACKET},
                {"null", TOKEN_NULL},
                {"&", TOKEN_AMPERSAND},
                {"\\", TOKEN_BACKSLASH}};

typedef enum { 
  ST_WHITESPACE, 
  ST_TOKEN, 
  ST_SINGLE_LINE_COMMENT,
  ST_STRING
} scanner_state;

static bool token_keyword(token_type_t *type, token_value_t *value,
                          char *word) {
  for (int i = 0; i < sizeof keywords / sizeof(*keywords); i++) {
    if (strcmp(keywords[i].keyword, word) == 0) {
      *type = keywords[i].type;
      return true;
    }
  }
  return false;
}

static bool token_boolean(token_type_t *type, token_value_t *value,
                          char *word) {
  if (strcmp(word, "#t") == 0) {
    *type = TOKEN_BOOLEAN;
    value->boolean = true;
    return true;
  }

  if (strcmp(word, "#f") == 0) {
    *type = TOKEN_BOOLEAN;
    value->boolean = false;
    return true;
  }

  return false;
}

static bool token_number(token_type_t *type, token_value_t *value, char *word) {
  char *endptr = NULL;

  errno = 0;
  long longval = strtol(word, &endptr, 0);
  if (errno == 0 && endptr == word + strlen(word) && endptr != word) {
    *type = TOKEN_INTEGER;
    value->integer = longval;
    return true;
  }

  errno = 0;
  double doubleval = strtod(word, &endptr);
  if (errno == 0 && endptr == word + strlen(word) && endptr != word) {
    *type = TOKEN_REAL;
    value->real = doubleval;
    return true;
  }

  return false;
}

static bool token_identifier(token_type_t *type, token_value_t *value,
                             char *word) {
  if (!strchr(word, '(') && 
      !strchr(word, ')') &&
      !strchr(word, '[') &&
      !strchr(word, ']') && 
      !strchr(word, '&') &&
      !strchr(word, '{') &&
      !strchr(word, '}') &&
      !strchr(word, '\\') &&
      !isdigit(word[0]) &&
      word[0] != '\'') {
    *type = TOKEN_IDENTIFIER;
    strcpy(value->character_sequence, word);
    return true;
  }
  return false;
}

static inline void chrcat(char *str, char chr) {
  size_t len = strlen(str);
  str[len] = chr;
  str[len + 1] = '\0';
}

static bool get_tokens(listptr token_list, const char *str, size_t line_number,
                       size_t column_number) {
  size_t offset = 0;
  char current_token[TOKEN_SIZE];

  while (offset < strlen(str)) {
    memset(current_token, '\0', sizeof current_token);

    token_type_t tkn_type = TOKEN_END_OF_FILE;
    token_value_t tkn_value;

    size_t previous_offset = offset;
    for (size_t i = offset; i < strlen(str); i++) {
      chrcat(current_token, str[i]);

      if (token_keyword(&tkn_type, &tkn_value, current_token) ||
          token_number(&tkn_type, &tkn_value, current_token) ||
          token_boolean(&tkn_type, &tkn_value, current_token) ||
          token_identifier(&tkn_type, &tkn_value, current_token)) {
        (offset = i + 1);
      }
    }

    if (tkn_type == TOKEN_END_OF_FILE) {
      return false;
    }

    token_t *token = malloc(sizeof(token_t));
    token->type = tkn_type;
    token->value = tkn_value;
    token->line = line_number;
    token->column = column_number + previous_offset;
    list_add(token_list, token);
  }

  return true;
}

listptr scanner(const char *input) {
  listptr token_list = new_list();

  char word[WORD_SIZE];
  memset(word, 0, sizeof word);

  scanner_state state = ST_WHITESPACE;

  size_t line_number = 1;
  size_t column_number = 1;
  size_t column_of_the_first_char_of_the_word = 1;
  size_t length = strlen(input);

  for (size_t i = 0, j = 0; i < length; i++) {
    /* get one character from input */
    char c = input[i];

    bool is_semicolon = (c == ';');
    bool is_newline = (c == '\n');
    bool is_quote = (c == '\"');
    bool is_space = isspace(c);

    switch (state) {
      /* if scanner is currently in a sequence of whitespace characters */
      case ST_WHITESPACE:
        if (is_semicolon) {
          state = ST_SINGLE_LINE_COMMENT;
         } else if (is_quote) {
           column_of_the_first_char_of_the_word = column_number;
           state = ST_STRING;
        } else if (!is_space) {
          word[j++] = c;
          column_of_the_first_char_of_the_word = column_number;
          state = ST_TOKEN;
        }
        break;
      /* if scanner is currently in a quoted string */
      case ST_STRING:
        if (is_quote) {
          word[j] = '\0';

          token_t *tkn = (token_t *)malloc(sizeof(token_t));
          tkn->line = line_number;
          tkn->column = column_of_the_first_char_of_the_word;
          tkn->type = TOKEN_STRING;
          strcpy(tkn->value.character_sequence, word);
          list_add(token_list, tkn);

          memset(word, 0, sizeof word);
          state = ST_WHITESPACE;
          j = 0;
        } else {
          word[j++] = c;
        }
        break;
      /* if scanner is currently in a token */
      case ST_TOKEN:
        if (is_semicolon || is_space) {
          word[j] = '\0';

          get_tokens(token_list, word, line_number, 
          column_of_the_first_char_of_the_word);

          memset(word, 0, sizeof word);
          j = 0;
        } else {
          word[j++] = c;
        }

        state = is_semicolon ? ST_SINGLE_LINE_COMMENT : state;
        state = is_space ? ST_WHITESPACE : state;
        break;
      /* if scanner is currently in a line comment */
      case ST_SINGLE_LINE_COMMENT:
        if (is_newline) {
          state = ST_WHITESPACE;
        }
        break;
    }

    /* update line and column numbers */
    if (is_newline) {
      column_number = 1;
      line_number++;
    } else {
      column_number++;
    }
  }

  /* add the last token that was being processed when end of file was reached */
  get_tokens(token_list, word, line_number, column_number);

  /* add a final token that denotes the end of the file */
  token_t *token_eof = (token_t *)malloc(sizeof(token_t));
  token_eof->type = TOKEN_END_OF_FILE;
  token_eof->line = line_number;
  token_eof->column = column_number;
  list_add(token_list, token_eof);

  return token_list;
}

char *token_tostring(token_t *token) {
  switch (token->type) {
    case TOKEN_END_OF_FILE:
      return strdup("$");
    case TOKEN_IDENTIFIER:
      return heap_format("%s", token->value.character_sequence);
    case TOKEN_INTEGER:
      return heap_format("%ld", token->value.integer);
    case TOKEN_REAL:
      return heap_format("%f", token->value.real);
    case TOKEN_STRING:
      return heap_format("\"%s\"", token->value.character_sequence);
    default:
      for (int i = 0; i < sizeof keywords / sizeof(keywords[0]); i++) {
        if (keywords[i].type == token->type) {
          return strdup(keywords[i].keyword);
        }
      }

      fprintf(stderr, "Invalid token type %d", token->type);
      return NULL;
  }
}

void delete_token_list(listptr token_list) {
  for (size_t i = 0; i < list_size(token_list); i++) {
    free(list_get(token_list, i));
  }
  delete_list(token_list);
}
