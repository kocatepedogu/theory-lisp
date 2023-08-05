#include <check.h>
#include <stdlib.h>
#include "parse.h"
#include "../../src/expressions/expression_base.h"
#include "../../src/expressions/cond.h"
#include "../../src/expressions/evaluation.h"

typedef struct {
  exprptr condition;
  exprptr true_case;
} cond_case;

typedef struct {
  listptr cases;
} cond_expr;

START_TEST(test_empty) {
  exprptr e = NULL;
  parse(e, "(cond)");

  ck_assert(is_cond_expr(e));
  cond_expr *ce = e->data;
  ck_assert_uint_eq(list_size(ce->cases), 0);

  delete_expr(e);
} END_TEST

START_TEST(test_constant_cond_constant_expr) {
  exprptr e = NULL;
  parse(e, "(cond (#t 10) (#f 20))");

  ck_assert(is_cond_expr(e));
  cond_expr *ce = e->data;
  ck_assert_uint_eq(list_size(ce->cases), 2);

  cond_case *cc1 = list_get(ce->cases, 0);
  assert_boolean(cc1->condition, true);
  assert_integer(cc1->true_case, 10);

  cond_case *cc2 = list_get(ce->cases, 1);
  assert_boolean(cc2->condition, false);
  assert_integer(cc2->true_case, 20);

  delete_expr(e);
} END_TEST

START_TEST(test_constant_cond_arbitrary_expr) {
  exprptr e = NULL;
  parse(e, "(cond (#f (+ 5 10)) (#t (let ((x 3)) (+ x 2))))");

  ck_assert(is_cond_expr(e));
  cond_expr *ce = e->data;
  ck_assert_uint_eq(list_size(ce->cases), 2);

  cond_case *cc1 = list_get(ce->cases, 0);
  assert_boolean(cc1->condition, false);
  ck_assert(is_evaluation_expr(cc1->true_case));
  
  cond_case *cc2 = list_get(ce->cases, 1);
  assert_boolean(cc2->condition, true);
  ck_assert(is_let_expr(cc2->true_case));

  delete_expr(e);
} END_TEST

START_TEST(test_arbitrary_cond_constant_expr) {
  exprptr e = NULL;
  parse(e, "(cond ((< x 0) 10) ((= x 0) 15) ((> x 0) 20))");

  ck_assert(is_cond_expr(e));
  cond_expr *ce = e->data;
  ck_assert_uint_eq(list_size(ce->cases), 3);

  cond_case *cc1 = list_get(ce->cases, 0);
  assert_integer(cc1->true_case, 10);
  ck_assert(is_evaluation_expr(cc1->condition));

  cond_case *cc2 = list_get(ce->cases, 1);
  assert_integer(cc2->true_case, 15);
  ck_assert(is_evaluation_expr(cc2->condition));

  cond_case *cc3 = list_get(ce->cases, 2);
  assert_integer(cc3->true_case, 20);
  ck_assert(is_evaluation_expr(cc3->condition));

  delete_expr(e);
} END_TEST

START_TEST(test_arbitrary_cond_arbitrary_expr) {
  exprptr e = NULL;
  parse(e, "(cond ((< x 0) (* 2 x))"
           "      ((let ((a 1) (b 2)) (< x (+ a b))) (* 3 x))"
           "      (bool_variable (* 4 x)))");

  ck_assert(is_cond_expr(e));
  cond_expr *ce = e->data;
  ck_assert_uint_eq(list_size(ce->cases), 3);

  cond_case *cc1 = list_get(ce->cases, 0);
  ck_assert(is_evaluation_expr(cc1->condition));
  ck_assert(is_evaluation_expr(cc1->true_case));

  cond_case *cc2 = list_get(ce->cases, 1);
  ck_assert(is_let_expr(cc2->condition));
  ck_assert(is_evaluation_expr(cc2->true_case));

  cond_case *cc3 = list_get(ce->cases, 2);
  assert_identifier(cc3->condition, "bool_variable");
  ck_assert(is_evaluation_expr(cc3->true_case));

  delete_expr(e);
} END_TEST

START_TEST(test_parse_error1) {
  assert_parse_error("(cond");
} END_TEST

START_TEST(test_parse_error2) {
  assert_parse_error("cond)");
} END_TEST

START_TEST(test_parse_error3) {
  assert_parse_error("(cond 0)");
} END_TEST

START_TEST(test_parse_error4) {
  assert_parse_error("(cond (0))");
} END_TEST

START_TEST(test_parse_error5) {
  assert_parse_error("(cond (1 2 3))");
} END_TEST

START_TEST(test_parse_error6) {
  assert_parse_error("(cond (0)");
} END_TEST

START_TEST(test_parse_error7) {
  assert_parse_error("(cond 0))");
} END_TEST

START_TEST(test_parse_error8) {
  assert_parse_error("(cond (1 2 3)");
} END_TEST

START_TEST(test_parse_error9) {
  assert_parse_error("(cond 1 2 3))");
} END_TEST

Suite *evaluation_suite(void) {
  TCase *tc_valid = tcase_create("Valid");
  tcase_add_test(tc_valid, test_empty);
  tcase_add_test(tc_valid, test_constant_cond_constant_expr);
  tcase_add_test(tc_valid, test_constant_cond_arbitrary_expr);
  tcase_add_test(tc_valid, test_arbitrary_cond_constant_expr);
  tcase_add_test(tc_valid, test_arbitrary_cond_arbitrary_expr);

  TCase *tc_invalid = tcase_create("Invalid");
  tcase_add_test(tc_invalid, test_parse_error1);
  tcase_add_test(tc_invalid, test_parse_error2);
  tcase_add_test(tc_invalid, test_parse_error3);
  tcase_add_test(tc_invalid, test_parse_error4);
  tcase_add_test(tc_invalid, test_parse_error5);
  tcase_add_test(tc_invalid, test_parse_error6);
  tcase_add_test(tc_invalid, test_parse_error7);
  tcase_add_test(tc_invalid, test_parse_error8);
  tcase_add_test(tc_invalid, test_parse_error9);

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
