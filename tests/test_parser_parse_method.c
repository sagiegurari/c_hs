#include "test.h"


void test_impl()
{
  assert_num_equal(hs_parser_parse_method(NULL), HS_HTTP_METHOD_UNKNOWN);
  assert_num_equal(hs_parser_parse_method("BAD"), HS_HTTP_METHOD_UNKNOWN);
  assert_num_equal(hs_parser_parse_method("GET"), HS_HTTP_METHOD_GET);
  assert_num_equal(hs_parser_parse_method("POST"), HS_HTTP_METHOD_POST);
  assert_num_equal(hs_parser_parse_method("PUT"), HS_HTTP_METHOD_PUT);
  assert_num_equal(hs_parser_parse_method("DELETE"), HS_HTTP_METHOD_DELETE);
  assert_num_equal(hs_parser_parse_method("HEAD"), HS_HTTP_METHOD_HEAD);
  assert_num_equal(hs_parser_parse_method("CONNECT"), HS_HTTP_METHOD_CONNECT);
  assert_num_equal(hs_parser_parse_method("OPTIONS"), HS_HTTP_METHOD_OPTIONS);
  assert_num_equal(hs_parser_parse_method("TRACE"), HS_HTTP_METHOD_TRACE);
  assert_num_equal(hs_parser_parse_method("PATCH"), HS_HTTP_METHOD_PATCH);
}


int main()
{
  test_run(test_impl);
}

