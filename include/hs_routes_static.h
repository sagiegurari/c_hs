#ifndef HS_ROUTES_STATIC_H
#define HS_ROUTES_STATIC_H

#include "hs_route.h"

/**
 * Returns a new route which simply returns the provided content string with the given mime type.
 * The content will be released with the route.
 */
struct HSRoute *hs_routes_static_route_new(char * /* content */, enum HSMimeType);

/**
 * Returns a new route which simply returns the provided plain text.
 * The content will be released with the route.
 */
struct HSRoute *hs_routes_static_text_route_new(char * /* text */);

/**
 * Returns a new route which simply returns the provided HTML.
 * The content will be released with the route.
 */
struct HSRoute *hs_routes_static_html_route_new(char * /* HTML */);

/**
 * Returns a new route which simply returns the provided CSS text.
 * The content will be released with the route.
 */
struct HSRoute *hs_routes_static_css_route_new(char * /* css text */);

/**
 * Returns a new route which simply returns the provided JS text.
 * The content will be released with the route.
 */
struct HSRoute *hs_routes_static_js_route_new(char * /* js text */);

#endif

