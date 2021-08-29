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
  struct HSRoute *route = hs_route_new();

  hs_route_release_route(route);

  route          = hs_route_new();
  route->path    = stringfn_new_empty_string();
  route->release = _test_release_fn;

  hs_route_release_route(route);

  assert_num_equal(count, 1);
}


int main()
{
  test_run(test_impl);
}

