#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../src/utils/heap-format.h"

START_TEST(test_heap_format_concatenation) {
  char first[] = "abcd";
  char second[] = "efgh";
  char *str = heap_format("%s%s", first, second); 
  ck_assert_str_eq(str, "abcdefgh");
  free(str);
}
END_TEST

START_TEST(test_heap_format_integer) {
  int x = 1, y = 2, z = 3, t = 4;
  char *str = heap_format("%d%d%d%d", x, y, z, t);
  ck_assert_str_eq(str, "1234");
  free(str);
}
END_TEST

START_TEST(test_heap_format_long) {
  long x = 1, y = 2, z = 3, t = 4;
  char *str = heap_format("%ld%ld%ld%ld", x, y, z, t);
  ck_assert_str_eq(str, "1234");
  free(str);
}
END_TEST

START_TEST(test_heap_format_double) {
  double x = 123.456;
  char *str = heap_format("%3.3f", x);
  ck_assert_str_eq(str, "123.456");
  free(str);
}
END_TEST

Suite *format_suite(void) {
  Suite *s = suite_create("Format");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_heap_format_concatenation);
  tcase_add_test(tc_core, test_heap_format_integer);
  tcase_add_test(tc_core, test_heap_format_long);
  tcase_add_test(tc_core, test_heap_format_double);
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = format_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
