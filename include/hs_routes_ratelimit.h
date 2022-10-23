#ifndef HS_ROUTES_RATELIMIT_H
#define HS_ROUTES_RATELIMIT_H

#include "hs_route.h"

/**
 * Sets an upper bound of requests that can be made from the same connection.
 * Value smaller than 2 will close the connection after a single request.
 */
struct HSRoute *hs_routes_ratelimit_max_connection_requests_route_new(size_t /* max requests */);

/**
 * Sets an upper bound of the amount of time a connection can stay open since
 * it was first created.
 * Any request coming after the max time will ensure the connection is closed
 * after it is handled.
 * Value 0 will close the connection after a single request.
 */
struct HSRoute *hs_routes_ratelimit_max_connection_time_route_new(size_t /* max time in seconds */);

#endif

