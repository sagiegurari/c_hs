#ifndef HS_ROUTES_SECURITY_H
#define HS_ROUTES_SECURITY_H

#include "hs_route.h"

struct HSRoutesSecurityResponseHeaders
{
  enum HSXFrameOptionsReponseHeader        x_frame_options;
  enum HSXContentTypeOptionsResponseHeader x_content_type_options;
  enum HSReferrerPolicyResponseHeader      referrer_policy;
  char                                     *content_security_policy;
  char                                     *permissions_policy;
};

/**
 * Returns a new instance of the security response headers with default values.
 */
struct HSRoutesSecurityResponseHeaders *hs_routes_security_headers_response_headers_new();

/**
 * Adds security related response headers.
 * If no headers are provided, the default will be used.
 * The headers struct and content will be released with the route.
 */
struct HSRoute *hs_routes_security_headers_route_new(struct HSRoutesSecurityResponseHeaders *);

/**
 * Returns a new basic auth route.
 * Only the realm value will be released when the route is released.
 */
struct HSRoute *hs_routes_security_basic_auth_route_new(char * /* realm */, bool (*auth)(char * /* base64 auth value */, void * /* context */), void * /* context */);

#endif

