#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../src/types/real.h"
#include "../../src/types/integer.h"
#include "../../src/types/boolean.h"

#define REAL_TEST_VALUE 123.456

START_TEST(test_real_common) {
  objectptr real_obj = make_real(REAL_TEST_VALUE);
  ck_assert(is_real(real_obj));
  ck_assert(real_equals(real_obj, real_obj));
  char *real_str = real_tostring(real_obj);
  real_t value1 = 0;
  sscanf(real_str, "%lf", &value1);
  ck_assert_double_eq(value1, REAL_TEST_VALUE);

  objectptr copy_obj = clone_object(real_obj);
  ck_assert(is_real(copy_obj));
  ck_assert(real_equals(copy_obj, copy_obj));
  char *copy_str = real_tostring(copy_obj);
  real_t value2 = 0;
  sscanf(copy_str, "%lf", &value2);
  ck_assert_double_eq(value2, REAL_TEST_VALUE);

  ck_assert(real_equals(real_obj, copy_obj));

  delete_object(real_obj);
  delete_object(copy_obj);
} END_TEST

START_TEST(test_real_op_add) {
  objectptr self = make_real(1.0);
  objectptr int2 = make_integer(2);
  objectptr real2 = make_real(2.0);

  objectptr sum1 = real_op_add(self, int2);
  ck_assert(is_real(sum1));
  ck_assert_double_eq(real_value(sum1), 3.0);

  objectptr sum2 = real_op_add(self, real2);
  ck_assert(is_real(sum2));
  ck_assert_double_eq(real_value(sum2), 3.0);

  delete_object(self);
  delete_object(int2);
  delete_object(real2);
  delete_object(sum1);
  delete_object(sum2);
} END_TEST

START_TEST(test_real_op_mul) {
  objectptr self = make_real(3.0);
  objectptr int2 = make_integer(4);
  objectptr real2 = make_real(4.0);

  objectptr product1 = real_op_mul(self, int2);
  ck_assert(is_real(product1));
  ck_assert_double_eq(real_value(product1), 12.0);

  objectptr product2 = real_op_mul(self, real2);
  ck_assert(is_real(product2));
  ck_assert_double_eq(real_value(product2), 12.0);

  delete_object(self);
  delete_object(int2);
  delete_object(real2);
  delete_object(product1);
  delete_object(product2);
} END_TEST

START_TEST(test_real_op_sub) {
  objectptr self = make_real(1.0);
  objectptr int2 = make_integer(2);
  objectptr real2 = make_real(2.0);

  objectptr diff1 = real_op_sub(self, int2);
  ck_assert(is_real(diff1));
  ck_assert_double_eq(real_value(diff1), -1.0);

  objectptr diff2 = real_op_sub(self, real2);
  ck_assert(is_real(diff2));
  ck_assert_double_eq(real_value(diff2), -1.0);

  delete_object(self);
  delete_object(int2);
  delete_object(real2);
  delete_object(diff1);
  delete_object(diff2);
} END_TEST

START_TEST(test_real_op_div) {
  objectptr self = make_real(7.0);
  objectptr int2 = make_integer(4);
  objectptr real2 = make_real(4.0);

  objectptr quotient1 = real_op_div(self, int2);
  ck_assert(is_real(quotient1));
  ck_assert_double_eq(real_value(quotient1), 1.75);

  objectptr quotient2 = real_op_div(self, real2);
  ck_assert(is_real(quotient2));
  ck_assert_double_eq(real_value(quotient2), 1.75);

  delete_object(self);
  delete_object(int2);
  delete_object(real2);
  delete_object(quotient1);
  delete_object(quotient2);
} END_TEST

START_TEST(test_real_less) {
  objectptr real10 = make_real(10.0);
  objectptr real20 = make_real(20.0);
  objectptr int10 = make_integer(10);
  objectptr int20 = make_integer(20);

  objectptr less_r10_i10 = real_less(real10, int10);
  ck_assert(!boolean_value(less_r10_i10));
  objectptr less_r10_i20 = real_less(real10, int20);
  ck_assert(boolean_value(less_r10_i20));
  objectptr less_r10_r10 = real_less(real10, real10);
  ck_assert(!boolean_value(less_r10_r10));
  objectptr less_r10_r20 = real_less(real10, real20);
  ck_assert(boolean_value(less_r10_r20));

  objectptr less_r20_i10 = real_less(real20, int10);
  ck_assert(!boolean_value(less_r20_i10));
  objectptr less_r20_i20 = real_less(real20, int20);
  ck_assert(!boolean_value(less_r20_i20));
  objectptr less_r20_r10 = real_less(real20, real10);
  ck_assert(!boolean_value(less_r20_r10));
  objectptr less_r20_r20 = real_less(real20, real20);
  ck_assert(!boolean_value(less_r20_r20));

  delete_object(int10);
  delete_object(int20);
  delete_object(real10);
  delete_object(real20);
  delete_object(less_r10_i10);
  delete_object(less_r10_i20);
  delete_object(less_r10_r10);
  delete_object(less_r10_r20);
  delete_object(less_r20_i10);
  delete_object(less_r20_i20);
  delete_object(less_r20_r10);
  delete_object(less_r20_r20);
} END_TEST

Suite *real_suite(void) {
  Suite *s = suite_create("Real");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_real_common);
  tcase_add_test(tc_core, test_real_op_add);
  tcase_add_test(tc_core, test_real_op_mul);
  tcase_add_test(tc_core, test_real_op_sub);
  tcase_add_test(tc_core, test_real_op_div);
  tcase_add_test(tc_core, test_real_less);
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = real_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
