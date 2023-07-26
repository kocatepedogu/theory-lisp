#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../src/types/boolean.h"

#define TRUE_STRING "#t"
#define FALSE_STRING "#f"

START_TEST(test_boolean_common) {
  object_t true_obj1 = make_boolean(true);
  ck_assert(is_boolean(true_obj1));
  char *str1 = boolean_tostring(true_obj1);
  ck_assert_str_eq(str1, TRUE_STRING);

  object_t false_obj1 = make_boolean(false);
  ck_assert(is_boolean(false_obj1));
  char *str2 = boolean_tostring(false_obj1);
  ck_assert_str_eq(str2, FALSE_STRING);

  object_t true_obj2 = copy_boolean(true_obj1);
  ck_assert(is_boolean(true_obj2));
  char *str3 = boolean_tostring(true_obj2);
  ck_assert_str_eq(str3, TRUE_STRING);

  object_t false_obj2 = copy_boolean(false_obj1);
  ck_assert(is_boolean(false_obj2));
  char *str4 = boolean_tostring(false_obj2);
  ck_assert_str_eq(boolean_tostring(false_obj2), FALSE_STRING);
  
  ck_assert(boolean_equals(true_obj1, true_obj1));
  ck_assert(boolean_equals(true_obj1, true_obj2));
  ck_assert(boolean_equals(false_obj1, false_obj1));
  ck_assert(boolean_equals(false_obj1, false_obj2));
  ck_assert(!boolean_equals(true_obj1, false_obj1));
  ck_assert(!boolean_equals(true_obj1, false_obj2));
  ck_assert(!boolean_equals(true_obj2, false_obj1));
  ck_assert(!boolean_equals(true_obj2, false_obj2));

  destroy_boolean(true_obj1);
  destroy_boolean(false_obj1);
  destroy_boolean(true_obj2);
  destroy_boolean(false_obj2);

  free(str1);
  free(str2);
  free(str3);
  free(str4);
}
END_TEST

START_TEST(test_and_operation) {
  object_t true_obj = make_boolean(true);
  object_t false_obj = make_boolean(false);

  object_t and00 = boolean_op_and(false_obj, false_obj);
  ck_assert_int_eq(boolean_value(and00), false);
  object_t and01 = boolean_op_and(false_obj, true_obj);
  ck_assert_int_eq(boolean_value(and01), false);
  object_t and10 = boolean_op_and(true_obj, false_obj);
  ck_assert_int_eq(boolean_value(and10), false);
  object_t and11 = boolean_op_and(true_obj, true_obj);
  ck_assert_int_eq(boolean_value(and11), true);

  destroy_object(and00);
  destroy_object(and01);
  destroy_object(and10);
  destroy_object(and11);
  destroy_object(true_obj);
  destroy_object(false_obj);
}
END_TEST

START_TEST(test_or_operation) {
  object_t true_obj = make_boolean(true);
  object_t false_obj = make_boolean(false);

  object_t or00 = boolean_op_or(false_obj, false_obj);
  ck_assert_int_eq(boolean_value(or00), false);
  object_t or01 = boolean_op_or(false_obj, true_obj);
  ck_assert_int_eq(boolean_value(or01), true);
  object_t or10 = boolean_op_or(true_obj, false_obj);
  ck_assert_int_eq(boolean_value(or10), true);
  object_t or11 = boolean_op_or(true_obj, true_obj);
  ck_assert_int_eq(boolean_value(or11), true);

  destroy_object(or00);
  destroy_object(or01);
  destroy_object(or10);
  destroy_object(or11);
  destroy_object(true_obj);
  destroy_object(false_obj);
}
END_TEST

START_TEST(test_xor_operation) {
  object_t true_obj = make_boolean(true);
  object_t false_obj = make_boolean(false);

  object_t xor00 = boolean_op_xor(false_obj, false_obj);
  ck_assert_int_eq(boolean_value(xor00), false);
  object_t xor01 = boolean_op_xor(false_obj, true_obj);
  ck_assert_int_eq(boolean_value(xor01), true);
  object_t xor10 = boolean_op_xor(true_obj, false_obj);
  ck_assert_int_eq(boolean_value(xor10), true);
  object_t xor11 = boolean_op_xor(true_obj, true_obj);
  ck_assert_int_eq(boolean_value(xor11), false);

  destroy_object(xor00);
  destroy_object(xor01);
  destroy_object(xor10);
  destroy_object(xor11);
  destroy_object(true_obj);
  destroy_object(false_obj);
}
END_TEST

START_TEST(test_not_operation) {
  object_t true_obj = make_boolean(true);
  object_t false_obj = make_boolean(false);

  object_t not0 = boolean_op_not(false_obj);
  ck_assert_int_eq(boolean_value(not0), true);
  object_t not1 = boolean_op_not(true_obj);
  ck_assert_int_eq(boolean_value(not1), false);

  destroy_object(not0);
  destroy_object(not1);
  destroy_object(true_obj);
  destroy_object(false_obj);
}
END_TEST

Suite *boolean_suite(void) {
  Suite *s = suite_create("Boolean");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_boolean_common);
  tcase_add_test(tc_core, test_and_operation);
  tcase_add_test(tc_core, test_or_operation);
  tcase_add_test(tc_core, test_xor_operation);
  tcase_add_test(tc_core, test_not_operation);
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = boolean_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
