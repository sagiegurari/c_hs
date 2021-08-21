#include "test.h"


void test_impl()
{
  assert_num_equal(hs_parser_parse_protocol_from_url(NULL), HS_HTTP_PROTOCOL_UNKNOWN);
  assert_num_equal(hs_parser_parse_protocol_from_url("http"), HS_HTTP_PROTOCOL_UNKNOWN);
  assert_num_equal(hs_parser_parse_protocol_from_url("abcd://abc"), HS_HTTP_PROTOCOL_UNKNOWN);
  assert_num_equal(hs_parser_parse_protocol_from_url("http://"), HS_HTTP_PROTOCOL_HTTP);
  assert_num_equal(hs_parser_parse_protocol_from_url("http://1"), HS_HTTP_PROTOCOL_HTTP);
  assert_num_equal(hs_parser_parse_protocol_from_url("HTTP://"), HS_HTTP_PROTOCOL_HTTP);
  assert_num_equal(hs_parser_parse_protocol_from_url("https://"), HS_HTTP_PROTOCOL_HTTPS);
  assert_num_equal(hs_parser_parse_protocol_from_url("https://1"), HS_HTTP_PROTOCOL_HTTPS);
  assert_num_equal(hs_parser_parse_protocol_from_url("HTTPS://"), HS_HTTP_PROTOCOL_HTTPS);
}


int main()
{
  test_run(test_impl);
}

