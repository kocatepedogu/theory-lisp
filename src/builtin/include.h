#ifndef BUILTIN_INCLUDE_H
#define BUILTIN_INCLUDE_H

#include "../types/object.h"
#include "../interpreter/stack_frame.h"

object_t builtin_include(size_t n, object_t *args, stack_frame_ptr sf);

#endif

