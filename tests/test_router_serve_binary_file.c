#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEST_FILE    "../../examples/files/test.png"

enum HSServeFlowResponse _test_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);
  assert_true(fsio_file_exists(TEST_FILE));

  params->response->code      = HS_HTTP_RESPONSE_CODE_OK;
  params->response->mime_type = HS_MIME_TYPE_IMAGE_PNG;

  params->response->content_file = strdup(TEST_FILE);

  return(HS_SERVE_FLOW_RESPONSE_DONE);
} /* _test_serve */


void test_impl()
{
  struct HSRouter *router = hs_router_new();

  struct HSRoute  *route = hs_route_new();

  route->path   = strdup("/test");
  route->serve  = _test_serve;
  route->is_get = true;
  hs_router_add_route(router, route);

  struct HSHttpRequest *request = hs_types_new_http_request();
  request->resource   = strdup("/test");
  request->method     = HS_HTTP_METHOD_GET;
  request->connection = HS_CONNECTION_TYPE_CLOSE;

  char *filename = "./test_router_serve_file.png";

  fsio_create_empty_file(filename);
  int                      socket = open(filename, O_WRONLY);

  struct HSServeFlowParams *params = hs_types_new_serve_flow_params_pre_populated(request);
  params->socket = socket;

  bool done = hs_router_serve(router, params);
  assert_true(done);

  close(socket);

  char                *content     = fsio_read_binary_file(filename);
  char                *png_content = fsio_read_binary_file(TEST_FILE);

  struct StringBuffer *buffer = string_buffer_new();
  string_buffer_append_string(buffer, "HTTP/1.1 200 200\r\n"
                              "Connection: close\r\n"
                              "Content-Type: image/png\r\n"
                              "Content-Length: 1293\r\n"
                              "\r\n");
  string_buffer_append_string(buffer, png_content);
  char *expected_content = string_buffer_to_string(buffer);
  string_buffer_release(buffer);
  hs_io_free(png_content);

  assert_string_equal(content, expected_content);

  hs_io_free(expected_content);
  hs_io_free(content);
  fsio_remove(filename);
  hs_types_release_serve_flow_params(params);
  hs_router_release(router);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

