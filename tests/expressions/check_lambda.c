#include <check.h>
#include "../../src/expressions/lambda_expr.h"
#include "../../src/expressions/data_expr.h"
#include "../../src/expressions/identifier_expr.h"
#include "../../src/types/integer.h"
#include "parse.h"

START_TEST(test_lambda_empty_args) {
  exprptr e = NULL;
  parse(e, "(lambda () 10)");

  ck_assert(is_lambda_expr(e));
  lambda_expr *le = e->data;
  ck_assert_uint_eq(list_size(&le->params), 0);
  ck_assert_int_eq(le->variadic, false);
  assert_integer(le->body, 10);

  delete_expr(e);
} END_TEST

START_TEST(test_lambda) {
  exprptr e = NULL;
  parse(e, "(lambda (x y z) y)");

  ck_assert(is_lambda_expr(e));
  lambda_expr *le = e->data;
  ck_assert_uint_eq(list_size(&le->params), 3);
  ck_assert_int_eq(le->variadic, false);
  assert_identifier(le->body, "y");

  delete_expr(e);
} END_TEST

START_TEST(test_lambda_empty_args_variadic) {
  exprptr e = NULL;
  parse(e, "(lambda (...) va_args)");

  ck_assert(is_lambda_expr(e));
  lambda_expr *le = e->data;
  ck_assert_uint_eq(list_size(&le->params), 0);
  ck_assert_int_eq(le->variadic, true);
  assert_identifier(le->body, "va_args");

  delete_expr(e);
} END_TEST

START_TEST(test_lambda_variadic) {
  exprptr e = NULL;
  parse(e, "(lambda (x y ...) x)");

  ck_assert(is_lambda_expr(e));
  lambda_expr *le = e->data;
  ck_assert_uint_eq(list_size(&le->params), 2);
  ck_assert_int_eq(le->variadic, true);
  assert_identifier(le->body, "x");

  delete_expr(e);
} END_TEST

START_TEST(test_lambda_error1) {
  assert_parse_error("(lambda)");
} END_TEST

START_TEST(test_lambda_error2) {
  assert_parse_error("(lambda x)");
} END_TEST

START_TEST(test_lambda_error3) {
  assert_parse_error("(lambda x y)");
} END_TEST

START_TEST(test_lambda_error4) {
  assert_parse_error("(lambda ( x)");
} END_TEST

START_TEST(test_lambda_error5) {
  assert_parse_error("(lambda ) x)");
} END_TEST

START_TEST(test_lambda_error6) {
  assert_parse_error("(lambda (x) x");
} END_TEST

START_TEST(test_lambda_error7) {
  assert_parse_error("lambda (x) x)");
} END_TEST

START_TEST(test_lambda_error8) {
  assert_parse_error("lambda (x) x");
} END_TEST

START_TEST(test_lambda_error9) {
  assert_parse_error("(lambda (... x) va_args)");
} END_TEST

Suite *lambda_suite(void) {
  TCase *tc_valid = tcase_create("Valid");
  tcase_add_test(tc_valid, test_lambda_empty_args);
  tcase_add_test(tc_valid, test_lambda);
  tcase_add_test(tc_valid, test_lambda_empty_args_variadic);
  tcase_add_test(tc_valid, test_lambda_variadic);

  TCase *tc_invalid = tcase_create("Invalid");
  tcase_add_test(tc_invalid, test_lambda_error1);
  tcase_add_test(tc_invalid, test_lambda_error2);
  tcase_add_test(tc_invalid, test_lambda_error3);
  tcase_add_test(tc_invalid, test_lambda_error4);
  tcase_add_test(tc_invalid, test_lambda_error5);
  tcase_add_test(tc_invalid, test_lambda_error6);
  tcase_add_test(tc_invalid, test_lambda_error7);
  tcase_add_test(tc_invalid, test_lambda_error8);
  tcase_add_test(tc_invalid, test_lambda_error9);

  Suite *s = suite_create("Lambda");
  suite_add_tcase(s, tc_valid);
  suite_add_tcase(s, tc_invalid);
  return s;
}

int main(void) {
  Suite *s = lambda_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
