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

#include "cond.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/boolean.h"
#include "../types/error.h"
#include "../types/void.h"
#include "../utils/string.h"
#include "../utils/list.h"
#include "expression.h"
#include "expression_base.h"

#define ERR_NO_LEFT_PARENTHESIS \
  "In cond expression, there is no left parenthesis at the beginning of case"

#define ERR_NO_RIGHT_PARENTHESIS \
  "In cond expression, there is no right parenthesis at the end of case"

static const expr_vtable cond_expr_vtable = {.destroy = destroy_cond_expr,
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

bool is_cond_expr(exprptr e) {
  if (e == NULL) {
    return false;
  }

  return strcmp(e->expr_name, cond_expr_name) == 0;
}

exprptr new_cond_expr(tokenptr tkn) {
  cond_expr *ce = malloc(sizeof *ce);
  ce->cases = new_list();

  return expr_base_new(ce, &cond_expr_vtable, cond_expr_name, tkn);
}

void destroy_cond_expr(exprptr self) {
  cond_expr *ce = self->data;
  for (size_t i = 0; i < list_size(ce->cases); ++i) {
    cond_case *c = (cond_case *)list_get(ce->cases, i);
    delete_expr(c->condition);
    delete_expr(c->true_case);
    free(c);
  }
  delete_list(ce->cases);
  free(ce);
}

void cond_expr_add_case(exprptr self, exprptr cond, exprptr true_case) {
  cond_expr *ce = self->data;
  cond_case *new_case = malloc(sizeof *new_case);
  new_case->condition = cond;
  new_case->true_case = true_case;
  list_add(ce->cases, new_case);
}

char *cond_expr_tostring(exprptr self) {
  cond_expr *ce = self->data;
  char *cases = NULL;
  for (size_t i = 0; i < list_size(ce->cases); ++i) {
    cond_case *c = list_get(ce->cases, i);
    char *cond = expr_tostring(c->condition);
    char *true_case = expr_tostring(c->true_case);
    char *newcase = unique_format("(%s %s)", cond, true_case);
    cases = unique_append_sep(cases, " ", newcase);
  }

  return unique_format("(cond %s)", cases);
}

exprptr cond_expr_parse(tokenstreamptr tkns, stack_frame_ptr sf) {
  tokenptr cond_token = next_tkn(tkns);
  assert(cond_token->type == TOKEN_COND);

  exprptr cond_expr = new_cond_expr(cond_token);
  while (current_tkn(tkns)->type != TOKEN_RIGHT_PARENTHESIS) {
    tokenptr left_p = next_tkn(tkns);
    if (left_p->type != TOKEN_LEFT_PARENTHESIS) {
      delete_expr(cond_expr);
      return parser_error(left_p, ERR_NO_LEFT_PARENTHESIS);
    }
    exprptr cond = expr_parse(tkns, sf);
    if (cond == NULL) {
      delete_expr(cond_expr);
      return NULL;
    }
    exprptr true_case = expr_parse(tkns, sf);
    if (true_case == NULL) {
      delete_expr(cond_expr);
      delete_expr(cond);
      return NULL;
    }
    tokenptr right_p = next_tkn(tkns);
    if (right_p->type != TOKEN_RIGHT_PARENTHESIS) {
      delete_expr(cond_expr);
      delete_expr(cond);
      delete_expr(true_case);
      return parser_error(right_p, ERR_NO_RIGHT_PARENTHESIS);
    }

    cond_expr_add_case(cond_expr, cond, true_case);
  }

  return cond_expr;
}

objectptr interpret_cond(exprptr self, stack_frame_ptr sf) {
  cond_expr *ce = self->data;
  for (size_t i = 0; i < list_size(ce->cases); ++i) {
    cond_case *cc = list_get(ce->cases, i);

    objectptr condition_result = interpret_expr(cc->condition, sf);
    if (is_error(condition_result)) {
      return condition_result;
    }

    if (!is_boolean(condition_result)) {
      delete_object(condition_result);
      return make_error(
          "Condition in cond expression does not yield a boolean.");
    }

    if (boolean_value(condition_result)) {
      delete_object(condition_result);
      return interpret_expr(cc->true_case, sf);
    }

    delete_object(condition_result);
  }

  return make_void();
}
