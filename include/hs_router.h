#ifndef HS_ROUTER_H
#define HS_ROUTER_H

#include "hs_route.h"
#include "hs_socket.h"
#include "hs_types.h"

struct HSRouter;

/**
 * Creates a new router and returns it.
 */
struct HSRouter *hs_router_new(void);

/**
 * Frees all memory used by the router directly (including routes).
 */
void hs_router_release(struct HSRouter *);

/**
 * Sets the keep alive support for the router (default is true)
 */
void hs_router_set_keep_alive_support(struct HSRouter *, bool);

/**
 * Adds a new route.
 */
void hs_router_add_route(struct HSRouter *, struct HSRoute *);

/**
 * Loops over HTTP requests and handles them until socket closed.
 */
bool hs_router_serve_forever(struct HSRouter *, struct HSSocket *, void * /* context */, bool (*should_stop)(struct HSRouter *, struct HSSocket *, size_t /* request counter */, void * /* context */));

/**
 * Reads the next HTTP request and handles it.
 */
bool hs_router_serve_next(struct HSRouter *, struct HSServerConnectionState *);

/**
 * Handles a HTTP request.
 */
bool hs_router_serve(struct HSRouter *, struct HSServeFlowParams *);

/**
 * Returns a route which calls an internal router which will contain sub routes.
 */
struct HSRoute *hs_router_as_route(struct HSRouter *);

/**
 * Creates a new buffer and writes common response header content, including
 * status line, headers and set cookie headers.
 * This is an internal function.
 */
struct StringBuffer *hs_router_write_common_response_header(enum HSHttpResponseCode, struct HSArrayStringPair *, struct HSCookies *, bool /* close_connection */);

/**
 * Internal function.
 * Removes the route path from the request resource, ensuring it starts with / character.
 * The value is replaced inside the request object, while the original value is returned.
 * The updated value in the request should not be freed directly.
 */
char *hs_router_remove_path_prefix(struct HSRoute *, struct HSHttpRequest *);

#endif

