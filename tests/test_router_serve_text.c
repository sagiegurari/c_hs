#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

bool callback_called = false;


void _test_callback(struct HSPostResponseCallback *callback)
{
  assert_true(callback != NULL);
  assert_true(callback->context != NULL);
  assert_true(stringfn_equal((char *)callback->context, "test callback"));
  callback_called = true;
}

struct HSRouteServeResponse *_test_serve(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  assert_true(route != NULL);
  assert_true(request != NULL);
  assert_true(socket);

  struct HSRouteServeResponse *response = hs_route_new_serve_response();

  response->code              = HS_HTTP_RESPONSE_CODE_NOT_FOUND;
  response->headers->count    = 3;
  response->headers->pairs    = malloc(sizeof(struct HSKeyValue) * response->headers->count);
  response->headers->pairs[0] = hs_types_new_key_value(strdup("header1"), strdup("value1"));
  response->headers->pairs[1] = hs_types_new_key_value(strdup("header2"), strdup("value2"));
  response->headers->pairs[2] = hs_types_new_key_value(strdup("header3"), strdup("value3"));
  response->cookies->count    = 3;
  response->cookies->cookies  = malloc(sizeof(struct HSCookie) * response->cookies->count);
  for (size_t index = 0; index < response->cookies->count; index++)
  {
    response->cookies->cookies[index] = hs_types_new_cookie();
  }
  response->cookies->cookies[0]->name      = strdup("c1");
  response->cookies->cookies[0]->value     = strdup("v1");
  response->cookies->cookies[0]->expires   = strdup("1 1 1980");
  response->cookies->cookies[0]->max_age   = 200;
  response->cookies->cookies[0]->secure    = true;
  response->cookies->cookies[0]->http_only = true;
  response->cookies->cookies[0]->domain    = strdup("mydomain");
  response->cookies->cookies[0]->path      = strdup("/somepath");
  response->cookies->cookies[0]->same_site = HS_COOKIE_SAME_SITE_NONE;
  response->cookies->cookies[1]->name      = strdup("c2");
  response->cookies->cookies[1]->value     = strdup("v2");
  response->cookies->cookies[1]->same_site = HS_COOKIE_SAME_SITE_STRICT;
  response->cookies->cookies[2]->name      = strdup("c3");
  response->cookies->cookies[2]->value     = strdup("v3");

  response->mime_type = HS_MIME_TYPE_TEXT_PLAIN;

  response->content_string = strdup("some content\nsecond line.");

  response->callback          = hs_route_new_post_response_callback();
  response->callback->context = "test callback";
  response->callback->run     = _test_callback;

  return(response);
} /* _test_serve */


void test_impl()
{
  struct HSRouter *router = hs_router_new();

  struct HSRoute  *route = hs_route_new_route();

  route->path   = strdup("/test");
  route->serve  = _test_serve;
  route->is_get = true;
  hs_router_add_route(router, route);

  struct HSHttpRequest *request = hs_types_new_http_request();
  request->resource   = strdup("/test");
  request->method     = HS_HTTP_METHOD_GET;
  request->connection = HS_CONNECTION_TYPE_KEEP_ALIVE;

  char *filename = "./test_router_serve_text.txt";

  fsio_create_empty_file(filename);
  int socket = open(filename, O_WRONLY);

  assert_true(!callback_called);
  bool done = hs_router_serve(router, request, socket);
  assert_true(done);
  assert_true(callback_called);

  close(socket);

  char *content = fsio_read_text_file(filename);

  assert_string_equal(content, "HTTP/1.1 404 404\r\n"
                      "header1: value1\r\n"
                      "header2: value2\r\n"
                      "header3: value3\r\n"
                      "Set-Cookie: c1=v1; Expires=1 1 1980; Max-Age=200; Secure; HttpOnly; Domain=mydomain; Path=/somepath; SameSite=None\r\n"
                      "Set-Cookie: c2=v2; SameSite=Strict\r\n"
                      "Set-Cookie: c3=v3; SameSite=Lax\r\n"
                      "Connection: keep-alive\r\n"
                      "Content-Type: text/plain\r\n"
                      "Content-Length: 25\r\n"
                      "\r\n"
                      "some content\n"
                      "second line.");

  hs_io_free(content);
  fsio_remove(filename);
  hs_types_release_http_request(request);
  hs_router_release(router);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

