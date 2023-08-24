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

#include "math.h"

#include <assert.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "../types/boolean.h"
#include "../types/integer.h"
#include "../types/real.h"
#include "../types/error.h"

objectptr builtin_cos(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(cos(cast_real(*args)));
  } else {
    return make_error("cos argument is not a number.");
  }
}

objectptr builtin_sin(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(sin(cast_real(*args)));
  } else {
    return make_error("sin argument is not a number.");
  }
}

objectptr builtin_tan(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(tan(cast_real(*args)));
  } else {
    return make_error("tan argument is not a number.");
  }
}

objectptr builtin_acos(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(acos(cast_real(*args)));
  } else {
    return make_error("acos argument is not a number.");
  }
}

objectptr builtin_asin(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(asin(cast_real(*args)));
  } else {
    return make_error("asin argument is not a number.");
  }
}

objectptr builtin_atan(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(atan(cast_real(*args)));
  } else {
    return make_error("atan argument is not a number.");
  }
}

objectptr builtin_atan2(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 2);
  if (is_number(args[0]) && is_number(args[1])) {
    return make_real(atan2(cast_real(args[0]), cast_real(args[1])));
  } else {
    return make_error("atan2 arguments are not numbers.");
  }
}

objectptr builtin_cosh(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(cosh(cast_real(*args)));
  } else {
    return make_error("cosh argument is not a number.");
  }
}

objectptr builtin_sinh(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(sinh(cast_real(*args)));
  } else {
    return make_error("sinh argument is not a number.");
  }
}

objectptr builtin_tanh(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(tanh(cast_real(*args)));
  } else {
    return make_error("tanh argument is not a number.");
  }
}

objectptr builtin_acosh(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(acosh(cast_real(*args)));
  } else {
    return make_error("acosh argument is not a number.");
  }
}

objectptr builtin_asinh(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(asinh(cast_real(*args)));
  } else {
    return make_error("asinh argument is not a number.");
  }
}

objectptr builtin_atanh(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(atanh(cast_real(*args)));
  } else {
    return make_error("atanh argument is not a number.");
  }
}

objectptr builtin_exp(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(exp(cast_real(*args)));
  } else {
    return make_error("exp argument is not a number.");
  }
}

objectptr builtin_log(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(log(cast_real(*args)));
  } else {
    return make_error("log argument is not a number.");
  }
}

objectptr builtin_log10(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(log10(cast_real(*args)));
  } else {
    return make_error("log10 argument is not a number.");
  }
}

objectptr builtin_pow(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 2);
  if (is_number(args[0]) && is_number(args[1])) {
    return make_real(pow(cast_real(args[0]), cast_real(args[1])));
  } else {
    return make_error("pow arguments are not numbers.");
  }
}

objectptr builtin_sqrt(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(sqrt(cast_real(*args)));
  } else {
    return make_error("sqrt argument is not a number.");
  }
}

objectptr builtin_cbrt(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(cbrt(cast_real(*args)));
  } else {
    return make_error("cbrt argument is not a number.");
  }
}

objectptr builtin_hypot(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 2);
  if (is_number(args[0]) && is_number(args[1])) {
    return make_real(hypot(cast_real(args[0]), cast_real(args[1])));
  } else {
    return make_error("hypot arguments are not numbers.");
  }
}

objectptr builtin_erf(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(erf(cast_real(*args)));
  } else {
    return make_error("erf argument is not a number.");
  }
}

objectptr builtin_erfc(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(erfc(cast_real(*args)));
  } else {
    return make_error("erfc argument is not a number.");
  }
}

objectptr builtin_tgamma(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(tgamma(cast_real(*args)));
  } else {
    return make_error("tgamma argument is not a number.");
  }
}

objectptr builtin_lgamma(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(lgamma(cast_real(*args)));
  } else {
    return make_error("lgamma argument is not a number.");
  }
}

objectptr builtin_ceil(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(ceil(cast_real(*args)));
  } else {
    return make_error("ceil argument is not a number.");
  }
}

objectptr builtin_floor(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(floor(cast_real(*args)));
  } else {
    return make_error("floor argument is not a number.");
  }
}

objectptr builtin_trunc(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(trunc(cast_real(*args)));
  } else {
    return make_error("trunc argument is not a number.");
  }
}

objectptr builtin_round(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_real(round(cast_real(*args)));
  } else {
    return make_error("round argument is not a number.");
  }
}

objectptr builtin_modulo(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 2);
  if (is_integer(args[0]) && is_integer(args[1])) {
    return make_integer(int_value(args[0]) % int_value(args[1]));
  } else {
    return make_error("modulo arguments are not integers.");
  }
}

objectptr builtin_isfinite(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_boolean(isfinite(cast_real(*args)));
  } else {
    return make_error("isfinite argument is not a number.");
  }
}

objectptr builtin_isinf(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_boolean(isinf(cast_real(*args)));
  } else {
    return make_error("isinf argument is not a number.");
  }
}

objectptr builtin_isnan(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_boolean(isnan(cast_real(*args)));
  } else {
    return make_error("isnan argument is not a number.");
  }
}

objectptr builtin_isnormal(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 1);
  if (is_number(*args)) {
    return make_boolean(isnormal(cast_real(*args)));
  } else {
    return make_error("isnormal argument is not a number.");
  }
}

objectptr builtin_random(size_t n, objectptr *args, stack_frame_ptr sf) {
  assert (n == 0);
  return make_real((double)rand()/(double)RAND_MAX);
}
