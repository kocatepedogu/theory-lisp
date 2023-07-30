#ifndef PARSE_H_DEFINED
#define PARSE_H_DEFINED

#include "../../src/parser/parser.h"
#include "../../src/scanner/scanner.h"
#include "../../src/expressions/identifier_expr.h"
#include "../../src/expressions/data_expr.h"
#include "../../src/types/boolean.h"
#include "../../src/types/integer.h"
#include "../../src/types/real.h"

#define parse(e, input) \
  do { \
    listptr _tokens = scanner(input); \
    listptr _parse_tree = parser(_tokens); \
    if (_parse_tree == NULL) { \
      (e) = NULL; \
    } else { \
      ck_assert_uint_eq(list_size(_parse_tree), 1); \
      (e) = list_get(_parse_tree, 0); \
    } \
  } while(false)

#define assert_parse_error(input) \
  do { \
    exprptr _e = NULL; \
    parse(_e, input); \
    ck_assert(_e == NULL); \
  } while(false)

#define assert_boolean(expr, value) \
  do { \
    ck_assert(is_data_expr(expr)); \
    ck_assert(is_boolean(get_data_value(expr))); \
    ck_assert_int_eq(boolean_value(get_data_value(expr)), value); \
  } while(false)

#define assert_integer(expr, value) \
  do { \
    ck_assert(is_data_expr(expr)); \
    ck_assert(is_integer(get_data_value(expr))); \
    ck_assert_int_eq(int_value(get_data_value(expr)), value); \
  } while(false)

#define assert_double(expr, value) \
  do { \
    ck_assert(is_data_expr(expr)); \
    ck_assert(is_real(get_data_value(expr))); \
    ck_assert_double_eq(real_value(get_data_value(expr)), value); \
  } while(false)

#define assert_identifier(expr, value) \
  do { \
    ck_assert(is_identifier_expr(expr)); \
    ck_assert_str_eq(identifier_expr_get_name(expr), value); \
  } while(false)

#endif
