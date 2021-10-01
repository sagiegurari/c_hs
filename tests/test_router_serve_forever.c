#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int                      global_counter = -2;

enum HSServeFlowResponse _test_redirect(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);

  assert_string_equal(params->router_state->base_path, "/admin/");

  params->response->code = HS_HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT;
  hs_types_array_string_pair_add(params->response->headers, strdup("Location"), strdup("/mylocation"));

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


enum HSServeFlowResponse _test_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);

  assert_string_equal(params->router_state->base_path, "/admin/");

  params->response->code           = HS_HTTP_RESPONSE_CODE_OK;
  params->response->mime_type      = HS_MIME_TYPE_TEXT_HTML;
  params->response->content_string = strdup("my html");

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


bool _test_should_stop(struct HSRouter *router, int socket, size_t counter, void *context)
{
  if (router == NULL || !socket || context == NULL)
  {
    test_fail();
    return(true);
  }

  char *context_string = (char *)context;
  assert_string_equal(context_string, "test context");

  global_counter = counter;

  char *request = NULL;
  switch (counter)
  {
  case 0:
    request = "GET /test HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    break;

  case 1:
    request = "GET /admin HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    break;

  case 2:
    request = "GET /admin/ HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    break;

  case 3:
    request = "GET /admin/gohome HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    break;

  case 4:
    request = "GET /admin/index.html HTTP/1.0\r\n"
              "Connection: close\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    break;

  default:
    test_fail();
  }

  bool done = hs_io_write_string_to_socket(socket, request, strlen(request));

  assert_true(done);
  lseek(socket, strlen(request) * -1, SEEK_END);

  return(false);
} /* _test_should_stop */


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
  route = hs_routes_error_404_not_found_route_new();
  hs_router_add_route(router, route);

  char *filename = "./test_router_serve_forever.txt";
  fsio_create_empty_file(filename);
  int  socket = open(filename, O_RDWR);

  bool done = hs_router_serve_forever(router, socket, "test context", _test_should_stop);
  assert_true(done);

  char *content = fsio_read_text_file(filename);
  fsio_remove(filename);
  assert_string_equal(content, "GET /test HTTP/1.0\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 404 404\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "GET /admin HTTP/1.0\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 404 404\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "GET /admin/ HTTP/1.0\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 404 404\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "GET /admin/gohome HTTP/1.0\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 307 307\r\n"
                      "Location: /mylocation\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "GET /admin/index.html HTTP/1.0\r\n"
                      "Connection: close\r\n"
                      "Content-Length: 0\r\n"
                      "\r\n"
                      "HTTP/1.1 200 200\r\n"
                      "Connection: close\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 7\r\n"
                      "\r\n"
                      "my html");
  hs_io_free(content);
  assert_num_equal(global_counter, 4); // the 5th closes connection so its not counted

  hs_router_release(router);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

