#include "stringfn.h"
#include "test.h"

size_t count = 0;


void _test_release_fn(struct HSRoute *route)
{
  assert_true(route != NULL);
  count = count + 1;
}


void test_impl()
{
  struct HSRouter *router = hs_router_new();

  hs_router_release(router);

  router = hs_router_new();
  struct HSRoute *route = hs_route_new_route();
  route->path = stringfn_new_empty_string();
  hs_router_add_route(router, route);
  route            = hs_route_new_route();
  route->path      = stringfn_new_empty_string();
  route->release   = _test_release_fn;
  route->extension = "do not free";
  hs_router_add_route(router, route);
  route          = hs_route_new_route();
  route->path    = stringfn_new_empty_string();
  route->release = _test_release_fn;
  hs_router_add_route(router, route);

  hs_router_release(router);
  assert_num_equal(count, 2);
}


int main()
{
  test_run(test_impl);
}

