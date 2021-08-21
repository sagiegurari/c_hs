#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct HSRouteRedirectResponse *_test_redirect(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  assert_true(route != NULL);
  assert_true(request != NULL);
  assert_true(socket);

  struct HSRouteRedirectResponse *response = hs_route_new_redirect_response();

  response->path = strdup("/mylocation");

  return(response);
}


struct HSRouteServeResponse *_test_serve(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  assert_true(route != NULL);
  assert_true(request != NULL);
  assert_true(socket);

  struct HSRouteServeResponse *response = hs_route_new_serve_response();

  response->code           = HS_HTTP_RESPONSE_CODE_OK;
  response->content_string = strdup("my html");

  return(response);
}


void _test_with_values(struct HSRouter *router, int socket, char *request)
{
  bool done = hs_io_write_string_to_socket(socket, request);

  assert_true(done);
  lseek(socket, strlen(request) * -1, SEEK_END);

  done = hs_router_serve_next(router, socket);
  assert_true(done);
}


void test_impl()
{
  struct HSRouter *router = hs_router_new();

  struct HSRoute  *route = hs_route_new_route();

  route->is_get   = true;
  route->path     = strdup("/gohome");
  route->redirect = _test_redirect;
  hs_router_add_route(router, route);

  route         = hs_route_new_route();
  route->is_get = true;
  route->path   = strdup("/index.html");
  route->serve  = _test_serve;
  hs_router_add_route(router, route);

  route       = hs_router_as_route(router);
  route->path = strdup("/admin/");

  router = hs_router_new();
  hs_router_add_route(router, route);
  route = hs_routes_new_404_route();
  hs_router_add_route(router, route);

  char *filename = "./test_router_serve_next.txt";
  fsio_create_empty_file(filename);
  int  socket = open(filename, O_RDWR);

  _test_with_values(router, socket, "GET /test HTTP/1.0\r\n"
                    "Connection: keep-alive\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, socket, "GET /admin HTTP/1.0\r\n"
                    "Connection: keep-alive\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, socket, "GET /admin/ HTTP/1.0\r\n"
                    "Connection: keep-alive\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, socket, "GET /admin/gohome HTTP/1.0\r\n"
                    "Connection: keep-alive\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, socket, "GET /admin/index.html HTTP/1.0\r\n"
                    "Connection: keep-alive\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  close(socket);

  char *content = fsio_read_text_file(filename);
  fsio_remove(filename);
  assert_string_equal(content, "GET /test HTTP/1.0\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 404 404\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "GET /admin HTTP/1.0\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 404 404\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "GET /admin/ HTTP/1.0\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 404 404\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "GET /admin/gohome HTTP/1.0\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 307 307\r\n"
                      "Connection: keep-alive\r\n"
                      "Location: /mylocation\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "GET /admin/index.html HTTP/1.0\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 200 200\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 7\r\n"
                      "\r\n"
                      "my html");
  hs_io_free(content);

  hs_router_release(router);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

