#include <check.h>
#include <wchar.h>

#include "../../src/scanner/scanner.h"


START_TEST(test_empty) {
  tokenstreamptr tkns = scanner("");
  ck_assert_uint_eq(list_size(tkns->tokens), 1);

  tokenptr tkn = next_tkn(tkns);
  ck_assert_uint_eq(tkn->line, 1);
  ck_assert_uint_eq(tkn->column, 1);
  ck_assert_int_eq(tkn->type, TOKEN_END_OF_FILE);

  delete_tokenstream(tkns);
} END_TEST

START_TEST(test_keywords) {
  static const char keywords[] = "  let    lambda \n if define\tcond null";
  static const token_type_t token_types[] = {
    TOKEN_LET, TOKEN_LAMBDA, TOKEN_IF, TOKEN_DEFINE,
    TOKEN_COND, TOKEN_NULL, TOKEN_END_OF_FILE
  };

  tokenstreamptr tkns = scanner(keywords);
  for (int i = 0; i < sizeof token_types / sizeof(token_type_t); i++) {
    token_t *tkn = list_get(tkns->tokens, i);
    ck_assert_int_eq(tkn->type, token_types[i]);
  }

  delete_tokenstream(tkns);
} END_TEST

START_TEST(test_boolean) {
  static const char words[] = "#t  #f\t     #t\n#f   ";
  static const bool values[] = {true, false, true, false};
  tokenstreamptr tkns = scanner(words);
  for (int i = 0; i < list_size(tkns->tokens) - 1; i++) {
    token_t *tkn = next_tkn(tkns);
    ck_assert_int_eq(tkn->type, TOKEN_BOOLEAN);
    ck_assert_int_eq(tkn->value.boolean, values[i]);
  }

  delete_tokenstream(tkns);
} END_TEST

START_TEST(test_numbers) {
  static const char words[] = "2.25 1234 0 0.0 1.75";
  tokenstreamptr tkns = scanner(words);
  ck_assert_uint_eq(list_size(tkns->tokens), 6);

  tokenptr t1 = next_tkn(tkns);
  tokenptr t2 = next_tkn(tkns);
  tokenptr t3 = next_tkn(tkns);
  tokenptr t4 = next_tkn(tkns);
  tokenptr t5 = next_tkn(tkns);
  tokenptr t6 = next_tkn(tkns);

  ck_assert_int_eq(t1->type, TOKEN_REAL);
  ck_assert_double_eq(t1->value.real, 2.25);

  ck_assert_int_eq(t2->type, TOKEN_INTEGER);
  ck_assert_int_eq(t2->value.integer, 1234);

  ck_assert_int_eq(t3->type, TOKEN_INTEGER);
  ck_assert_int_eq(t3->value.integer, 0);

  ck_assert_int_eq(t4->type, TOKEN_REAL);
  ck_assert_double_eq(t4->value.real, 0.0);

  ck_assert_int_eq(t5->type, TOKEN_REAL);
  ck_assert_double_eq(t5->value.real, 1.75);

  ck_assert_int_eq(t6->type, TOKEN_END_OF_FILE);

  delete_tokenstream(tkns);
} END_TEST

#include <stdio.h>

START_TEST(test_mixed_alphanumeric) {
  static const char words[] = "123abc \"first string word\" xyzt123 iflambda \"long string\" ";
  tokenstreamptr tkns = scanner(words);
  ck_assert_uint_eq(list_size(tkns->tokens), 7);

  tokenptr t1 = next_tkn(tkns);
  tokenptr t2 = next_tkn(tkns);
  tokenptr t3 = next_tkn(tkns);
  tokenptr t4 = next_tkn(tkns);
  tokenptr t5 = next_tkn(tkns);
  tokenptr t6 = next_tkn(tkns);
  tokenptr t7 = next_tkn(tkns);

  ck_assert(t1->type == TOKEN_INTEGER);
  ck_assert_int_eq(t1->value.integer, 123);

  ck_assert(t2->type == TOKEN_IDENTIFIER);
  ck_assert_str_eq(t2->value.character_sequence, "abc");

  ck_assert(t3->type == TOKEN_STRING);
  ck_assert_str_eq(t3->value.character_sequence, "first string word");

  ck_assert(t4->type == TOKEN_IDENTIFIER);
  ck_assert_str_eq(t4->value.character_sequence, "xyzt123");

  ck_assert(t5->type == TOKEN_IDENTIFIER);
  ck_assert_str_eq(t5->value.character_sequence, "iflambda");

  ck_assert(t6->type == TOKEN_STRING);
  ck_assert_str_eq(t6->value.character_sequence, "long string");

  ck_assert(t7->type == TOKEN_END_OF_FILE);

  delete_tokenstream(tkns);
} END_TEST

START_TEST(test_mixed_with_parenthesis) {
  static const char words[] = 
    "(lambda (x y z ...)"
    " (if (= x y) z (display %va_args)))";

  static const token_type_t expected_token_types[] = 
  {TOKEN_LEFT_PARENTHESIS, TOKEN_LAMBDA, TOKEN_LEFT_PARENTHESIS,
    TOKEN_IDENTIFIER, TOKEN_IDENTIFIER, TOKEN_IDENTIFIER, TOKEN_IDENTIFIER,
    TOKEN_RIGHT_PARENTHESIS, TOKEN_LEFT_PARENTHESIS, TOKEN_IF,
    TOKEN_LEFT_PARENTHESIS, TOKEN_IDENTIFIER, TOKEN_IDENTIFIER, TOKEN_IDENTIFIER,
    TOKEN_RIGHT_PARENTHESIS, TOKEN_IDENTIFIER, TOKEN_LEFT_PARENTHESIS,
    TOKEN_IDENTIFIER, TOKEN_PERCENT, TOKEN_IDENTIFIER,
    TOKEN_RIGHT_PARENTHESIS, TOKEN_RIGHT_PARENTHESIS, TOKEN_RIGHT_PARENTHESIS,
    TOKEN_END_OF_FILE
  };

  tokenstreamptr tkns = scanner(words);
  for (int i = 0; i < list_size(tkns->tokens); i++) {
    tokenptr tkn = next_tkn(tkns);
    ck_assert_int_eq(tkn->type, expected_token_types[i]);
  }

  delete_tokenstream(tkns);

} END_TEST

Suite *scanner_suite(void) {
  Suite *s = suite_create("Scanner");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_empty);
  tcase_add_test(tc_core, test_keywords);
  tcase_add_test(tc_core, test_boolean);
  tcase_add_test(tc_core, test_numbers);
  tcase_add_test(tc_core, test_mixed_alphanumeric);
  tcase_add_test(tc_core, test_mixed_with_parenthesis);
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
