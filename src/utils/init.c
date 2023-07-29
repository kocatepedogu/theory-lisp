#include "init.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

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

