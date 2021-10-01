#include "test.h"


void test_impl()
{
  struct HSRoute           *route  = hs_routes_payload_limit_route_new(20);
  struct HSServeFlowParams *params = hs_types_serve_flow_params_new();

  params->request->content_length = 0;
  params->response->code          = HS_HTTP_RESPONSE_CODE_OK;
  enum HSServeFlowResponse response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_OK);

  params->request->content_length = 20;
  params->response->code          = HS_HTTP_RESPONSE_CODE_OK;
  response                        = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_OK);

  params->request->content_length = 21;
  params->response->code          = HS_HTTP_RESPONSE_CODE_OK;
  response                        = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_PAYLOAD_TOO_LARGE);

  hs_types_serve_flow_params_release(params);
  hs_route_release_route(route);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

