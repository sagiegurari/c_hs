#include "hs_io.h"
#include "hs_routes_redirection.h"
#include <stdlib.h>
#include <string.h>

enum HSServeFlowResponse _hs_routes_redirection_serve(struct HSRoute *, struct HSServeFlowParams *);
void _hs_routes_redirection_release(struct HSRoute *);

struct HsRoutesRedirectionContext
{
  char *to_path;
  bool temporary_redirect;
};

struct HSRoute *hs_routes_redirection_route_new(char *from_path, char *to_path)
{
  return(hs_routes_redirection_route_new_with_options(from_path, to_path, true));
}

struct HSRoute *hs_routes_redirection_route_new_with_options(char *from_path, char *to_path, bool temporary_redirect)
{
  if (from_path == NULL || to_path == NULL)
  {
    return(NULL);
  }

  struct HsRoutesRedirectionContext *context = malloc(sizeof(struct HsRoutesRedirectionContext));
  context->to_path            = to_path;
  context->temporary_redirect = temporary_redirect;

  struct HSRoute *route = hs_route_new();
  route->path      = from_path;
  route->is_get    = true;
  route->serve     = _hs_routes_redirection_serve;
  route->release   = _hs_routes_redirection_release;
  route->extension = context;

  return(route);
}


bool hs_routes_redirection_set_header_and_status_code(struct HSServeFlowParams *params, char *path, bool temporary_redirect)
{
  if (params == NULL || params->response == NULL || params->response->headers == NULL || path == NULL)
  {
    return(false);
  }

  params->response->code = HS_HTTP_RESPONSE_CODE_PERMANENT_REDIRECT;
  if (temporary_redirect)
  {
    params->response->code = HS_HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT;
  }

  hs_types_array_string_pair_add(params->response->headers, strdup("Location"), strdup(path));

  return(true);
}

enum HSServeFlowResponse _hs_routes_redirection_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || route->extension == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HsRoutesRedirectionContext *context = (struct HsRoutesRedirectionContext *)route->extension;
  if (!hs_routes_redirection_set_header_and_status_code(params, context->to_path, context->temporary_redirect))
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


void _hs_routes_redirection_release(struct HSRoute *route)
{
  if (route == NULL || route->extension == NULL)
  {
    return;
  }

  struct HsRoutesRedirectionContext *context = (struct HsRoutesRedirectionContext *)route->extension;

  hs_io_free(context->to_path);
  hs_io_free(context);
}

