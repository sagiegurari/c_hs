#include "test.h"
#include <string.h>


void test_impl()
{
  struct HSRoute           *route  = hs_routes_error_411_length_required_route_new();
  struct HSServeFlowParams *params = hs_types_serve_flow_params_new();

  params->response->code = HS_HTTP_RESPONSE_CODE_OK;
  enum HSServeFlowResponse response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_LENGTH_REQUIRED);

  hs_types_array_string_pair_add(params->request->headers, strdup("content-length"), strdup("0"));
  params->response->code = HS_HTTP_RESPONSE_CODE_OK;
  response               = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_OK);

  hs_types_serve_flow_params_release(params);
  hs_route_release_route(route);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

