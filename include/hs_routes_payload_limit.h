#ifndef __HS_ROUTES_PAYLOAD_LIMIT_H__
#define __HS_ROUTES_PAYLOAD_LIMIT_H__

#include "hs_route.h"

/**
 * Enables to limit the payload size.
 * Requests with content length which is bigger will get rejected
 * with a 413 error.
 */
struct HSRoute *hs_routes_payload_limit_route_new(size_t);

#endif

