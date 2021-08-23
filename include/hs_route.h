#ifndef __HS_ROUTE_H__
#define __HS_ROUTE_H__

#include "hs_types.h"

struct HSPostResponseCallback
{
  void *context;
  void (*run)(struct HSPostResponseCallback *);
  void (*release)(struct HSPostResponseCallback *);
};

struct HSRouteRedirectResponse
{
  // The path to redirect to
  char                          *path;
  // The status code (should be 3xx)
  enum HSHttpResponseCode       code;
  // Any extra headers to return
  struct HSKeyValueArray        *headers;
  // Any cookies we want to set/delete as part of the redirection
  struct HSCookies              *cookies;
  // Optional callback after response is written
  struct HSPostResponseCallback *callback;
};

struct HSRouteServeResponse
{
  // The status code
  enum HSHttpResponseCode       code;
  // Any extra headers to return
  struct HSKeyValueArray        *headers;
  // Any cookies we want to set/delete as part of the redirection
  struct HSCookies              *cookies;
  // The content type header value, if the enum doesn't contain a relevant value
  // use the HS_MIME_TYPE_NONE and add the actual value manually to the headers array.
  enum HSMimeType               mime_type;
  // The content to return (one of the following)
  char                          *content_string;
  char                          *content_file;
  // Optional callback after response is written
  struct HSPostResponseCallback *callback;
};

struct HSRoute
{
  // The path (resource) this route handles
  char                           *path;
  // if true, all sub resources are also handled by this route
  bool                           is_parent_path;

  // The HTTP methods that are supported by this route
  bool                           is_get;
  bool                           is_post;
  bool                           is_put;
  bool                           is_delete;

  // Enables the route to redirect to another route
  struct HSRouteRedirectResponse * (*redirect)(struct HSRoute *, struct HSHttpRequest *, int /* socket */);

  // The route implementation
  struct HSRouteServeResponse    * (*serve)(struct HSRoute *, struct HSHttpRequest *, int /* socket */);

  // Called by the server in order to free the route internal memory
  void                           (*release)(struct HSRoute *);

  // Any extended data/functions needed on the route (must be manually released)
  void                           *extension;
};

/**
 * Creates and returns the new struct.
 */
struct HSPostResponseCallback *hs_route_new_post_response_callback();

/**
 * Releases the struct memory, not including the context.
 * Optional release function (if defined) will be invoked.
 */
void hs_route_release_post_response_callback(struct HSPostResponseCallback *callback);

/**
 * Creates and returns a new redirect response struct.
 */
struct HSRouteRedirectResponse *hs_route_new_redirect_response(void);

/**
 * Frees all internal memory and struct.
 */
void hs_route_release_redirect_response(struct HSRouteRedirectResponse *);

/**
 * Creates and returns a new serve response struct.
 */
struct HSRouteServeResponse *hs_route_new_serve_response(void);

/**
 * Frees all internal memory and struct.
 */
void hs_route_release_serve_response(struct HSRouteServeResponse *);

/**
 * Creates and returns a new route struct.
 */
struct HSRoute *hs_route_new_route(void);

/**
 * Frees all internal memory and struct.
 */
void hs_route_release_route(struct HSRoute *);

/**
 * Returns true if the request method is supported by the given route.
 */
bool hs_route_is_allowed_for_method(struct HSRoute *, struct HSHttpRequest *);

/**
 * Returns true if the request path is supported by the given route.
 */
bool hs_route_is_supported_path(struct HSRoute *, struct HSHttpRequest *);

#endif

