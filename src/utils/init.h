#ifndef UTILS_INIT_H
#define UTILS_INIT_H

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
