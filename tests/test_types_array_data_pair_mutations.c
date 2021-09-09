#include "test.h"
#include <string.h>


void test_impl()
{
  struct HSArrayDataPair *array = hs_types_array_data_pair_new();

  assert_num_equal(hs_types_array_data_pair_count(array), 0);
  hs_types_array_data_pair_remove_by_key(array, "test");
  assert_num_equal(hs_types_array_data_pair_count(array), 0);
  hs_types_array_data_pair_add(array, strdup("test"), "value1");
  hs_types_array_data_pair_add(array, strdup("test"), "value2");
  assert_num_equal(hs_types_array_data_pair_count(array), 2);
  assert_string_equal(hs_types_array_data_pair_get_by_key(array, "test"), "value1");
  assert_string_equal(hs_types_array_data_pair_get_key(array, 0), "test");
  assert_string_equal(hs_types_array_data_pair_get_value(array, 0), "value1");
  assert_string_equal(hs_types_array_data_pair_get_key(array, 1), "test");
  assert_string_equal(hs_types_array_data_pair_get_value(array, 1), "value2");
  hs_types_array_data_pair_remove_by_key(array, "test");
  assert_num_equal(hs_types_array_data_pair_count(array), 0);
  hs_types_array_data_pair_remove_by_key(array, "test");
  assert_num_equal(hs_types_array_data_pair_count(array), 0);

  hs_types_array_data_pair_add(array, strdup("test1"), "value1");
  hs_types_array_data_pair_add(array, strdup("test2"), "value2");
  assert_num_equal(hs_types_array_data_pair_count(array), 2);
  assert_string_equal(hs_types_array_data_pair_get_by_key(array, "test1"), "value1");
  assert_string_equal(hs_types_array_data_pair_get_by_key(array, "test2"), "value2");
  assert_string_equal(hs_types_array_data_pair_get_key(array, 0), "test1");
  assert_string_equal(hs_types_array_data_pair_get_value(array, 0), "value1");
  assert_string_equal(hs_types_array_data_pair_get_key(array, 1), "test2");
  assert_string_equal(hs_types_array_data_pair_get_value(array, 1), "value2");
  hs_types_array_data_pair_remove_by_key(array, "test2");
  assert_num_equal(hs_types_array_data_pair_count(array), 1);
  hs_types_array_data_pair_remove_by_key(array, "test1");
  assert_num_equal(hs_types_array_data_pair_count(array), 0);

  hs_types_array_data_pair_release(array);
}


int main()
{
  test_run(test_impl);
}

