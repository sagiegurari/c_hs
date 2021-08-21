#include "test.h"


void test_impl()
{
  struct HSRoute       *route   = hs_route_new_route();
  struct HSHttpRequest *request = hs_types_new_http_request();

  assert_true(!hs_route_is_supported_path(route, request));

  route->path = "/";
  assert_true(!hs_route_is_supported_path(route, request));

  route->path       = NULL;
  request->resource = "/";
  assert_true(hs_route_is_supported_path(route, request));

  route->path = "/";
  assert_true(hs_route_is_supported_path(route, request));

  route->path       = "/test/test/";
  request->resource = "/test/test/";
  assert_true(hs_route_is_supported_path(route, request));

  route->path       = "/test/test/";
  request->resource = "/test/test/test/";
  assert_true(!hs_route_is_supported_path(route, request));

  route->is_parent_path = true;
  assert_true(hs_route_is_supported_path(route, request));

  route->is_parent_path = false;
  route->path           = "/test1/test/";
  request->resource     = "/test2/test/";

  assert_true(!hs_route_is_supported_path(route, request));

  route->is_parent_path = true;
  route->path           = "";
  request->resource     = "/test2/test/";
  assert_true(!hs_route_is_supported_path(route, request));

  route->is_parent_path = true;
  route->path           = "test2/test/";
  request->resource     = "test2/test/";
  assert_true(!hs_route_is_supported_path(route, request));

  route->is_parent_path = false;
  route->path           = "/test2/test/index.html";
  request->resource     = "/test2/test/index.html";
  assert_true(hs_route_is_supported_path(route, request));

  route->is_parent_path = true;
  route->path           = "/test2/test/index.html";
  request->resource     = "/test2/test/index.html";
  assert_true(hs_route_is_supported_path(route, request));

  route->is_parent_path = true;
  route->path           = "/test2/test/index.html/";
  request->resource     = "/test2/test/index.html/abc";
  assert_true(hs_route_is_supported_path(route, request));

  route->is_parent_path = true;
  route->path           = "/test2/test/index.html";
  request->resource     = "/test2/test/index.html/abc";
  assert_true(hs_route_is_supported_path(route, request));

  route->path       = NULL;
  request->resource = NULL;
  hs_route_release_route(route);
  hs_types_release_http_request(request);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

