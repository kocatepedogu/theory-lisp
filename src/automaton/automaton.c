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
#include <stdio.h>

#include "../utils/string.h"
#include "../utils/list.h"
#include "../types/void.h"
#include "../types/null.h"
#include "../types/pair.h"
#include "../types/procedure.h"
#include "../types/boolean.h"
#include "../types/integer.h"
#include "../types/error.h"
#include "../expressions/lambda.h"

typedef struct tape {
  size_t head;
  listptr data;
} tape_t;

static void destroy_head_operation(size_t arity, head_op_t *op) {
  if (op->op == HEAD_OP_WRITE) {
    delete_expr(op->write_value);
  }
}

static void destroy_transition(size_t arity, transition_t *tr) {
  delete_expr(tr->condition);
  delete_expr(tr->output);
  for (size_t i = 0; i < arity; i++) {
    destroy_head_operation(arity, &tr->head_operations[i]);
  }
  free(tr->head_operations);
}

static void destroy_state(size_t arity, state_t *st) {
  delete_expr(st->output);
  for (size_t i = 0; i < st->number_of_transitions; i++) {
    destroy_transition(arity, st->transitions + i);
  }
  if (st->base_machine) {
    delete_expr(st->base_machine);
  }

  free(st->transitions);
  st->number_of_transitions = 0;
}

static void destroy_automaton(size_t arity, automaton_t *aut) {
  for (size_t i = 0; i < aut->number_of_states; i++) {
    destroy_state(arity, aut->states + i);
  }
  free(aut->states);
  aut->number_of_states = 0;
}

void delete_automaton(automaton_t *aut) {
  destroy_automaton(aut->number_of_tapes, aut);
  free(aut);
}

static void delete_object_list(listptr lst) {
  for (size_t i = 0; i < list_size(lst); i++) {
    object_t *obj = list_get(lst, i);
    destroy_object(*obj);
    free(obj);
  }

  delete_list(lst);
}

static void delete_tape_list(listptr lst) {
  for (size_t i = 0; i < list_size(lst); i++) {
    tape_t *tp = list_get(lst, i);
    delete_object_list(tp->data);
    free(tp);
  }

  delete_list(lst);
}

static void clone_head_op(head_op_t *clone_op, head_op_t *op) {
  clone_op->op = op->op;
  if (op->op == HEAD_OP_WRITE) {
    clone_op->write_value = clone_expr(op->write_value);
  } else {
    clone_op->write_value = NULL;
  }
}

static void clone_transition(size_t arity, transition_t *clone_tr, transition_t *tr) {
  clone_tr->condition = clone_expr(tr->condition);
  clone_tr->output = tr->output? clone_expr(tr->output) : NULL;
  clone_tr->next_state_index = tr->next_state_index;
  clone_tr->action = tr->action;
  clone_tr->head_operations = malloc(arity *sizeof(head_op_t));

  for (size_t k = 0; k < arity; k++) {
    head_op_t *op = &tr->head_operations[k];
    head_op_t *clone_op = &clone_tr->head_operations[k];
    clone_head_op(clone_op, op);
  }
}

static void clone_state(automaton_t *aut, state_t *clone_st, state_t *st) {
  clone_st->base_machine = st->base_machine ? clone_expr(st->base_machine) : NULL;
  clone_st->output = st->output ? clone_expr(st->output) : NULL;
  clone_st->number_of_transitions = st->number_of_transitions;
  clone_st->transitions = (transition_t *)malloc(st->number_of_transitions *
      sizeof(transition_t));

  for (size_t j = 0; j < st->number_of_transitions; j++) {
    transition_t *tr = &st->transitions[j];
    transition_t *clone_tr = &clone_st->transitions[j];
    clone_transition(aut->number_of_tapes, clone_tr, tr);
  }
}

automaton_t *clone_automaton(automaton_t *aut) {
  if (aut == NULL) {
    return NULL;
  }

  automaton_t *clone_aut = (automaton_t *)malloc(sizeof(automaton_t));
  clone_aut->number_of_states = aut->number_of_states;
  clone_aut->states = (state_t *)malloc(aut->number_of_states * sizeof(state_t));
  clone_aut->number_of_tapes = aut->number_of_tapes;

  for (size_t i = 0; i < clone_aut->number_of_states; i++) {
    state_t *st = &aut->states[i];
    state_t *clone_st = &clone_aut->states[i];
    clone_state(aut, clone_st, st);
  }

  return clone_aut;
}

/** static void debug_print_tape(tape_t *tp) {
  char *result = NULL;
  for (size_t i = 0; i < list_size(tp->data); i++) {
    object_t *obj = list_get(tp->data, i);
    result = unique_append_sep(result, " ", object_tostring(*obj));
  }

  printf("%ld | %s\n", tp->head, result);
  free(result);
} */

