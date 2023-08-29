#include <check.h>
#include <stdlib.h>
#include "../../src/utils/stack.h"

#define FIRST_STRING "first string"
#define SECOND_STRING "second string"
#define THIRD_STRING "third string"

START_TEST(test_stack_simple) {
  char *first_string = malloc(sizeof(FIRST_STRING));
  memcpy(first_string, FIRST_STRING, sizeof(FIRST_STRING));
  char *second_string = malloc(sizeof(SECOND_STRING));
  memcpy(second_string, SECOND_STRING, sizeof(SECOND_STRING));
  char *third_string = malloc(sizeof(THIRD_STRING));
  memcpy(third_string, THIRD_STRING, sizeof(THIRD_STRING));

  stackptr st = new_stack();
  ck_assert_uint_eq(stack_size(st), 0);
  stack_push(st, first_string);
  ck_assert_str_eq(stack_get(st, 0), first_string);
  ck_assert_str_eq(stack_peek(st), first_string);
  ck_assert_uint_eq(stack_size(st), 1);
  stack_push(st, second_string);
  ck_assert_str_eq(stack_get(st, 0), first_string);
  ck_assert_str_eq(stack_get(st, 1), second_string);
  ck_assert_str_eq(stack_peek(st), second_string);
  ck_assert_uint_eq(stack_size(st), 2);
  stack_push(st, third_string);
  ck_assert_str_eq(stack_get(st, 0), first_string);
  ck_assert_str_eq(stack_get(st, 1), second_string);
  ck_assert_str_eq(stack_get(st, 2), third_string);
  ck_assert_str_eq(stack_peek(st), third_string);
  ck_assert_uint_eq(stack_size(st), 3);
  
  char *popped_value = NULL;
  popped_value = stack_pop(st);
  ck_assert_uint_eq(stack_size(st), 2);
  ck_assert_str_eq(popped_value, third_string);
  ck_assert_str_eq(stack_get(st, 0), first_string);
  ck_assert_str_eq(stack_get(st, 1), second_string);
  ck_assert_str_eq(stack_peek(st), second_string);
  popped_value = stack_pop(st);
  ck_assert_uint_eq(stack_size(st), 1);
  ck_assert_str_eq(popped_value, second_string);
  ck_assert_str_eq(stack_get(st, 0), first_string);
  ck_assert_str_eq(stack_peek(st), first_string);
  popped_value = stack_pop(st);
  ck_assert_uint_eq(stack_size(st), 0);
  ck_assert_str_eq(popped_value, first_string);

  free(first_string);
  free(second_string);
  free(third_string);
  delete_stack(st);
}
END_TEST

Suite *stack_suite(void) {
  Suite *s = suite_create("Stack");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_stack_simple);
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = stack_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
