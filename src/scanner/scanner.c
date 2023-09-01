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

#include "scanner.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../utils/list.h"
#include "../utils/string.h"

#define INITIAL_TOKEN_SIZE 128
#define INITIAL_WORD_SIZE 128

static const struct {
  const char *keyword;
  token_type_t type;
} keywords[] = {
    {"let", TOKEN_LET},         {"lambda", TOKEN_LAMBDA},
    {"if", TOKEN_IF},           {"define", TOKEN_DEFINE},
    {"cond", TOKEN_COND},       {"null", TOKEN_NULL},
    {"set!", TOKEN_SET},        {"automaton", TOKEN_AUTOMATON},
    {"<-", TOKEN_MOVE_LEFT},    {"->", TOKEN_MOVE_RIGHT},
    {".", TOKEN_NOP},           {"define-syntax", TOKEN_DEFINE_SYNTAX},
    {"include", TOKEN_INCLUDE}, {"try", TOKEN_TRY},
    {"catch", TOKEN_CATCH}};

static const struct {
  const char c;
  token_type_t type;
} special_characters[] = {{'(', TOKEN_LEFT_PARENTHESIS},
                          {')', TOKEN_RIGHT_PARENTHESIS},
                          {'[', TOKEN_LEFT_SQUARE_BRACKET},
                          {']', TOKEN_RIGHT_SQUARE_BRACKET},
                          {'{', TOKEN_LEFT_CURLY_BRACKET},
                          {'}', TOKEN_RIGHT_CURLY_BRACKET},
                          {'%', TOKEN_PERCENT},
                          {'\\', TOKEN_BACKSLASH},
                          {':', TOKEN_COLON}};

typedef enum {
  ST_WHITESPACE,
  ST_TOKEN,
  ST_SINGLE_LINE_COMMENT,
  ST_STRING
} scanner_state;

typedef struct {
  char *word;
  size_t word_capacity;
  size_t line;
  size_t column;
  size_t first_column_of_word;
  size_t position_in_input;
  size_t position_in_current_word;
} scanner_position_data;

static inline bool is_semicolon(char c) { return c == L';'; }

static inline bool is_newline(char c) { return c == L'\n'; }

static inline bool is_quote(char c) { return c == L'\"'; }

static inline bool is_space(char c) { return isspace(c); }

static inline void chrcat(char **str, size_t *capacity, char chr) {
  if (strlen(*str) == *capacity - 1) {
    *str = realloc(*str, (*capacity *= 2));
  }

  size_t len = strlen(*str);
  (*str)[len] = chr;
  (*str)[len + 1] = '\0';
}

static bool token_special_character(token_type_t *type, token_value_t *value,
                                    const char *word) {
  if (strlen(word) != 1) {
    return false;
  }

  for (size_t i = 0; i < sizeof special_characters / sizeof *special_characters;
       ++i) {
    if (word[0] == special_characters[i].c) {
      *type = special_characters[i].type;
      return true;
    }
  }

  return false;
}

static bool token_keyword(token_type_t *type, token_value_t *value,
                          const char *word) {
  for (int i = 0; i < sizeof keywords / sizeof(*keywords); ++i) {
    if (strcmp(keywords[i].keyword, word) == 0) {
      *type = keywords[i].type;
      return true;
    }
  }
  return false;
}