static object_t check_arguments(size_t arity, size_t nargs, object_t *args, stack_frame_ptr sf) {
  /** Check that the number of tapes matches the number of tapes used by the automaton */
  if (nargs != arity) {
    return make_error("Automaton expected %ld tapes, but %ld given", arity, nargs);
  }

  /* Check that each argument is a cons pair whose first element 
   * is the head position (an integer) and second element is the tape 
   * contents (a list) */
  for (size_t i = 0; i < nargs; i++) {
    object_t *tape_obj = &args[i];
    if (!is_pair(*tape_obj)) {
      return make_error(
          "Each tape must be a cons pair consisting of "
          "a head position (integer) and tape contents (a list)");
    }

    object_t first = pair_first(*tape_obj);
    if (!is_integer(first)) {
      return make_error("Head position is not an integer");
    }
    
    object_t second = pair_second(*tape_obj);
    if (!is_pair(second)) {
      return make_error("Tape contents is not a list");
    }
  }

  return make_void();
}

static object_t get_tapes(listptr output, size_t arity, size_t nargs, object_t *args, stack_frame_ptr sf) {
  /* Check arguments */
  object_t error = check_arguments(arity, nargs, args, sf);
  if (is_error(error)) {
    return error;
  }
  destroy_object(error);

  /* Convert given tapes to a list of tape_t's */
  for (size_t i = 0; i < nargs; i++) {
    object_t *tape_obj = &args[i];
    object_t position_obj = pair_first(*tape_obj);
    object_t contents_obj = pair_second(*tape_obj);

    listptr contents = new_list();
    if (!cons_list_to_internal_list(contents_obj, contents)) {
      delete_object_list(contents);
      return make_error("Given tape is not in proper list form");
    }

    tape_t *tp = (tape_t *)malloc(sizeof(tape_t));
    tp->head = int_value(position_obj);
    tp->data = contents;
    list_add(output, tp);
  }

  return make_void();
}

static object_t *make_args_array(listptr tapes) {
  size_t ntapes = list_size(tapes);
  
  /* Return NULL if any of the tape heads is not in the bounds of its tape */
  for (size_t i = 0; i < ntapes; i++) {
    tape_t *tp = list_get(tapes, i);
    if (tp->head >= list_size(tp->data)) {
      fprintf(stderr, "Assertion failure: Tape head indices are out of bounds.");
      abort();
    }
  }

  /* Make an array of current symbols under tapes heads */
  object_t *args_array = (object_t *)malloc(sizeof(object_t) * ntapes);
  for (size_t i = 0; i < ntapes; i++) {
    tape_t *tp = list_get(tapes, i);
    args_array[i] = *(object_t *)list_get(tp->data, tp->head);
  }
  return args_array;
}

static object_t run_base_machine(state_t *st, listptr tapes, stack_frame_ptr sf) {
  /* Interpret base machine into a procedure object */
  object_t base_machine = interpret_expr(st->base_machine, sf); 
  if (is_error(base_machine)) {
    return base_machine;
  }

  object_t result = object_op_call_internal(base_machine, tapes, sf);
  destroy_object(base_machine);
  return result;
}

static void run_state_output(state_t *st, object_t *args_array,
                                       size_t ntapes, stack_frame_ptr sf) {
  if (st->output) {
    object_t proc = interpret_expr(st->output, sf);
    if (is_procedure(proc)) {
      object_t result = object_op_call(proc, ntapes, args_array, sf);
      destroy_object(result);
    }
    destroy_object(proc);
  }
}

static object_t run_transition_condition(transition_t *tr, object_t *args_array,
                                                   size_t ntapes, stack_frame_ptr sf) {
 
  object_t proc = interpret_expr(tr->condition, sf);
  if (is_error(proc)) {
    return proc;
  }

  object_t result = object_op_call(proc, ntapes, args_array, sf);
  destroy_object(proc);
  if (is_boolean(result)) {
    return result;
  }
  destroy_object(result);

  return make_error("Transition condition does not yield a boolean value.");
}

static void run_transition_output(transition_t *tr, object_t *args_array,
                                            size_t ntapes, stack_frame_ptr sf) {
  if (tr->output) {
    object_t proc = interpret_expr(tr->output, sf);
    if (is_procedure(proc)) {
      object_t result = object_op_call(proc, ntapes, args_array, sf);
      destroy_object(result);
    }
    destroy_object(proc);
  }
}

static void apply_head_operations(size_t ntapes, listptr tapes, 
                                  head_op_t *head_operations, stack_frame_ptr sf) {
  for (size_t j = 0; j < ntapes; j++) {
    tape_t *tp = list_get(tapes, j);
    if (tp->head == 0) {
      /* do not allow staying on the left end symbol 
       * or replacing it with a different symbol */
      tp->head = 1;
      break;
    }

    head_op_t *head_op = &head_operations[j];
    switch (head_op->op) {
      case HEAD_OP_MOVE_LEFT:
        tp->head--;
        break;
      case HEAD_OP_MOVE_RIGHT:
        if (++tp->head >= list_size(tp->data)) {
          object_t *nullobj = malloc(sizeof(object_t));
          *nullobj = make_null();
          list_add(tp->data, nullobj);
        }
        break;
      case HEAD_OP_WRITE:
        {
          object_t *ptr = list_get(tp->data, tp->head);
          object_t write_value = interpret_expr(head_op->write_value, sf);
          assign_object(ptr, write_value);
        }
        break;
      case HEAD_NOP:
        break;
    }
  }
}

