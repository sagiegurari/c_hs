#include "hs_io.h"
#include "hs_routes_common.h"
#include "hs_routes_payload_limit.h"
#include <stdlib.h>

static enum HSServeFlowResponse _hs_routes_payload_limit_serve(struct HSRoute *, struct HSServeFlowParams *);
static void _hs_routes_payload_limit_release(struct HSRoute *);

struct HSRoute *hs_routes_payload_limit_route_new(size_t size_limit)
{
  struct HSRoute *route = hs_routes_common_serve_all_route_new();

  // 0 means, no limit
  if (size_limit)
  {
    size_t *ptr = malloc(sizeof(size_t));
    *ptr             = size_limit;
    route->extension = ptr;
    route->serve     = _hs_routes_payload_limit_serve;
    route->release   = _hs_routes_payload_limit_release;
  }

  return(route);
}

static enum HSServeFlowResponse _hs_routes_payload_limit_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || route->extension == NULL || params == NULL || params->request == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  size_t *ptr       = (size_t *)route->extension;
  size_t size_limit = *ptr;

  if (params->request->content_length <= size_limit)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  params->response->code = HS_HTTP_RESPONSE_CODE_PAYLOAD_TOO_LARGE;
  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


static void _hs_routes_payload_limit_release(struct HSRoute *route)
{
  if (route == NULL)
  {
    return;
  }

  hs_io_free(route->extension);
}

