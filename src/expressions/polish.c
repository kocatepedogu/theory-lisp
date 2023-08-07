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

#include "polish.h"

#include <string.h>
#include <assert.h>

#include "../interpreter/stack_frame.h"
#include "../interpreter/variable.h"
#include "../scanner/scanner.h"
#include "../parser/parser.h"
#include "../utils/string.h"
#include "../utils/stack.h"
#include "../types/error.h"
#include "../types/pair.h"
#include "../types/void.h"
#include "expression.h"
#include "expression_base.h"
#include "lambda.h"
#include "common.h"

typedef struct {
  listptr body;
  listptr captured;
  bool evaluated;
} pn_expr;

static const char pn_expr_name[] = "pn_expr";

static const expr_vtable pn_expr_vtable = {
  .destroy = destroy_pn_expr,
  .to_string = pn_expr_tostring,
  .interpret = interpret_pn_expr,
  .call = pn_expr_call
};

inline bool is_pn_expr(exprptr e) {
  return strcmp(e->expr_name, pn_expr_name) == 0;
}

exprptr new_pn_expr(void) {
  pn_expr *pe = malloc(sizeof *pe);
  pe->body = new_list();
  pe->captured = new_list();
  pe->evaluated = false;

  return expr_base_new(pe, &pn_expr_vtable, pn_expr_name, 0, 0);
}

void destroy_pn_expr(exprptr self) {
  pn_expr *pe = self->data;

  for (size_t i = 0; i < list_size(pe->body); ++i) {
    exprptr arg = list_get(pe->body, i);
    delete_expr(arg);
  }

  for (size_t i = 0; i < list_size(pe->captured); ++i) {
    char *name = list_get(pe->captured, i);
    free(name);
  }

  delete_list(pe->body);
  delete_list(pe->captured);
  free(pe);
}

/**
 * Equivalent string representation
 */
char *pn_expr_tostring(exprptr self) {
  pn_expr *pe = self->data;

  char *body_str = NULL;
  for (size_t i = 0; i < list_size(pe->body); ++i) {
    exprptr arg = list_get(pe->body, i);
    char *str = expr_tostring(arg);
    if (body_str == NULL) {
      body_str = str;
    } else {
      char *new_body_str = format("%s %s", body_str, str);
      free(body_str);
      free(str);
      body_str = new_body_str;
    }
  }

  char *captures_str = NULL;
  for (size_t i = 0; i < list_size(pe->captured); ++i) {
    char *capture  = list_get(pe->captured, i);
    if (captures_str == NULL) {
      captures_str = strdup(capture);
    } else {
      char *new_captures_str = format("%s %s", captures_str, capture);
      free(captures_str);
      captures_str = new_captures_str;
    }
  }

  assert(body_str != NULL);

  char *result = NULL;
  if (captures_str) {
    result = format("{[%s] %s}", captures_str, body_str);
  } else {
    result = format("{%s}", body_str);
  }

  free(captures_str);
  free(body_str);
  return result;
}

/**
 * Adds an expression to the PN expression body 
 */

void pn_expr_add_body_expr(exprptr self, exprptr body_expr) {
  pn_expr *pe = self->data;
  list_add(pe->body, body_expr);
}

/**
 * Adds a captured variable name
 */
void pn_expr_add_captured_var(exprptr self, char *name) {
  pn_expr *pe = self->data;
  list_add(pe->captured, strdup(name));
}

/**
 * Parses PN expression
 * A PN expression can be one of the following
 *  1. a single value,
 *      Example: {1}
 *  2. a list of function evaluations that yields a single value,
 *      Example {+ $1 - $3 $2}
 *  3. or one of these with a captured variable list.
 *      Examples: {[x y] 1}, {[x y] + x y}
 */
exprptr pn_expr_parse(tokenstreamptr tkns) {
  tokenptr first_token = current_tkn(tkns);
  if (first_token->type == TOKEN_RIGHT_CURLY_BRACKET) {
    return parser_error(first_token, "Polish notation expression cannot be empty.");
  }

  listptr captures = get_capture_list(tkns);
  if (!captures) {
    return NULL;
  }

  exprptr expr = new_pn_expr();

  for (size_t i = 0; i < list_size(captures); ++i) {
    pn_expr_add_captured_var(expr, list_get(captures, i));
  }
  delete_list(captures);

  while (current_tkn(tkns)->type != TOKEN_RIGHT_CURLY_BRACKET) {
    exprptr body_expr = expr_parse(tkns);
    if (!body_expr) {
      delete_expr(expr);
      return NULL;
    }

    pn_expr_add_body_expr(expr, body_expr);
  }

  tokenptr right_curly_bracket = next_tkn(tkns);
  assert(right_curly_bracket->type == TOKEN_RIGHT_CURLY_BRACKET);

  return expr;
}

