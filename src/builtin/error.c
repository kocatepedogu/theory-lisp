#include "error.h"

#include <assert.h>

#include "../types/error.h"
#include "../types/string.h"

object_t builtin_error(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 1);
  if (!is_string(*args)) {
    return make_error("error argument is not a string");
  }

  return make_error("%s", string_value(*args));
}

object_t builtin_exit(size_t n, object_t *args, stack_frame_ptr sf) {
  assert(n == 0);
  return make_exit();
}
