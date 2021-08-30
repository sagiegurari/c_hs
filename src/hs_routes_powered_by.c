#include "hs_routes_powered_by.h"
#include <string.h>

enum HSServeFlowResponse _hs_routes_powered_by_serve(struct HSRoute *, struct HSServeFlowParams *);

struct HSRoute           *hs_routes_powered_by_route_new(char *powered_by)
{
  struct HSRoute *route = hs_route_new();

  hs_route_set_all_methods(route, true);
  route->is_parent_path = true;

  route->extension = powered_by;
  if (route->extension == NULL)
  {
    route->extension = HS_ROUTES_POWERED_BY;
  }
  route->serve = _hs_routes_powered_by_serve;

  return(route);
}

enum HSServeFlowResponse _hs_routes_powered_by_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || params == NULL || params->response == NULL || params->response->headers == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  char *value = (char *)route->extension;

  hs_types_key_value_array_add(params->response->headers, strdup("X-Powered-By"), strdup(value));

  return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
}

