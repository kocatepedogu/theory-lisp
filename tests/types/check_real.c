#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../src/types/real.h"
#include "../../src/types/integer.h"
#include "../../src/types/boolean.h"

#define REAL_TEST_VALUE 123.456

START_TEST(test_real_common) {
  object_t real_obj = make_real(REAL_TEST_VALUE);
  ck_assert(is_real(real_obj));
  ck_assert(real_equals(real_obj, real_obj));
  char *real_str = real_tostring(real_obj);
  real_t value1 = 0;
  sscanf(real_str, "%lf", &value1);
  ck_assert_double_eq(value1, REAL_TEST_VALUE);

  object_t copy_obj = clone_real(real_obj);
  ck_assert(is_real(copy_obj));
  ck_assert(real_equals(copy_obj, copy_obj));
  char *copy_str = real_tostring(copy_obj);
  real_t value2 = 0;
  sscanf(copy_str, "%lf", &value2);
  ck_assert_double_eq(value2, REAL_TEST_VALUE);

  ck_assert(real_equals(real_obj, copy_obj));

  destroy_real(real_obj);
  destroy_real(copy_obj);
} END_TEST

START_TEST(test_real_op_add) {
  object_t self = make_real(1.0);
  object_t int2 = make_integer(2);
  object_t real2 = make_real(2.0);

  object_t sum1 = real_op_add(self, int2);
  ck_assert(is_real(sum1));
  ck_assert_double_eq(real_value(sum1), 3.0);

  object_t sum2 = real_op_add(self, real2);
  ck_assert(is_real(sum2));
  ck_assert_double_eq(real_value(sum2), 3.0);

  destroy_real(self);
  destroy_integer(int2);
  destroy_real(real2);
  destroy_real(sum1);
  destroy_real(sum2);
} END_TEST

START_TEST(test_real_op_mul) {
  object_t self = make_real(3.0);
  object_t int2 = make_integer(4);
  object_t real2 = make_real(4.0);

  object_t product1 = real_op_mul(self, int2);
  ck_assert(is_real(product1));
  ck_assert_double_eq(real_value(product1), 12.0);

  object_t product2 = real_op_mul(self, real2);
  ck_assert(is_real(product2));
  ck_assert_double_eq(real_value(product2), 12.0);

  destroy_real(self);
  destroy_integer(int2);
  destroy_real(real2);
  destroy_real(product1);
  destroy_real(product2);
} END_TEST

START_TEST(test_real_op_sub) {
  object_t self = make_real(1.0);
  object_t int2 = make_integer(2);
  object_t real2 = make_real(2.0);

  object_t diff1 = real_op_sub(self, int2);
  ck_assert(is_real(diff1));
  ck_assert_double_eq(real_value(diff1), -1.0);

  object_t diff2 = real_op_sub(self, real2);
  ck_assert(is_real(diff2));
  ck_assert_double_eq(real_value(diff2), -1.0);

  destroy_real(self);
  destroy_integer(int2);
  destroy_real(real2);
  destroy_real(diff1);
  destroy_real(diff2);
} END_TEST

START_TEST(test_real_op_div) {
  object_t self = make_real(7.0);
  object_t int2 = make_integer(4);
  object_t real2 = make_real(4.0);

  object_t quotient1 = real_op_div(self, int2);
  ck_assert(is_real(quotient1));
  ck_assert_double_eq(real_value(quotient1), 1.75);

  object_t quotient2 = real_op_div(self, real2);
  ck_assert(is_real(quotient2));
  ck_assert_double_eq(real_value(quotient2), 1.75);

  destroy_real(self);
  destroy_integer(int2);
  destroy_real(real2);
  destroy_real(quotient1);
  destroy_real(quotient2);
} END_TEST

START_TEST(test_real_less) {
  object_t real10 = make_real(10.0);
  object_t real20 = make_real(20.0);
  object_t int10 = make_integer(10);
  object_t int20 = make_integer(20);

  object_t less_r10_i10 = real_less(real10, int10);
  ck_assert(!boolean_value(less_r10_i10));
  object_t less_r10_i20 = real_less(real10, int20);
  ck_assert(boolean_value(less_r10_i20));
  object_t less_r10_r10 = real_less(real10, real10);
  ck_assert(!boolean_value(less_r10_r10));
  object_t less_r10_r20 = real_less(real10, real20);
  ck_assert(boolean_value(less_r10_r20));

  object_t less_r20_i10 = real_less(real20, int10);
  ck_assert(!boolean_value(less_r20_i10));
  object_t less_r20_i20 = real_less(real20, int20);
  ck_assert(!boolean_value(less_r20_i20));
  object_t less_r20_r10 = real_less(real20, real10);
  ck_assert(!boolean_value(less_r20_r10));
  object_t less_r20_r20 = real_less(real20, real20);
  ck_assert(!boolean_value(less_r20_r20));

  destroy_integer(int10);
  destroy_integer(int20);
  destroy_real(real10);
  destroy_real(real20);
  destroy_boolean(less_r10_i10);
  destroy_boolean(less_r10_i20);
  destroy_boolean(less_r10_r10);
  destroy_boolean(less_r10_r20);
  destroy_boolean(less_r20_i10);
  destroy_boolean(less_r20_i20);
  destroy_boolean(less_r20_r10);
  destroy_boolean(less_r20_r20);
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
