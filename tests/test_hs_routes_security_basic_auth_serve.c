#include "fsio.h"
#include "test.h"
#include <string.h>


bool _test_auth(char *value, void *context)
{
  assert_string_equal("test context", (char *)context);
  assert_string_equal("test value", value);

  return(true);
}


void test_impl()
{
  struct HSServeFlowParams *params  = hs_types_serve_flow_params_new();
  struct HSRoute           *route   = hs_routes_security_basic_auth_route_new(strdup("Test Realm"), _test_auth, "test context");
  enum HSServeFlowResponse response = route->serve(route, params);

  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_UNAUTHORIZED);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "WWW-Authenticate"), "Basic realm=\"Test Realm\"");
  hs_types_serve_flow_params_release(params);

  params                         = hs_types_serve_flow_params_new();
  params->request->authorization = strdup("Basic test value");
  response                       = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  hs_types_serve_flow_params_release(params);

  hs_route_release_route(route);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

