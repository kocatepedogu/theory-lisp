#ifndef BUILTIN_ERROR_H
#define BUILTIN_ERROR_H

#include "../types/object.h"
#include "../interpreter/stack_frame.h"

object_t builtin_error(size_t n, object_t *args, stack_frame_ptr sf);

object_t builtin_exit(size_t n, object_t *args, stack_frame_ptr sf);

#endif
