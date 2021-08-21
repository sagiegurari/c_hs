#include "test.h"


void test_impl()
{
  struct HSHttpRequest *request = hs_parser_create_request_from_url(NULL);

  assert_true(request == NULL);

  request = hs_parser_create_request_from_url("abc://mydomain");
  assert_true(request == NULL);

  request = hs_parser_create_request_from_url("http://mydomain");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/");
  assert_true(!request->ssl);
  assert_num_equal(request->port, -1);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("http://mydomain/");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/");
  assert_true(!request->ssl);
  assert_num_equal(request->port, -1);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/");
  assert_true(request->ssl);
  assert_num_equal(request->port, -1);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain:8080/");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/");
  assert_true(request->ssl);
  assert_num_equal(request->port, 8080);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain:8080");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/");
  assert_true(request->ssl);
  assert_num_equal(request->port, 8080);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain/");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/");
  assert_true(request->ssl);
  assert_num_equal(request->port, -1);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain/resource1/Resource2/");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/resource1/Resource2/");
  assert_true(request->ssl);
  assert_num_equal(request->port, -1);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain/resource1/resource2:3/");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/resource1/resource2:3/");
  assert_true(request->ssl);
  assert_num_equal(request->port, -1);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain:8080/resource1/resource2/");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/resource1/resource2/");
  assert_true(request->ssl);
  assert_num_equal(request->port, 8080);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain:8080/resource1/resource2/?V=1&t=2");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/resource1/resource2/");
  assert_true(request->ssl);
  assert_num_equal(request->port, 8080);
  assert_string_equal(request->query_string, "V=1&t=2");
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain:8080/resource1/resource2/?");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/resource1/resource2/");
  assert_true(request->ssl);
  assert_num_equal(request->port, 8080);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://www.google.com");
  assert_string_equal(request->domain, "www.google.com");
  assert_string_equal(request->resource, "/");
  assert_true(request->ssl);
  assert_num_equal(request->port, -1);
  assert_true(request->query_string == NULL);
  hs_types_release_http_request(request);

  request = hs_parser_create_request_from_url("https://mydomain/resource1/resource2/?V=1&t=2");
  assert_string_equal(request->domain, "mydomain");
  assert_string_equal(request->resource, "/resource1/resource2/");
  assert_true(request->ssl);
  assert_num_equal(request->port, -1);
  assert_string_equal(request->query_string, "V=1&t=2");
  hs_types_release_http_request(request);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

