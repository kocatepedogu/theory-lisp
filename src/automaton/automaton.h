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

/// @file automaton.h

#ifndef THEORYLISP_AUTOMATON_AUTOMATON_H
#define THEORYLISP_AUTOMATON_AUTOMATON_H

#include "../expressions/expression.h"
#include "../types/object.h"
#include "../interpreter/stack_frame.h"

typedef enum head_operation_type {
  HEAD_OP_MOVE_LEFT,
  HEAD_OP_MOVE_RIGHT,
  HEAD_OP_WRITE,
  HEAD_NOP
} head_op_type_t;

typedef struct head_operation {
  head_op_type_t op;
  exprptr write_value;
} head_op_t;

typedef enum next_action {
  ACT_HALT,
  ACT_ACCEPT,
  ACT_REJECT,
  ACT_CONTINUE
} next_action_t;

typedef struct transition {
  exprptr condition;
  head_op_t *head_operations;
  exprptr output;
  size_t next_state_index;
  next_action_t action;
} transition_t;

typedef struct state {
  exprptr output;
  exprptr base_machine;
  struct transition *transitions;
  size_t number_of_transitions;
} state_t;

typedef struct automaton {
  state_t *states;
  size_t number_of_states;
  size_t number_of_tapes;
} automaton_t;

objectptr automaton_run(automaton_t *self, size_t nargs, 
                       objectptr *args, stack_frame_ptr sf);

objectptr automaton_run_internal(automaton_t *self, listptr args,
                                stack_frame_ptr sf);

void delete_automaton(automaton_t *aut);

#endif
