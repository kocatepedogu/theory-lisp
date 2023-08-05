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

#include "init.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage_and_exit(char *program_name) {
  printf("Theory Lisp\n");
  printf("Usage: %s [options] filename\n", program_name);
  printf("Options:\n");
  printf("-v verbose output\n");
  printf("-q quiet output (do not print each expression result)\n");
  printf("-x exit after executing file (no read-evaluate-print loop)\n");
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
    if (strcmp(&arg[1], "-help") == 0) {
      print_usage_and_exit(program_name);
    }

    bool known_arg = false;
    if (strchr(&arg[1], 'v')) {
      args->verbose = true;
      known_arg = true;
    }
    if (strchr(&arg[1], 'q')) {
      args->quiet = true;
      known_arg = true;
    }
    if (strchr(&arg[1], 'x')) {
      args->exit = true;
      known_arg = true;
    }

    if (!known_arg) {
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
  args->quiet = false;
  args->exit = false;

  for (int i = 1; i < argc; i++) {
    char *arg = *(++argv);
    parse_arg(arg, args, program_name);
  }
}
