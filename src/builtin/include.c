#include "include.h"

#include <assert.h>
#include <stdio.h>

#include "../types/string.h"
#include "../types/error.h"
#include "../types/void.h"
#include "../utils/file.h"
#include "../scanner/scanner.h"
#include "../parser/parser.h"
#include "../utils/init.h"
#include "../utils/heap-format.h"

/* 
 * This directory is normally given by autotools depending 
 * on the actual installation prefix
 */
#ifndef LIBRARY_DIR
#define LIBRARY_DIR "/usr/local/lib"
#endif

char *read_code(char *file_name) {
  /* Try to read the file at the given path */
  char *code = read_file(file_name);
  if (code) {
    return code;
  }

  /* If the file at the given path is not accessable, search
   * for the file in the Theory Lisp library directory */
  char *library_file_name = heap_format("%s/%s", LIBRARY_DIR, file_name);
  code = read_file(library_file_name);
  if (code) {
    free(library_file_name);
    return code;
  }

  /* If the file is not found, return NULL */
  free(library_file_name);
  return NULL;
}

object_t builtin_include(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);

  /* Obtain file name */
  object_t file_name_obj = *args;
  if (!is_string(file_name_obj)) {
    return make_error("File name must be a string"); 
  }

  char *file_name = string_value(file_name_obj);

  /* Check if the file is already included */
  char *include_guard = heap_format("%s_included", file_name);
  if (stack_frame_defined(sf, include_guard)) {
    free(include_guard);
    return make_void();
  }

  /* Read file */
  char *code = read_code(file_name);
  if (!code) {
    return make_error("%s is not accessible.", file_name);
  }

  /* Define an include guard variable in global scope to prevent
   * multiple inclusions of the same file */
  stack_frame_set_global_variable(sf, include_guard, make_void());
  free(include_guard);

  /* Parse included file */
  list *tokens = scanner(code);
  list *parse_tree = parser(tokens);
  if (!parse_tree) {
    delete_token_list(tokens);
    free(code);
    return make_error("An error occured in the included file");
  }

  /* Execute included file in the same global scope */
  object_t result = interpreter(parse_tree, false, true, sf);
  destroy_object(result);

  delete_token_list(tokens);
  delete_parse_tree(parse_tree);
  free(code);

  return make_void();
}
