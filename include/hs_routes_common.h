#ifndef __HS_ROUTES_COMMON_H__
#define __HS_ROUTES_COMMON_H__

#include "hs_route.h"

/**
 * Simply common utility for routes to have a simple release function that
 * does free on the extension member.
 */
void hs_routes_common_extension_release(struct HSRoute *);

#endif

