#include "test.h"


void test_impl()
{
  struct HSRoute       *route   = hs_route_new();
  struct HSHttpRequest *request = hs_types_http_request_new();

  route->is_get   = true;
  request->method = HS_HTTP_METHOD_GET;
  assert_true(hs_route_is_allowed_for_method(route, request));
  route->is_get = false;
  assert_true(!hs_route_is_allowed_for_method(route, request));

  route->is_post  = true;
  request->method = HS_HTTP_METHOD_POST;
  assert_true(hs_route_is_allowed_for_method(route, request));
  route->is_post = false;
  assert_true(!hs_route_is_allowed_for_method(route, request));

  route->is_put   = true;
  request->method = HS_HTTP_METHOD_PUT;
  assert_true(hs_route_is_allowed_for_method(route, request));
  route->is_put = false;
  assert_true(!hs_route_is_allowed_for_method(route, request));

  route->is_delete = true;
  request->method  = HS_HTTP_METHOD_DELETE;
  assert_true(hs_route_is_allowed_for_method(route, request));
  route->is_delete = false;
  assert_true(!hs_route_is_allowed_for_method(route, request));

  hs_route_release_route(route);
  hs_types_http_request_release(request);
}


int main()
{
  test_run(test_impl);
}

