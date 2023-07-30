#include "pn_expr.h"

#include <string.h>
#include <assert.h>

#include "../scanner/scanner.h"
#include "../parser/parser.h"
#include "../utils/heap-format.h"
#include "../utils/stack.h"
#include "../types/error.h"
#include "../types/pair.h"
#include "../types/void.h"
#include "expression.h"
#include "lambda_expr.h"

typedef struct {
  listptr body;
  listptr captured;
  bool evaluated;
} pn_expr;

static const char pn_expr_name[] = "pn_expr";

static const expr_vtable pn_expr_vtable = {
  .deallocate = delete_pn_expr,
  .clone = clone_pn_expr,
  .to_string = pn_expr_tostring,
  .interpret = interpret_pn_expr
};

exprptr new_pn_expr(void) {
  pn_expr *pe = malloc(sizeof(pn_expr));
  pe->body = new_list();
  pe->captured = new_list();
  pe->evaluated = false;

  expr_t *e = malloc(sizeof(expr_t));
  e->data = pe;
  e->vtable = &pn_expr_vtable;
  e->expr_name = pn_expr_name;
  return e;
}

void delete_pn_expr(exprptr self) {
  pn_expr *pe = self->data;

  for (size_t i = 0; i < list_size(pe->body); i++) {
    exprptr arg = list_get(pe->body, i);
    delete_expr(arg);
  }

  for (size_t i = 0; i < list_size(pe->captured); i++) {
    char *name = list_get(pe->captured, i);
    free(name);
  }

  delete_list(pe->body);
  delete_list(pe->captured);
  free(pe);
  free(self);
}

exprptr clone_pn_expr(exprptr self) {
  pn_expr *new_pn = (pn_expr *)malloc(sizeof(pn_expr));
  pn_expr *self_pn = self->data;
  new_pn->body = new_list();
  new_pn->captured = new_list();
  new_pn->evaluated = self_pn->evaluated;

  for (size_t i = 0; i < list_size(self_pn->body); i++) {
    exprptr self_arg = list_get(self_pn->body, i);
    list_add(new_pn->body, clone_expr(self_arg));
  }

  for (size_t i = 0; i < list_size(self_pn->captured); i++) {
    char *name = list_get(self_pn->captured, i);
    list_add(new_pn->captured, strdup(name));
  }

  return base_clone(self, new_pn);
}

char *pn_expr_tostring(exprptr self) {
  pn_expr *pe = self->data;
  char *args_str = NULL;
  for (size_t i = 0; i < list_size(pe->body); i++) {
    exprptr arg = list_get(pe->body, i);
    char *str = expr_tostring(arg);
    if (args_str == NULL) {
      args_str = str;
    } else {
      char *new_args_str = heap_format("%s %s", args_str, str);
      free(args_str);
      free(str);
      args_str = new_args_str;
    }
  }

  char *result = NULL;
  if (args_str) {
    result = heap_format("{%s}", args_str);
  } else {
    result = heap_format("{}");
  }

  free(args_str);
  return result;
}

void pn_expr_add_body_expr(exprptr self, exprptr body_expr) {
  pn_expr *pe = self->data;
  list_add(pe->body, body_expr);
}

void pn_expr_add_captured_var(exprptr self, char *name) {
  pn_expr *pe = self->data;
  list_add(pe->captured, strdup(name));
}

/**
 * Parses captured variable list in a PN expression.
 * This function is exactly the same as capture parser for lambda expressions.
 */
static listptr pn_expr_parse_get_captures(listptr tokens, int *index) {
  listptr captured_variables = new_list();

  token_t *left_bracket = list_get(tokens, *index);
  if (left_bracket->type == TOKEN_LEFT_SQUARE_BRACKET) {
    (*index)++;
    
    token_t *captured_token = NULL;
    while ((captured_token = list_get(tokens, *index))->type == TOKEN_IDENTIFIER) {
      list_add(captured_variables, captured_token->value.character_sequence);
      (*index) += 1;
    }

    token_t *right_bracket = list_get(tokens, (*index)++);
    if (right_bracket->type != TOKEN_RIGHT_SQUARE_BRACKET) {
      delete_list(captured_variables);
      return parser_error(right_bracket->line,
			  right_bracket->column, "Capture list is incomplete");
    }
  }

  return captured_variables;
}

/**
 * Parses PN expression
 * A PN expression can be one of the following
 *  1. a single value,
 *      Example: {1}
 *  2. a list of function evaluations that yield a single value,
 *      Example {+ $1 - $3 $2}
 *  3. or one of these with a captured variable list.
 *      Examples: {[x y] 1}, {[x y] + x y}
 */
