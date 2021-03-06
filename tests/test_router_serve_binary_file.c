#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

enum HSServeFlowResponse _test_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);
  assert_true(fsio_file_exists(TEST_BINARY_FILE));

  params->response->code      = HS_HTTP_RESPONSE_CODE_OK;
  params->response->mime_type = HS_MIME_TYPE_IMAGE_PNG;

  params->response->content_file = strdup(TEST_BINARY_FILE);

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

  struct HSHttpRequest *request = hs_types_http_request_new();
  request->resource   = strdup("/test");
  request->method     = HS_HTTP_METHOD_GET;
  request->connection = HS_CONNECTION_TYPE_CLOSE;

  char *filename = "./test_router_serve_file.png";

  fsio_create_empty_file(filename);
  test_generate_binary_file();
  int                            socket    = open(filename, O_WRONLY);
  struct HSSocket                *hssocket = hs_socket_plain_new(socket);

  struct HSServerConnectionState *connection_state = hs_types_server_connection_state_new();
  connection_state->socket = hssocket;
  struct HSServeFlowParams       *params = hs_types_serve_flow_params_new_pre_populated(request);
  params->connection_state = connection_state;

  bool done = hs_router_serve(router, params);
  assert_true(done);

  hs_socket_close_and_release(hssocket);

  char                *content     = fsio_read_binary_file(filename);
  char                *png_content = fsio_read_binary_file(TEST_BINARY_FILE);

  struct StringBuffer *buffer = stringbuffer_new();
  stringbuffer_append_string(buffer, "HTTP/1.1 200 200\r\n"
                             "Connection: close\r\n"
                             "Content-Type: image/png\r\n"
                             "Content-Length: 500\r\n"
                             "\r\n");
  stringbuffer_append_string(buffer, png_content);
  char *expected_content = stringbuffer_to_string(buffer);
  stringbuffer_release(buffer);
  hs_io_free(png_content);

  assert_string_equal(content, expected_content);

  hs_io_free(expected_content);
  hs_io_free(content);
  fsio_remove(filename);
  fsio_remove(TEST_BINARY_FILE);
  hs_types_serve_flow_params_release(params);
  hs_types_server_connection_state_release(connection_state);
  hs_router_release(router);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

