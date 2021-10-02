#ifndef __HS_ROUTES_COMMON_H__
#define __HS_ROUTES_COMMON_H__

#include "hs_route.h"

/**
 * Simply common utility for routes to have a simple release function that
 * does free on the extension member.
 */
void hs_routes_common_extension_release(struct HSRoute *);

/**
 * Returns a route that is setup to serve all methods and all paths.
 * This utility is good for middlewares that modify requests/responses.
 */
struct HSRoute *hs_routes_common_serve_all_route_new();

#endif