exprptr pn_expr_parse(listptr tokens, int *index) {
  token_t *first_token = list_get(tokens, *index);
  if (first_token->type == TOKEN_RIGHT_CURLY_BRACKET) {
    return parser_error(first_token->line, first_token->column, 
        "Polish notation expression cannot be empty.");
  }

  listptr captures = pn_expr_parse_get_captures(tokens, index);
  if (!captures) {
    return NULL;
  }

  exprptr expr = new_pn_expr();

  for (size_t i = 0; i < list_size(captures); i++) {
    pn_expr_add_captured_var(expr, list_get(captures, i));
  }
  delete_list(captures);

  while (((token_t *)list_get(tokens, *index))->type != TOKEN_RIGHT_CURLY_BRACKET) {
    exprptr body_expr = expr_parse(tokens, index);
    if (!body_expr) {
      delete_expr(expr);
      return NULL;
    }

    pn_expr_add_body_expr(expr, body_expr);
  }

  token_t *right_curly_bracket = list_get(tokens, (*index)++);
  assert(right_curly_bracket->type == TOKEN_RIGHT_CURLY_BRACKET);

  return expr;
}

bool is_pn_expr(exprptr e) {
  return strcmp(e->expr_name, pn_expr_name) == 0;
}

/**
 * Embeds the PN expression in a lambda expression body, so that
 * PN expressions can be stored inside procedure objects, and used
 * like other functions.
 */
static object_t convert_pn_expr_to_lambda(exprptr self, stack_frame_ptr sf) {
  /* Make a copy of the PN expression and mark it as evaluated */
  exprptr clone_expr = clone_pn_expr(self);
  pn_expr *pe = clone_expr->data;
  pe->evaluated = true;

  /* Create the enclosing lambda with the same captured variables */
  exprptr lambda_expr = new_lambda_expr(clone_expr, true);
  for (size_t i = 0; i < list_size(pe->captured); i++) {
    char *var = list_get(pe->captured, i);
    lambda_expr_add_captured_var(lambda_expr, var);
  }

  /* Interpret lambda to get a procedure object */
  object_t result = interpret_lambda(lambda_expr, sf);
  delete_expr(lambda_expr);
  return result;
}

/** 
 * Creates a new local stack frame and saves variadic arguments of
 * enclosing lambda in local variables named $1, $2, $3, ...
 */
static stack_frame_ptr obtain_arguments(stack_frame_ptr sf) {
  /* Convert va_args to an internal list */
  object_t va_args_obj = stack_frame_get_variable(sf, "va_args");
  assert(!is_error(va_args_obj));
  listptr va_args = new_list();
  assert(cons_list_to_internal_list(va_args_obj, va_args));
  destroy_object(va_args_obj);

  stack_frame_ptr local_frame = new_stack_frame(sf);
  for (size_t i = 0; i < list_size(va_args); i++) {
    /* Add value of va_args[i] as $i to local stack frame */
    char *var_name = heap_format("$%ld", i + 1);
    object_t *value = list_get(va_args, i);
    stack_frame_set_variable(local_frame, var_name, *value);

    /* Deallocate va_args[i] */
    free(var_name);
    destroy_object(*value);
    free(value);
  }

  delete_list(va_args);
  return local_frame;
}

/** 
 * Interprets each expression in the PN expr body within 
 * the new local stack frame. The first error in the expressions
 * is returned. Returns void object if no error occurs.
 */
static object_t interpret_body_expressions(listptr body,
                                           stack_frame_ptr local_frame, 
                                           stackptr *waiting) {
  *waiting = new_stack();

  for (size_t i = 0; i < list_size(body); i++) {
    exprptr body_expr = list_get(body, i);
    object_t *evaluated_arg = (object_t *)malloc(sizeof(object_t));
    *evaluated_arg = interpret_expr(body_expr, local_frame);

    if (is_error(*evaluated_arg)) {
      for (size_t j = i; j != 0; j--) {
        object_t *obj = stack_get(*waiting, j - 1);
        destroy_object(*obj);
        free(obj);
      }
      
      delete_stack(*waiting);
      object_t error = *evaluated_arg;
      free(evaluated_arg);
      return error;
    }

    stack_push(*waiting, evaluated_arg);
  }

  return make_void();
}

/**
 * Interprets entire body.
 */
