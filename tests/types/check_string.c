#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include "../../src/types/string.h"

#define STRING "test string"
#define STRING_STR "\"" STRING "\""

START_TEST(test_symbol_common) {
  objectptr str_obj = make_string(STRING);
  ck_assert(is_string(str_obj));
  ck_assert(string_equals(str_obj, str_obj));
  char *str = string_tostring(str_obj);
  ck_assert_str_eq(str, STRING_STR);
  ck_assert_str_eq(string_value(str_obj), STRING);

  objectptr copy_obj = clone_object(str_obj);
  ck_assert(is_string(copy_obj));
  ck_assert(string_equals(copy_obj, copy_obj));
  char *str2 = string_tostring(copy_obj);
  ck_assert_str_eq(str2, STRING_STR);
  ck_assert_str_eq(string_value(copy_obj), STRING);

  ck_assert(string_equals(str_obj, copy_obj));

  delete_object(str_obj);
  delete_object(copy_obj);
  free(str);
  free(str2);
} END_TEST

Suite *symbol_suite(void) {
  Suite *s = suite_create("Symbol");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_symbol_common);
  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  Suite *s = symbol_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
