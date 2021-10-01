#include "fsio.h"
#include "test.h"
#include <string.h>


void test_impl()
{
  struct HSServeFlowParams *params = hs_types_serve_flow_params_new();

  test_generate_binary_file();

  struct HSRoute           *route   = hs_routes_favicon_route_new(strdup(TEST_BINARY_FILE), 60);
  enum HSServeFlowResponse response = route->serve(route, params);

  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "Cache-Control"), "public, max-age=60");
  assert_string_equal(params->response->content_file, TEST_BINARY_FILE);
  hs_route_release_route(route);

  fsio_remove(TEST_BINARY_FILE);

  route    = hs_routes_favicon_route_new(strdup(TEST_BINARY_FILE), 60);
  response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  hs_route_release_route(route);

  hs_types_serve_flow_params_release(params);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

