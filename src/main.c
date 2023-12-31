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

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>

#include "builtin/builtin.h"
#include "expressions/expression.h"
#include "interpreter/interpreter.h"
#include "interpreter/stack_frame.h"
#include "parser/parser.h"
#include "scanner/scanner.h"
#include "types/error.h"
#include "types/void.h"
#include "utils/file.h"
#include "utils/init.h"

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");
  srand (time (0));

  program_arguments args;
  parse_args(argc, argv, &args);

  stack_frame_ptr global_frame = new_stack_frame(NULL);
  define_builtin_function_wrappers(global_frame);

  objectptr result = make_void();
  if (args.filename) {
    char *code = read_file(args.filename);
    if (!code) {
      delete_stack_frame(global_frame);
      delete_object(result);
      print_error_and_exit(2, "Cannot read file %s\n", args.filename);
    }

    tokenstreamptr tokens = scanner(code);
    free(code);
    if (!tokens) {
      delete_stack_frame(global_frame);
      delete_object(result);
      print_error_and_exit(3, "A scanner error has occured.\n");
    }

    listptr parse_tree = parser(tokens, global_frame);
    delete_tokenstream(tokens);
    if (!parse_tree) {
      delete_stack_frame(global_frame);
      delete_object(result);
      print_error_and_exit(4, "A parser error has occured.\n");
    }

    assign_object(&result, interpreter(parse_tree, args.verbose, args.quiet,
                                       global_frame));
    delete_parse_tree(parse_tree);
  }

  if (!is_error(result) && !args.exit) {
    repl(global_frame);
  }

  delete_object(result);
  delete_stack_frame(global_frame);

  return EXIT_SUCCESS;
}
