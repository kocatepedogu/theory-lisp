#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../src/utils/list.h"

#define FIRST_STRING "first string"
#define SECOND_STRING "second string"
#define THIRD_STRING "third string"

START_TEST(test_list_simple)
{
  char *first_string = malloc(sizeof(FIRST_STRING));
  memcpy(first_string, FIRST_STRING, sizeof(FIRST_STRING));
  char *second_string = malloc(sizeof(SECOND_STRING));
  memcpy(second_string, SECOND_STRING, sizeof(SECOND_STRING));
  char *third_string = malloc(sizeof(THIRD_STRING));
  memcpy(third_string, THIRD_STRING, sizeof(THIRD_STRING));

  listptr lst = new_list();
  ck_assert_uint_eq(list_size(lst), 0);
  list_add(lst, strdup("first string"));
  ck_assert_str_eq(list_get(lst, 0), first_string);
  ck_assert_uint_eq(list_size(lst), 1);
  list_add(lst, strdup("second string"));
  ck_assert_str_eq(list_get(lst, 0), first_string);
  ck_assert_str_eq(list_get(lst, 1), second_string);
  ck_assert_uint_eq(list_size(lst), 2);
  list_add(lst, strdup("third string"));
  ck_assert_str_eq(list_get(lst, 0), first_string);
  ck_assert_str_eq(list_get(lst, 1), second_string);
  ck_assert_str_eq(list_get(lst, 2), third_string);
  ck_assert_uint_eq(list_size(lst), 3);

  free(first_string);
  free(second_string);
  free(third_string);
  delete_list(lst);
}
END_TEST

#define FIRST_LIST_SIZE 10
#define SECOND_LIST_SIZE 10

START_TEST(test_list_add_all) {
  listptr first_list = new_list();
  listptr second_list = new_list();

  for (int i = 0; i < FIRST_LIST_SIZE; i++) {
    int *new_int = (int *)malloc(sizeof(int));
    *new_int = i;
    list_add(first_list, new_int);
  }
  for (int i = 0; i < SECOND_LIST_SIZE; i++) {
    int *new_int = (int *)malloc(sizeof(int));
    *new_int = FIRST_LIST_SIZE + i;
    list_add(second_list, new_int);
  }

  list_add_all(first_list, second_list);
  for (int i = 0; i < FIRST_LIST_SIZE + SECOND_LIST_SIZE; i++) {
    int *int_from_list = list_get(first_list, i);
    ck_assert_int_eq(*int_from_list, i);
    free(int_from_list);
  }

  delete_list(first_list);
  delete_list(second_list);
}
END_TEST

Suite *list_suite(void) {
  Suite *s = suite_create("List");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_list_simple);
  tcase_add_test(tc_core, test_list_add_all);
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = list_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
