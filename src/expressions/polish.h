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

#ifndef THEORYLISP_EXPRESSIONS_POLISH_H
#define THEORYLISP_EXPRESSIONS_POLISH_H

#include "expression.h"
#include "../utils/list.h"
#include "../scanner/scanner.h"

/* PN expression "new" operation */
exprptr new_pn_expr(void);

/* PN expression "delete" operation */
void delete_pn_expr(exprptr self);

/* PN expression clone */
exprptr clone_pn_expr(exprptr self);

/* PN expression tostring implementation */
char *pn_expr_tostring(exprptr self);

/* Adds argument to a PN expression */
void pn_expr_add_body_expr(exprptr self, exprptr body_expr);

/* PN expression parser */
exprptr pn_expr_parse(tokenstreamptr tkns);

/* true if e is PN expression */
bool is_pn_expr(exprptr e);

/* PN expression interpreter */
object_t interpret_pn_expr(exprptr self, stack_frame_ptr sf);

/* Calls PN expression */
object_t pn_expr_call(exprptr self, size_t nargs,
                      object_t *args, stack_frame_ptr sf); 

#endif
