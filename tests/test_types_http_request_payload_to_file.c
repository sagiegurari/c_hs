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
              "Content-Length: 13\r\n"
              "Content-Type: application/x-www-form-urlencoded\r\n"
              "\r\n"
              "12345\n67890\n\nDO NOT READ THIS";

  char *filename        = "./test_types_http_request_payload_to_file.txt";
  char *output_filename = "./test_types_http_request_payload_to_file.out.txt";

  fsio_write_text_file(filename, raw);

  int                         socket    = open(filename, O_RDONLY);
  struct HSSocket             *hssocket = hs_socket_plain_new(socket);
  struct HSHttpRequest        *request  = hs_parser_parse_request(hssocket);

  struct HSHttpRequestPayload *payload = request->payload;
  assert_true(payload != NULL);

  assert_true(!hs_types_http_request_payload_is_loaded(request));
  bool done = hs_types_http_request_payload_to_file(request, output_filename);
  assert_true(hs_types_http_request_payload_is_loaded(request));
  assert_true(done);
  char *content = fsio_read_text_file(output_filename);
  assert_string_equal(content, "12345\n67890\n\n");
  hs_io_free(content);
  done = hs_types_http_request_payload_to_file(request, output_filename);
  assert_true(!done);

  hs_types_http_request_release(request);
  hs_socket_close_and_release(hssocket);
  fsio_remove(filename);
  fsio_remove(output_filename);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

