#include "hs_io.h"
#include "hs_route.h"
#include "stringfn.h"
#include <stdlib.h>
#include <string.h>

struct HSRoute *hs_route_new()
{
  struct HSRoute *route = malloc(sizeof(struct HSRoute));

  route->path           = NULL;
  route->is_parent_path = false;
  hs_route_set_all_methods(route, false);
  route->serve     = NULL;
  route->release   = NULL;
  route->extension = NULL;

  return(route);
}


void hs_route_release_route(struct HSRoute *route)
{
  if (route == NULL)
  {
    return;
  }

  if (route->release != NULL)
  {
    route->release(route);
  }

  hs_io_free(route->path);

  hs_io_free(route);
}


void hs_route_set_all_methods(struct HSRoute *route, bool enable)
{
  if (route == NULL)
  {
    return;
  }

  route->is_get     = enable;
  route->is_post    = enable;
  route->is_put     = enable;
  route->is_delete  = enable;
  route->is_head    = enable;
  route->is_connect = enable;
  route->is_options = enable;
  route->is_trace   = enable;
  route->is_patch   = enable;
}


bool hs_route_is_allowed_for_method(struct HSRoute *route, struct HSHttpRequest *request)
{
  if (route == NULL || request == NULL)
  {
    return(false);
  }

  if (route->is_get && request->method == HS_HTTP_METHOD_GET)
  {
    return(true);
  }
  if (route->is_post && request->method == HS_HTTP_METHOD_POST)
  {
    return(true);
  }
  if (route->is_put && request->method == HS_HTTP_METHOD_PUT)
  {
    return(true);
  }
  if (route->is_delete && request->method == HS_HTTP_METHOD_DELETE)
  {
    return(true);
  }
  if (route->is_head && request->method == HS_HTTP_METHOD_HEAD)
  {
    return(true);
  }
  if (route->is_connect && request->method == HS_HTTP_METHOD_CONNECT)
  {
    return(true);
  }
  if (route->is_options && request->method == HS_HTTP_METHOD_OPTIONS)
  {
    return(true);
  }
  if (route->is_trace && request->method == HS_HTTP_METHOD_TRACE)
  {
    return(true);
  }
  if (route->is_patch && request->method == HS_HTTP_METHOD_PATCH)
  {
    return(true);
  }

  return(false);
} /* hs_route_is_allowed_for_method */


bool hs_route_is_supported_path(struct HSRoute *route, struct HSHttpRequest *request)
{
  if (route == NULL || request == NULL || request->resource == NULL)
  {
    return(false);
  }

  // if route does not define path, it is catch all
  if (route->path == NULL)
  {
    return(true);
  }

  // skip invalid route paths
  size_t route_length = strlen(route->path);
  if (!route_length || route->path[0] != '/')
  {
    return(false);
  }

  // request resource is shorter than the route path
  size_t request_length = strlen(request->resource);
  if (request_length < route_length)
  {
    return(false);
  }

  if (request_length == route_length)
  {
    return(stringfn_equal(route->path, request->resource));
  }

  if (  !route->is_parent_path
     || (  request->resource[route_length - 1] != '/'
        && request->resource[route_length] != '/'))
  {
    return(false);
  }

  return(stringfn_starts_with(request->resource, route->path));
}

