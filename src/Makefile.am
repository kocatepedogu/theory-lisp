lib_LTLIBRARIES = libtlisp.la
libtlisp_la_SOURCES =\
    types/object.c\
    types/object.h\
    types/object-base.h\
    types/boolean.c\
    types/boolean.h\
    types/integer.c\
    types/integer.h\
    types/real.c\
    types/real.h\
    types/string.c\
    types/string.h\
    types/pair.c\
    types/pair.h\
    types/procedure.c\
    types/procedure.h\
    types/void.c\
    types/void.h\
    types/null.c\
    types/null.h\
    types/error.c\
    types/error.h\
    utils/init.c\
    utils/init.h\
    utils/string.c\
    utils/string.h\
    utils/list.c\
    utils/list.h\
    utils/stack.c\
    utils/stack.h\
    utils/file.c\
    utils/file.h\
    utils/hashtable.c\
    utils/hashtable.h\
    scanner/scanner.c\
    scanner/scanner.h\
    expressions/expression.c\
    expressions/expression.h\
    expressions/expression_base.h\
    expressions/if.c\
    expressions/if.h\
    expressions/cond.c\
    expressions/cond.h\
    expressions/evaluation.c\
    expressions/evaluation.h\
    expressions/definition.c\
    expressions/definition.h\
    expressions/set.c\
    expressions/set.h\
    expressions/let.c\
    expressions/let.h\
    expressions/lambda.c\
    expressions/lambda.h\
    expressions/data.c\
    expressions/data.h\
    expressions/identifier.c\
    expressions/identifier.h\
    expressions/expanded.c\
    expressions/expanded.h\
    expressions/polish.c\
    expressions/polish.h\
    expressions/automaton.c\
    expressions/automaton.h\
    expressions/common.c\
    expressions/common.h\
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
    builtin/include.c\
    builtin/include.h\
    builtin/object.c\
    builtin/object.h\
    builtin/display.c\
    builtin/display.h\
    builtin/boolean.c\
    builtin/boolean.h\
    builtin/arithmetic.c\
    builtin/arithmetic.h\
    builtin/list.c \
    builtin/list.h \
    builtin/string.c \
    builtin/string.h \
    builtin/eval.c \
    builtin/eval.h \
    builtin/error.c \
    builtin/error.h \
    automaton/automaton.c \
    automaton/automaton.h

tlispdir = $(libdir)/tlisp
dist_tlisp_DATA =\
    lib/list.tl\
    lib/util.tl\
    lib/primitives.tl\
    lib/string.tl

libtlisp_la_CFLAGS = -DLIBRARY_DIR='"$(tlispdir)"'

bin_PROGRAMS = tlisp
tlisp_SOURCES = main.c
tlisp_LDADD = libtlisp.la
