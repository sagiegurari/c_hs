#ifndef __HS_ROUTES_ERROR_H__
#define __HS_ROUTES_ERROR_H__

#include "hs_route.h"

/**
 * Simple route which returns 404 when invoked.
 */
struct HSRoute *hs_routes_error_404_route_new(void);

#endif

