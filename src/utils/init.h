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

#ifndef THEORYLISP_UTILS_INIT_H
#define THEORYLISP_UTILS_INIT_H

#include <stdbool.h>

typedef struct {
  bool verbose;
  bool quiet;
  bool exit;
  char *filename;
} program_arguments;

void print_usage_and_exit(char *program_name);

void print_error_and_exit(int exit_code, char *fmt, ...);

void parse_arg(char *arg, program_arguments *args, char *program_name);

void parse_args(int argc, char **argv, program_arguments *args);

#endif
