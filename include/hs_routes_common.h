#ifndef HS_ROUTES_COMMON_H
#define HS_ROUTES_COMMON_H

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
struct HSRoute *hs_routes_common_serve_all_route_new(void);

#endif

