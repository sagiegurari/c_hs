#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct HSRouteServeResponse *_test_serve(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  assert_true(route != NULL);
  assert_true(request != NULL);
  assert_true(socket);

  struct HSRouteServeResponse *response = hs_route_new_serve_response();

  response->code = HS_HTTP_RESPONSE_CODE_OK;

  return(response);
}


void test_impl()
{
  struct HSRouter      *router = hs_router_new();

  struct HSHttpRequest *request = hs_types_new_http_request();

  request->resource = strdup("/test");
  request->method   = HS_HTTP_METHOD_GET;

  struct HSRoute *route = hs_route_new_route();
  route->path   = strdup("/test");
  route->serve  = _test_serve;
  route->is_get = true;
  hs_router_add_route(router, route);

  char *filename = "./test_router_serve_multi_routes.txt";

  fsio_create_empty_file(filename);
  int socket = open(filename, O_WRONLY);

  request->state.closed_connection = false;
  bool done = hs_router_serve(router, request, socket);
  assert_true(done);

  close(socket);

  char *content = fsio_read_text_file(filename);

  assert_string_equal(content, "HTTP/1.1 200 200\r\n"
                      "Connection: close\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n");

  hs_io_free(content);
  fsio_remove(filename);

  route->is_get                    = false;
  route->is_post                   = true;
  request->state.closed_connection = false;
  done                             = hs_router_serve(router, request, socket);
  assert_true(!done);

  route         = hs_route_new_route();
  route->path   = strdup("/test");
  route->serve  = _test_serve;
  route->is_get = true;
  hs_router_add_route(router, route);

  fsio_create_empty_file(filename);
  socket = open(filename, O_WRONLY);

  request->state.closed_connection = false;
  done                             = hs_router_serve(router, request, socket);
  assert_true(done);

  close(socket);

  content = fsio_read_text_file(filename);

  assert_string_equal(content, "HTTP/1.1 200 200\r\n"
                      "Connection: close\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n");

  hs_io_free(content);
  fsio_remove(filename);

  route->is_get                    = false;
  route->is_post                   = true;
  request->state.closed_connection = false;
  done                             = hs_router_serve(router, request, socket);
  assert_true(!done);

  route = hs_routes_new_404_route();
  hs_router_add_route(router, route);

  fsio_create_empty_file(filename);
  socket = open(filename, O_WRONLY);

  request->state.closed_connection = false;
  done                             = hs_router_serve(router, request, socket);
  assert_true(done);

  close(socket);

  content = fsio_read_text_file(filename);

  assert_string_equal(content, "HTTP/1.1 404 404\r\n"
                      "Connection: close\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n");

  hs_io_free(content);
  fsio_remove(filename);

  fsio_create_empty_file(filename);
  socket = open(filename, O_WRONLY);

  request->state.closed_connection = true;
  done                             = hs_router_serve(router, request, socket);
  assert_true(done);

  close(socket);

  content = fsio_read_text_file(filename);

  assert_string_equal(content, "");

  hs_io_free(content);
  fsio_remove(filename);

  hs_types_release_http_request(request);
  hs_router_release(router);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

