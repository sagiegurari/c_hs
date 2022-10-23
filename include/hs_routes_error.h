#ifndef HS_ROUTES_ERROR_H
#define HS_ROUTES_ERROR_H

#include "hs_route.h"

/**
 * Simple route which returns 404 when invoked.
 */
struct HSRoute *hs_routes_error_404_not_found_route_new(void);

/**
 * Validates content length header is provided and if not returns 411.
 */
struct HSRoute *hs_routes_error_411_length_required_route_new(void);

#endif

