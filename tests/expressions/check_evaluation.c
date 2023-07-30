#include <check.h>
#include <stdlib.h>
#include "../../src/expressions/evaluation_expr.h"
#include "../../src/expressions/let_expr.h"
#include "../../src/expressions/lambda_expr.h"
#include "parse.h"

typedef struct {
  exprptr procexpr;
  listptr arguments;
} evaluation_expr;

START_TEST(test_empty_call_using_name) {
  exprptr e = NULL;
  parse(e, "(func)");
  
  ck_assert(is_evaluation_expr(e));
  evaluation_expr *ee = e->data;
  assert_identifier(ee->procexpr, "func");
  ck_assert_uint_eq(list_size(ee->arguments), 0);

  delete_expr(e);
} END_TEST

START_TEST(test_call_using_name) {
  exprptr e = NULL;
  parse(e, "(+ 1 2 3)");

  ck_assert(is_evaluation_expr(e));
  evaluation_expr *ee = e->data;
  assert_identifier(ee->procexpr, "+");
  ck_assert_uint_eq(list_size(ee->arguments), 3);

  delete_expr(e);
} END_TEST

START_TEST(test_call_using_name_complex_params) {
  exprptr e = NULL;
  parse(e, "(op (func 4 5 6 7) (lambda (x y) (+ x y)))");

  ck_assert(is_evaluation_expr(e));
  evaluation_expr *ee = e->data;
  assert_identifier(ee->procexpr, "op");
  ck_assert_uint_eq(list_size(ee->arguments), 2);

  delete_expr(e);
} END_TEST

START_TEST(test_empty_call_using_expr) {
  exprptr e = NULL;
  parse(e, "((let ((func (lambda (x y) (+ x y)))) func))");

  ck_assert(is_evaluation_expr(e));
  evaluation_expr *ee = e->data;
  ck_assert(is_let_expr(ee->procexpr));
  ck_assert_uint_eq(list_size(ee->arguments), 0);

  delete_expr(e);
} END_TEST

START_TEST(test_call_using_expr) {
  exprptr e = NULL;
  parse(e, "((lambda (...) (* &va_args)) 1 2 3 4 5)");

  ck_assert(is_evaluation_expr(e));
  evaluation_expr *ee = e->data;
  ck_assert(is_lambda_expr(ee->procexpr));
  ck_assert_uint_eq(list_size(ee->arguments), 5);

  delete_expr(e);
} END_TEST

START_TEST(test_call_using_expr_complex_params) {
  exprptr e = NULL;
  parse(e, "((lambda (x ...) (* x (+ &va_args)))"
           "  10"
	   "  (let ((a 3) (b 4)) (- a b))"
	   "  (let ((a 5) (b 7)) (- a b))"
	   ")");

  ck_assert(is_evaluation_expr(e));
  evaluation_expr *ee = e->data;
  ck_assert(is_lambda_expr(ee->procexpr));
  ck_assert_uint_eq(list_size(ee->arguments), 3);

  delete_expr(e);
} END_TEST

START_TEST(test_parse_error1) {
  assert_parse_error("(3)");
} END_TEST

START_TEST(test_parse_error2) {
  assert_parse_error("(3 4 5)");
} END_TEST

START_TEST(test_parse_error3) {
  assert_parse_error("(a");
} END_TEST

START_TEST(test_parse_error4) {
  assert_parse_error("a)");
} END_TEST

START_TEST(test_parse_error5) {
  assert_parse_error("(a 1 2 3");
} END_TEST

START_TEST(test_parse_error6) {
  assert_parse_error("a 1 2 3)");
} END_TEST

Suite *evaluation_suite(void) {
  TCase *tc_valid = tcase_create("Valid");
  tcase_add_test(tc_valid, test_empty_call_using_name);
  tcase_add_test(tc_valid, test_call_using_name);
  tcase_add_test(tc_valid, test_call_using_name_complex_params);
  tcase_add_test(tc_valid, test_empty_call_using_expr);
  tcase_add_test(tc_valid, test_call_using_expr);
  tcase_add_test(tc_valid, test_call_using_expr_complex_params);

  TCase *tc_invalid = tcase_create("Invalid");
  tcase_add_test(tc_invalid, test_parse_error1);
  tcase_add_test(tc_invalid, test_parse_error2);
  tcase_add_test(tc_invalid, test_parse_error3);
  tcase_add_test(tc_invalid, test_parse_error4);
  tcase_add_test(tc_invalid, test_parse_error5);
  tcase_add_test(tc_invalid, test_parse_error6);

  Suite *s = suite_create("Evaluation");
  suite_add_tcase(s, tc_valid);
  suite_add_tcase(s, tc_invalid);
  return s;
}

int main(void) {
  Suite *s = evaluation_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
