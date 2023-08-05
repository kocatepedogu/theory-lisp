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

#include "automaton.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "../parser/parser.h"
#include "../scanner/scanner.h"
#include "../types/error.h"
#include "../types/void.h"
#include "../types/object.h"
#include "../utils/list.h"
#include "../utils/string.h"
#include "../automaton/automaton.h"
#include "common.h"
#include "expression.h"
#include "expression_base.h"
#include "lambda.h"
#include "polish.h"

typedef enum {
  HEAD_OP_MOVE_LEFT_EXPR,
  HEAD_OP_MOVE_RIGHT_EXPR,
  HEAD_OP_WRITE_EXPR,
  HEAD_NOP_EXPR
} head_operation_type_expr;

typedef struct {
  head_operation_type_expr op;
  exprptr write_value;
} head_operation_expr;

typedef struct {
  exprptr condition;
  listptr head_operations;
  exprptr output;
  char *next_state_name;
} transition_expr;

typedef struct {
  exprptr base_machine;
  char *name;
  exprptr output;
  listptr transitions;
} state_expr;

typedef struct {
  listptr captures;
  size_t number_of_tapes;
  listptr states;
  automaton_t *compiled;
} automaton_expr;

static const char automaton_expr_name[] = "automaton_expr";

static const expr_vtable automaton_expr_vtable = {
    .clone = clone_automaton_expr,
    .deallocate = delete_automaton_expr,
    .interpret = interpret_automaton,
    .to_string = automaton_expr_tostring,
    .call = call_automaton,
    .call_internal = call_automaton_internal};

bool is_automaton_expr(exprptr e) {
  return strcmp(e->expr_name, automaton_expr_name) == 0;
}

exprptr new_automaton_expr(size_t number_of_tapes) {
  automaton_expr *ae = (automaton_expr *)malloc(sizeof(automaton_expr));
  ae->states = new_list();
  ae->captures = new_list();
  ae->compiled = NULL;
  ae->number_of_tapes = number_of_tapes;

  return base_new(ae, &automaton_expr_vtable, automaton_expr_name, 0, 0);
}

static void delete_head_operation_list(listptr lst) {
  for (size_t i = 0; i < list_size(lst); i++) {
    head_operation_expr *head_op = list_get(lst, i);
    if (head_op->write_value) {
      delete_expr(head_op->write_value);
    }
    free(head_op);
  }
  delete_list(lst);
}

static void delete_transition(transition_expr *tr) {
  delete_head_operation_list(tr->head_operations);
  free(tr->next_state_name);
  delete_expr(tr->condition);
  if (tr->output) {
    delete_expr(tr->output);
  }
  free(tr);
}

static void delete_state(state_expr *st) {
  for (size_t j = 0; j < list_size(st->transitions); j++) {
    transition_expr *tr = list_get(st->transitions, j);
    delete_transition(tr);
  }

  if (st->base_machine) {
    delete_expr(st->base_machine);
  }

  if (st->output) {
    delete_expr(st->output);
  }

  free(st->name);
  delete_list(st->transitions);
  free(st);
}

void delete_automaton_expr(exprptr self) {
  automaton_expr *ae = self->data;

  for (size_t i = 0; i < list_size(ae->states); i++) {
    state_expr *st = list_get(ae->states, i);
    delete_state(st);
  }

  for (size_t i = 0; i < list_size(ae->captures); i++) {
    char *capture = list_get(ae->captures, i);
    free(capture);
  }

  if (ae->compiled) {
    delete_automaton(ae->compiled);
  }

  delete_list(ae->states);
  delete_list(ae->captures);
  free(ae);
  free(self);
}

static head_operation_expr *clone_head_operation(head_operation_expr *self_op) {
  head_operation_expr *new_head_op = malloc(sizeof(head_operation_expr));
  new_head_op->op = self_op->op;
  if (self_op->write_value) {
    new_head_op->write_value = clone_expr(self_op->write_value);
  } else {
    new_head_op->write_value = NULL;
  }

  return new_head_op;
}

