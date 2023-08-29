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

#include "if.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/boolean.h"
#include "../types/error.h"
#include "../utils/string.h"
#include "expression.h"
#include "expression_base.h"

/* (if (cond) true-case false-case) */
typedef struct {
  exprptr condition;
  exprptr true_case;
  exprptr false_case;
} if_expr;

static const char if_expr_name[] = "if_expr";

static const expr_vtable if_expr_vtable = {.destroy = destroy_if_expr,
                                           .to_string = if_expr_tostring,
                                           .interpret = interpret_if};

bool is_if_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, if_expr_name) == 0;
}

exprptr new_if_expr(exprptr condition, exprptr true_case, exprptr false_case, tokenptr tkn) {
  if_expr *ie = malloc(sizeof *ie);
  ie->condition = condition;
  ie->true_case = true_case;
  ie->false_case = false_case;

  return expr_base_new(ie, &if_expr_vtable, if_expr_name, tkn);
}

void destroy_if_expr(exprptr self) {
  if_expr *expr = self->data;
  delete_expr(expr->condition);
  delete_expr(expr->true_case);
  delete_expr(expr->false_case);
  free(expr);
}

char *if_expr_tostring(exprptr self) {
  if_expr *expr = self->data;
  char *cond_str = expr_tostring(expr->condition);
  char *true_case_str = expr_tostring(expr->true_case);
  char *false_case_str = expr_tostring(expr->false_case);
  char *result = unique_format("(if %s %s %s)", cond_str, true_case_str, false_case_str);
  return result;
}

exprptr if_expr_parse(tokenstreamptr tkns, stack_frame_ptr sf) {
  tokenptr if_token = next_tkn(tkns);
  assert(if_token->type == TOKEN_IF);

  exprptr condition = expr_parse(tkns, sf);
  if (condition == NULL) {
    return NULL;
  }
  exprptr true_case = expr_parse(tkns, sf);
  if (true_case == NULL) {
    delete_expr(condition);
    return NULL;
  }
  exprptr false_case = expr_parse(tkns, sf);
  if (false_case == NULL) {
    delete_expr(condition);
    delete_expr(true_case);
    return NULL;
  }

  return new_if_expr(condition, true_case, false_case, if_token);
}

objectptr interpret_if(exprptr self, stack_frame_ptr sf) {
  if_expr *ie = self->data;

  objectptr condition_result = interpret_expr(ie->condition, sf);
  if (is_error(condition_result)) {
    return condition_result;
  }

  if (!is_boolean(condition_result)) {
    delete_object(condition_result);
    return make_error("Condition in if expression does not yield a boolean.");
  }

  exprptr case_expr = NULL;
  if (boolean_value(condition_result)) {
    case_expr = ie->true_case;
  } else {
    case_expr = ie->false_case;
  }

  objectptr result = interpret_expr(case_expr, sf);
  delete_object(condition_result);
  return result;
}
