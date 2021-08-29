#ifndef __HS_ROUTE_H__
#define __HS_ROUTE_H__

#include "hs_types.h"

struct HSRoute
{
  // The path (resource) this route handles
  char *path;
  // if true, all sub resources are also handled by this route
  bool is_parent_path;
  // The HTTP methods that are supported by this route
  bool is_get;
  bool is_post;
  bool is_put;
  bool is_delete;
  bool is_head;
  bool is_connect;
  bool is_options;
  bool is_trace;
  bool is_patch;
  // The route implementation
  enum HSServeFlowResponse (*serve)(struct HSRoute *, struct HSServeFlowParams *);
  // Called by the server in order to free the route internal memory
  void (*release)(struct HSRoute *);
  // Any extended data/functions needed on the route (must be manually released)
  void *extension;
};

/**
 * Creates and returns a new route struct.
 */
struct HSRoute *hs_route_new(void);

/**
 * Frees all internal memory and struct.
 */
void hs_route_release_route(struct HSRoute *);

/**
 * Enables/disables all http methods for route.
 */
void hs_route_set_all_methods(struct HSRoute *, bool /* enable */);

/**
 * Returns true if the request method is supported by the given route.
 */
bool hs_route_is_allowed_for_method(struct HSRoute *, struct HSHttpRequest *);

/**
 * Returns true if the request path is supported by the given route.
 */
bool hs_route_is_supported_path(struct HSRoute *, struct HSHttpRequest *);

#endif

