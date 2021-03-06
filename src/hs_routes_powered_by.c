#include "hs_routes_common.h"
#include "hs_routes_powered_by.h"
#include <string.h>

static enum HSServeFlowResponse _hs_routes_powered_by_serve(struct HSRoute *, struct HSServeFlowParams *);

struct HSRoute                  *hs_routes_powered_by_route_new(char *powered_by)
{
  struct HSRoute *route = hs_routes_common_serve_all_route_new();

  route->extension = powered_by;
  if (route->extension == NULL)
  {
    route->extension = HS_ROUTES_POWERED_BY;
  }
  route->serve = _hs_routes_powered_by_serve;

  return(route);
}

static enum HSServeFlowResponse _hs_routes_powered_by_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || params == NULL || params->response == NULL || params->response->headers == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  char *value = (char *)route->extension;

  hs_types_array_string_pair_add(params->response->headers, strdup("X-Powered-By"), strdup(value));

  return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
}

