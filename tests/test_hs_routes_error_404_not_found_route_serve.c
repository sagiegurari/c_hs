#include "test.h"
#include <string.h>


void test_impl()
{
  struct HSRoute           *route  = hs_routes_error_404_not_found_route_new();
  struct HSServeFlowParams *params = hs_types_new_serve_flow_params();

  params->response->code = HS_HTTP_RESPONSE_CODE_OK;
  enum HSServeFlowResponse response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_NOT_FOUND);

  hs_types_release_serve_flow_params(params);
  hs_route_release_route(route);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

