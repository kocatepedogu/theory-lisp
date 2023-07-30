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

#include "cond_expr.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/error.h"
#include "../types/void.h"
#include "../types/boolean.h"
#include "../utils/heap-format.h"
#include "../utils/list.h"
#include "expression.h"

#define ERR_NO_LEFT_PARENTHESIS \
  "In cond expression, there is no left parenthesis at the beginning of case"

#define ERR_NO_RIGHT_PARENTHESIS \
  "In cond expression, there is no right parenthesis at the end of case"



static const expr_vtable cond_expr_vtable = {.deallocate = delete_cond_expr,
					                                   .clone = clone_cond_expr,
                                             .to_string = cond_expr_tostring,
                                             .interpret = interpret_cond};

/* ((cond) expr-if-cond) */
typedef struct {
  exprptr condition;
  exprptr true_case;
} cond_case;

/* (cond ((cond1) expr-if-cond1) ((cond2) expr-if-cond2) ... ) */
typedef struct {
  listptr cases; /* list of cond_case*'s */
} cond_expr; 

static const char cond_expr_name[] = "cond_expr";

exprptr new_cond_expr(void) {
  cond_expr *ce = (cond_expr *)malloc(sizeof(cond_expr));
  ce->cases = new_list();

  exprptr e = (exprptr)malloc(sizeof(expr_t));
  e->data = ce;
  e->vtable = &cond_expr_vtable;
  e->expr_name = cond_expr_name;
  return e;
}

void delete_cond_expr(exprptr self) {
  cond_expr *ce = self->data;
  for (size_t i = 0; i < list_size(ce->cases); i++) {
    cond_case *c = (cond_case *)list_get(ce->cases, i);
    delete_expr(c->condition);
    delete_expr(c->true_case);
    free(c);
  }
  delete_list(ce->cases);
  free(ce);
  free(self);
}

exprptr clone_cond_expr(exprptr self) {
  cond_expr *new_ce = (cond_expr *)malloc(sizeof(cond_expr));
  new_ce->cases = new_list();
  
  cond_expr *self_ce = self->data;
  for (size_t i = 0; i < list_size(self_ce->cases); i++) {
    cond_case *self_case = list_get(self_ce->cases, i);
    cond_case *new_case = (cond_case *)malloc(sizeof(cond_case));
    new_case->condition = clone_expr(self_case->condition);
    new_case->true_case = clone_expr(self_case->true_case);
    list_add(new_ce->cases, new_case);
  }

  return base_clone(self, new_ce);
}

void cond_expr_add_case(exprptr self, exprptr cond, exprptr true_case) {
  cond_expr *ce = self->data;
  cond_case *new_case = malloc(sizeof(cond_case));
  new_case->condition = cond;
  new_case->true_case = true_case;
  list_add(ce->cases, new_case);
}

char *cond_expr_tostring(exprptr self) {
  cond_expr *ce = self->data;
  char *concatenated_cases = NULL;
  for (size_t i = 0; i < list_size(ce->cases); i++) {
    /* Compute the string representation of the current case */
    cond_case *c = (cond_case *)list_get(ce->cases, i);
    char *cond_str = expr_tostring(c->condition);
    char *true_case_str = expr_tostring(c->true_case);
    char *case_str = heap_format("(%s %s) ", cond_str, true_case_str);
    free(cond_str);
    free(true_case_str);
    /* Concatenate the result with the previously computed string */
    if (!concatenated_cases) {
      concatenated_cases = case_str;
    } else {
      char *new_concatenated_cases = heap_concat(concatenated_cases, case_str);
      free(concatenated_cases);
      free(case_str);
      concatenated_cases = new_concatenated_cases;
    }
  }

  char *result = heap_format("(cond %s)", concatenated_cases);
  free(concatenated_cases);
  return result;
}

exprptr cond_expr_parse(listptr tokens, int *index) {
  token_t *cond_token = list_get(tokens, (*index)++);
  assert(cond_token->type == TOKEN_COND);

  exprptr cond_expr = new_cond_expr();
  while (((token_t *)list_get(tokens, *index))->type !=
         TOKEN_RIGHT_PARENTHESIS) {
    token_t *left_p = list_get(tokens, (*index)++);
    if (left_p->type != TOKEN_LEFT_PARENTHESIS) {
      delete_expr(cond_expr);
      return parser_error(left_p->line, left_p->column, ERR_NO_LEFT_PARENTHESIS);
    }
    exprptr cond = expr_parse(tokens, index);
    if (cond == NULL) {
      delete_expr(cond_expr);
      return NULL;
    }
    exprptr true_case = expr_parse(tokens, index);
    if (true_case == NULL) {
      delete_expr(cond_expr);
      delete_expr(cond);
      return NULL;
    }
    token_t *right_p = list_get(tokens, (*index)++);
    if (right_p->type != TOKEN_RIGHT_PARENTHESIS) {
      delete_expr(cond_expr);
      delete_expr(cond);
      delete_expr(true_case);
      return parser_error(right_p->line, right_p->column, ERR_NO_RIGHT_PARENTHESIS);
    }

    cond_expr_add_case(cond_expr, cond, true_case);
  }

  cond_expr->line_number = cond_token->line;
  cond_expr->column_number = cond_token->column;
  return cond_expr;
}

bool is_cond_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, cond_expr_name) == 0;
}

object_t interpret_cond(exprptr self, stack_frame_ptr sf) {
  cond_expr *ce = self->data;
  for (size_t i = 0; i < list_size(ce->cases); i++) {
    cond_case *cc = list_get(ce->cases, i);

    object_t condition_result = interpret_expr(cc->condition, sf);
    if (is_error(condition_result)) {
      return condition_result;
    }

    if (!is_boolean(condition_result)) {
      destroy_object(condition_result);
      return make_error("Condition in cond expression does not yield a boolean.");
    }

    if (boolean_value(condition_result)) {
      destroy_object(condition_result);
      return interpret_expr(cc->true_case, sf);
    }

    destroy_object(condition_result);
  }

  return make_void();
}
