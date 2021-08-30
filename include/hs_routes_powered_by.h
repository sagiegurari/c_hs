#ifndef __HS_ROUTES_POWERED_BY_H__
#define __HS_ROUTES_POWERED_BY_H__

#include "hs_route.h"

#define HS_ROUTES_POWERED_BY    "CHS"

/**
 * Adds the X-Powered-By response header.
 * If no value is provided, the default HS server value is returned.
 * The provided value will not be released when the route is released.
 */
struct HSRoute *hs_routes_powered_by_route_new(char *);

#endif