static transition_expr *clone_transition_expr(transition_expr *self_tr) {
  transition_expr *new_tr = (transition_expr *)malloc(sizeof(transition_expr));
  new_tr->condition = clone_expr(self_tr->condition);
  new_tr->next_state_name = strdup(self_tr->next_state_name);
  new_tr->output = clone_expr(self_tr->output);
  new_tr->head_operations = new_list();

  for (size_t k = 0; k < list_size(self_tr->head_operations); k++) {
    head_operation_expr *new_head_op = 
      clone_head_operation(list_get(self_tr->head_operations, k));
    list_add(new_tr->head_operations, new_head_op);
  }

  return new_tr;
}

exprptr clone_automaton_expr(exprptr self) {
  automaton_expr *self_ae = self->data;
  automaton_expr *new_ae = (automaton_expr *)malloc(sizeof(automaton_expr));
  new_ae->compiled = clone_automaton(self_ae->compiled);
  new_ae->states = new_list();
  new_ae->number_of_tapes = self_ae->number_of_tapes;
  new_ae->captures = new_list();

  for (size_t i = 0; i < list_size(self_ae->states); i++) {
    state_expr *self_st = list_get(self_ae->states, i);
    state_expr *new_st = (state_expr *)malloc(sizeof(state_expr));
    new_st->transitions = new_list();

    for (size_t j = 0; j < list_size(self_st->transitions); j++) {
      transition_expr *new_tr = 
        clone_transition_expr(list_get(self_st->transitions, j));
      list_add(new_st->transitions, new_tr);
    }

    new_st->name = strdup(self_st->name);
    new_st->base_machine = clone_expr(self_st->base_machine);
    new_st->output = clone_expr(self_st->output);
    list_add(new_ae->states, new_st);
  }

  for (size_t i = 0; i < list_size(self_ae->captures); i++) {
    char *capture = list_get(self_ae->captures, i);
    list_add(new_ae->captures, strdup(capture));
  }

  return base_clone(self, new_ae);
}

char *transition_tostring(transition_expr *tr) {
  char *cond_str = expr_tostring(tr->condition);
  char *next_name_str = tr->next_state_name;
  
  char *output_str = NULL;
  if (tr->output) {
    output_str = expr_tostring(tr->output);
  } else {
    output_str = strdup("");
  }

  char *beginning = format("(%s", cond_str);
  free(cond_str);

  for (size_t i = 0; i < list_size(tr->head_operations); i++) {
    head_operation_expr *head_op = list_get(tr->head_operations, i);
    char *str = NULL;
    switch (head_op->op) {
      case HEAD_OP_MOVE_RIGHT_EXPR:
        str = strdup("->");
        break;
      case HEAD_OP_MOVE_LEFT_EXPR:
        str = strdup("<-");
        break;
      case HEAD_OP_WRITE_EXPR:
        str = expr_tostring(head_op->write_value);
        break;
      case HEAD_NOP_EXPR:
        str = strdup(".");
        break;
    }

    beginning = unique_append_sep(beginning, " ", str);
  }

  char *result = format("%s %s) %s", beginning, next_name_str, output_str);
  free(beginning);
  free(output_str);
  return result;
}

char *state_tostring(state_expr *st) {
  char *output_str = NULL;
  if (st->output) {
    output_str = expr_tostring(st->output);
  } else {
    output_str = strdup("");
  }

  char *result = format("(%s %s", st->name, output_str);
  free(output_str);

  for (size_t i = 0; i < list_size(st->transitions); i++) {
    transition_expr *tr = list_get(st->transitions, i);
    char *tr_str = transition_tostring(tr);
    char *new_result = format("%s\n%s", result, tr_str);
    free(result);
    free(tr_str);
    result = new_result;
  }

  char *new_result = format("%s)", result);
  free(result);
  return new_result;
}

char *automaton_expr_tostring(exprptr self) { 
  automaton_expr *ae = self->data;
  char *captures = capture_list_tostring(ae->captures);
  char *result = format("(automaton\\%ld %s", ae->number_of_tapes,
      captures);
  free(captures);
  for (size_t i = 0; i < list_size(ae->states); i++) {
    state_expr *st = list_get(ae->states, i);
    char *st_str = state_tostring(st);
    char *new_result = format("%s\n%s", result, st_str);
    free(result);
    free(st_str);
    result = new_result;
  }

  char *new_result = format("%s)", result);
  free(result);
  return new_result;
}

