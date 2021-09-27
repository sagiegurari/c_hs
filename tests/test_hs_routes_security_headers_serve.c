#include "fsio.h"
#include "test.h"
#include <string.h>


void test_impl()
{
  struct HSServeFlowParams *params  = hs_types_new_serve_flow_params();
  struct HSRoute           *route   = hs_routes_security_headers_route_new(NULL);
  enum HSServeFlowResponse response = route->serve(route, params);

  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "X-Frame-Options"), "DENY");
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "X-Content-Type-Options"), "nosniff");
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "Referrer-Policy"), "same-origin");
  assert_true(hs_types_array_string_pair_get_by_key(params->response->headers, "Content-Policy") == NULL);
  assert_true(hs_types_array_string_pair_get_by_key(params->response->headers, "Permissions-Policy") == NULL);
  hs_types_release_serve_flow_params(params);
  hs_route_release_route(route);

  params = hs_types_new_serve_flow_params();
  struct HSRoutesSecurityResponseHeaders *headers = hs_routes_security_headers_response_headers_new();
  headers->x_frame_options         = HS_X_FRAME_OPTIONS_RESPONSE_HEADER_SAMEORIGIN;
  headers->x_content_type_options  = HS_X_CONTENT_TYPE_OPTIONS_RESPONSE_HEADER_NONE;
  headers->referrer_policy         = HS_REFERRER_POLICY_RESPONSE_HEADER_ORIGIN_WHEN_CROSS_ORIGIN;
  headers->content_security_policy = strdup("test content policy");
  headers->permissions_policy      = strdup("test permission policy");
  route                            = hs_routes_security_headers_route_new(headers);
  response                         = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "X-Frame-Options"), "SAMEORIGIN");
  assert_true(hs_types_array_string_pair_get_by_key(params->response->headers, "X-Content-Type-Options") == NULL);
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "Referrer-Policy"), "origin-when-cross-origin");
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "Content-Security-Policy"), "test content policy");
  assert_string_equal(hs_types_array_string_pair_get_by_key(params->response->headers, "Permissions-Policy"), "test permission policy");
  hs_types_release_serve_flow_params(params);
  hs_route_release_route(route);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

