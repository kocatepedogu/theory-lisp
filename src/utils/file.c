#include "file.h"
#include "init.h"

#include <stdio.h>
#include <stdlib.h>

#define INITIAL_BUFFER_LENGTH 10000
#define ASCII_MAX_VALUE 127

static inline char int_to_char(int c) {
  if (0 <= c && c <= ASCII_MAX_VALUE) {
    return c;
  }
  return '\0';
}

char *read_file(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return NULL;
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
