#include "test.h"


void test_impl()
{
  struct HSArrayStringPair *pairs = hs_parser_parse_query_string(NULL);

  assert_true(pairs == NULL);

  pairs = hs_parser_parse_query_string("");
  assert_true(pairs == NULL);

  pairs = hs_parser_parse_query_string("abc123");
  assert_num_equal(hs_types_array_string_pair_count(pairs), 1);
  assert_string_equal(hs_types_array_string_pair_get_key(pairs, 0), "abc123");
  assert_true(hs_types_array_string_pair_get_value(pairs, 0) == NULL);
  hs_types_array_string_pair_release(pairs);

  pairs = hs_parser_parse_query_string("abc123&");
  assert_num_equal(hs_types_array_string_pair_count(pairs), 1);
  assert_string_equal(hs_types_array_string_pair_get_key(pairs, 0), "abc123");
  assert_true(hs_types_array_string_pair_get_value(pairs, 0) == NULL);
  hs_types_array_string_pair_release(pairs);

  pairs = hs_parser_parse_query_string("abc123=");
  assert_num_equal(hs_types_array_string_pair_count(pairs), 1);
  assert_string_equal(hs_types_array_string_pair_get_key(pairs, 0), "abc123");
  assert_true(hs_types_array_string_pair_get_value(pairs, 0) == NULL);
  hs_types_array_string_pair_release(pairs);

  pairs = hs_parser_parse_query_string("abc123=&");
  assert_num_equal(hs_types_array_string_pair_count(pairs), 1);
  assert_string_equal(hs_types_array_string_pair_get_key(pairs, 0), "abc123");
  assert_true(hs_types_array_string_pair_get_value(pairs, 0) == NULL);
  hs_types_array_string_pair_release(pairs);

  pairs = hs_parser_parse_query_string("abc123=A123");
  assert_num_equal(hs_types_array_string_pair_count(pairs), 1);
  assert_string_equal(hs_types_array_string_pair_get_key(pairs, 0), "abc123");
  assert_string_equal(hs_types_array_string_pair_get_value(pairs, 0), "A123");
  hs_types_array_string_pair_release(pairs);

  pairs = hs_parser_parse_query_string("abc123=A123&");
  assert_num_equal(hs_types_array_string_pair_count(pairs), 1);
  assert_string_equal(hs_types_array_string_pair_get_key(pairs, 0), "abc123");
  assert_string_equal(hs_types_array_string_pair_get_value(pairs, 0), "A123");
  hs_types_array_string_pair_release(pairs);

  pairs = hs_parser_parse_query_string("key1=value1&key2=value2&key3=value3");
  assert_num_equal(hs_types_array_string_pair_count(pairs), 3);
  assert_string_equal(hs_types_array_string_pair_get_key(pairs, 0), "key1");
  assert_string_equal(hs_types_array_string_pair_get_value(pairs, 0), "value1");
  assert_string_equal(hs_types_array_string_pair_get_key(pairs, 1), "key2");
  assert_string_equal(hs_types_array_string_pair_get_value(pairs, 1), "value2");
  assert_string_equal(hs_types_array_string_pair_get_key(pairs, 2), "key3");
  assert_string_equal(hs_types_array_string_pair_get_value(pairs, 2), "value3");
  hs_types_array_string_pair_release(pairs);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

