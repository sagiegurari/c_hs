#include "fsio.h"
#include "test.h"
#include <string.h>


void test_impl()
{
  struct HSServeFlowParams *params  = hs_types_new_serve_flow_params();
  struct HSRoute           *route   = hs_routes_redirection_route_new(strdup("/fs"), strdup("/fs/"));
  enum HSServeFlowResponse response = route->serve(route, params);

  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "Location"), "/fs/");
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT);
  hs_route_release_route(route);
  hs_types_release_serve_flow_params(params);

  params   = hs_types_new_serve_flow_params();
  route    = hs_routes_redirection_route_new_with_options(strdup("/fs"), strdup("/fs/"), true);
  response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "Location"), "/fs/");
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT);
  hs_route_release_route(route);
  hs_types_release_serve_flow_params(params);

  params   = hs_types_new_serve_flow_params();
  route    = hs_routes_redirection_route_new_with_options(strdup("/fs"), strdup("/fs/"), false);
  response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "Location"), "/fs/");
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_PERMANENT_REDIRECT);
  hs_route_release_route(route);
  hs_types_release_serve_flow_params(params);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

