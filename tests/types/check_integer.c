#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "../../src/types/integer.h"
#include "../../src/types/real.h"
#include "../../src/types/boolean.h"

#define INT_TEST_VALUE 1234

START_TEST(test_integer_common) {
  object_t int_obj = make_integer(INT_TEST_VALUE);
  ck_assert(is_integer(int_obj));
  ck_assert(integer_equals(int_obj, int_obj));
  char *int_str = integer_tostring(int_obj);
  integer_t value1 = 0;
  sscanf(int_str, "%ld", &value1);
  ck_assert_int_eq(value1, INT_TEST_VALUE);

  object_t copy_obj = clone_integer(int_obj);
  ck_assert(is_integer(copy_obj));
  ck_assert(integer_equals(copy_obj, copy_obj));
  char *copy_str = integer_tostring(copy_obj);
  integer_t value2 = 0;
  sscanf(copy_str, "%ld", &value2);
  ck_assert_int_eq(value2, INT_TEST_VALUE);

  ck_assert(integer_equals(int_obj, copy_obj));

  destroy_integer(int_obj);
  destroy_integer(copy_obj);
}
END_TEST

START_TEST(test_integer_op_add) {
  object_t self = make_integer(1);
  object_t int2 = make_integer(2);
  object_t real2 = make_real(2.0);

  object_t sum1 = integer_op_add(self, int2);
  ck_assert(is_integer(sum1));
  ck_assert_int_eq(int_value(sum1), 3);

  object_t sum2 = integer_op_add(self, real2);
  ck_assert(is_real(sum2));
  ck_assert_double_eq(real_value(sum2), 3.0);

  destroy_integer(self);
  destroy_integer(int2);
  destroy_real(real2);
  destroy_integer(sum1);
  destroy_real(sum2);
} END_TEST

START_TEST(test_integer_op_mul) {
  object_t self = make_integer(3);
  object_t int4 = make_integer(4);
  object_t real4 = make_real(4.0);

  object_t product1 = integer_op_mul(self, int4);
  ck_assert(is_integer(product1));
  ck_assert_int_eq(int_value(product1), 12);

  object_t product2 = integer_op_mul(self, real4);
  ck_assert(is_real(product2));
  ck_assert_double_eq(real_value(product2), 12.0);

  destroy_integer(self);
  destroy_integer(int4);
  destroy_real(real4);
  destroy_integer(product1);
  destroy_real(product2);
} END_TEST

START_TEST(test_integer_op_sub) {
  object_t self = make_integer(1);
  object_t int2 = make_integer(2);
  object_t real2 = make_real(2.0);

  object_t diff1 = integer_op_sub(self, int2);
  ck_assert(is_integer(diff1));
  ck_assert_int_eq(int_value(diff1), -1);

  object_t diff2 = integer_op_sub(self, real2);
  ck_assert(is_real(diff2));
  ck_assert_double_eq(real_value(diff2), -1.0);

  destroy_integer(self);
  destroy_integer(int2);
  destroy_real(real2);
  destroy_integer(diff1);
  destroy_real(diff2);
} END_TEST

START_TEST(test_integer_op_div) {
  object_t self = make_integer(7);
  object_t int4 = make_integer(4);
  object_t real4 = make_real(4.0);

  object_t quotient1 = integer_op_div(self, int4);
  ck_assert(is_integer(quotient1));
  ck_assert_int_eq(int_value(quotient1), 1);

  object_t quotient2 = integer_op_div(self, real4);
  ck_assert(is_real(quotient2));
  ck_assert_double_eq(real_value(quotient2), 1.75);

  destroy_integer(self);
  destroy_integer(int4);
  destroy_real(real4);
  destroy_integer(quotient1);
  destroy_real(quotient2);
} END_TEST

START_TEST(test_integer_less) {
  object_t int10 = make_integer(10);
  object_t int20 = make_integer(20);
  object_t real10 = make_real(10);
  object_t real20 = make_real(20);

  object_t less_i10_i10 = integer_less(int10, int10);
  ck_assert(!boolean_value(less_i10_i10));
  object_t less_i10_i20 = integer_less(int10, int20);
  ck_assert(boolean_value(less_i10_i20));
  object_t less_i10_r10 = integer_less(int10, real10);
  ck_assert(!boolean_value(less_i10_r10));
  object_t less_i10_r20 = integer_less(int10, real20);
  ck_assert(boolean_value(less_i10_r20));

  object_t less_i20_i10 = integer_less(int20, int10);
  ck_assert(!boolean_value(less_i20_i10));
  object_t less_i20_i20 = integer_less(int20, int20);
  ck_assert(!boolean_value(less_i20_i20));
  object_t less_i20_r10 = integer_less(int20, real10);
  ck_assert(!boolean_value(less_i20_r10));
  object_t less_i20_r20 = integer_less(int20, real20);
  ck_assert(!boolean_value(less_i20_r20));

  destroy_integer(int10);
  destroy_integer(int20);
  destroy_real(real10);
  destroy_real(real20);
  destroy_boolean(less_i10_i10);
  destroy_boolean(less_i10_i20);
  destroy_boolean(less_i10_r10);
  destroy_boolean(less_i10_r20);
  destroy_boolean(less_i20_i10);
  destroy_boolean(less_i20_i20);
  destroy_boolean(less_i20_r10);
  destroy_boolean(less_i20_r20);
} END_TEST

Suite *integer_suite(void) {
  Suite *s = suite_create("Integer");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_integer_common);
  tcase_add_test(tc_core, test_integer_op_add);
  tcase_add_test(tc_core, test_integer_op_mul);
  tcase_add_test(tc_core, test_integer_op_sub);
  tcase_add_test(tc_core, test_integer_op_div);
  tcase_add_test(tc_core, test_integer_less);
  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  Suite *s = integer_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
