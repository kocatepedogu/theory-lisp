#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../src/types/procedure.h"

Suite *procedure_suite(void) {
  Suite *s = suite_create("Procedure");
  TCase *tc_core = tcase_create("Core");
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = procedure_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
