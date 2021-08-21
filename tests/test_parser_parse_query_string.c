#include "test.h"


void test_impl()
{
  struct HSKeyValueArray *pairs = hs_parser_parse_query_string(NULL);

  assert_true(pairs == NULL);

  pairs = hs_parser_parse_query_string("");
  assert_true(pairs == NULL);

  pairs = hs_parser_parse_query_string("abc123");
  assert_num_equal(pairs->count, 1);
  assert_string_equal(pairs->pairs[0]->key, "abc123");
  assert_true(pairs->pairs[0]->value == NULL);
  hs_types_release_key_value_array(pairs);

  pairs = hs_parser_parse_query_string("abc123&");
  assert_num_equal(pairs->count, 1);
  assert_string_equal(pairs->pairs[0]->key, "abc123");
  assert_true(pairs->pairs[0]->value == NULL);
  hs_types_release_key_value_array(pairs);

  pairs = hs_parser_parse_query_string("abc123=");
  assert_num_equal(pairs->count, 1);
  assert_string_equal(pairs->pairs[0]->key, "abc123");
  assert_true(pairs->pairs[0]->value == NULL);
  hs_types_release_key_value_array(pairs);

  pairs = hs_parser_parse_query_string("abc123=&");
  assert_num_equal(pairs->count, 1);
  assert_string_equal(pairs->pairs[0]->key, "abc123");
  assert_true(pairs->pairs[0]->value == NULL);
  hs_types_release_key_value_array(pairs);

  pairs = hs_parser_parse_query_string("abc123=A123");
  assert_num_equal(pairs->count, 1);
  assert_string_equal(pairs->pairs[0]->key, "abc123");
  assert_string_equal(pairs->pairs[0]->value, "A123");
  hs_types_release_key_value_array(pairs);

  pairs = hs_parser_parse_query_string("abc123=A123&");
  assert_num_equal(pairs->count, 1);
  assert_string_equal(pairs->pairs[0]->key, "abc123");
  assert_string_equal(pairs->pairs[0]->value, "A123");
  hs_types_release_key_value_array(pairs);

  pairs = hs_parser_parse_query_string("key1=value1&key2=value2&key3=value3");
  assert_num_equal(pairs->count, 3);
  assert_string_equal(pairs->pairs[0]->key, "key1");
  assert_string_equal(pairs->pairs[0]->value, "value1");
  assert_string_equal(pairs->pairs[1]->key, "key2");
  assert_string_equal(pairs->pairs[1]->value, "value2");
  assert_string_equal(pairs->pairs[2]->key, "key3");
  assert_string_equal(pairs->pairs[2]->value, "value3");
  hs_types_release_key_value_array(pairs);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

