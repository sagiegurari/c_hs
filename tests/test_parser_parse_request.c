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
  assert_true(request->query_string == NULL);
  assert_num_equal(request->content_length, 11);
  assert_true(request->payload != NULL);

  assert_num_equal(request->headers->count, 4);
  assert_string_equal(request->headers->pairs[0]->key, "host");
  assert_string_equal(request->headers->pairs[0]->value, "MyHost");
  assert_string_equal(request->headers->pairs[1]->key, "connection");
  assert_string_equal(request->headers->pairs[1]->value, "close");
  assert_string_equal(request->headers->pairs[2]->key, "content-length");
  assert_string_equal(request->headers->pairs[2]->value, "11");
  assert_string_equal(request->headers->pairs[3]->key, "content-type");
  assert_string_equal(request->headers->pairs[3]->value, "application/x-www-form-urlencoded");

  hs_types_release_http_request(request);
  close(socket);
  fsio_remove(filename);
}


int main()
{
  test_run(test_impl);
}