static void automaton_expr_add_state(exprptr self, state_expr *st) {
  automaton_expr *ae = self->data;
  list_add(ae->states, st);
}

static void automaton_expr_add_capture(exprptr self, char *capture) {
  automaton_expr *ae = self->data;
  list_add(ae->captures, strdup(capture));
}

static listptr head_operations_parse(size_t arity, tokenstreamptr tkns) {
  listptr head_operations = new_list();

  for (size_t i = 0; i < arity; i++) {
    head_operation_expr *head_op = malloc(sizeof(head_operation_expr));
    head_op->write_value = NULL;

    tokenptr head_operation_tkn = current_tkn(tkns);
    switch (head_operation_tkn->type) {
      case TOKEN_MOVE_LEFT:
        (void)next_tkn(tkns);
        head_op->op = HEAD_OP_MOVE_LEFT_EXPR;
        break;
      case TOKEN_MOVE_RIGHT:
        (void)next_tkn(tkns);
        head_op->op = HEAD_OP_MOVE_RIGHT_EXPR;
        break;
      case TOKEN_NOP:
        (void)next_tkn(tkns);
        head_op->op = HEAD_NOP_EXPR;
        break;
      default:
        head_op->op = HEAD_OP_WRITE_EXPR;
        if (!(head_op->write_value = expr_parse(tkns))) {
          delete_head_operation_list(head_operations);
          free(head_op);
          return NULL;
        }
        break;
    }

    list_add(head_operations, head_op);
  }

  return head_operations;
}


transition_expr *automaton_expr_parse_transition_parse(size_t arity, tokenstreamptr tkns) {
  exprptr condition = expr_parse(tkns);
  if (condition == NULL) {
    return NULL;
  }

  listptr head_operations = head_operations_parse(arity, tkns);
  if (head_operations == NULL) {
    delete_expr(condition);
    return NULL;
  }

  tokenptr next_state_name_tkn = next_tkn(tkns);
  if (next_state_name_tkn->type != TOKEN_IDENTIFIER) {
    delete_expr(condition);
    return parser_error(next_state_name_tkn, "Expected next state name");
  }

  tokenptr right_p = next_tkn(tkns);
  if (right_p->type != TOKEN_RIGHT_PARENTHESIS) {
    delete_expr(condition);
    return parser_error(right_p, "Expected right parenthesis");
  }

  exprptr transition_output = NULL;
  token_t *tkn = current_tkn(tkns);
  if (tkn->type == TOKEN_LEFT_CURLY_BRACKET) {
    (void)next_tkn(tkns);
    transition_output = pn_expr_parse(tkns);
  }

  transition_expr *tr = (transition_expr *)malloc(sizeof(transition_expr));
  tr->condition = condition;
  tr->next_state_name = strdup(next_state_name_tkn->value.character_sequence);
  tr->output = transition_output;
  tr->head_operations = head_operations;
  return tr;
}

state_expr *automaton_expr_state_parse(size_t arity, tokenstreamptr tkns) {
  tokenptr left_p = next_tkn(tkns);
  if (left_p->type != TOKEN_LEFT_PARENTHESIS) {
    return parser_error(left_p, "Expected left parenthesis");
  }

  tokenptr tkn_state_name = next_tkn(tkns);
  if (tkn_state_name->type != TOKEN_IDENTIFIER) {
    return parser_error(tkn_state_name, "Expected state name identifier");
  }

  tokenptr tkn_colon = current_tkn(tkns);
  exprptr base_machine = NULL;
  if (tkn_colon->type == TOKEN_COLON) {
    (void)next_tkn(tkns);
    base_machine = expr_parse(tkns);
  }

  tokenptr tkn = current_tkn(tkns);
  exprptr state_output = NULL;
  if (tkn->type == TOKEN_LEFT_CURLY_BRACKET) {
    (void)next_tkn(tkns);
    state_output = pn_expr_parse(tkns);
  }

  state_expr *st = (state_expr *)malloc(sizeof(state_expr));
  st->transitions = new_list();
  st->name = strdup(tkn_state_name->value.character_sequence);
  st->output = state_output;
  st->base_machine = base_machine;

  while ((tkn = next_tkn(tkns))->type != TOKEN_RIGHT_PARENTHESIS) {
    if (tkn->type != TOKEN_LEFT_PARENTHESIS) {
      delete_state(st);
      return parser_error(tkn, "Expected left parenthesis");
    }

    transition_expr *tr = automaton_expr_parse_transition_parse(arity, tkns);
    if (tr == NULL) {
      delete_state(st);
      return NULL;
    }
    list_add(st->transitions, tr);
  }

  return st;
}

