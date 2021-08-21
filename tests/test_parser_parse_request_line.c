#include "test.h"


void test_impl()
{
  struct HSHttpRequest *request = hs_parser_parse_request_line("POST /testpost HTTP/1.0");

  assert_string_equal(request->domain, "");
  assert_string_equal(request->resource, "/testpost");
  assert_true(!request->ssl);
  assert_num_equal(request->port, -1);
  assert_num_equal(request->method, HS_HTTP_METHOD_POST);
  assert_true(request->query_string == NULL);

  hs_types_release_http_request(request);

  request = hs_parser_parse_request_line("GET /testget HTTP/1.0");

  assert_string_equal(request->domain, "");
  assert_string_equal(request->resource, "/testget");
  assert_true(!request->ssl);
  assert_num_equal(request->port, -1);
  assert_num_equal(request->method, HS_HTTP_METHOD_GET);
  assert_true(request->query_string == NULL);

  hs_types_release_http_request(request);

  request = hs_parser_parse_request_line("GET /testget HTTP/1.0 a");
  assert_true(request == NULL);

  request = hs_parser_parse_request_line("GET /testget");
  assert_true(request == NULL);
}


int main()
{
  test_run(test_impl);
}

