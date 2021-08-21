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

  char *filename = "./test_types_http_request_payload_to_file.txt";

  fsio_write_text_file(filename, raw);

  int                         socket   = open(filename, O_RDONLY);
  struct HSHttpRequest        *request = hs_parser_parse_request(socket);

  struct HSHttpRequestPayload *payload = request->payload;
  assert_true(payload != NULL);

  assert_true(!hs_types_http_request_payload_is_loaded(payload));
  char *content = hs_types_http_request_payload_to_string(payload);
  assert_true(hs_types_http_request_payload_is_loaded(payload));
  assert_string_equal(content, "12345\n67890\n\n");
  hs_io_free(content);
  content = hs_types_http_request_payload_to_string(payload);
  assert_true(content == NULL);

  hs_types_release_http_request(request);
  close(socket);
  fsio_remove(filename);
}


int main()
{
  test_run(test_impl);
}

