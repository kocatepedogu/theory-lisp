#ifndef PN_EXPR_H
#define PN_EXPR_H

#include "expression.h"
#include "../utils/list.h"

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
exprptr pn_expr_parse(listptr tokens, int *index);

/* true if e is PN expression */
bool is_pn_expr(exprptr e);

/* PN expression interpreter */
object_t interpret_pn_expr(exprptr self, stack_frame_ptr sf);

#endif