exprptr automaton_expr_parse(tokenstreamptr tkns) {
  tokenptr automaton_token = next_tkn(tkns);
  assert(automaton_token->type == TOKEN_AUTOMATON);

  tokenptr backslash = next_tkn(tkns);
  if (backslash->type != TOKEN_BACKSLASH) {
    return parser_error(backslash, "Expected backslash followed by an arity number");
  }

  tokenptr arity_token = next_tkn(tkns);
  if (arity_token->type != TOKEN_INTEGER) {
    return parser_error(arity_token, "Expected an integer denoting number of tapes");
  }

  tokenptr optional_left_curly_bracket = current_tkn(tkns);
  listptr captures = NULL;
  if (optional_left_curly_bracket->type == TOKEN_LEFT_SQUARE_BRACKET) {
    captures = get_capture_list(tkns);
    if (captures == NULL) {
      return NULL;
    }
  }

  size_t arity = arity_token->value.integer;
  exprptr e = new_automaton_expr(arity);

  if (captures) {
    for (size_t i = 0; i < list_size(captures); i++) {
      automaton_expr_add_capture(e, list_get(captures, i));
    }
    delete_list(captures);
  }

  while (current_tkn(tkns)->type != TOKEN_RIGHT_PARENTHESIS) {
    state_expr *st = automaton_expr_state_parse(arity, tkns);
    if (!st) {
      delete_expr(e);
      return NULL;
    }

    automaton_expr_add_state(e, st);
  }

  return e;
}

static size_t find_state_index(listptr states, char *name) {
  for (size_t k = 0; k < list_size(states); k++) {
    state_expr *search_st = list_get(states, k);
    if (strcmp(search_st->name, name) == 0) {
      return k;
      break;
    }
  }

  return (size_t)(-1);
}

/**
 * Helper function of compile_automaton.
 * Returns the first error in the compiled automaton
 */
static object_t find_compilation_errors(automaton_t *aut) {
  for (size_t i = 0; i < aut->number_of_states; i++) {
    state_t *st = &aut->states[i];

    for (size_t j = 0; j < st->number_of_transitions; j++) {
      transition_t *tr = &st->transitions[j];

      if (tr->action == ACT_CONTINUE && tr->next_state_index == (size_t)(-1)) {
        return make_error("Next state does not exist.");
      }
    }
  }

  return make_void();
}

static void compile_head_operations(size_t ntapes, transition_expr *expr_tr,
                                    transition_t *aut_tr, stack_frame_ptr sf) {
  aut_tr->head_operations = malloc(ntapes * sizeof(head_op_t));
  for (size_t k = 0; k < ntapes; k++) {
    head_operation_expr *expr_op = list_get(expr_tr->head_operations, k);
    head_op_t *head_op = &aut_tr->head_operations[k];
    switch (expr_op->op) {
      case HEAD_OP_MOVE_LEFT_EXPR: 
        head_op->op = HEAD_OP_MOVE_LEFT; 
        head_op->write_value = NULL;
        break;
      case HEAD_OP_MOVE_RIGHT_EXPR: 
        head_op->op = HEAD_OP_MOVE_RIGHT; 
        head_op->write_value = NULL;
        break;
      case HEAD_OP_WRITE_EXPR:
        head_op->op = HEAD_OP_WRITE;
        head_op->write_value = clone_expr(expr_op->write_value);
        break;
      case HEAD_NOP_EXPR:
        head_op->op = HEAD_NOP;
        head_op->write_value = NULL;
        break;
    }
  }
}

