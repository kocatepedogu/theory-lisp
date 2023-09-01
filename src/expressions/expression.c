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

#include <assert.h>
#include <stdio.h>

#include "../builtin/io.h"
#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/boolean.h"
#include "../types/error.h"
#include "../types/integer.h"
#include "../types/null.h"
#include "../types/real.h"
#include "../types/rational.h"
#include "../types/string.h"
#include "../types/void.h"
#include "../types/internal.h"
#include "../utils/list.h"
#include "automaton.h"
#include "data.h"
#include "definition.h"
#include "evaluation.h"
#include "expanded.h"
#include "expression_base.h"
#include "identifier.h"
#include "if.h"
#include "lambda.h"
#include "let.h"
#include "polish.h"
#include "set.h"
#include "try_catch.h"

/* Parser error messages */
#define ERR_UNEXPECTED_EOF "Unexpected end of file"
#define ERR_INVALID_TOKEN "Invalid token"
#define ERR_UNMATCHED_PARENTHESIS "Unmatched parenthesis"

/* base constructor */
exprptr expr_base_new(void *data, const expr_vtable *vtable,
                      const char *expr_name, tokenptr tkn) {
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

/* 
 * Includes another source file.
 * The inclusion of the file occurs during parsing, 
 * not at runtime. Macro definitions inside the included file become available.
 */
static exprptr include_file(tokenstreamptr tkns, stack_frame_ptr sf) {
  tokenptr include_tkn = next_tkn(tkns);
  assert(include_tkn->type == TOKEN_INCLUDE);

  /* Get file name */
  tokenptr filename_tkn = next_tkn(tkns);
  if (filename_tkn->type != TOKEN_STRING) {
    return parser_error(filename_tkn, "Included file name must be a string");
  }

  /* Load file using the builtin 'load' function. */
  objectptr *arg = malloc(sizeof(objectptr));
  *arg = make_string(filename_tkn->value.character_sequence);
  objectptr result = builtin_load(1, arg, sf);
  delete_object(*arg);
  free(arg);

  /* If an error occurs, return the error */
  if (is_error(result)) {
    char *error_message = error_tostring(result);
    parser_error(filename_tkn, error_message);
    free(error_message);
    delete_object(result);
    return NULL;
  }

  /* If the file is loaded successfully, return 'result' which is void. */
  exprptr void_expr = new_data_expr(result, filename_tkn);
  delete_object(result);
  return void_expr;
}

/* Defines a macro */
static exprptr define_syntax(tokenstreamptr tkns, stack_frame_ptr sf) {
  tokenptr define_syntax_tkn = next_tkn(tkns);

  assert(define_syntax_tkn->type == TOKEN_DEFINE_SYNTAX);
  assert(sf != NULL);

  /* Obtain the macro name */
  tokenptr nametkn = next_tkn(tkns);
  if (nametkn->type != TOKEN_IDENTIFIER) {
    return parser_error(nametkn, "First argument of define-syntax must be a valid macro name.");
  }

  /* Obtain the macro procedure */
  tokenptr macro_tkn = current_tkn(tkns);
  exprptr macro_expr = expr_parse(tkns, sf);
  if (macro_expr == NULL) {
    return NULL;
  }

  objectptr macro_obj = interpret_expr(macro_expr, sf);
  delete_expr(macro_expr);
  if (!is_procedure(macro_obj)) {
    delete_object(macro_obj);
    return parser_error(macro_tkn, "A macro must be a procedure.");
  }

  /* The macro procedure must work with a single argument */
  if (procedure_get_pn_arity(macro_obj) != 1) {
    delete_object(macro_obj);
    return parser_error(macro_tkn, "The arity of the macro procedure must be 1");
  }

  /* Put the macro inside the stack frame for later use */
  char *macro_variable_name = format("#%s", nametkn->value.character_sequence);
  stack_frame_set_global_variable(sf, macro_variable_name, macro_obj);
  free(macro_variable_name);
  delete_object(macro_obj);

  /* The value of a define-syntax expression is #void */
  objectptr voidobj = make_void();
  exprptr result = new_data_expr(voidobj, macro_tkn);
  delete_object(voidobj);
  return result;
}

/* Executes a macro */
static exprptr evaluate_macro(const char *macro_name, tokenptr macro_tkn, 
                              tokenstreamptr tkns, stack_frame_ptr sf) {
  assert(stack_frame_defined(sf, macro_name));

  /* Macros take a single argument that refers to the token stream */
  objectptr *macro_arg = malloc(sizeof(objectptr));
  *macro_arg = make_internal(tkns);

  /* Run the macro procedure */
  objectptr macro_obj = stack_frame_get_variable(sf, macro_name);
  objectptr result = object_op_call(macro_obj, 1, macro_arg, sf);
  delete_object(macro_obj);
  delete_object(*macro_arg);
  free(macro_arg);

  /* If macro returned an error, return the same error */
  if (is_error(result)) {
    char *error_message = error_tostring(result);
    parser_error(macro_tkn, error_message);
    delete_object(result);
    free(error_message);
    return NULL;
  }

  /* Macro must return a string that contains source code */
  if (!is_string(result)) {
    delete_object(result);
    return parser_error(macro_tkn, "Macro does not yield a string value");
  }

  /* Get generated code from the macro result */
  const char *generated_code = string_value(result);

  /* Parse the generated code. */
  tokenstreamptr tokens = scanner(generated_code);
  delete_object(result);
  listptr parse_tree = parser(tokens, sf);
  delete_tokenstream(tokens);

  /* If a parser error has occured, return NULL */
  if (parse_tree == NULL) {
    return NULL;
  }

  /* The generated parse tree must contain a single expression */
  if (list_size(parse_tree) != 1) {
    delete_parse_tree(parse_tree);
    return parser_error(macro_tkn, "Macro yields multiple expressions");
  }

  /* If a single expression is successfully created, return it. */
  exprptr generated_expression = list_get(parse_tree, 0);
  delete_list(parse_tree);
  return generated_expression;
} 

/* Parses an expression that starts with a left parenthesis */
static exprptr parenthesized_expr_parse(tokenstreamptr tkns, stack_frame_ptr sf) {
  tokenptr tkn = current_tkn(tkns);

  if (tkn->type == TOKEN_END_OF_FILE) {
    return parser_error(tkn, ERR_UNEXPECTED_EOF);
  }

  if (tkn->type == TOKEN_RIGHT_PARENTHESIS) {
    (void)next_tkn(tkns);
    objectptr null_obj = make_null();
    exprptr result = new_data_expr(null_obj, tkn);
    delete_object(null_obj);
    return result;
  }

  exprptr subexpr = NULL;
  switch (tkn->type) {
    case TOKEN_INCLUDE:
      subexpr = include_file(tkns, sf);
      break;
    case TOKEN_LET:
      subexpr = let_expr_parse(tkns, sf);
      break;
    case TOKEN_LAMBDA:
      subexpr = lambda_expr_parse(tkns, sf);
      break;
    case TOKEN_IF:
      subexpr = if_expr_parse(tkns, sf);
      break;
    case TOKEN_DEFINE:
      subexpr = definition_expr_parse(tkns, sf);
      break;
    case TOKEN_DEFINE_SYNTAX:
      subexpr = define_syntax(tkns, sf);
      break;
    case TOKEN_SET:
      subexpr = set_expr_parse(tkns, sf);
      break;
    case TOKEN_COND:
      subexpr = cond_expr_parse(tkns, sf);
      break;
    case TOKEN_AUTOMATON:
      subexpr = automaton_expr_parse(tkns, sf);
      break;
    case TOKEN_TRY:
      subexpr = try_catch_expr_parse(tkns, sf);
      break;
    case TOKEN_IDENTIFIER:
      /* Check if a macro with the given name exists. In case a macro is found,
       * execute the macro and then break the case. Otherwise it is an ordinary
       * function evaluation, so continue. */
      {
        char *macro_name = format("#%s", tkn->value.character_sequence);
        if (stack_frame_defined(sf, macro_name)) {
          (void)next_tkn(tkns);
          subexpr = evaluate_macro(macro_name, tkn, tkns, sf);
          free(macro_name);
          break;
        }
        free(macro_name);
      }
    case TOKEN_STRING:
    case TOKEN_INTEGER:
    case TOKEN_REAL:
    case TOKEN_RATIONAL:
    case TOKEN_LEFT_PARENTHESIS:
    case TOKEN_LEFT_CURLY_BRACKET:
      subexpr = evaluation_expr_parse(tkns, sf);
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
exprptr expr_parse(tokenstreamptr tkns, stack_frame_ptr sf) {
  tokenptr tkn = next_tkn(tkns);

  if (tkn->type == TOKEN_END_OF_FILE) {
    return parser_error(tkn, ERR_UNEXPECTED_EOF);
  }
  if (tkn->type == TOKEN_RIGHT_PARENTHESIS) {
    return parser_error(tkn, ERR_UNMATCHED_PARENTHESIS);
  }
  if (tkn->type == TOKEN_LEFT_PARENTHESIS) {
    return parenthesized_expr_parse(tkns, sf);
  }
  if (tkn->type == TOKEN_LEFT_CURLY_BRACKET) {
    return pn_expr_parse(tkns, sf);
  }
  if (tkn->type == TOKEN_PERCENT) {
    exprptr inner = expr_parse(tkns, sf);
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
    case TOKEN_RATIONAL:
      assign_object(
          &obj, make_rational(tkn->value.rational[0], tkn->value.rational[1]));
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
objectptr expr_call(exprptr self, size_t nargs, objectptr *args,
                    stack_frame_ptr sf) {
  return self->vtable->call(self, nargs, args, sf);
}

objectptr expr_call_internal(exprptr self, void *args, stack_frame_ptr sf) {
  return self->vtable->call_internal(self, args, sf);
}

size_t expr_get_arity(exprptr self) { return self->vtable->get_arity(self); }

size_t expr_get_pn_arity(exprptr self) {
  return self->vtable->get_pn_arity(self);
}
