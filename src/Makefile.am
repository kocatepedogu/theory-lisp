lib_LTLIBRARIES = libtlisp.la
libtlisp_la_SOURCES =\
    types/types.c\
    types/types.h\
    types/boolean.c\
    types/boolean.h\
    types/integer.c\
    types/integer.h\
    types/real.c\
    types/real.h\
    types/symbol.c\
    types/symbol.h\
    types/pair.c\
    types/pair.h\
    types/procedure.c\
    types/procedure.h\
    types/void.c\
    types/void.h\
    types/error.c\
    types/error.h\
    utils/heap-format.c\
    utils/heap-format.h\
    utils/list.c\
    utils/list.h\
    scanner/scanner.c\
    scanner/scanner.h\
    expressions/expression.c\
    expressions/expression.h\
    expressions/if_expr.c\
    expressions/if_expr.h\
    expressions/cond_expr.c\
    expressions/cond_expr.h\
    expressions/evaluation_expr.c\
    expressions/evaluation_expr.h\
    expressions/definition_expr.c\
    expressions/definition_expr.h\
    expressions/let_expr.c\
    expressions/let_expr.h\
    expressions/lambda_expr.c\
    expressions/lambda_expr.h\
    expressions/data_expr.c\
    expressions/data_expr.h\
    expressions/identifier_expr.c\
    expressions/identifier_expr.h\
    expressions/expanded_expr.c\
    expressions/expanded_expr.h\
    parser/parser.c\
    parser/parser.h\
    interpreter/variable.c\
    interpreter/variable.h\
    interpreter/stack_frame.c\
    interpreter/stack_frame.h\
    interpreter/interpreter.c\
    interpreter/interpreter.h\
    builtin/builtin.c\
    builtin/builtin.h\
    builtin/object.c\
    builtin/object.h\
    builtin/display.c\
    builtin/display.h\
    builtin/boolean.c\
    builtin/boolean.h\
    builtin/arithmetic.c\
    builtin/arithmetic.h\
    builtin/list.c \
    builtin/list.h

libtlisp_la_CFLAGS = -O3 -g -Wall -Werror -pedantic -fsanitize=undefined

bin_PROGRAMS = tlisp
tlisp_SOURCES = main.c
tlisp_LDADD = libtlisp.la
tlisp_CFLAGS = -O3 -g -Wall -Werror -pedantic -fsanitize=undefined
