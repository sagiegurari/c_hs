#include "hs_routes_auth.h"
#include "hs_routes_common.h"
#include "hs_types.h"
#include "string_buffer.h"
#include "stringfn.h"
#include <stdlib.h>
#include <string.h>

struct HSRoutesBasicAuthContext
{
  char *realm;
  bool (*auth)(char *, void *);
  void *context;
};

enum HSServeFlowResponse _hs_routes_basic_auth_serve(struct HSRoute *, struct HSServeFlowParams *);

struct HSRoute           *hg_routes_auth_basic_new(char *realm, bool (*auth)(char *, void *), void *auth_context)
{
  if (realm == NULL || auth == NULL)
  {
    return(NULL);
  }

  struct HSRoute *route = hs_route_new();
  hs_route_set_all_methods(route, true);
  route->is_parent_path = true;

  struct HSRoutesBasicAuthContext *context = malloc(sizeof(struct HSRoutesBasicAuthContext));
  context->realm   = realm;
  context->auth    = auth;
  context->context = auth_context;
  route->extension = context;
  route->serve     = _hs_routes_basic_auth_serve;
  route->release   = hs_routes_common_extension_release;

  return(route);
}

enum HSServeFlowResponse _hs_routes_basic_auth_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (  route == NULL
     || params == NULL
     || params->request == NULL
     || params->response == NULL
     || route->extension == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HSRoutesBasicAuthContext *context = (struct HSRoutesBasicAuthContext *)route->extension;

  bool                            valid = false;
  if (params->request->authorization != NULL)
  {
    if (stringfn_starts_with(params->request->authorization, "Basic ") && strlen(params->request->authorization) > 6)
    {
      char *authorization = stringfn_mut_substring(params->request->authorization, 6, 0);
      valid = context->auth(authorization, context->context);

      // auth is valid, go to next route
      if (valid)
      {
        return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
      }
    }
  }

  struct StringBuffer *buffer = string_buffer_new();
  string_buffer_append_string(buffer, "Basic realm=\"");
  string_buffer_append_string(buffer, context->realm);
  string_buffer_append(buffer, '"');
  char *realm = string_buffer_to_string(buffer);

  params->response->code = HS_HTTP_RESPONSE_CODE_UNAUTHORIZED;
  hs_types_key_value_array_add(params->response->headers, strdup("WWW-Authenticate"), realm);
  params->response->content_string = strdup("Unauthenticated");

  return(HS_SERVE_FLOW_RESPONSE_DONE);
} /* _hs_routes_basic_auth_serve */

