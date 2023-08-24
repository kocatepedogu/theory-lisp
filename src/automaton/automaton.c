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
  for (size_t i = 0; i < arity; ++i) {
    destroy_head_operation(arity, &tr->head_operations[i]);
  }
  free(tr->head_operations);
}

static void destroy_state(size_t arity, state_t *st) {
  delete_expr(st->output);
  for (size_t i = 0; i < st->number_of_transitions; ++i) {
    destroy_transition(arity, st->transitions + i);
  }
  if (st->base_machine) {
    delete_expr(st->base_machine);
  }

  free(st->transitions);
  st->number_of_transitions = 0;
}

static void destroy_automaton(size_t arity, automaton_t *aut) {
  for (size_t i = 0; i < aut->number_of_states; ++i) {
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
  for (size_t i = 0; i < list_size(lst); ++i) {
    delete_object(list_get(lst, i));
  }

  delete_list(lst);
}

static void delete_tape_list(listptr lst) {
  for (size_t i = 0; i < list_size(lst); ++i) {
    tape_t *tp = list_get(lst, i);
    delete_object_list(tp->data);
    free(tp);
  }

  delete_list(lst);
}

static objectptr check_arguments(size_t arity, size_t nargs, objectptr *args) {
  /** Check that the number of tapes matches the number of tapes used by the automaton */
  if (nargs != arity) {
    return make_error("Automaton expected %ld tapes, but %ld given", arity, nargs);
  }

  /* Check that each argument is a cons pair whose first element 
   * is the head position (an integer) and second element is the tape 
   * contents (a list) */
  for (size_t i = 0; i < nargs; ++i) {
    objectptr tape_obj = args[i];
    if (!is_pair(tape_obj)) {
      return make_error(
          "Each tape must be a cons pair consisting of "
          "a head position (integer) and tape contents (a list)");
    }

    objectptr first = pair_first(tape_obj);
    if (!is_integer(first)) {
      return make_error("Head position is not an integer");
    }
    
    objectptr second = pair_second(tape_obj);
    if (!is_pair(second)) {
      return make_error("Tape contents is not a list");
    }
  }

  return make_void();
}

static objectptr get_tapes(listptr output, size_t arity, size_t nargs, objectptr *args) {
  /* Check arguments */
  objectptr error = check_arguments(arity, nargs, args);
  if (is_error(error)) {
    return error;
  }
  delete_object(error);

  /* Convert given tapes to a list of tape_t's */
  for (size_t i = 0; i < nargs; ++i) {
    objectptr tape_obj = args[i];
    objectptr position_obj = pair_first(tape_obj);
    objectptr contents_obj = pair_second(tape_obj);

    listptr contents = new_list();
    if (!cons_list_to_internal_list(contents_obj, contents)) {
      delete_object_list(contents);
      return make_error("Given tape is not in proper list form");
    }

    tape_t *tp = malloc(sizeof *tp);
    tp->head = int_value(position_obj);
    tp->data = contents;
    list_add(output, tp);
  }

  return make_void();
}

static objectptr *make_args_array(listptr tapes) {
  size_t ntapes = list_size(tapes);
  
  /* Return NULL if any of the tape heads is not in the bounds of its tape */
  for (size_t i = 0; i < ntapes; ++i) {
    tape_t *tp = list_get(tapes, i);
    if (tp->head >= list_size(tp->data)) {
      fprintf(stderr, "Assertion failure: Tape head indices are out of bounds.");
      abort();
    }
  }

  /* Make an array of current symbols under tapes heads */
  objectptr *args_array = malloc(ntapes * sizeof(objectptr));
  for (size_t i = 0; i < ntapes; ++i) {
    tape_t *tp = list_get(tapes, i);
    args_array[i] = list_get(tp->data, tp->head);
  }
  return args_array;
}

static objectptr run_base_machine(state_t *st, listptr tapes, stack_frame_ptr sf) {
  /* Interpret base machine into a procedure object */
  objectptr base_machine = interpret_expr(st->base_machine, sf); 

  if (is_error(base_machine)) {
    return base_machine;
  }

  if (!is_procedure(base_machine)) {
    char *base_machine_str = object_tostring(base_machine);
    objectptr err = make_error("Given base machine %s is not a procedure", base_machine_str);
    delete_object(base_machine);
    free(base_machine_str);
    return err;
  }

  if (procedure_get_arity(base_machine) != list_size(tapes)) {
    delete_object(base_machine);
    return make_error("Number of tapes is %d, but the base machine requires %d tapes",
                      list_size(tapes), procedure_get_arity(base_machine));
  }

  objectptr result = object_op_call_internal(base_machine, tapes, sf);
  delete_object(base_machine);
  return result;
}

static void run_state_output(state_t *st, objectptr *args_array,
                                       size_t ntapes, stack_frame_ptr sf) {
  if (st->output) {
    objectptr proc = interpret_expr(st->output, sf);
    if (is_procedure(proc)) {
      objectptr result = object_op_call(proc, ntapes, args_array, sf);
      delete_object(result);
    }
    delete_object(proc);
  }
}

static objectptr run_transition_condition(transition_t *tr, objectptr *args_array,
                                                   size_t ntapes, stack_frame_ptr sf) {
 
  objectptr proc = interpret_expr(tr->condition, sf);
  if (is_error(proc)) {
    return proc;
  }

  objectptr result = object_op_call(proc, ntapes, args_array, sf);
  delete_object(proc);
  if (is_boolean(result)) {
    return result;
  }

  char *objstr = object_tostring(result);
  assign_object(&result, make_error("Transition condition does not yield a boolean value. %s", objstr));
  free(objstr);
  return result;
}

static void run_transition_output(transition_t *tr, objectptr *args_array,
                                            size_t ntapes, stack_frame_ptr sf) {
  if (tr->output) {
    objectptr proc = interpret_expr(tr->output, sf);
    if (is_procedure(proc)) {
      objectptr result = object_op_call(proc, ntapes, args_array, sf);
      delete_object(result);
    }
    delete_object(proc);
  }
}

static void apply_head_operations(size_t ntapes, listptr tapes, 
                                  head_op_t *head_operations, stack_frame_ptr sf) {
  for (size_t j = 0; j < ntapes; ++j) {
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
        --tp->head;
        break;
      case HEAD_OP_MOVE_RIGHT:
        if (++tp->head >= list_size(tp->data)) {
          list_add(tp->data, make_null());
        }
        break;
      case HEAD_OP_WRITE:
        {
          objectptr new_value = interpret_expr(head_op->write_value, sf);
          objectptr old_value = list_set(tp->data, tp->head, new_value);
          delete_object(old_value);
        }
        break;
      case HEAD_NOP:
        break;
    }
  }
}

