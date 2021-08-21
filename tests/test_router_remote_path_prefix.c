#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void _test_with_values(char *route_path, char *request_path, char *expected_path)
{
  struct HSRoute *route = hs_route_new_route();

  route->path = strdup(route_path);
  struct HSHttpRequest *request = hs_types_new_http_request();
  request->resource = strdup(request_path);
  char                 *value = hs_router_remove_path_prefix(route, request);
  assert_string_equal(value, request_path);
  assert_string_equal(request->resource, expected_path);
  request->resource = value;
  hs_route_release_route(route);
  hs_types_release_http_request(request);
}


void test_impl()
{
  _test_with_values("/admin", "/admin", "/");
  _test_with_values("/admin", "/admin/", "/");
  _test_with_values("/admin/", "/admin/", "/");
  _test_with_values("/admin/", "/admin/index.html", "/index.html");
}


int main()
{
  test_run(test_impl);
}