/** 
 * Saves variadic arguments in local variables named $1, $2, $3, ...
 */
static void obtain_arguments(stack_frame_ptr local_frame,
                             size_t nargs, objectptr *args) {
  for (size_t i = 0; i < nargs; ++i) {
    char *name = format("$%ld", i + 1);
    objectptr value = args[i];
    stack_frame_set_local_variable(local_frame, name, value);
    free(name);
  }
}

/** 
 * Interprets each expression in the PN expr body within 
 * the new local stack frame. The first error in the expressions
 * is returned. Returns void object if no error occurs.
 */
static objectptr interpret_body_expressions(listptr body,
                                           stack_frame_ptr local_frame, 
                                           stackptr *waiting) {
  *waiting = new_stack();

  for (size_t i = 0; i < list_size(body); ++i) {
    exprptr body_expr = list_get(body, i);
    objectptr evaluated_arg = interpret_expr(body_expr, local_frame);

    if (is_error(evaluated_arg)) {
      for (size_t j = i; j != 0; --j) {
        objectptr obj = stack_get(*waiting, j - 1);
        delete_object(obj);
        free(obj);
      }
      
      delete_stack(*waiting);
      return evaluated_arg;
    }

    stack_push(*waiting, evaluated_arg);
  }

  return make_void();
}

/**
 * Interprets entire body.
 */
static objectptr interpret_body(stackptr waiting, stack_frame_ptr sf,
                               stackptr *computed) {
  *computed = new_stack();

  while (!stack_is_empty(waiting)) {
    objectptr body_obj = stack_pop(waiting);
    if (is_procedure(body_obj)) {
      size_t pn_arity = procedure_get_pn_arity(body_obj);
      objectptr *arguments = malloc(pn_arity * sizeof(objectptr));
      /* 
       * Obtain arguments that are not available in the "computed" stack from $ variables 
       * so that simplified syntax such as {!= "a"} can be used in place of {!= $1 "a"}.
       * This use is valid only for the last procedure in the body.
       * */
      size_t i = 0;
      for (; i + stack_size(*computed) < pn_arity; ++i) {
        char *name = format("$%d", i + 1);
        arguments[i] = stack_frame_get_variable(sf, name);
        free(name);
      }

      /* Obtain lambda arguments from "computed" stack */
      for (; i < pn_arity; ++i) {
        arguments[i] = stack_pop(*computed);
      }

      /* Call the lambda with its closure and arguments */
      objectptr new_result_object = object_op_call(body_obj, pn_arity, arguments, sf);

      /* Push the resulting value to the "computed" stack */
      bool err = is_error(new_result_object); 
      if (!err) {
        stack_push(*computed, new_result_object);
      }

      /* Deallocate arguments */
      for (size_t i = 0; i < pn_arity; ++i) {
        delete_object(arguments[i]);
      }
      free(arguments);

      /* Deallocate procedure itself */
      delete_object(body_obj);

      /* If there was an error, return it immediately */
      if (err) {
        return new_result_object;
      }
      
    } else {
      /* If the expression is not a function, move it to the "computed" stack */
      stack_push(*computed, body_obj);
    }
  }

  objectptr result = stack_pop(*computed);

  /* Do not allow multiple results. */
  if (!is_error(result) && !stack_is_empty(*computed)) {
    delete_object(result);
    return make_error("Polish notation expression yields multiple values.");
  }

  return result;
}

/**
 * Converts the PN expression to a procedure object.
 */
objectptr interpret_pn_expr(exprptr self, stack_frame_ptr sf) {
  pn_expr *pe = self->data;
  return make_procedure(self, pe->captured, sf);
}

/**
 * Calls PN expression
 */

objectptr pn_expr_call(exprptr self, size_t nargs,
                      objectptr *args, stack_frame_ptr local_frame) {
  pn_expr *pe = self->data;

  /* Define local variables in local stack frame using argument values*/
  obtain_arguments(local_frame, nargs, args);

  /* Interpret body expressions inside new stack frame */
  stackptr waiting = NULL;
  objectptr error = interpret_body_expressions(pe->body, local_frame, &waiting);
  if (is_error(error)) {
    return error;
  }
  delete_object(error);
  
  /* Interpret entire body to obtain a single value */
  stackptr computed = NULL;
  objectptr result = interpret_body(waiting, local_frame, &computed);

  /* If an error occurs, it is possible that things remain on stacks. Deallocate them. */
  while(!stack_is_empty(waiting)) {
    objectptr ptr = stack_pop(waiting);
    delete_object(ptr);
    free(ptr);
  }
  while(!stack_is_empty(computed)) {
    objectptr ptr = stack_pop(computed);
    delete_object(ptr);
    free(ptr);
  }

  /* Clean */
  delete_stack(computed);
  delete_stack(waiting);

  return result;
}
