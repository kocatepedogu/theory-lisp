#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../src/types/void.h"

START_TEST(test_void_all) {
  object_t void_obj = make_void();
  ck_assert(is_void(void_obj));
  char *str1 = void_tostring(void_obj);
  ck_assert_str_eq(str1, "#<void>");

  object_t copy_obj = clone_void(void_obj);
  ck_assert(is_void(void_obj));
  char *str2 = void_tostring(copy_obj);
  ck_assert_str_eq(str2, "#<void>");

  ck_assert(void_equals(void_obj, copy_obj));

  destroy_object(copy_obj);
  destroy_object(void_obj);
  free(str1);
  free(str2);
}
END_TEST

Suite *void_suite(void) {
  Suite *s = suite_create("Void");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_void_all);
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = void_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
