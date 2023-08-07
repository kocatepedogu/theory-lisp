#include <check.h>
#include <stdlib.h>

#include "../../src/interpreter/stack_frame.h"
#include "../../src/types/integer.h"
#include "../../src/types/void.h"
#include "../../src/types/error.h"

START_TEST(test_stack_frame) {
  stack_frame_ptr sf = new_stack_frame(NULL);
  stack_frame_set_local_variable(sf, "x", move(make_integer(10)));
  stack_frame_set_local_variable(sf, "y", move(make_integer(20)));
  stack_frame_set_local_variable(sf, "z", move(make_integer(30)));

  objectptr result = make_void();
  assign_object(&result, stack_frame_get_variable(sf, "x"));
  ck_assert(is_integer(result));
  ck_assert_int_eq(int_value(result), 10);
  
  assign_object(&result, stack_frame_get_variable(sf, "y"));
  ck_assert(is_integer(result));
  ck_assert_int_eq(int_value(result), 20);

  assign_object(&result, stack_frame_get_variable(sf, "z"));
  ck_assert(is_integer(result));
  ck_assert_int_eq(int_value(result), 30);

  assign_object(&result, stack_frame_get_variable(sf, "t"));
  ck_assert(is_error(result));

  delete_object(result);
  delete_stack_frame(sf);

} END_TEST

START_TEST(test_nested_stack_frame) {
  stack_frame_ptr sf_global = new_stack_frame(NULL);
  stack_frame_set_local_variable(sf_global, "x", move(make_integer(10)));
  stack_frame_ptr sf_local = new_stack_frame(sf_global);
  stack_frame_set_local_variable(sf_local, "x", move(make_integer(20)));

  objectptr result = make_void();
  assign_object(&result, stack_frame_get_variable(sf_local, "x"));
  ck_assert(is_integer(result));
  ck_assert_int_eq(int_value(result), 20);
  delete_stack_frame(sf_local);

  assign_object(&result, stack_frame_get_variable(sf_global, "x"));
  ck_assert(is_integer(result));
  ck_assert_int_eq(int_value(result), 10);
  delete_stack_frame(sf_global);

  delete_object(result);

} END_TEST

Suite *scanner_suite(void) {
  Suite *s = suite_create("Scanner");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_stack_frame);
  tcase_add_test(tc_core, test_nested_stack_frame);
  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  Suite *s = scanner_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
