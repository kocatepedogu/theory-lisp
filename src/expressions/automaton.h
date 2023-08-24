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

#ifndef THEORYLISP_EXPRESSIONS_AUTOMATON_H
#define THEORYLISP_EXPRESSIONS_AUTOMATON_H

#include "expression.h"
#include "../interpreter/interpreter.h"
#include "../scanner/scanner.h"
#include "../types/object.h"

#include <stdbool.h>

exprptr new_automaton_expr(size_t number_of_tapes, tokenptr tkn);

void destroy_automaton_expr(exprptr self);

char *automaton_expr_tostring(exprptr self);

exprptr automaton_expr_parse(tokenstreamptr tkns, stack_frame_ptr sf);

bool is_automaton_expr(exprptr e);

objectptr interpret_automaton(exprptr self, stack_frame_ptr sf);

objectptr call_automaton(exprptr self, size_t nargs, objectptr *args, 
                        stack_frame_ptr local_frame);

objectptr call_automaton_internal(exprptr self, void *, stack_frame_ptr sf);

size_t automaton_expr_get_arity(exprptr self);

size_t automaton_expr_get_pn_arity(exprptr self);

#endif
