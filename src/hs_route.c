#include "hs_io.h"
#include "hs_route.h"
#include "stringfn.h"
#include <stdlib.h>
#include <string.h>

struct HSPostResponseCallback *hs_route_new_post_response_callback()
{
  struct HSPostResponseCallback *callback = malloc(sizeof(struct HSPostResponseCallback));

  callback->context = NULL;
  callback->run     = NULL;
  callback->release = NULL;

  return(callback);
}


void hs_route_release_post_response_callback(struct HSPostResponseCallback *callback)
{
  if (callback == NULL)
  {
    return;
  }

  if (callback->release != NULL)
  {
    callback->release(callback);
  }

  hs_io_free(callback);
}

struct HSRouteRedirectResponse *hs_route_new_redirect_response()
{
  struct HSRouteRedirectResponse *response = malloc(sizeof(struct HSRouteRedirectResponse));

  response->path     = NULL;
  response->code     = HS_HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT;
  response->headers  = hs_types_new_key_value_array();
  response->cookies  = hs_types_new_cookies();
  response->callback = NULL;

  return(response);
}


void hs_route_release_redirect_response(struct HSRouteRedirectResponse *response)
{
  if (response == NULL)
  {
    return;
  }

  hs_io_free(response->path);
  hs_types_release_key_value_array(response->headers);
  hs_types_release_cookies(response->cookies);
  hs_route_release_post_response_callback(response->callback);

  hs_io_free(response);
}

struct HSRouteServeResponse *hs_route_new_serve_response()
{
  struct HSRouteServeResponse *response = malloc(sizeof(struct HSRouteServeResponse));

  response->code           = HS_HTTP_RESPONSE_CODE_OK;
  response->headers        = hs_types_new_key_value_array();
  response->cookies        = hs_types_new_cookies();
  response->mime_type      = HS_MIME_TYPE_TEXT_HTML;
  response->content_string = NULL;
  response->content_file   = NULL;
  response->callback       = NULL;

  return(response);
}


void hs_route_release_serve_response(struct HSRouteServeResponse *response)
{
  if (response == NULL)
  {
    return;
  }

  hs_types_release_key_value_array(response->headers);
  hs_types_release_cookies(response->cookies);
  hs_io_free(response->content_string);
  hs_io_free(response->content_file);
  hs_route_release_post_response_callback(response->callback);

  hs_io_free(response);
}

struct HSRoute *hs_route_new_route()
{
  struct HSRoute *route = malloc(sizeof(struct HSRoute));

  route->path           = NULL;
  route->is_parent_path = false;
  route->is_get         = false;
  route->is_post        = false;
  route->is_put         = false;
  route->is_delete      = false;
  route->redirect       = NULL;
  route->serve          = NULL;
  route->release        = NULL;
  route->extension      = NULL;

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

  return(false);
}


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

