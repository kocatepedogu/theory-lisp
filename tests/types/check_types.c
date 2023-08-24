#include <check.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../src/types/object.h"
#include "../../src/types/boolean.h"
#include "../../src/types/error.h"
#include "../../src/types/integer.h"
#include "../../src/types/pair.h"
#include "../../src/types/procedure.h"
#include "../../src/types/real.h"
#include "../../src/types/rational.h"
#include "../../src/types/string.h"
#include "../../src/types/void.h"

/* Ensure that all operators on all possible
 * operand types yield either proper errors or valid objects
 * so that no undefined behaviour or crash occurs
 */

typedef objectptr (*operator_t)(objectptr, objectptr);
operator_t const operators[] = {
  object_less, object_op_add, object_op_mul,
  object_op_sub, object_op_div, object_op_and,
  object_op_or, object_op_xor
};

typedef bool (*is_operator_t)(objectptr);
is_operator_t const is_operators[] = {
  is_void, is_error, is_integer, is_real, is_rational,
  is_string, is_boolean, is_procedure, is_pair
};

const int len_operators = sizeof operators / sizeof(operator_t);
const int len_is_operators = sizeof is_operators / sizeof(is_operator_t);

START_TEST(test_operations) {
  objectptr bool_obj = make_boolean(true);
  objectptr int_obj = make_integer(1);
  objectptr pair_obj = make_pair(bool_obj, int_obj);
  objectptr proc_obj = make_procedure(NULL, NULL, NULL);
  objectptr real_obj = make_real(1.0);
  objectptr sym_obj = make_string("string word");
  objectptr void_obj = make_void(); 
  objectptr objects[] = {
    bool_obj, int_obj, pair_obj,
    proc_obj, real_obj, sym_obj,
    void_obj
  };

  const int len_objects = sizeof objects / sizeof(objectptr);
  for (int i = 0; i < len_objects; i++) {
    for (int j = 0; j < len_objects; j++) {
      for (int k = 0; k < len_operators; k++) {
        objectptr result = make_void();
        assign_object(&result, operators[k](objects[i], objects[j]));

	bool found = false;
	for (int l = 0; l < len_is_operators; l++) {
	  found |= is_operators[l](result);
	}

	ck_assert(found);
      }
    }
  }

  for (int i = 0; i < sizeof objects / sizeof(objectptr); i++) {
    delete_object(objects[i]);
  }

} END_TEST

Suite *types_suite(void) {
  Suite *s = suite_create("Types");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_operations);
  suite_add_tcase(s, tc_core);
  return s;
}


int main(void) {
  Suite *s = types_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
