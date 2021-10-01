#include "fsio.h"
#include "test.h"
#include <string.h>


void test_impl()
{
  struct HSServeFlowParams *params  = hs_types_serve_flow_params_new();
  struct HSRoute           *route   = hs_routes_static_route_new(strdup("test\nsecond line"), HS_MIME_TYPE_TEXT_XML);
  enum HSServeFlowResponse response = route->serve(route, params);

  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_OK);
  assert_string_equal(params->response->content_string, "test\nsecond line");
  assert_num_equal(params->response->mime_type, HS_MIME_TYPE_TEXT_XML);
  hs_route_release_route(route);
  hs_types_serve_flow_params_release(params);

  params   = hs_types_serve_flow_params_new();
  route    = hs_routes_static_text_route_new(strdup("test\nsecond line"));
  response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_OK);
  assert_string_equal(params->response->content_string, "test\nsecond line");
  assert_num_equal(params->response->mime_type, HS_MIME_TYPE_TEXT_PLAIN);
  hs_route_release_route(route);
  hs_types_serve_flow_params_release(params);

  params   = hs_types_serve_flow_params_new();
  route    = hs_routes_static_html_route_new(strdup("test\nsecond line"));
  response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_OK);
  assert_string_equal(params->response->content_string, "test\nsecond line");
  assert_num_equal(params->response->mime_type, HS_MIME_TYPE_TEXT_HTML);
  hs_route_release_route(route);
  hs_types_serve_flow_params_release(params);

  params   = hs_types_serve_flow_params_new();
  route    = hs_routes_static_css_route_new(strdup("test\nsecond line"));
  response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_OK);
  assert_string_equal(params->response->content_string, "test\nsecond line");
  assert_num_equal(params->response->mime_type, HS_MIME_TYPE_TEXT_CSS);
  hs_route_release_route(route);
  hs_types_serve_flow_params_release(params);

  params   = hs_types_serve_flow_params_new();
  route    = hs_routes_static_js_route_new(strdup("test\nsecond line"));
  response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_DONE);
  assert_num_equal(params->response->code, HS_HTTP_RESPONSE_CODE_OK);
  assert_string_equal(params->response->content_string, "test\nsecond line");
  assert_num_equal(params->response->mime_type, HS_MIME_TYPE_TEXT_JAVASCRIPT);
  hs_route_release_route(route);
  hs_types_serve_flow_params_release(params);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

