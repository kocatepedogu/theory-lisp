#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../src/types/error.h"

#define STR(X) #X
#define MACRO_STR(X) STR(X)

#define TEST_STRING "string"
#define TEST_INT 1234
#define TEST_REAL 123.456
#define TEST_INT_STR MACRO_STR(TEST_INT)
#define TEST_REAL_STR MACRO_STR(TEST_REAL)

START_TEST(test_error) {
  static const char *expected_result = 
    TEST_STRING " " TEST_INT_STR " " TEST_REAL_STR;

  object_t err_obj = make_error("%s %d %3.3f", 
      TEST_STRING, TEST_INT, TEST_REAL);
  ck_assert(is_error(err_obj));
  ck_assert(error_equals(err_obj, err_obj));
  char *err_str = error_tostring(err_obj);
  ck_assert_str_eq(err_str, expected_result);

  object_t copy_obj = clone_error(err_obj);
  ck_assert(is_error(copy_obj));
  ck_assert(error_equals(copy_obj, copy_obj));
  char *copy_str = error_tostring(copy_obj);
  ck_assert_str_eq(copy_str, expected_result);

  ck_assert(error_equals(err_obj, copy_obj));

  destroy_error(err_obj);
  destroy_error(copy_obj);
  free(err_str);
  free(copy_str);
}
END_TEST

Suite *error_suite(void) {
  Suite *s = suite_create("Error");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_error);
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = error_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
