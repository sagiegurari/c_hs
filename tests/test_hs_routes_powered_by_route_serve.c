#include "test.h"


void test_impl()
{
  struct HSServeFlowParams *params = hs_types_new_serve_flow_params();

  struct HSRoute           *route = hs_routes_powered_by_route_new(NULL);

  assert_string_equal((char *)route->extension, "CHS");
  hs_types_array_string_pair_remove_by_key(params->response->headers, "X-Powered-By");
  enum HSServeFlowResponse response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "X-Powered-By"), "CHS");
  hs_route_release_route(route);

  route = hs_routes_powered_by_route_new("test");
  assert_string_equal((char *)route->extension, "test");
  hs_types_array_string_pair_remove_by_key(params->response->headers, "X-Powered-By");
  response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "X-Powered-By"), "test");
  hs_route_release_route(route);

  hs_types_release_serve_flow_params(params);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