static object_t interpret_body(stackptr waiting, stack_frame_ptr sf,
                               stackptr *computed) {
  *computed = new_stack();

  while (!stack_is_empty(waiting)) {
    object_t *body_obj = stack_pop(waiting);
    if (is_procedure(*body_obj)) {
      /* If the expression is a procedure, get its lambda and closure */
      exprptr lambda = procedure_get_lambda(*body_obj);
      listptr closure = procedure_get_closure(*body_obj);
      size_t pn_arity = procedure_get_pn_arity(*body_obj);

      object_t *arguments = (object_t *)malloc(pn_arity * sizeof(object_t));
      size_t i = 0;
      /* 
       * Obtain arguments that are not available in the "computed" stack from $ variables 
       * so that simplified syntax such as {!= "a"} can be used in place of {!= $1 "a"}.
       * This use is valid only for the last procedure in the body.
       * */
      for (; i + stack_size(*computed) < pn_arity; i++) {
        char *name = heap_format("$%d", i + 1);
        arguments[i] = stack_frame_get_variable(sf, name);
        free(name);
      }

      /* Obtain lambda arguments from "computed" stack */
      for (; i < pn_arity; i++) {
        object_t *arg = stack_pop(*computed);
        arguments[i] = *arg;
        free(arg);
      }

      /* Call the lambda with its closure and arguments */
      object_t *new_result_object = (object_t *)malloc(sizeof(object_t));
      *new_result_object = call_lambda(lambda, closure, pn_arity, arguments, sf);

      /* Push the resulting value to the "computed" stack */
      bool err = is_error(*new_result_object); 
      if (!err) {
        stack_push(*computed, new_result_object);
      }

      /* Deallocate arguments */
      for (size_t i = 0; i < pn_arity; i++) {
        destroy_object(arguments[i]);
      }
      free(arguments);

      /* Deallocate procedure itself */
      destroy_object(*body_obj);
      free(body_obj);

      /* If there was an error, return it immediately */
      if (err) {
        object_t result = *new_result_object;
        free(new_result_object);
        return result;
      }
      
    } else {
      /* If the expression is not a function, move it to the "computed" stack */
      stack_push(*computed, body_obj);
    }
  }

  object_t *obj = stack_pop(*computed);
  object_t result = *obj;
  free(obj);

  /* Do not allow multiple results. */
  if (!is_error(result) && !stack_is_empty(*computed)) {
    destroy_object(result);
    return make_error("Polish notation expression yields multiple values.");
  }

  return result;
}

/**
 * Interprets the PN expression.
 * The evaluation of a PN expression has two stages.
 *
 * When the interpreter first encounters an (unevaluated) PN expression,
 * a clone of it gets embedded inside a lambda expression and returned as 
 * a procedure object. The embedded version is marked as evaluated, 
 * so that the interpreter can recognize that it has completed its first stage.
 * 
 * When the resulting lambda expression inside the procedure object 
 * gets evaluated, which happens when the program tries to call the procedure 
 * object, the lambda tries to interpret its body just like any other expression.
 * This time, the PN expression has already been marked as evaluated, so the 
 * second stage begins. It obtains its arguments from the va_args variable
 * created by lambda, and then actually starts evaluating the PN expression 
 * using these arguments.
 */
object_t interpret_pn_expr(exprptr self, stack_frame_ptr sf) {
  pn_expr *pe = self->data;

  /* First stage */
  if (!pe->evaluated) {
    return convert_pn_expr_to_lambda(self, sf);
  }

  /* Create local stack frame with arguments */
  stack_frame_ptr local_frame = obtain_arguments(sf);

  /* Interpret body expressions inside new stack frame */
  stackptr waiting = NULL;
  object_t error = interpret_body_expressions(pe->body, local_frame, &waiting);
  if (is_error(error)) {
    delete_stack_frame(local_frame);
    return error;
  }
  destroy_object(error);
  
  /* Interpret entire body to obtain a single value */
  stackptr computed = NULL;
  object_t result = interpret_body(waiting, local_frame, &computed);

  /* If an error occurs, it is possible that things remain on stacks. Deallocate them. */
  while(!stack_is_empty(waiting)) {
    object_t *ptr = stack_pop(waiting);
    destroy_object(*ptr);
    free(ptr);
  }
  while(!stack_is_empty(computed)) {
    object_t *ptr = stack_pop(computed);
    destroy_object(*ptr);
    free(ptr);
  }

  /* Clean */
  delete_stack_frame(local_frame);
  delete_stack(computed);
  delete_stack(waiting);

  return result;
}
