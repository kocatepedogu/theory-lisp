#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../src/types/pair.h"
#include "../../src/types/integer.h"
#include "../../src/types/real.h"

START_TEST(test_pair_common) {
  objectptr integer_obj = make_integer(1);
  objectptr real_obj = make_real(2.0);

  objectptr pair_obj = make_pair(integer_obj, real_obj);
  ck_assert(is_pair(pair_obj));
  ck_assert(pair_equals(pair_obj, pair_obj));

  objectptr copy_obj = clone_object(pair_obj);
  ck_assert(is_pair(copy_obj));
  ck_assert(pair_equals(copy_obj, copy_obj));

  ck_assert(pair_equals(pair_obj, copy_obj));

  delete_object(integer_obj);
  delete_object(real_obj);
  delete_object(pair_obj);
  delete_object(copy_obj);

} END_TEST

#define CONVERSION_TEST_SIZE 100

START_TEST(test_list_conversions) {
  listptr internal_list = new_list();
  for (int i = 0; i < CONVERSION_TEST_SIZE; i++) {
    list_add(internal_list, make_integer(i));
  } 

  objectptr cons_list = internal_list_to_cons_list(internal_list);
  listptr new_internal_list = new_list();
  ck_assert(cons_list_to_internal_list(cons_list, new_internal_list));
  for (int i = 0; i < CONVERSION_TEST_SIZE; i++) {
    objectptr obj = list_get(new_internal_list, i);
    ck_assert_int_eq(int_value(obj), i);
    delete_object(obj);
  }
} END_TEST

Suite *pair_suite(void) {
  Suite *s = suite_create("Pair");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_pair_common);
  tcase_add_test(tc_core, test_list_conversions);
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = pair_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