static objectptr run_automatic_state(automaton_t *self, state_t **st, size_t *st_index) {
  *st_index += 1;

  if (*st_index >= self->number_of_states) {
    /* Halt if the current state was the last state */
    return make_integer(0);
  } 
  else {
    /* Go to the next state */
    *st = &self->states[*st_index];
    return make_void();
  }
}

static objectptr run_normal_state(automaton_t *self, listptr tapes, objectptr *args,
                                 state_t **st, size_t *st_index, stack_frame_ptr sf) {
  /* Try all transitions until one transition condition is satisfied. */
  for (size_t i = 0; i < (*st)->number_of_transitions; ++i) {
    transition_t *tr = &(*st)->transitions[i];
    objectptr condition_result = run_transition_condition(tr, args, 
        self->number_of_tapes, sf);
    if (is_error(condition_result)) {
      return condition_result;
    }

    bool satisfied = boolean_value(condition_result);
    delete_object(condition_result);
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

static objectptr run_state(automaton_t *self, listptr tapes, objectptr *args, 
                          state_t **st, size_t *st_index, stack_frame_ptr sf) {
  if ((*st)->number_of_transitions == 0) {
    return run_automatic_state(self, st, st_index);
  } else {
    return run_normal_state(self, tapes, args, st, st_index, sf);
  }
}

static objectptr construct_results(objectptr exitcode, listptr tapes) {
  objectptr tape_result = make_null();
  for (size_t i = 0; i < list_size(tapes); ++i) {
    tape_t *tp = list_get(tapes, i);
    objectptr tape_contents = internal_list_to_cons_list(tp->data);
    objectptr head_position = make_integer((long)tp->head);
    objectptr tape_obj = make_pair(head_position, tape_contents);
    delete_object(tape_contents);
    delete_object(head_position);
    assign_object(&tape_result, make_pair(tape_obj, tape_result));
    delete_object(tape_obj);
  }

  objectptr result = make_pair(exitcode, tape_result);
  delete_object(tape_result);
  return result;
}

objectptr automaton_run_internal(automaton_t *self, listptr tapes,
                                       stack_frame_ptr sf) {
  size_t state_index = 0;

  /* Loop until one transition leads to halting state */
  for (state_t *current_state = &self->states[state_index]; ;) {
    /* Run base machine of the current state */
    if (current_state->base_machine) {
      objectptr exitcode = run_base_machine(current_state, tapes, sf);
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
      delete_object(exitcode);
    }

    /* Make an argument list from current symbols under tape heads */
    objectptr *current_symbols = make_args_array(tapes);

    /* Evaluate output of the current state */
    run_state_output(current_state, current_symbols, self->number_of_tapes, sf);
    
    /* Apply transitions */
    objectptr result = run_state(self, tapes, current_symbols, &current_state, &state_index, sf);
    free(current_symbols);

    /* Return error if an error has occured */
    if (is_error(result)) {
      return result;
    }

    /* Return normally if the machine has stopped normally */
    if (is_integer(result)) {
      return result;
    }

    delete_object(result);
  } 

  return make_error("Internal error. ");
}

objectptr automaton_run(automaton_t *self, size_t nargs, objectptr *args, 
                       stack_frame_ptr sf) {
  /* Get tapes from arguments */
  listptr tapes = new_list();
  objectptr err = get_tapes(tapes, self->number_of_tapes, nargs, args);
  if (is_error(err)) {
    delete_tape_list(tapes);
    return err;
  }
  delete_object(err);

  /* Run the machine */
  objectptr exitcode = automaton_run_internal(self, tapes, sf);
  if (is_error(exitcode)) {
    delete_tape_list(tapes);
    return exitcode;
  }

  /* Return resulting tape contents, head position and exit code */
  assert(is_integer(exitcode));
  objectptr result = construct_results(exitcode, tapes);
  delete_object(exitcode);
  delete_tape_list(tapes);
  return result;
}
