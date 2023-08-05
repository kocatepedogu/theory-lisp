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

#include "file.h"

#include <stdio.h>
#include <stdlib.h>

#include "init.h"

#define MAX_FILENAME_SIZE 1024
#define INITIAL_BUFFER_LENGTH 10000

char *read_file(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return NULL;
  }

  size_t capacity = INITIAL_BUFFER_LENGTH;
  char *code = malloc(capacity);

  int c = 0;
  size_t length = 0;
  while ((c = fgetc(file)) != EOF) {
    code[length++] = c;

    if (length == capacity) {
      capacity *= 2;
      code = realloc(code, capacity);
    }
  }

  code[length] = '\0';

  fclose(file);
  return code;
}
