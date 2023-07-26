#include <check.h>
#include "../../src/scanner/scanner.h"
#include "../../src/expressions/definition_expr.h"
#include "../../src/expressions/data_expr.h"
#include "../../src/types/integer.h"
#include "parse.h"

START_TEST(test_define_parse_constant) {
  exprptr e = NULL;
  parse(e, "(define x 3)");

  ck_assert(is_definition_expr(e));
  definition_expr *def_expr = e->data;
  ck_assert_str_eq(def_expr->name, "x");
  assert_integer(def_expr->value, 3);

  delete_expr(e);
} END_TEST

START_TEST(test_define_parse_arbitary) {
  exprptr e = NULL;
  parse(e, "(define func (lambda (x y) (+ x y)))");

  ck_assert(is_definition_expr(e));
  definition_expr *def_expr = e->data;
  ck_assert(is_lambda_expr(def_expr->value));

  delete_expr(e);
} END_TEST

START_TEST(test_define_parse_error1) {
  assert_parse_error("(define)");
} END_TEST

START_TEST(test_define_parse_error2) {
  assert_parse_error("(define x)");
} END_TEST

START_TEST(test_define_parse_error3) {
  assert_parse_error("(define 3 x)");
} END_TEST

START_TEST(test_define_parse_error4) {
  assert_parse_error("(define (cons 2 3) 5)");
} END_TEST

START_TEST(test_define_parse_error5) {
  assert_parse_error("(define x 5");
} END_TEST

START_TEST(test_define_parse_error6) {
  assert_parse_error("define x 5)");
} END_TEST

START_TEST(test_define_parse_error7) {
  assert_parse_error("define x 5");
} END_TEST

START_TEST(test_define_parse_error8) {
  assert_parse_error("(define x (cons 2 3");
} END_TEST

Suite *define_suite(void) {
  TCase *tc_valid = tcase_create("Core");
  tcase_add_test(tc_valid, test_define_parse_constant);
  tcase_add_test(tc_valid, test_define_parse_arbitary);

  TCase *tc_invalid = tcase_create("Invalid");
  tcase_add_test(tc_invalid, test_define_parse_error1);
  tcase_add_test(tc_invalid, test_define_parse_error2);
  tcase_add_test(tc_invalid, test_define_parse_error3);
  tcase_add_test(tc_invalid, test_define_parse_error4);
  tcase_add_test(tc_invalid, test_define_parse_error5);
  tcase_add_test(tc_invalid, test_define_parse_error6);
  tcase_add_test(tc_invalid, test_define_parse_error7);
  tcase_add_test(tc_invalid, test_define_parse_error8);
  
  Suite *s = suite_create("Define");
  suite_add_tcase(s, tc_valid);
  suite_add_tcase(s, tc_invalid);
  return s;
}

int main(void) {
  Suite *s = define_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
