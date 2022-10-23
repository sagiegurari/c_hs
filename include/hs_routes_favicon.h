#ifndef HS_ROUTES_FAVICON_H
#define HS_ROUTES_FAVICON_H

#include "hs_route.h"

/**
 * Returns a new route which serves the provided file as the favicon.
 * The provided string will be released with the route.
 */
struct HSRoute *hs_routes_favicon_route_new(char * /* file path */, int /* max age in seconds */);

#endif

