TESTS = check_list \
        check_stack \
        check_format \
	check_void \
	check_boolean \
	check_error \
	check_integer \
	check_pair \
	check_procedure \
	check_real \
	check_string \
	check_types \
	check_scanner \
	check_stack_frame \
	check_define \
	check_if \
	check_lambda \
	check_let \
	check_evaluation \
	check_cond

check_PROGRAMS = $(TESTS)

LDADD = $(top_builddir)/src/libtlisp.la @CHECK_LIBS@

SRC_DIR = $(top_builddir)/src

# Utils Tests

UTIL_DIR = $(SRC_DIR)/utils

check_list_SOURCES = utils/check_list.c \
                     $(UTIL_DIR)/list.h

check_stack_SOURCES = utils/check_stack.c \
                      $(UTIL_DIR)/stack.h

check_format_SOURCES = utils/check_format.c \
                       $(UTIL_DIR)/heap-format.h

# Types Tests

TYPES_DIR = $(SRC_DIR)/types

check_void_SOURCES = types/check_void.c \
                     $(TYPES_DIR)/void.h

check_boolean_SOURCES = types/check_boolean.c \
                        $(TYPES_DIR)/boolean.h

check_error_SOURCES = types/check_error.c \
                      $(TYPES_DIR)/error.h

check_integer_SOURCES = types/check_integer.c \
                        $(TYPES_DIR)/integer.h \
                        $(TYPES_DIR)/real.h \
                        $(TYPES_DIR)/boolean.h

check_pair_SOURCES = types/check_pair.c \
                     $(TYPES_DIR)/pair.h \
                     $(TYPES_DIR)/integer.h \
                     $(TYPES_DIR)/real.h

check_procedure_SOURCES = types/check_procedure.c \
                          $(TYPES_DIR)/procedure.h

check_real_SOURCES = types/check_real.c \
                     $(TYPES_DIR)/real.h \
                     $(TYPES_DIR)/integer.h \
                     $(TYPES_DIR)/boolean.h

check_string_SOURCES = types/check_string.c \
                       $(TYPES_DIR)/string.h

check_types_SOURCES = types/check_types.c \
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

check_scanner_SOURCES = scanner/check_scanner.c \
                        $(SRC_DIR)/scanner/scanner.h

# Interpreter Tests

INTERPRETER_DIR = $(SRC_DIR)/interpreter

check_stack_frame_SOURCES = interpreter/check_stack_frame.c \
                            $(INTERPRETER_DIR)/stack_frame.h \
                            $(INTERPRETER_DIR)/variable.h \
                            $(TYPES_DIR)/object.h \
                            $(UTILS_DIR)/list.h

# Expression Tests

EXPR_DIR = $(SRC_DIR)/expressions

check_define_SOURCES = expressions/check_define.c \
                       expressions/parse.h \
                       $(EXPR_DIR)/definition_expr.h

check_if_SOURCES = expressions/check_if.c \
                   expressions/parse.h \
                   $(EXPR_DIR)/if_expr.h

check_lambda_SOURCES = expressions/check_lambda.c \
                       expressions/parse.h \
                       $(EXPR_DIR)/lambda_expr.h

check_let_SOURCES = expressions/check_let.c \
                    expressions/parse.h \
                    $(EXPR_DIR)/let_expr.h

check_evaluation_SOURCES = expressions/check_evaluation.c \
                           expressions/parse.h \
                           $(EXPR_DIR)/evaluation_expr.h

check_cond_SOURCES = expressions/check_cond.c \
                     expressions/parse.h \
                     $(EXPR_DIR)/cond_expr.h
