#include "hs_routes_error.h"

enum HSServeFlowResponse _hs_routes_404_serve(struct HSRoute *, struct HSServeFlowParams *);

struct HSRoute           *hs_routes_error_404_route_new()
{
  struct HSRoute *route = hs_route_new();

  route->serve = _hs_routes_404_serve;
  hs_route_set_all_methods(route, true);
  route->is_parent_path = true;

  return(route);
}

enum HSServeFlowResponse _hs_routes_404_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || params == NULL || params->response == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  params->response->code = HS_HTTP_RESPONSE_CODE_NOT_FOUND;

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}

