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

enum HSServeFlowResponse _test_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);

  params->response->code              = HS_HTTP_RESPONSE_CODE_NOT_FOUND;
  params->response->headers->count    = 3;
  params->response->headers->pairs    = malloc(sizeof(struct HSKeyValue) * params->response->headers->count);
  params->response->headers->pairs[0] = hs_types_new_key_value(strdup("header1"), strdup("value1"));
  params->response->headers->pairs[1] = hs_types_new_key_value(strdup("header2"), strdup("value2"));
  params->response->headers->pairs[2] = hs_types_new_key_value(strdup("header3"), strdup("value3"));
  params->response->cookies->count    = 3;
  params->response->cookies->cookies  = malloc(sizeof(struct HSCookie) * params->response->cookies->count);
  for (size_t index = 0; index < params->response->cookies->count; index++)
  {
    params->response->cookies->cookies[index] = hs_types_new_cookie();
  }
  params->response->cookies->cookies[0]->name      = strdup("c1");
  params->response->cookies->cookies[0]->value     = strdup("v1");
  params->response->cookies->cookies[0]->expires   = strdup("1 1 1980");
  params->response->cookies->cookies[0]->max_age   = 200;
  params->response->cookies->cookies[0]->secure    = true;
  params->response->cookies->cookies[0]->http_only = true;
  params->response->cookies->cookies[0]->domain    = strdup("mydomain");
  params->response->cookies->cookies[0]->path      = strdup("/somepath");
  params->response->cookies->cookies[0]->same_site = HS_COOKIE_SAME_SITE_NONE;
  params->response->cookies->cookies[1]->name      = strdup("c2");
  params->response->cookies->cookies[1]->value     = strdup("v2");
  params->response->cookies->cookies[1]->same_site = HS_COOKIE_SAME_SITE_STRICT;
  params->response->cookies->cookies[2]->name      = strdup("c3");
  params->response->cookies->cookies[2]->value     = strdup("v3");

  params->response->mime_type = HS_MIME_TYPE_TEXT_PLAIN;

  params->response->content_string = strdup("some content\nsecond line.");

  params->callback->context = "test callback";
  params->callback->run     = _test_callback;

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

  char *filename = "./test_router_serve_text.txt";

  fsio_create_empty_file(filename);
  int                      socket = open(filename, O_WRONLY);

  struct HSServeFlowParams *params = hs_types_new_serve_flow_params();
  params->socket              = socket;
  params->request->resource   = strdup("/test");
  params->request->method     = HS_HTTP_METHOD_GET;
  params->request->connection = HS_CONNECTION_TYPE_KEEP_ALIVE;

  assert_true(!callback_called);
  bool done = hs_router_serve(router, params);
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
  hs_types_release_serve_flow_params(params);
  hs_router_release(router);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

