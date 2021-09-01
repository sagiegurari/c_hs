#include "stringfn.h"
#include "test.h"


void test_impl()
{
  char *id1 = hs_routes_session_route_generate_cookie_id(NULL);

  assert_true(id1 != NULL);
  char *id2 = hs_routes_session_route_generate_cookie_id(NULL);
  assert_true(id2 != NULL);
  assert_true(!stringfn_equal(id1, id2));

  hs_io_free(id1);
  hs_io_free(id2);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