static object_t run_automatic_state(automaton_t *self, state_t **st, size_t *st_index) {
  *st_index += 1;

  if (*st_index >= self->number_of_states) {
    /* Halt if the current state was the last state */
    return make_integer(0);
  } 
  else 
  {
    /* Go to the next state */
    *st = &self->states[*st_index];
    return make_void();
  }
}

static object_t run_normal_state(automaton_t *self, listptr tapes, object_t *args,
                                 state_t **st, size_t *st_index, stack_frame_ptr sf) {
  /* Try all transitions until one transition condition is satisfied. */
  for (size_t i = 0; i < (*st)->number_of_transitions; i++) {
    transition_t *tr = &(*st)->transitions[i];
    object_t condition_result = run_transition_condition(tr, args, 
        self->number_of_tapes, sf);
    if (is_error(condition_result)) {
      return condition_result;
    }

    bool satisfied = boolean_value(condition_result);
    destroy_object(condition_result);
    if (satisfied) {
      apply_head_operations(self->number_of_tapes, tapes, tr->head_operations, sf);
      run_transition_output(tr, args, self->number_of_tapes, sf);

      switch(tr->action) {
        case ACT_HALT:
          return make_integer(0);
        case ACT_ACCEPT:
          return make_integer(1);
        case ACT_REJECT:
          return make_integer(-1);
        case ACT_CONTINUE:
          *st_index = tr->next_state_index;
          *st = &self->states[*st_index];
          return make_void();
      }
    }
  }

  /* The transition function must be defined on the entire tape alphabet. */
  return make_error("None of the transition conditions is satisfied.");    
}

static object_t run_state(automaton_t *self, listptr tapes, object_t *args, 
                          state_t **st, size_t *st_index, stack_frame_ptr sf) {
  if ((*st)->number_of_transitions == 0) {
    return run_automatic_state(self, st, st_index);
  } else {
    return run_normal_state(self, tapes, args, st, st_index, sf);
  }
}

static object_t construct_results(object_t exitcode, listptr tapes) {
  object_t tape_result = make_null();
  for (size_t i = 0; i < list_size(tapes); i++) {
    tape_t *tp = list_get(tapes, i);
    object_t tape_contents = internal_list_to_cons_list(tp->data);
    object_t head_position = make_integer((long)tp->head);
    object_t tape_obj = make_pair(head_position, tape_contents);
    destroy_object(tape_contents);
    destroy_object(head_position);
    assign_object(&tape_result, make_pair(tape_obj, tape_result));
    destroy_object(tape_obj);
  }

  object_t result = make_pair(exitcode, tape_result);
  destroy_object(tape_result);
  return result;
}

object_t automaton_run_internal(automaton_t *self, listptr tapes,
                                       stack_frame_ptr sf) {
  size_t state_index = 0;

  /* Loop until one transition leads to halting state */
  for (state_t *current_state = &self->states[state_index]; ;) {
    /* Print state of tapes */
    /** for (size_t i = 0; i < list_size(tapes); i++) {
      debug_print_tape(list_get(tapes, i));
    } */

    /* Run base machine of the current state */
    if (current_state->base_machine) {
      object_t exitcode = run_base_machine(current_state, tapes, sf);
      if (is_error(exitcode)) {
        return exitcode;
      }

      /* If the base machine has made a decision (acception or rejection)
       * instead of finishing normally, return that decision without continuing the
       * execution of the outer machine */
      assert(is_integer(exitcode));
      if (int_value(exitcode) != 0) {
        return exitcode;
      }
      destroy_object(exitcode);
    }

    /* Make an argument list from current symbols under tape heads */
    object_t *current_symbols = make_args_array(tapes);

    /* Evaluate output of the current state */
    run_state_output(current_state, current_symbols, self->number_of_tapes, sf);
    
    /* Apply transitions */
    object_t result = run_state(self, tapes, current_symbols, &current_state, &state_index, sf);
    free(current_symbols);

    /* Return error if an error has occured */
    if (is_error(result)) {
      return result;
    }

    /* Return normally if the machine has stopped normally */
    if (is_integer(result)) {
      return result;
    }

    destroy_object(result);
  } 

  return make_error("Internal error. ");
}

object_t automaton_run(automaton_t *self, size_t nargs, object_t *args, 
                       stack_frame_ptr sf) {
  /* Get tapes from arguments */
  listptr tapes = new_list();
  object_t err = get_tapes(tapes, self->number_of_tapes, nargs, args, sf);
  if (is_error(err)) {
    delete_tape_list(tapes);
    return err;
  }
  destroy_object(err);

  /* Run the machine */
  object_t exitcode = automaton_run_internal(self, tapes, sf);
  if (is_error(exitcode)) {
    delete_tape_list(tapes);
    return exitcode;
  }

  /* Return resulting tape contents, head position and exit code */
  assert(is_integer(exitcode));
  object_t result = construct_results(exitcode, tapes);
  destroy_object(exitcode);
  delete_tape_list(tapes);
  return result;
}
