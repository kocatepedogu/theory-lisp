#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../src/types/symbol.h"

#define SYMBOL "test_symbol"
#define SYMBOL_STR "'" SYMBOL

START_TEST(test_symbol_common) {
  object_t symbol_obj = make_symbol(SYMBOL);
  ck_assert(is_symbol(symbol_obj));
  ck_assert(symbol_equals(symbol_obj, symbol_obj));
  char *str = symbol_tostring(symbol_obj);
  ck_assert_str_eq(str, SYMBOL_STR);
  ck_assert_str_eq(symbol_value(symbol_obj), SYMBOL);

  object_t copy_obj = copy_symbol(symbol_obj);
  ck_assert(is_symbol(copy_obj));
  ck_assert(symbol_equals(copy_obj, copy_obj));
  char *str2 = symbol_tostring(copy_obj);
  ck_assert_str_eq(str2, SYMBOL_STR);
  ck_assert_str_eq(symbol_value(copy_obj), SYMBOL);

  ck_assert(symbol_equals(symbol_obj, copy_obj));

  destroy_symbol(symbol_obj);
  destroy_symbol(copy_obj);
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
