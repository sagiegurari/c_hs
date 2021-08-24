#include "fsio.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void test_impl()
{
  char *raw = "POST /testpost HTTP/1.0\r\n"
              "Host: MyHost\r\n"
              "User-Agent: MyBrowser\r\n"
              "Authorization: Basic 123\r\n"
              "Connection: close\r\n"
              "Content-Length: 11\r\n"
              "Content-Type: application/x-www-form-urlencoded\r\n"
              "\r\n"
              "12345\n67890\n\n";

  char *filename = "./test_parser_parse_request.txt";

  fsio_write_text_file(filename, raw);

  int                  socket   = open(filename, O_RDONLY);
  struct HSHttpRequest *request = hs_parser_parse_request(socket);

  assert_string_equal(request->domain, "");
  assert_string_equal(request->resource, "/testpost");
  assert_true(!request->ssl);
  assert_num_equal(request->port, -1);
  assert_num_equal(request->method, HS_HTTP_METHOD_POST);
  assert_string_equal(request->user_agent, "MyBrowser");
  assert_string_equal(request->authorization, "Basic 123");
  assert_true(request->query_string == NULL);
  assert_num_equal(request->content_length, 11);
  assert_true(request->payload != NULL);

  assert_num_equal(request->headers->count, 6);
  size_t index = 0;
  assert_string_equal(request->headers->pairs[index]->key, "host");
  assert_string_equal(request->headers->pairs[index]->value, "MyHost");
  index++;
  assert_string_equal(request->headers->pairs[index]->key, "user-agent");
  assert_string_equal(request->headers->pairs[index]->value, "MyBrowser");
  index++;
  assert_string_equal(request->headers->pairs[index]->key, "authorization");
  assert_string_equal(request->headers->pairs[index]->value, "Basic 123");
  index++;
  assert_string_equal(request->headers->pairs[index]->key, "connection");
  assert_string_equal(request->headers->pairs[index]->value, "close");
  index++;
  assert_string_equal(request->headers->pairs[index]->key, "content-length");
  assert_string_equal(request->headers->pairs[index]->value, "11");
  index++;
  assert_string_equal(request->headers->pairs[index]->key, "content-type");
  assert_string_equal(request->headers->pairs[index]->value, "application/x-www-form-urlencoded");

  hs_types_release_http_request(request);
  close(socket);
  fsio_remove(filename);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

