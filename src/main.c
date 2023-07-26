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

#include "expressions/expression.h"
#include "parser/parser.h"
#include "scanner/scanner.h"
#include "interpreter/interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define INITIAL_BUFFER_LENGTH 10000
#define ASCII_MAX_VALUE 127

static char int_to_char_table[ASCII_MAX_VALUE + 1];
static inline void initialize_int_to_char_table(void) {
  char c = 0;
  for (int i = 0; i < sizeof(int_to_char_table); i++) {
    int_to_char_table[i] = c++;
  }
}
static inline char int_to_char(int c) {
  if (0 <= c && c <= ASCII_MAX_VALUE) {
    return int_to_char_table[c];
  }
  return '\0';
}

typedef struct {
  bool verbose;
  char *filename;
} program_arguments;

void print_usage_and_exit(char *program_name) {
  printf("Lisp Interpreter\n");
  printf("Usage: %s [options] filename\n", program_name);
  printf("Options:\n");
  printf("-v verbose output\n");
  exit(0);
}

void print_error_and_exit(int exit_code, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(exit_code);
}

void parse_arg(char *arg, program_arguments *args, char *program_name) {
  if (arg[0] == '-') {
    if (strcmp(&arg[1], "v") == 0) {
      args->verbose = true;
    } else if (strcmp(&arg[1], "-help") == 0) {
      print_usage_and_exit(program_name);
    } else {
      print_error_and_exit(1, "Unknown option: %s\n", arg);
    }
  } else if (args->filename == NULL) {
    args->filename = arg;
  } else {
    print_error_and_exit(1, "Multiple file names are given.\n");
  }
}

void parse_args(int argc, char **argv, program_arguments *args) {
  char *program_name = *argv;

  args->filename = NULL;
  args->verbose = false;

  if (argc <= 1) {
    print_usage_and_exit(program_name);
  } else {
    for (int i = 1; i < argc; i++) {
      char *arg = *(++argv);
      parse_arg(arg, args, program_name);
    }
  }

  if (args->filename == NULL) {
    print_error_and_exit(1, "File name is not given.\n");
  }
}

char *read_file(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    print_error_and_exit(2, "Cannot read file %s\n", filename);
  }

  size_t capacity = INITIAL_BUFFER_LENGTH;
  char *code = malloc(capacity);

  int c = 0;
  size_t length = 0;
  while((c = fgetc(file)) != EOF) {
    code[length++] = int_to_char(c);

    if (length == capacity) {
      capacity *= 2;
      code = realloc(code, capacity);
    }
  }

  code[length] = '\0';

  fclose(file);
  return code;
}

int main(int argc, char **argv) {
  initialize_int_to_char_table();

  program_arguments args;
  parse_args(argc, argv, &args);

  char *code = read_file(args.filename);
  list *tokens = scanner(code);
  list *parse_tree = parser(tokens);
  if (parse_tree) {
    interpreter(parse_tree, args.verbose);
  }

  free(code);
  delete_token_list(tokens);
  delete_parse_tree(parse_tree);
  return 0;
}
