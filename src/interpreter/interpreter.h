/*
 *
 * Copyright 2023 Doğu Kocatepe
 * This file is part of Theory Lisp.

 * Theory Lisp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Theory Lisp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along with Theory Lisp.
 * If not, see <https://www.gnu.org/licenses/>.
 */

/// @file interpreter.h

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../utils/list.h"
#include "../expressions/expression.h"
#include "../types/object.h"
#include "stack_frame.h"

/**
 * Takes a list of expressions and interprets them one by one.
 * If any one of the expressions returns an error, the execution is
 * terminated.
 *
 * @param verbose if verbose is set to true, each expression will
 * be printed before getting evaluated, and global variables will
 * be displayed after evaluating each expression.
 */
object_t interpreter(listptr parse_tree, bool verbose, bool show, stack_frame_ptr sf);

/**
 * Read-Evaluate-Print-Loop
 */
void repl(stack_frame_ptr sf);

#endif
