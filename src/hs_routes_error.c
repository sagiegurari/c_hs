#include "hs_routes_error.h"

enum HSServeFlowResponse _hs_routes_404_not_found_serve(struct HSRoute *, struct HSServeFlowParams *);
enum HSServeFlowResponse _hs_routes_411_length_required_serve(struct HSRoute *, struct HSServeFlowParams *);

struct HSRoute           *hs_routes_error_404_not_found_route_new()
{
  struct HSRoute *route = hs_route_new();

  route->serve = _hs_routes_404_not_found_serve;
  hs_route_set_all_methods(route, true);
  route->is_parent_path = true;

  return(route);
}

struct HSRoute *hs_routes_error_411_length_required_route_new()
{
  struct HSRoute *route = hs_route_new();

  route->serve          = _hs_routes_411_length_required_serve;
  route->is_post        = true;
  route->is_put         = true;
  route->is_parent_path = true;

  return(route);
}

enum HSServeFlowResponse _hs_routes_404_not_found_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || params == NULL || params->response == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  params->response->code = HS_HTTP_RESPONSE_CODE_NOT_FOUND;

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}

enum HSServeFlowResponse _hs_routes_411_length_required_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || params == NULL || params->request == NULL || params->response == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  char *value = hs_types_array_string_pair_get_by_key(params->request->headers, "content-length");
  if (value != NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  params->response->code = HS_HTTP_RESPONSE_CODE_LENGTH_REQUIRED;
  return(HS_SERVE_FLOW_RESPONSE_DONE);
}

