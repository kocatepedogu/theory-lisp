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

#include "builtin.h"

#include <string.h>

#include "../expressions/lambda.h"
#include "../expressions/evaluation.h"
#include "../expressions/identifier.h"
#include "../expressions/expanded.h"
#include "../expressions/expression.h"
#include "../utils/string.h"
#include "eval.h"
#include "io.h"
#include "object.h"

#define MAX_PN_ARITY 2048

const builtin_function builtin_functions[] = {
    /* IO functions */
    {"include", builtin_include, 1},
    {"display", builtin_display, 1, 1, true},
    {"getchar", builtin_getchar, 0},
    {"putchar", builtin_putchar, 0, 1, true},
    {"system", builtin_system, 1},
    {"current-seconds", builtin_current_seconds, 0},

    /* Comparison operators */
    {"=", builtin_equals, 1, 2, true},
    {"!=", builtin_not_equals, 1, 2, true},
    {"<", builtin_less, 2},
    {"<=", builtin_less_or_eq, 2},
    {">", builtin_greater, 2},
    {">=", builtin_greater_or_eq, 2},

    /* Type predicates */
    {"null?", builtin_is_null, 1},
    {"void?", builtin_is_void, 1},
    {"boolean?", builtin_is_boolean, 1},
    {"integer?", builtin_is_integer, 1},
    {"real?", builtin_is_real, 1},
    {"rational?", builtin_is_rational, 1},
    {"number?", builtin_is_number, 1},
    {"string?", builtin_is_string, 1},
    {"pair?", builtin_is_pair, 1},
    {"procedure?", builtin_is_procedure, 1},

    /* Arithmetic operators */
    {"+", builtin_add, 0, 2, true},
    {"*", builtin_mul, 0, 2, true},
    {"-", builtin_sub, 1, 2, true},
    {"/", builtin_div, 1, 2, true},

    /* Boolean operators */
    {"and", builtin_and, 0, 2, true},
    {"or", builtin_or, 0, 2, true},
    {"xor", builtin_xor, 0, 2, true},
    {"not", builtin_not, 1},

    /* List functions */
    {"cons", builtin_cons, 2},
    {"car", builtin_car, 1},
    {"cdr", builtin_cdr, 1},
    {"list", builtin_list, 0, MAX_PN_ARITY, true},

    /* String functions */
    {"strlen", builtin_strlen, 1},
    {"strcat", builtin_strcat, 0, 2, true},
    {"charat", builtin_charat, 2},
    {"substr", builtin_substr, 3},
    {"strcar", builtin_strcar, 1},
    {"strcdr", builtin_strcdr, 1},
    {"newline", builtin_newline, 0},
    {"tab", builtin_tab, 0},
    {"backspace", builtin_backspace, 0},
    {"i2s", builtin_i2s, 0, MAX_PN_ARITY, true},
    {"s2i", builtin_s2i, 1},

    /* Reflection */
    {"eval", builtin_eval, 1},
    {"defined?", builtin_defined, 1},

    /* Exceptions */
    {"error", builtin_error, 1},
    {"exit", builtin_exit, 0},

    /* Math functions */
    {"cos", builtin_cos, 1},
    {"sin", builtin_sin, 1},
    {"tan", builtin_tan, 1},
    {"acos", builtin_acos, 1},
    {"asin", builtin_asin, 1},
    {"atan", builtin_atan, 1},
    {"atan2", builtin_atan2, 2},
    {"cosh", builtin_cosh, 1},
    {"sinh", builtin_sinh, 1},
    {"tanh", builtin_tanh, 1},
    {"acosh", builtin_acosh, 1},
    {"asinh", builtin_asinh, 1},
    {"atanh", builtin_atanh, 1},
    {"exp", builtin_exp, 1},
    {"log", builtin_log, 1},
    {"log10", builtin_log10, 1},
    {"pow", builtin_pow, 2},
    {"sqrt", builtin_sqrt, 1},
    {"cbrt", builtin_cbrt, 1},
    {"hypot", builtin_hypot, 2},
    {"erf", builtin_erf, 1},
    {"erfc", builtin_erfc, 1},
    {"tgamma", builtin_tgamma, 1},
    {"lgamma", builtin_lgamma, 1},
    {"ceil", builtin_ceil, 1},
    {"floor", builtin_floor, 1},
    {"trunc", builtin_trunc, 1},
    {"round", builtin_round, 1},
    {"modulo", builtin_modulo, 2},
    {"isfinite", builtin_isfinite, 1},
    {"isinf", builtin_isinf, 1},
    {"isnan", builtin_isnan, 1},
    {"isnormal", builtin_isnormal, 1},
    {"random", builtin_random, 0}};

const size_t number_of_builtin_functions =
    sizeof builtin_functions / sizeof *builtin_functions;

const builtin_function *find_builtin_function(const char *name) {
  for (size_t i = 0; i < number_of_builtin_functions; ++i) {
    if (strcmp(builtin_functions[i].name, name) == 0) {
      return &builtin_functions[i];
    }
  }
  return NULL;
}

bool is_builtin_name(const char *name) {
  return find_builtin_function(name) != NULL;
}

void define_builtin_function_wrappers(stack_frame_ptr sf) {
  for (size_t i = 0; i < number_of_builtin_functions; ++i) {
    const builtin_function *f = &builtin_functions[i];

    exprptr lambda_body = new_evaluation_expr(new_identifier_expr(f->name, NULL), NULL);
    if (f->variadic) {
      evaluation_expr_add_arg(lambda_body, 
          new_expanded_expr(new_identifier_expr("va_args", NULL), NULL));
    } else {
      for (size_t i = 0; i < f->arity; ++i) {
        char *id = format("arg%ld", i);
        evaluation_expr_add_arg(lambda_body, new_identifier_expr(id, NULL));
        free(id);
      }
    }

    exprptr lambda = new_lambda_expr(lambda_body, f->variadic, NULL);
    if (f->variadic) {
      lambda_expr_set_pn_arity(lambda, f->pn_arity);
    }
    else {
      lambda_expr_set_pn_arity(lambda, f->arity);
      for (size_t i = 0; i < f->arity; ++i) {
        char *id = format("arg%ld", i);
        lambda_expr_add_param(lambda, id);
        free(id);
      }
    }

    objectptr procedure = make_procedure(lambda, NULL, sf);
    stack_frame_set_global_variable(sf, f->name, procedure);
    delete_object(procedure);
    delete_expr(lambda);
  }
}
