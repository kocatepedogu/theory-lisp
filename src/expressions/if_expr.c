/*
 *
 * Copyright 2023 Doğu Kocatepe
 * This file is part of Theory Lisp.

 * Theory Lisp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Theory Lisp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along with Theory Lisp.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "if_expr.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/boolean.h"
#include "../types/error.h"
#include "../utils/heap-format.h"
#include "expression.h"

/* (if (cond) true-case false-case) */
typedef struct {
  exprptr condition;
  exprptr true_case;
  exprptr false_case;
} if_expr;

static const char if_expr_name[] = "if_expr";

static const expr_vtable if_expr_vtable = {.deallocate = delete_if_expr,
					                                 .clone = clone_if_expr,
                                           .to_string = if_expr_tostring,
                                           .interpret = interpret_if};

exprptr new_if_expr(exprptr condition, exprptr true_case, exprptr false_case) {
  if_expr *ie = malloc(sizeof(if_expr));
  ie->condition = condition;
  ie->true_case = true_case;
  ie->false_case = false_case;

  expr_t *e = (expr_t *)malloc(sizeof(expr_t));
  e->data = ie;
  e->vtable = &if_expr_vtable;
  e->expr_name = if_expr_name;
  return e;
}

void delete_if_expr(exprptr self) {
  if_expr *expr = self->data;
  delete_expr(expr->condition);
  delete_expr(expr->true_case);
  delete_expr(expr->false_case);
  free(expr);
  free(self);
}

exprptr clone_if_expr(exprptr self) {
  if_expr *self_ie = self->data;
  if_expr *new_ie = (if_expr *)malloc(sizeof(if_expr));
  new_ie->condition = clone_expr(self_ie->condition);
  new_ie->false_case = clone_expr(self_ie->false_case);
  new_ie->true_case = clone_expr(self_ie->true_case);

  return base_clone(self, new_ie);
}

char *if_expr_tostring(exprptr self) {
  if_expr *expr = self->data;
  static const char *if_expr_format = "(if %s\n\t%s\n\t%s)";
  char *cond_str = expr_tostring(expr->condition);
  char *true_case_str = expr_tostring(expr->true_case);
  char *false_case_str = expr_tostring(expr->false_case);
  char *result =
      heap_format(if_expr_format, cond_str, true_case_str, false_case_str);
  free(cond_str);
  free(true_case_str);
  free(false_case_str);
  return result;
}

exprptr if_expr_parse(listptr tokens, int *index) {
  token_t *if_token = list_get(tokens, (*index)++);
  assert(if_token->type == TOKEN_IF);

  exprptr condition = expr_parse(tokens, index);
  if (condition == NULL) {
    return NULL;
  }
  exprptr true_case = expr_parse(tokens, index);
  if (true_case == NULL) {
    delete_expr(condition);
    return NULL;
  }
  exprptr false_case = expr_parse(tokens, index);
  if (false_case == NULL) {
    delete_expr(condition);
    delete_expr(true_case);
    return NULL;
  }

  exprptr result = new_if_expr(condition, true_case, false_case);
  result->line_number = if_token->line;
  result->column_number = if_token->column;
  return result;
}

bool is_if_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, if_expr_name) == 0;
}

object_t interpret_if(exprptr self, stack_frame_ptr sf) {
  if_expr *ie = self->data;

  object_t condition_result = interpret_expr(ie->condition, sf);
  if (is_error(condition_result)) {
    return condition_result;
  }

  if (!is_boolean(condition_result)) {
    destroy_object(condition_result);
    return make_error("Condition in if expression does not yield a boolean.");
  }

  exprptr case_expr = NULL;
  if (boolean_value(condition_result)) {
    case_expr = ie->true_case;
  } else {
    case_expr = ie->false_case;
  }

  object_t result = interpret_expr(case_expr, sf);
  destroy_object(condition_result);
  return result;
}
