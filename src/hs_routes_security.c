#include "hs_io.h"
#include "hs_routes_common.h"
#include "hs_routes_security.h"
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

enum HSServeFlowResponse _hs_routes_security_headers_route_serve(struct HSRoute *, struct HSServeFlowParams *);
void _hs_routes_security_headers_route_release(struct HSRoute *);
enum HSServeFlowResponse _hs_routes_security_basic_auth_route_serve(struct HSRoute *, struct HSServeFlowParams *);
void              _hs_routes_security_basic_auth_route_release(struct HSRoute *);

struct HSRoutesSecurityResponseHeaders *hs_routes_security_headers_response_headers_new()
{
  struct HSRoutesSecurityResponseHeaders *headers = malloc(sizeof(struct HSRoutesSecurityResponseHeaders));

  headers->x_frame_options         = HS_X_FRAME_OPTIONS_RESPONSE_HEADER_DENY;
  headers->x_content_type_options  = HS_X_CONTENT_TYPE_OPTIONS_RESPONSE_HEADER_NOSNIFF;
  headers->referrer_policy         = HS_REFERRER_POLICY_RESPONSE_HEADER_SAME_ORIGIN;
  headers->content_security_policy = NULL;
  headers->permissions_policy      = NULL;

  return(headers);
}

struct HSRoute *hs_routes_security_headers_route_new(struct HSRoutesSecurityResponseHeaders *headers)
{
  if (headers == NULL)
  {
    headers = hs_routes_security_headers_response_headers_new();
  }

  struct HSRoute *route = hs_route_new();
  route->is_parent_path = true;
  hs_route_set_all_methods(route, true);
  route->serve     = _hs_routes_security_headers_route_serve;
  route->release   = _hs_routes_security_headers_route_release;
  route->extension = headers;

  return(route);
}

struct HSRoute *hs_routes_security_basic_auth_route_new(char *realm, bool (*auth)(char *, void *), void *auth_context)
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
  route->serve     = _hs_routes_security_basic_auth_route_serve;
  route->release   = _hs_routes_security_basic_auth_route_release;

  return(route);
}

enum HSServeFlowResponse _hs_routes_security_headers_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || route->extension == NULL || params == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HSRoutesSecurityResponseHeaders *headers = (struct HSRoutesSecurityResponseHeaders *)route->extension;

  switch (headers->x_frame_options)
  {
  case HS_X_FRAME_OPTIONS_RESPONSE_HEADER_NONE:
    break;

  case HS_X_FRAME_OPTIONS_RESPONSE_HEADER_DENY:
    hs_types_array_string_pair_add(params->response->headers, strdup("X-Frame-Options"), strdup("DENY"));
    break;

  case HS_X_FRAME_OPTIONS_RESPONSE_HEADER_SAMEORIGIN:
    hs_types_array_string_pair_add(params->response->headers, strdup("X-Frame-Options"), strdup("SAMEORIGIN"));
    break;
  }

  switch (headers->x_content_type_options)
  {
  case HS_X_CONTENT_TYPE_OPTIONS_RESPONSE_HEADER_NONE:
    break;

  case HS_X_CONTENT_TYPE_OPTIONS_RESPONSE_HEADER_NOSNIFF:
    hs_types_array_string_pair_add(params->response->headers, strdup("X-Content-Type-Options"), strdup("nosniff"));
    break;
  }

  switch (headers->referrer_policy)
  {
  case HS_REFERRER_POLICY_RESPONSE_HEADER_NONE:
    break;

  case HS_REFERRER_POLICY_RESPONSE_HEADER_NO_REFERRER:
    hs_types_array_string_pair_add(params->response->headers, strdup("Referrer-Policy"), strdup("no-referrer"));
    break;

  case HS_REFERRER_POLICY_RESPONSE_HEADER_NO_REFERRER_WHEN_DOWNGRADE:
    hs_types_array_string_pair_add(params->response->headers, strdup("Referrer-Policy"), strdup("no-referrer-when-downgrade"));
    break;

  case HS_REFERRER_POLICY_RESPONSE_HEADER_ORIGIN:
    hs_types_array_string_pair_add(params->response->headers, strdup("Referrer-Policy"), strdup("origin"));
    break;

  case HS_REFERRER_POLICY_RESPONSE_HEADER_ORIGIN_WHEN_CROSS_ORIGIN:
    hs_types_array_string_pair_add(params->response->headers, strdup("Referrer-Policy"), strdup("origin-when-cross-origin"));
    break;

  case HS_REFERRER_POLICY_RESPONSE_HEADER_SAME_ORIGIN:
    hs_types_array_string_pair_add(params->response->headers, strdup("Referrer-Policy"), strdup("same-origin"));
    break;

  case HS_REFERRER_POLICY_RESPONSE_HEADER_STRICT_ORIGIN:
    hs_types_array_string_pair_add(params->response->headers, strdup("Referrer-Policy"), strdup("strict-origin"));
    break;

  case HS_REFERRER_POLICY_RESPONSE_HEADER_STRICT_ORIGIN_WHEN_CROSS_ORIGIN:
    hs_types_array_string_pair_add(params->response->headers, strdup("Referrer-Policy"), strdup("strict-origin-when-cross-origin"));
    break;

  case HS_REFERRER_POLICY_RESPONSE_HEADER_UNSAFE_URL:
    hs_types_array_string_pair_add(params->response->headers, strdup("Referrer-Policy"), strdup("unsafe-url"));
    break;
  }

  if (headers->content_security_policy != NULL)
  {
    hs_types_array_string_pair_add(params->response->headers, strdup("Content-Security-Policy"), strdup(headers->content_security_policy));
  }

  if (headers->permissions_policy != NULL)
  {
    hs_types_array_string_pair_add(params->response->headers, strdup("Permissions-Policy"), strdup(headers->permissions_policy));
  }

  return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
} /* _hs_routes_security_headers_route_serve */


void _hs_routes_security_headers_route_release(struct HSRoute *route)
{
  if (route == NULL || route->extension == NULL)
  {
    return;
  }

  struct HSRoutesSecurityResponseHeaders *headers = (struct HSRoutesSecurityResponseHeaders *)route->extension;

  hs_io_free(headers->content_security_policy);
  hs_io_free(headers->permissions_policy);
  hs_io_free(headers);
}

enum HSServeFlowResponse _hs_routes_security_basic_auth_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
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
  string_buffer_release(buffer);

  params->response->code = HS_HTTP_RESPONSE_CODE_UNAUTHORIZED;
  hs_types_array_string_pair_add(params->response->headers, strdup("WWW-Authenticate"), realm);
  params->response->content_string = strdup("Unauthenticated");

  return(HS_SERVE_FLOW_RESPONSE_DONE);
} /* _hs_routes_basic_auth_serve */


void              _hs_routes_security_basic_auth_route_release(struct HSRoute *route)
{
  if (route == NULL || route->extension == NULL)
  {
    return;
  }

  struct HSRoutesBasicAuthContext *context = (struct HSRoutesBasicAuthContext *)route->extension;

  hs_io_free(context->realm);
  hs_io_free(context);
}

