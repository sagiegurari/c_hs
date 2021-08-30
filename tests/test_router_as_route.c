#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

enum HSServeFlowResponse _test_redirect(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);

  params->response->code = HS_HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT;
  hs_types_key_value_array_add(params->response->headers, strdup("Location"), strdup("/mylocation"));

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


enum HSServeFlowResponse _test_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);

  params->response->code           = HS_HTTP_RESPONSE_CODE_OK;
  params->response->mime_type      = HS_MIME_TYPE_TEXT_HTML;
  params->response->content_string = strdup("my html");

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


void _test_with_values(struct HSRouter *router, char *request_path, char *expected_result)
{
  char                 *filename = "./test_router_as_route.txt";

  struct HSHttpRequest *request = hs_types_new_http_request();

  request->resource = strdup(request_path);
  request->method   = HS_HTTP_METHOD_GET;
  fsio_remove(filename);
  assert_true(!fsio_path_exists(filename));
  fsio_create_empty_file(filename);
  int                      socket = open(filename, O_WRONLY);

  struct HSServeFlowParams *params = hs_types_new_serve_flow_params_pre_populated(request);
  params->socket = socket;

  bool done = hs_router_serve(router, params);
  assert_true(done);
  close(socket);
  assert_string_equal(request->resource, request_path);
  hs_types_release_serve_flow_params(params);

  char *content = fsio_read_text_file(filename);
  fsio_remove(filename);
  assert_string_equal(content, expected_result);
  hs_io_free(content);
}


void test_impl()
{
  struct HSRouter *router = hs_router_new();

  struct HSRoute  *route = hs_route_new();

  route->is_get = true;
  route->path   = strdup("/gohome");
  route->serve  = _test_redirect;
  hs_router_add_route(router, route);

  route         = hs_route_new();
  route->is_get = true;
  route->path   = strdup("/index.html");
  route->serve  = _test_serve;
  hs_router_add_route(router, route);

  route       = hs_router_as_route(router);
  route->path = strdup("/admin/");

  router = hs_router_new();
  hs_router_add_route(router, route);
  route = hs_routes_error_404_route_new();
  hs_router_add_route(router, route);

  _test_with_values(router, "/test", "HTTP/1.1 404 404\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, "/admin", "HTTP/1.1 404 404\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, "/admin/", "HTTP/1.1 404 404\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, "/admin/gohome", "HTTP/1.1 307 307\r\n"
                    "Location: /mylocation\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, "/admin/index.html", "HTTP/1.1 200 200\r\n"
                    "Connection: close\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: 7\r\n"
                    "\r\n"
                    "my html");

  hs_router_release(router);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

