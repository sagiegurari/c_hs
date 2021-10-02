#include "hs_constants.h"
#include "hs_routes_common.h"
#include "hs_routes_ratelimit.h"
#include <stdlib.h>

struct HSRoute           *_hs_routes_ratelimit_base_route_new(size_t);
enum HSServeFlowResponse _hs_routes_ratelimit_max_connection_requests_route_serve(struct HSRoute *, struct HSServeFlowParams *);
enum HSServeFlowResponse _hs_routes_ratelimit_max_connection_time_route_serve(struct HSRoute *, struct HSServeFlowParams *);

struct HSRoutesRateLimitValue
{
  size_t value;
};

struct HSRoute *hs_routes_ratelimit_max_connection_requests_route_new(size_t max_requests)
{
  struct HSRoute *route = _hs_routes_ratelimit_base_route_new(max_requests);

  route->serve = _hs_routes_ratelimit_max_connection_requests_route_serve;

  return(route);
}

struct HSRoute *hs_routes_ratelimit_max_connection_time_route_new(size_t max_time_seconds)
{
  struct HSRoute *route = _hs_routes_ratelimit_base_route_new(max_time_seconds);

  route->serve = _hs_routes_ratelimit_max_connection_time_route_serve;

  return(route);
}

struct HSRoute *_hs_routes_ratelimit_base_route_new(size_t value)
{
  struct HSRoute *route = hs_routes_common_serve_all_route_new();

  route->release = hs_routes_common_extension_release;

  struct HSRoutesRateLimitValue *extension = malloc(sizeof(struct HSRoutesRateLimitValue));
  extension->value = value;
  route->extension = extension;

  return(route);
}

enum HSServeFlowResponse _hs_routes_ratelimit_max_connection_requests_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (  route == NULL
     || params == NULL
     || params->request == NULL
     || params->connection_state == NULL
     || route->extension == NULL
        )
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HSRoutesRateLimitValue *extension = (struct HSRoutesRateLimitValue *)route->extension;
  if (extension->value < 2 || params->connection_state->request_counter >= extension->value)
  {
    params->request->connection = HS_CONNECTION_TYPE_CLOSE;
  }

  return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
}

enum HSServeFlowResponse _hs_routes_ratelimit_max_connection_time_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (  route == NULL
     || params == NULL
     || params->request == NULL
     || params->connection_state == NULL
     || route->extension == NULL
        )
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HSRoutesRateLimitValue *extension      = (struct HSRoutesRateLimitValue *)route->extension;
  time_t                        connection_time = time(NULL) - params->connection_state->creation_time;
  if (extension->value < 1 || connection_time < 0 || (size_t)connection_time >= extension->value)
  {
    params->request->connection = HS_CONNECTION_TYPE_CLOSE;
  }

  return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
}