static void compile_transitions(size_t ntapes, listptr states, state_expr *expr_st,
                                size_t self_index, state_t *aut_st, stack_frame_ptr sf) {
 
  if (aut_st->number_of_transitions == 0) {
    aut_st->transitions = NULL;
  } else { 
    aut_st->transitions = malloc(aut_st->number_of_transitions * sizeof(transition_t));

    for (size_t j = 0; j < aut_st->number_of_transitions; j++) {
      transition_expr *expr_tr = list_get(expr_st->transitions, j);

      transition_t *aut_tr = &aut_st->transitions[j];
      aut_tr->condition = clone_expr(expr_tr->condition);
      aut_tr->output = expr_tr->output ? clone_expr(expr_tr->output) : NULL;

      if (strcmp(expr_tr->next_state_name, "self") == 0) {
        aut_tr->next_state_index = self_index;
        aut_tr->action = ACT_CONTINUE;
      } else if (strcmp(expr_tr->next_state_name, "next") == 0) {
        aut_tr->next_state_index = self_index + 1;
        aut_tr->action = ACT_CONTINUE;
      } else if (strcmp(expr_tr->next_state_name, "halt") == 0) {
        aut_tr->next_state_index = (size_t)(-1);
        aut_tr->action = ACT_HALT;
      } else if (strcmp(expr_tr->next_state_name, "accept") == 0) {
        aut_tr->next_state_index = (size_t)(-1);
        aut_tr->action = ACT_ACCEPT;
      } else if (strcmp(expr_tr->next_state_name, "reject") == 0) {
        aut_tr->next_state_index = (size_t)(-1);
        aut_tr->action = ACT_REJECT;
      } else {
        aut_tr->next_state_index = find_state_index(states, expr_tr->next_state_name);
        aut_tr->action = ACT_CONTINUE;
      }

      compile_head_operations(ntapes, expr_tr, aut_tr, sf);
    }
  }
}

static object_t compile_automaton(automaton_expr *ae, stack_frame_ptr sf) {
  automaton_t *aut = malloc(sizeof(automaton_t));
  aut->number_of_states = list_size(ae->states);
  aut->states = malloc(aut->number_of_states * sizeof(state_t));
  aut->number_of_tapes = ae->number_of_tapes;

  for (size_t i = 0; i < aut->number_of_states; i++) {
    state_expr *expr_st = list_get(ae->states, i); 

    state_t *aut_st = &aut->states[i];
    aut_st->base_machine = expr_st->base_machine ? clone_expr(expr_st->base_machine) : NULL; 
    aut_st->output = expr_st->output ? clone_expr(expr_st->output) : NULL;
    aut_st->number_of_transitions = list_size(expr_st->transitions);
    compile_transitions(ae->number_of_tapes, ae->states, expr_st, i, aut_st, sf);
  }

  object_t result = find_compilation_errors(aut);
  if (is_error(result)) {
    delete_automaton(aut);
    return result;
  }

  ae->compiled = aut;
  return result;
}

/* 
 * Interprets the automaton expression.
 */
object_t interpret_automaton(exprptr self, stack_frame_ptr sf) {
  automaton_expr *ae = self->data;
  object_t error = compile_automaton(ae, sf);
  if (is_error(error)) {
    return error;
  }
  destroy_object(error);
  return make_procedure(self, ae->captures, sf);
}

/*
 * Calls the automaton with tape arguments
 */

object_t call_automaton(exprptr self, size_t nargs, object_t *args, 
                       stack_frame_ptr sf) {
  automaton_expr *ae = self->data;
  automaton_t *aut = ae->compiled;
  return automaton_run(aut, nargs, args, sf);
}


object_t call_automaton_internal(exprptr self, void *args, stack_frame_ptr sf) {
  automaton_expr *ae = self->data;
  automaton_t *aut = ae->compiled;
  return automaton_run_internal(aut, args, sf);
}
