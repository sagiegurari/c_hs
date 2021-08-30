#ifndef __HS_ROUTES_AUTH_H__
#define __HS_ROUTES_AUTH_H__

#include "hs_route.h"

/**
 * Returns a new basic auth route.
 * None of the input parameters will be released when the route is released.
 */
struct HSRoute *hg_routes_auth_basic_new(char * /* realm */, bool (*auth)(char * /* base64 auth value */, void * /* context */), void * /* context */);

#endif

