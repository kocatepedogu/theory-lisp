TESTS = \
    check_util_list \
    check_util_stack \
    check_util_string \
    check_type_void \
    check_type_boolean \
    check_type_error \
    check_type_integer \
    check_type_pair \
    check_type_procedure \
    check_type_real \
    check_type_string \
    check_type_types \
    check_scanner_scanner \
    check_interpreter_stack_frame \
    check_expr_define \
    check_expr_if \
    check_expr_lambda \
    check_expr_let \
    check_expr_evaluation \
    check_expr_cond

check_PROGRAMS = $(TESTS)

AM_CFLAGS = @CHECK_CFLAGS@
LDADD = $(top_builddir)/src/libtlisp.la @CHECK_LIBS@ -lm

SRC_DIR = $(top_builddir)/src

# Utils Tests

UTIL_DIR = $(SRC_DIR)/utils

check_util_list_SOURCES = \
    utils/check_list.c \
    $(UTIL_DIR)/list.h

check_util_stack_SOURCES = \
    utils/check_stack.c \
    $(UTIL_DIR)/stack.h

check_util_string_SOURCES = \
    utils/check_string.c \
    $(UTIL_DIR)/string.h

# Types Tests

TYPES_DIR = $(SRC_DIR)/types

check_type_void_SOURCES = \
    types/check_void.c \
    $(TYPES_DIR)/void.h

check_type_boolean_SOURCES = \
    types/check_boolean.c \
    $(TYPES_DIR)/boolean.h

check_type_error_SOURCES = \
    types/check_error.c \
    $(TYPES_DIR)/error.h

check_type_integer_SOURCES = \
    types/check_integer.c \
    $(TYPES_DIR)/integer.h \
    $(TYPES_DIR)/real.h \
    $(TYPES_DIR)/boolean.h

check_type_pair_SOURCES = \
    types/check_pair.c \
    $(TYPES_DIR)/pair.h \
    $(TYPES_DIR)/integer.h \
    $(TYPES_DIR)/real.h

check_type_procedure_SOURCES = \
    types/check_procedure.c \
    $(TYPES_DIR)/procedure.h

check_type_real_SOURCES = \
    types/check_real.c \
    $(TYPES_DIR)/real.h \
    $(TYPES_DIR)/integer.h \
    $(TYPES_DIR)/boolean.h

check_type_string_SOURCES = \
    types/check_string.c \
    $(TYPES_DIR)/string.h

check_type_types_SOURCES = \
    types/check_types.c \
    $(TYPES_DIR)/object.h \
    $(TYPES_DIR)/void.h \
    $(TYPES_DIR)/boolean.h \
    $(TYPES_DIR)/error.h \
    $(TYPES_DIR)/integer.h \
    $(TYPES_DIR)/real.h \
    $(TYPES_DIR)/procedure.h \
    $(TYPES_DIR)/pair.h \
    $(TYPES_DIR)/string.h

# Scanner Tests

check_scanner_scanner_SOURCES = \
    scanner/check_scanner.c \
    $(SRC_DIR)/scanner/scanner.h

# Interpreter Tests

INTERPRETER_DIR = $(SRC_DIR)/interpreter

check_interpreter_stack_frame_SOURCES = \
    interpreter/check_stack_frame.c \
    $(INTERPRETER_DIR)/stack_frame.h \
    $(INTERPRETER_DIR)/variable.h \
    $(TYPES_DIR)/object.h \
    $(UTIL_DIR)/list.h

# Expression Tests

EXPR_DIR = $(SRC_DIR)/expressions

check_expr_define_SOURCES = \
    expressions/check_define.c \
    expressions/parse.h \
    $(EXPR_DIR)/definition.h

check_expr_if_SOURCES = \
    expressions/check_if.c \
    expressions/parse.h \
    $(EXPR_DIR)/if.h

check_expr_lambda_SOURCES = \
    expressions/check_lambda.c \
    expressions/parse.h \
    $(EXPR_DIR)/lambda.h

check_expr_let_SOURCES = \
    expressions/check_let.c \
    expressions/parse.h \
    $(EXPR_DIR)/let.h

check_expr_evaluation_SOURCES = \
    expressions/check_evaluation.c \
    expressions/parse.h \
    $(EXPR_DIR)/evaluation.h

check_expr_cond_SOURCES = \
    expressions/check_cond.c \
    expressions/parse.h \
    $(EXPR_DIR)/cond.h
