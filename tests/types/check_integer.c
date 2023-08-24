#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "../../src/types/integer.h"
#include "../../src/types/real.h"
#include "../../src/types/rational.h"
#include "../../src/types/boolean.h"

#define INT_TEST_VALUE 1234

START_TEST(test_integer_common) {
  objectptr int_obj = make_integer(INT_TEST_VALUE);
  ck_assert(is_integer(int_obj));
  ck_assert(integer_equals(int_obj, int_obj));
  char *int_str = integer_tostring(int_obj);
  integer_t value1 = 0;
  sscanf(int_str, "%ld", &value1);
  ck_assert_int_eq(value1, INT_TEST_VALUE);

  objectptr copy_obj = clone_object(int_obj);
  ck_assert(is_integer(copy_obj));
  ck_assert(integer_equals(copy_obj, copy_obj));
  char *copy_str = integer_tostring(copy_obj);
  integer_t value2 = 0;
  sscanf(copy_str, "%ld", &value2);
  ck_assert_int_eq(value2, INT_TEST_VALUE);

  ck_assert(integer_equals(int_obj, copy_obj));

  delete_object(int_obj);
  delete_object(copy_obj);
}
END_TEST

START_TEST(test_integer_op_add) {
  objectptr self = make_integer(1);
  objectptr int2 = make_integer(2);
  objectptr real2 = make_real(2.0);

  objectptr sum1 = integer_op_add(self, int2);
  ck_assert(is_integer(sum1));
  ck_assert_int_eq(int_value(sum1), 3);

  objectptr sum2 = integer_op_add(self, real2);
  ck_assert(is_real(sum2));
  ck_assert_double_eq(real_value(sum2), 3.0);

  delete_object(self);
  delete_object(int2);
  delete_object(real2);
  delete_object(sum1);
  delete_object(sum2);
} END_TEST

START_TEST(test_integer_op_mul) {
  objectptr self = make_integer(3);
  objectptr int4 = make_integer(4);
  objectptr real4 = make_real(4.0);

  objectptr product1 = integer_op_mul(self, int4);
  ck_assert(is_integer(product1));
  ck_assert_int_eq(int_value(product1), 12);

  objectptr product2 = integer_op_mul(self, real4);
  ck_assert(is_real(product2));
  ck_assert_double_eq(real_value(product2), 12.0);

  delete_object(self);
  delete_object(int4);
  delete_object(real4);
  delete_object(product1);
  delete_object(product2);
} END_TEST

START_TEST(test_integer_op_sub) {
  objectptr self = make_integer(1);
  objectptr int2 = make_integer(2);
  objectptr real2 = make_real(2.0);

  objectptr diff1 = integer_op_sub(self, int2);
  ck_assert(is_integer(diff1));
  ck_assert_int_eq(int_value(diff1), -1);

  objectptr diff2 = integer_op_sub(self, real2);
  ck_assert(is_real(diff2));
  ck_assert_double_eq(real_value(diff2), -1.0);

  delete_object(self);
  delete_object(int2);
  delete_object(real2);
  delete_object(diff1);
  delete_object(diff2);
} END_TEST

START_TEST(test_integer_op_div) {
  objectptr self = make_integer(14);
  objectptr int4 = make_integer(8);
  objectptr real4 = make_real(4.0);

  objectptr r1 = integer_op_div(self, int4);
  ck_assert(is_rational(r1));
  rational_t val1 = rational_value(r1);
  ck_assert_int_eq(val1.x, 7);
  ck_assert_int_eq(val1.y, 4);

  objectptr r2 = integer_op_div(self, real4);
  ck_assert(is_real(r2));
  ck_assert_double_eq(real_value(r2), 3.5);

  delete_object(self);
  delete_object(int4);
  delete_object(real4);
  delete_object(r1);
  delete_object(r2);
} END_TEST

START_TEST(test_integer_less) {
  objectptr int10 = make_integer(10);
  objectptr int20 = make_integer(20);
  objectptr real10 = make_real(10);
  objectptr real20 = make_real(20);

  objectptr less_i10_i10 = integer_less(int10, int10);
  ck_assert(!boolean_value(less_i10_i10));
  objectptr less_i10_i20 = integer_less(int10, int20);
  ck_assert(boolean_value(less_i10_i20));
  objectptr less_i10_r10 = integer_less(int10, real10);
  ck_assert(!boolean_value(less_i10_r10));
  objectptr less_i10_r20 = integer_less(int10, real20);
  ck_assert(boolean_value(less_i10_r20));

  objectptr less_i20_i10 = integer_less(int20, int10);
  ck_assert(!boolean_value(less_i20_i10));
  objectptr less_i20_i20 = integer_less(int20, int20);
  ck_assert(!boolean_value(less_i20_i20));
  objectptr less_i20_r10 = integer_less(int20, real10);
  ck_assert(!boolean_value(less_i20_r10));
  objectptr less_i20_r20 = integer_less(int20, real20);
  ck_assert(!boolean_value(less_i20_r20));

  delete_object(int10);
  delete_object(int20);
  delete_object(real10);
  delete_object(real20);
  delete_object(less_i10_i10);
  delete_object(less_i10_i20);
  delete_object(less_i10_r10);
  delete_object(less_i10_r20);
  delete_object(less_i20_i10);
  delete_object(less_i20_i20);
  delete_object(less_i20_r10);
  delete_object(less_i20_r20);
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
