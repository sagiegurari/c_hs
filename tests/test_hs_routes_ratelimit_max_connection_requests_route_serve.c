#include "fsio.h"
#include "test.h"
#include <string.h>


void test_impl()
{
  struct HSServeFlowParams *params = hs_types_serve_flow_params_new();

  params->connection_state = hs_types_server_connection_state_new();

  params->request->connection               = HS_CONNECTION_TYPE_KEEP_ALIVE;
  params->connection_state->request_counter = 0;
  struct HSRoute           *route   = hs_routes_ratelimit_max_connection_requests_route_new(0);
  enum HSServeFlowResponse response = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_num_equal(params->request->connection, HS_CONNECTION_TYPE_CLOSE);
  hs_route_release_route(route);

  params->request->connection               = HS_CONNECTION_TYPE_KEEP_ALIVE;
  params->connection_state->request_counter = 0;
  route                                     = hs_routes_ratelimit_max_connection_requests_route_new(1);
  response                                  = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_num_equal(params->request->connection, HS_CONNECTION_TYPE_CLOSE);
  hs_route_release_route(route);

  params->request->connection               = HS_CONNECTION_TYPE_KEEP_ALIVE;
  params->connection_state->request_counter = 0;
  route                                     = hs_routes_ratelimit_max_connection_requests_route_new(2);
  response                                  = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_num_equal(params->request->connection, HS_CONNECTION_TYPE_KEEP_ALIVE);
  hs_route_release_route(route);

  params->request->connection               = HS_CONNECTION_TYPE_KEEP_ALIVE;
  params->connection_state->request_counter = 1;
  route                                     = hs_routes_ratelimit_max_connection_requests_route_new(2);
  response                                  = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_num_equal(params->request->connection, HS_CONNECTION_TYPE_KEEP_ALIVE);
  hs_route_release_route(route);

  params->request->connection               = HS_CONNECTION_TYPE_KEEP_ALIVE;
  params->connection_state->request_counter = 2;
  route                                     = hs_routes_ratelimit_max_connection_requests_route_new(2);
  response                                  = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_num_equal(params->request->connection, HS_CONNECTION_TYPE_CLOSE);
  hs_route_release_route(route);

  params->request->connection               = HS_CONNECTION_TYPE_KEEP_ALIVE;
  params->connection_state->request_counter = 3;
  route                                     = hs_routes_ratelimit_max_connection_requests_route_new(2);
  response                                  = route->serve(route, params);
  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);
  assert_num_equal(params->request->connection, HS_CONNECTION_TYPE_CLOSE);
  hs_route_release_route(route);

  hs_types_server_connection_state_release(params->connection_state);
  hs_types_serve_flow_params_release(params);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