static bool token_boolean(token_type_t *type, token_value_t *value,
                          const char *word) {
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

static bool token_number(token_type_t *type, token_value_t *value,
                         const char *word) {
  char *endptr = NULL;

  /* Integers */

  errno = 0;
  long longval = strtol(word, &endptr, 0);
  if (errno == 0 && endptr == word + strlen(word) && endptr != word) {
    *type = TOKEN_INTEGER;
    value->integer = longval;
    return true;
  }

  /* Real numbers */

  errno = 0;
  double doubleval = strtod(word, &endptr);
  if (errno == 0 && endptr == word + strlen(word) && endptr != word) {
    *type = TOKEN_REAL;
    value->real = doubleval;
    return true;
  }

  /* Rational numbers */

  char *ptr = strchr(word, '/');
  if (ptr) {
    errno = 0;
    long x = strtol(word, &endptr, 0);
    if (errno == 0 && endptr == ptr && endptr != word) {
      errno = 0;
      long y = strtol(ptr + 1, &endptr, 0);
      if (errno == 0 && endptr == word + strlen(word) && endptr != ptr + 1) {
        *type = TOKEN_RATIONAL;
        value->rational[0] = x;
        value->rational[1] = y;
        return true;
      }
    }
  }

  return false;
}

static bool token_identifier(token_type_t *type, token_value_t *value,
                             const char *word) {
  for (size_t i = 0; i < sizeof special_characters / sizeof *special_characters;
       ++i) {
    if (strchr(word, special_characters[i].c)) {
      return false;
    }
  }

  if (isdigit(word[0])) {
    return false;
  }

  *type = TOKEN_IDENTIFIER;
  value->character_sequence = malloc(strlen(word) + 1);
  strcpy(value->character_sequence, word);

  return true;
}

char *token_tostring(tokenptr token) {
  switch (token->type) {
    case TOKEN_END_OF_FILE:
      return strdup("$");
    case TOKEN_IDENTIFIER:
      return format("%s", token->value.character_sequence);
    case TOKEN_BOOLEAN:
      return format(token->value.boolean ? "#t" : "#f");
    case TOKEN_INTEGER:
      return format("%ld", token->value.integer);
    case TOKEN_REAL:
      return format("%f", token->value.real);
    case TOKEN_STRING:
      return format("\"%s\"", token->value.character_sequence);
    default:
      for (size_t i = 0; i < sizeof keywords / sizeof(keywords[0]); ++i) {
        if (keywords[i].type == token->type) {
          return strdup(keywords[i].keyword);
        }
      }

      for (size_t i = 0;
           i < sizeof special_characters / sizeof special_characters[0]; ++i) {
        if (special_characters[i].type == token->type) {
          return format("%lc", special_characters[i].c);
        }
      }

      fprintf(stderr, "Invalid token type %d\n", token->type);
      abort();
  }
}

static bool get_tokens(listptr token_list, const char *str, size_t line_number,
                       size_t column_number) {
  size_t offset = 0;

  while (offset < strlen(str)) {
    size_t current_token_capacity = INITIAL_TOKEN_SIZE;
    char *current_token = calloc(current_token_capacity, 1);

    token_type_t tkn_type = TOKEN_END_OF_FILE;
    token_value_t tkn_value;

    size_t previous_offset = offset;
    for (size_t i = offset; i < strlen(str); ++i) {
      chrcat(&current_token, &current_token_capacity, str[i]);

      token_type_t new_tkn_type = TOKEN_END_OF_FILE;
      token_value_t new_tkn_value;

      if (token_special_character(&new_tkn_type, &new_tkn_value,
                                  current_token) ||
          token_keyword(&new_tkn_type, &new_tkn_value, current_token) ||
          token_number(&new_tkn_type, &new_tkn_value, current_token) ||
          token_boolean(&new_tkn_type, &new_tkn_value, current_token) ||
          token_identifier(&new_tkn_type, &new_tkn_value, current_token)) {
        if (tkn_type == TOKEN_IDENTIFIER) {
          free(tkn_value.character_sequence);
        }

        tkn_type = new_tkn_type;
        tkn_value = new_tkn_value;
        (offset = i + 1);
      }
    }

    if (tkn_type == TOKEN_END_OF_FILE) {
      free(current_token);
      return false;
    }

    token_t *token = malloc(sizeof *token);
    token->type = tkn_type;
    token->value = tkn_value;
    token->line = line_number;
    token->column = column_number + previous_offset;
    list_add(token_list, token);

    free(current_token);
  }

  return true;
}

static scanner_state scanner_state_whitespace(char c, scanner_position_data *p,
                                              listptr tokens) {
  if (is_semicolon(c)) {
    return ST_SINGLE_LINE_COMMENT;
  }

  if (is_quote(c)) {
    p->first_column_of_word = p->column;
    return ST_STRING;
  }

  if (!is_space(c)) {
    if (p->position_in_current_word == p->word_capacity - 1) {
      p->word = realloc(p->word, (p->word_capacity *= 2));
    }

    p->word[p->position_in_current_word++] = c;
    p->first_column_of_word = p->column;
    return ST_TOKEN;
  }

  return ST_WHITESPACE;
}

static scanner_state scanner_state_string(char c, scanner_position_data *p,
                                          listptr tokens) {
  if (is_quote(c)) {
    p->word[p->position_in_current_word] = '\0';

    token_t *tkn = malloc(sizeof *tkn);
    tkn->line = p->line;
    tkn->column = p->first_column_of_word;
    tkn->type = TOKEN_STRING;
    tkn->value.character_sequence = malloc(strlen(p->word) + 1);
    strcpy(tkn->value.character_sequence, p->word);
    list_add(tokens, tkn);

    memset(p->word, 0, p->word_capacity);
    p->position_in_current_word = 0;
    return ST_WHITESPACE;
  }

  if (p->position_in_current_word == p->word_capacity - 1) {
    p->word = realloc(p->word, (p->word_capacity *= 2));
  }

  p->word[p->position_in_current_word++] = c;
  return ST_STRING;
}

static scanner_state scanner_state_token(char c, scanner_position_data *p,
                                         listptr tokens) {
  if (is_semicolon(c) || is_space(c) || is_quote(c)) {
    p->word[p->position_in_current_word] = '\0';
    get_tokens(tokens, p->word, p->line, p->first_column_of_word);
    memset(p->word, 0, p->word_capacity);
    p->position_in_current_word = 0;
  }

  if (is_semicolon(c)) {
    return ST_SINGLE_LINE_COMMENT;
  }

  if (is_quote(c)) {
    p->first_column_of_word = p->column;
    return ST_STRING;
  }

  if (is_space(c)) {
    return ST_WHITESPACE;
  }

  p->word[p->position_in_current_word++] = c;
  return ST_TOKEN;
}

static scanner_state scanner_state_comment(char c, scanner_position_data *p,
                                           listptr tokens) {
  if (is_newline(c)) {
    return ST_WHITESPACE;
  }

  return ST_SINGLE_LINE_COMMENT;
}

tokenstreamptr scanner(const char *input) {
  listptr token_list = new_list();

  scanner_state state = ST_WHITESPACE;
  scanner_position_data position = {.position_in_input = 0,
                                    .position_in_current_word = 0,
                                    .column = 1,
                                    .line = 1,
                                    .first_column_of_word = 1,
                                    .word_capacity = INITIAL_WORD_SIZE,
                                    .word = calloc(INITIAL_WORD_SIZE, 1)};

  size_t length = strlen(input);

  for (; position.position_in_input < length; ++position.position_in_input) {
    char c = input[position.position_in_input];

    switch (state) {
      case ST_WHITESPACE:
        state = scanner_state_whitespace(c, &position, token_list);
        break;
      case ST_STRING:
        state = scanner_state_string(c, &position, token_list);
        break;
      case ST_TOKEN:
        state = scanner_state_token(c, &position, token_list);
        break;
      case ST_SINGLE_LINE_COMMENT:
        state = scanner_state_comment(c, &position, token_list);
        break;
    }

    if (is_newline(c)) {
      position.column = 1;
      position.line++;
    } else {
      position.column++;
    }
  }

  /* add the last token that was being processed when end of file was reached */
  get_tokens(token_list, position.word, position.line, position.column);
  free(position.word);

  /* add a final token that denotes the end of the file */
  tokenptr token_eof = malloc(sizeof *token_eof);
  token_eof->type = TOKEN_END_OF_FILE;
  token_eof->line = position.line;
  token_eof->column = position.column;
  list_add(token_list, token_eof);

  /* return a "token stream" object that stores the current token position
   * and the list of tokens */
  tokenstreamptr tkns = malloc(sizeof *tkns);
  tkns->tokens = token_list;
  tkns->index = 0;
  return tkns;
}

void delete_tokenstream(tokenstreamptr tkns) {
  for (size_t i = 0; i < list_size(tkns->tokens); ++i) {
    tokenptr tkn = list_get(tkns->tokens, i);
    if (tkn->type == TOKEN_IDENTIFIER || tkn->type == TOKEN_STRING) {
      free(tkn->value.character_sequence);
    }
    free(tkn);
  }
  delete_list(tkns->tokens);
  free(tkns);
}

tokenptr next_tkn(tokenstreamptr tkns) {
  return list_get(tkns->tokens, tkns->index++);
}

tokenptr current_tkn(tokenstreamptr tkns) {
  return list_get(tkns->tokens, tkns->index);
}

tokenptr ahead_tkn(tokenstreamptr tkns) {
  return list_get(tkns->tokens, tkns->index + 1);
}

tokenptr prev_tkn(tokenstreamptr tkns) {
  return list_get(tkns->tokens, --tkns->index);
}
