#include <check.h>
#include "../../src/expressions/if_expr.h"
#include "../../src/expressions/data_expr.h"
#include "../../src/types/real.h"
#include "../../src/types/integer.h"
#include "../../src/types/boolean.h"
#include "parse.h"

START_TEST(test_if) {
  exprptr e = NULL;
  parse(e, "(if #t 10 20.0)");
  
  ck_assert(is_if_expr(e));
  if_expr *ie = e->data;
  
  assert_boolean(ie->condition, true);  
  assert_integer(ie->true_case, 10);
  assert_double(ie->false_case, 20.0);

  delete_expr(e);
} END_TEST

START_TEST(test_if_arbitrary_cond) {
  exprptr e = NULL;
  parse(e, "(if (< x 0) x #f)");

  ck_assert(is_if_expr(e));
  if_expr *ie = e->data;

  ck_assert(is_evaluation_expr(ie->condition));
  assert_identifier(ie->true_case, "x");
  assert_boolean(ie->false_case, false);
     
  delete_expr(e);
} END_TEST

START_TEST(test_if_parse_error1) {
  assert_parse_error("(if)");
} END_TEST

START_TEST(test_if_parse_error2) {
  assert_parse_error("(if #t)");
} END_TEST

START_TEST(test_if_parse_error3) {
  assert_parse_error("(if #t 10)");
} END_TEST

START_TEST(test_if_parse_error4) {
  assert_parse_error("(if");
} END_TEST

START_TEST(test_if_parse_error5) {
  assert_parse_error("(if #t");
} END_TEST

START_TEST(test_if_parse_error6) {
  assert_parse_error("(if #t 10)");
} END_TEST

START_TEST(test_if_parse_error7) {
  assert_parse_error("(if #t 10 20");
} END_TEST

START_TEST(test_if_parse_error8) {
  assert_parse_error("if #t 10 20");
} END_TEST

Suite *if_suite(void) {
  TCase *tc_valid = tcase_create("Valid");
  tcase_add_test(tc_valid, test_if);
  tcase_add_test(tc_valid, test_if_arbitrary_cond);

  TCase *tc_invalid = tcase_create("Invalid");
  tcase_add_test(tc_invalid, test_if_parse_error1);
  tcase_add_test(tc_invalid, test_if_parse_error2);
  tcase_add_test(tc_invalid, test_if_parse_error3);
  tcase_add_test(tc_invalid, test_if_parse_error4);
  tcase_add_test(tc_invalid, test_if_parse_error5);
  tcase_add_test(tc_invalid, test_if_parse_error6);
  tcase_add_test(tc_invalid, test_if_parse_error7);
  tcase_add_test(tc_invalid, test_if_parse_error8);

  Suite *s = suite_create("If");
  suite_add_tcase(s, tc_valid);
  suite_add_tcase(s, tc_invalid);
  return s;
}

int main(void) {
  Suite *s = if_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
