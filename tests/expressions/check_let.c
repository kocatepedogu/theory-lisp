#include <check.h>
#include "../../src/expressions/let.h"
#include "../../src/expressions/data.h"
#include "../../src/expressions/identifier.h"
#include "../../src/expressions/expression_base.h"
#include "../../src/types/integer.h"
#include "parse.h"

typedef struct {
  listptr declarations;
  exprptr body;
} let_expr;

START_TEST(test_empty_let_expr) {
  exprptr e = NULL;
  parse(e, "(let () 0)");

  ck_assert(is_let_expr(e));
  let_expr *le = e->data;
  ck_assert_uint_eq(list_size(le->declarations), 0);
  assert_integer(le->body, 0); 

  delete_expr(e);

} END_TEST

START_TEST(test_let_expr) {
  exprptr e = NULL;
  parse(e, "(let ((x 3) (y 4)) x)");

  ck_assert(is_let_expr(e));
  let_expr *le = e->data;
  ck_assert_uint_eq(list_size(le->declarations), 2);
  assert_identifier(le->body, "x");
  
  delete_expr(e);

} END_TEST

START_TEST(test_let_error1) {
  assert_parse_error("(let)");
} END_TEST

START_TEST(test_let_error2) {
  assert_parse_error("(let 0)");
} END_TEST

START_TEST(test_let_error3) {
  assert_parse_error("(let 0 x)");
} END_TEST

START_TEST(test_let_error4) {
  assert_parse_error("(let (cons 2 3))");
} END_TEST

START_TEST(test_let_error5) {
  assert_parse_error("(let (cons 2 3) x)");
} END_TEST

START_TEST(test_let_error6) {
  assert_parse_error("(let (let))");
} END_TEST

START_TEST(test_let_error7) {
  assert_parse_error("(let (let) x)");
} END_TEST

START_TEST(test_let_error8) {
  assert_parse_error("(let ( x)");
} END_TEST

START_TEST(test_let_error9) {
  assert_parse_error("(let ) x)");
} END_TEST

START_TEST(test_let_error10) {
  assert_parse_error("(let ((x 2)))");
} END_TEST

START_TEST(test_let_error11) {
  assert_parse_error("(let ((x 2) 3)");
} END_TEST

START_TEST(test_let_error12) {
  assert_parse_error("(let (((x 3)) (y 4)) 10)");
} END_TEST

Suite *let_suite(void) {
  TCase *tc_valid = tcase_create("Valid");
  tcase_add_test(tc_valid, test_empty_let_expr);
  tcase_add_test(tc_valid, test_let_expr);

  TCase *tc_invalid = tcase_create("Invalid");
  tcase_add_test(tc_invalid, test_let_error1);
  tcase_add_test(tc_invalid, test_let_error2);
  tcase_add_test(tc_invalid, test_let_error3);
  tcase_add_test(tc_invalid, test_let_error4);
  tcase_add_test(tc_invalid, test_let_error5);
  tcase_add_test(tc_invalid, test_let_error6);
  tcase_add_test(tc_invalid, test_let_error7);
  tcase_add_test(tc_invalid, test_let_error8);
  tcase_add_test(tc_invalid, test_let_error9);
  tcase_add_test(tc_invalid, test_let_error10);
  tcase_add_test(tc_invalid, test_let_error11);
  tcase_add_test(tc_invalid, test_let_error12);
  
  Suite *s = suite_create("Let");
  suite_add_tcase(s, tc_valid);
  suite_add_tcase(s, tc_invalid);
  return s;
}

int main(void) {
  Suite *s = let_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
