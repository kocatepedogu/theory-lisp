#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../src/types/boolean.h"

#define TRUE_STRING "#t"
#define FALSE_STRING "#f"

START_TEST(test_boolean_common) {
  objectptr true_obj1 = make_boolean(true);
  ck_assert(is_boolean(true_obj1));
  char *str1 = boolean_tostring(true_obj1);
  ck_assert_str_eq(str1, TRUE_STRING);

  objectptr false_obj1 = make_boolean(false);
  ck_assert(is_boolean(false_obj1));
  char *str2 = boolean_tostring(false_obj1);
  ck_assert_str_eq(str2, FALSE_STRING);

  objectptr true_obj2 = clone_object(true_obj1);
  ck_assert(is_boolean(true_obj2));
  char *str3 = boolean_tostring(true_obj2);
  ck_assert_str_eq(str3, TRUE_STRING);

  objectptr false_obj2 = clone_object(false_obj1);
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

  delete_object(true_obj1);
  delete_object(false_obj1);
  delete_object(true_obj2);
  delete_object(false_obj2);

  free(str1);
  free(str2);
  free(str3);
  free(str4);
}
END_TEST

START_TEST(test_and_operation) {
  objectptr true_obj = make_boolean(true);
  objectptr false_obj = make_boolean(false);

  objectptr and00 = boolean_op_and(false_obj, false_obj);
  ck_assert_int_eq(boolean_value(and00), false);
  objectptr and01 = boolean_op_and(false_obj, true_obj);
  ck_assert_int_eq(boolean_value(and01), false);
  objectptr and10 = boolean_op_and(true_obj, false_obj);
  ck_assert_int_eq(boolean_value(and10), false);
  objectptr and11 = boolean_op_and(true_obj, true_obj);
  ck_assert_int_eq(boolean_value(and11), true);

  delete_object(and00);
  delete_object(and01);
  delete_object(and10);
  delete_object(and11);
  delete_object(true_obj);
  delete_object(false_obj);
}
END_TEST

START_TEST(test_or_operation) {
  objectptr true_obj = make_boolean(true);
  objectptr false_obj = make_boolean(false);

  objectptr or00 = boolean_op_or(false_obj, false_obj);
  ck_assert_int_eq(boolean_value(or00), false);
  objectptr or01 = boolean_op_or(false_obj, true_obj);
  ck_assert_int_eq(boolean_value(or01), true);
  objectptr or10 = boolean_op_or(true_obj, false_obj);
  ck_assert_int_eq(boolean_value(or10), true);
  objectptr or11 = boolean_op_or(true_obj, true_obj);
  ck_assert_int_eq(boolean_value(or11), true);

  delete_object(or00);
  delete_object(or01);
  delete_object(or10);
  delete_object(or11);
  delete_object(true_obj);
  delete_object(false_obj);
}
END_TEST

START_TEST(test_xor_operation) {
  objectptr true_obj = make_boolean(true);
  objectptr false_obj = make_boolean(false);

  objectptr xor00 = boolean_op_xor(false_obj, false_obj);
  ck_assert_int_eq(boolean_value(xor00), false);
  objectptr xor01 = boolean_op_xor(false_obj, true_obj);
  ck_assert_int_eq(boolean_value(xor01), true);
  objectptr xor10 = boolean_op_xor(true_obj, false_obj);
  ck_assert_int_eq(boolean_value(xor10), true);
  objectptr xor11 = boolean_op_xor(true_obj, true_obj);
  ck_assert_int_eq(boolean_value(xor11), false);

  delete_object(xor00);
  delete_object(xor01);
  delete_object(xor10);
  delete_object(xor11);
  delete_object(true_obj);
  delete_object(false_obj);
}
END_TEST

START_TEST(test_not_operation) {
  objectptr true_obj = make_boolean(true);
  objectptr false_obj = make_boolean(false);

  objectptr not0 = boolean_op_not(false_obj);
  ck_assert_int_eq(boolean_value(not0), true);
  objectptr not1 = boolean_op_not(true_obj);
  ck_assert_int_eq(boolean_value(not1), false);

  delete_object(not0);
  delete_object(not1);
  delete_object(true_obj);
  delete_object(false_obj);
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
