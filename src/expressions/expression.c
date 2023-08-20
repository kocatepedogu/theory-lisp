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

#include "expression.h"
#include "expression_base.h"

#include <stdio.h>
#include <assert.h>


#include "../scanner/scanner.h"
#include "../utils/list.h"
#include "data.h"
#include "definition.h"
#include "set.h"
#include "evaluation.h"
#include "expanded.h"
#include "identifier.h"
#include "if.h"
#include "lambda.h"
#include "let.h"
#include "polish.h"
#include "automaton.h"
#include "../parser/parser.h"

#include "../types/null.h"
#include "../types/boolean.h"
#include "../types/integer.h"
#include "../types/real.h"
#include "../types/string.h"
#include "../types/error.h"

/* Parser error messages */
#define ERR_UNEXPECTED_EOF "Unexpected end of file"
#define ERR_INVALID_TOKEN "Invalid token"
#define ERR_UNMATCHED_PARENTHESIS "Unmatched parenthesis"

/* base constructor */
exprptr expr_base_new(void *data, const expr_vtable *vtable, const char *expr_name,
                 tokenptr tkn) {
  exprptr e = malloc(sizeof *e);
  e->data = data;
  e->vtable = vtable;
  e->expr_name = expr_name;
  e->line_number = tkn ? tkn->line : 0;
  e->column_number = tkn ? tkn->column : 0;
  e->ref_count = 1;
  return e;
}

/* base clone */
exprptr expr_base_clone(exprptr other, void *new_data) {
  exprptr e = malloc(sizeof *e);
  *e = *other;
  e->data = new_data;
  return e;
}

/* destroys and deallocates an expression */
void delete_expr(exprptr self) {
  if (self != NULL) {
    if (self->vtable->deallocate) {
      self->vtable->deallocate(self); 
    } else {
      assert(self->vtable->destroy);
      if (--self->ref_count == 0) {
        self->vtable->destroy(self);
        free(self);
      }
    }
  }
}

/* clones an expression */
exprptr clone_expr(exprptr self) {
  if (self != NULL) {
    if (self->vtable->clone) {
      return self->vtable->clone(self);
    } else {
      ++self->ref_count;
      return self;
    }
  }
  return NULL;
}

/* Returns string representation of an expression */
char *expr_tostring(exprptr self) { return self->vtable->to_string(self); }

/* Parses an expression that starts with a left parenthesis */
static exprptr parenthesized_expr_parse(tokenstreamptr tkns) {
  tokenptr tkn = current_tkn(tkns);

  if (tkn->type == TOKEN_END_OF_FILE) {
    return parser_error(tkn, ERR_UNEXPECTED_EOF);
  }

  if (tkn->type == TOKEN_RIGHT_PARENTHESIS) {
    (void)next_tkn(tkns);
    return new_data_expr(make_null(), tkn);
  }

  exprptr subexpr = NULL;
  switch (tkn->type) {
    case TOKEN_LET:
      subexpr = let_expr_parse(tkns);
      break;
    case TOKEN_LAMBDA:
      subexpr = lambda_expr_parse(tkns);
      break;
    case TOKEN_IF:
      subexpr = if_expr_parse(tkns);
      break;
    case TOKEN_DEFINE:
      subexpr = definition_expr_parse(tkns);
      break;
    case TOKEN_SET:
      subexpr = set_expr_parse(tkns);
      break;
    case TOKEN_COND:
      subexpr = cond_expr_parse(tkns);
      break;
    case TOKEN_AUTOMATON:
      subexpr = automaton_expr_parse(tkns);
      break; 
    case TOKEN_LEFT_PARENTHESIS:
    case TOKEN_LEFT_CURLY_BRACKET:
    case TOKEN_IDENTIFIER:
    case TOKEN_STRING:
    case TOKEN_INTEGER:
    case TOKEN_REAL:
      subexpr = evaluation_expr_parse(tkns);
      break;
    default:
      return parser_error(tkn, ERR_INVALID_TOKEN);
  }

  if (subexpr) {
    tokenptr right_p = next_tkn(tkns);
    if (right_p->type != TOKEN_RIGHT_PARENTHESIS) {
      delete_expr(subexpr);
      return parser_error(tkn, ERR_UNMATCHED_PARENTHESIS);
    }
  }

  return subexpr;
}

/* Parses an arbitrary expression */
exprptr expr_parse(tokenstreamptr tkns) {
  tokenptr tkn = next_tkn(tkns);

  if (tkn->type == TOKEN_END_OF_FILE) {
    return parser_error(tkn, ERR_UNEXPECTED_EOF);
  }
  if (tkn->type == TOKEN_RIGHT_PARENTHESIS) {
    return parser_error(tkn, ERR_UNMATCHED_PARENTHESIS);
  }
  if (tkn->type == TOKEN_LEFT_PARENTHESIS) {
    return parenthesized_expr_parse(tkns);
  }
  if (tkn->type == TOKEN_LEFT_CURLY_BRACKET) {
    return pn_expr_parse(tkns);
  }
  if (tkn->type == TOKEN_AMPERSAND) {
    exprptr inner = expr_parse(tkns);
    if (!inner) {
      return NULL;
    }

    return new_expanded_expr(inner, tkn);
  }

  objectptr obj = make_error("uninitialized object");
  exprptr result = NULL;
  switch (tkn->type) {
    case TOKEN_NULL:
      assign_object(&obj, make_null());
      result = new_data_expr(obj, tkn);
      delete_object(obj);
      break;
    case TOKEN_BOOLEAN:
      assign_object(&obj, make_boolean(tkn->value.boolean));
      result = new_data_expr(obj, tkn);
      delete_object(obj);
      break;
    case TOKEN_INTEGER:
      assign_object(&obj, make_integer(tkn->value.integer));
      result = new_data_expr(obj, tkn);
      delete_object(obj);
      break;
    case TOKEN_REAL:
      assign_object(&obj, make_real(tkn->value.real));
      result = new_data_expr(obj, tkn);
      delete_object(obj);
      break;
    case TOKEN_IDENTIFIER:
      delete_object(obj);
      result = new_identifier_expr(tkn->value.character_sequence, tkn);
      break;
    case TOKEN_STRING:
      assign_object(&obj, make_string(tkn->value.character_sequence));
      result = new_data_expr(obj, tkn);
      delete_object(obj);
      break;
    default:
      delete_object(obj);
      result = parser_error(tkn, ERR_INVALID_TOKEN);
      break;
  }

  if (result != NULL) {
    result->line_number = tkn->line;
    result->column_number = tkn->column;
  }

  return result;
}

/* interpretes an arbitrary expression */
objectptr interpret_expr(exprptr self, stack_frame_ptr sf) {
  return self->vtable->interpret(self, sf);
}

/* calls an expression with given closure, arguments and stack frame */
objectptr expr_call(exprptr self, size_t nargs,
                   objectptr *args, stack_frame_ptr sf) {
  return self->vtable->call(self, nargs, args, sf);
}

objectptr expr_call_internal(exprptr self, void *args, stack_frame_ptr sf) {
  return self->vtable->call_internal(self, args, sf);
}
