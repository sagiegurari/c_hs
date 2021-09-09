#include "test.h"


void test_impl()
{
  char **header = hs_parser_parse_header("Content-Type: application/x-www-form-urlencoded");

  assert_string_equal(header[0], "content-type");
  assert_string_equal(header[1], "application/x-www-form-urlencoded");

  hs_io_free(header[0]);
  hs_io_free(header[1]);
  hs_io_free(header);

  header = hs_parser_parse_header("someheader: a b 123");

  assert_string_equal(header[0], "someheader");
  assert_string_equal(header[1], "a b 123");

  hs_io_free(header[0]);
  hs_io_free(header[1]);
  hs_io_free(header);

  header = hs_parser_parse_header("someheader: ");

  assert_string_equal(header[0], "someheader");
  assert_true(header[1] == NULL);

  hs_io_free(header[0]);
  hs_io_free(header[1]);
  hs_io_free(header);

  header = hs_parser_parse_header("someheader:");

  assert_true(header == NULL);
}


int main()
{
  test_run(test_impl);
}

