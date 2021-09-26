#ifndef __HS_ROUTES_REDIRECTION_H__
#define __HS_ROUTES_REDIRECTION_H__

#include "hs_route.h"

/**
 * Returns a new route which simply redirects the request from the path to the requested path.
 * The redirection will be using temporary redirect code.
 * The from and to paths will be released when the route is released.
 */
struct HSRoute *hs_routes_redirection_route_new(char * /* from path */, char * /* to path */);

/**
 * Returns a new route which simply redirects the request from the path to the requested path.
 * The from and to paths will be released when the route is released.
 */
struct HSRoute *hs_routes_redirection_route_new_with_options(char * /* from path */, char * /* to path */, bool /* temporary redirect */);

/**
 * Utility function to set the location header to enable redirection
 * and the relevant status code.
 */
bool hs_routes_redirection_set_header_and_status_code(struct HSServeFlowParams *, char * /* to path */, bool /* temporary redirect */);

#endif

