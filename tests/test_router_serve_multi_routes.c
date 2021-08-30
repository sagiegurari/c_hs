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

  params->response->code = HS_HTTP_RESPONSE_CODE_OK;

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


void _test_with_values(struct HSRouter *router, struct HSRoute *route, bool is_get, bool is_post, bool closed, char *expected_response)
{
  route->is_get  = is_get;
  route->is_post = is_post;

  char                     *filename = "./test_router_serve_multi_routes.txt";
  fsio_create_empty_file(filename);
  int                      socket = open(filename, O_WRONLY);

  struct HSServeFlowParams *params = hs_types_new_serve_flow_params();
  params->socket            = socket;
  params->request->resource = strdup("/test");
  params->request->method   = HS_HTTP_METHOD_GET;

  params->router_state->closed_connection = closed;
  bool done = hs_router_serve(router, params);

  if (expected_response == NULL || closed)
  {
    assert_true(!done);
  }
  else
  {
    assert_true(done);
  }

  close(socket);

  if (expected_response != NULL)
  {
    char *content = fsio_read_text_file(filename);

    assert_string_equal(content, expected_response);

    hs_io_free(content);
  }

  fsio_remove(filename);
  hs_types_release_serve_flow_params(params);
} /* _test_with_values */


void test_impl()
{
  struct HSRouter *router = hs_router_new();

  hs_router_add_route(router, hs_routes_powered_by_route_new(NULL));

  struct HSRoute *route = hs_route_new();

  route->path   = strdup("/test");
  route->serve  = _test_serve;
  route->is_get = true;
  hs_router_add_route(router, route);

  _test_with_values(router, route, true, false, false, "HTTP/1.1 200 200\r\n"
                    "X-Powered-By: CHS\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, route, false, true, false, NULL);

  _test_with_values(router, route, true, false, false, "HTTP/1.1 200 200\r\n"
                    "X-Powered-By: CHS\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, route, false, true, false, NULL);

  hs_router_add_route(router, hs_routes_error_404_route_new());

  _test_with_values(router, route, false, false, false, "HTTP/1.1 404 404\r\n"
                    "X-Powered-By: CHS\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, route, true, true, true, "");

  hs_router_release(router);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

