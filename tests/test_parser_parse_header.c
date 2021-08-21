#include "test.h"


void test_impl()
{
  struct HSKeyValue *header = hs_parser_parse_header("Content-Type: application/x-www-form-urlencoded");

  assert_string_equal(header->key, "content-type");
  assert_string_equal(header->value, "application/x-www-form-urlencoded");

  hs_types_release_key_value(header);

  header = hs_parser_parse_header("someheader: a b 123");

  assert_string_equal(header->key, "someheader");
  assert_string_equal(header->value, "a b 123");

  hs_types_release_key_value(header);

  header = hs_parser_parse_header("someheader: ");

  assert_string_equal(header->key, "someheader");
  assert_true(header->value == NULL);

  hs_types_release_key_value(header);

  header = hs_parser_parse_header("someheader:");

  assert_true(header == NULL);
}


int main()
{
  test_run(test_impl);
}

