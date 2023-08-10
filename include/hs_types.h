#ifndef HS_TYPES_H
#define HS_TYPES_H

#include "hs_constants.h"
#include "hs_socket.h"
#include "hs_types_array.h"
#include "hs_types_cookie.h"
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

struct HSHttpRequestPayload;

struct HSRouterFlowState
{
  bool done;
  bool closed_connection;
  char *base_path;
};

struct HSRouteFlowState
{
  // string key/value pairs (all values will be freed at end of request)
  struct HSArrayStringPair *string_pairs;
  // data table (values are freed at end of request based on provided release function)
  struct HashTable         *data;
};

struct HSHttpRequest
{
  enum HSHttpMethod           method;
  char                        *domain;
  int                         port;
  bool                        ssl;
  char                        *resource;
  char                        *query_string;
  // specific headers
  char                        *user_agent;
  char                        *authorization;
  size_t                      content_length;
  enum HSConnectionType       connection;
  struct HSCookies            *cookies;
  // all headers
  struct HSArrayStringPair    *headers;
  struct HSHttpRequestPayload *payload;
};

struct HSHttpResponse
{
  // The status code
  enum HSHttpResponseCode  code;
  // Any cookies we want to set/delete
  struct HSCookies         *cookies;
  // Any extra headers to return
  struct HSArrayStringPair *headers;
  // The content type header value, if the enum doesn't contain a relevant value
  // use the HS_MIME_TYPE_NONE and add the actual value manually to the headers array.
  enum HSMimeType          mime_type;
  // The content to return (one of the following)
  char                     *content_string;
  char                     *content_file;
};

struct HSPostResponseCallback
{
  void *context;
  void (*run)(struct HSPostResponseCallback *);
  void (*release)(struct HSPostResponseCallback *);
};

struct HSPostResponseCallbacks
{
  struct HSPostResponseCallback **callbacks;
  size_t                        count;
  size_t                        capacity;
};

struct HSServerConnectionState
{
  struct HSSocket *socket;
  size_t          request_counter;
  time_t          creation_time;
};

struct HSServeFlowParams
{
  struct HSHttpRequest           *request;
  struct HSHttpResponse          *response;
  // Optional callbacks after response is written
  struct HSPostResponseCallbacks *callbacks;
  // route state, can be used by the routes to store data in the context of the request
  struct HSRouteFlowState        *route_state;
  // router state, used internally by the router
  struct HSRouterFlowState       *router_state;
  // connection state, can be used to check connection tracking info
  struct HSServerConnectionState *connection_state;
};

enum HSServeFlowResponse
{
  HS_SERVE_FLOW_RESPONSE_CONTINUE = 1,
  HS_SERVE_FLOW_RESPONSE_DONE     = 2,
};

/**
 * Creates and returns a new struct.
 */
struct HSServeFlowParams *hs_types_serve_flow_params_new(void);

/**
 * Creates and returns a new struct.
 */
struct HSServeFlowParams *hs_types_serve_flow_params_new_pre_populated(struct HSHttpRequest *);

/**
 * Frees all internal memory and struct.
 */
void hs_types_serve_flow_params_release(struct HSServeFlowParams *);

/**
 * Creates and returns a new http request struct.
 */
struct HSHttpRequest *hs_types_http_request_new(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_http_request_release(struct HSHttpRequest *);

/**
 * Creates and returns a new http response struct.
 */
struct HSHttpResponse *hs_types_http_response_new(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_http_response_release(struct HSHttpResponse *);

/**
 * Creates and returns the new struct.
 */
struct HSPostResponseCallback *hs_types_post_response_callback_new(void);

/**
 * Releases the struct memory, not including the context.
 * Optional release function (if defined) will be invoked.
 */
void hs_types_post_response_callback_release(struct HSPostResponseCallback *);

/**
 * Creates and returns the new struct.
 */
struct HSPostResponseCallbacks *hs_types_post_response_callbacks_new(size_t /* capacity */);

/**
 * Releases the struct memory, including all sub callbacks.
 */
void hs_types_post_response_callbacks_release(struct HSPostResponseCallbacks *);

/**
 * Adds additional callback.
 * If needed, a new internal array will be allocated with enough capacity.
 */
bool hs_types_post_response_callbacks_add(struct HSPostResponseCallbacks *, struct HSPostResponseCallback *);

/**
 * Creates and returns a new state struct.
 */
struct HSRouterFlowState *hs_types_router_flow_state_new(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_router_flow_state_release(struct HSRouterFlowState *);

/**
 * Creates and returns a new state struct.
 */
struct HSRouteFlowState *hs_types_route_flow_state_new(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_route_flow_state_release(struct HSRouteFlowState *);

/**
 * Creates and returns a new state struct.
 */
struct HSServerConnectionState *hs_types_server_connection_state_new(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_server_connection_state_release(struct HSServerConnectionState *);

/**
 * Internal function.
 */
struct HSHttpRequestPayload *hs_types_http_request_new_payload(void *);

/**
 * Returns true if we already loaded the payload content, making this struct
 * no longer usable.
 */
bool hs_types_http_request_payload_is_loaded(struct HSHttpRequest *);

/**
 * Loads the entire payload to memory and returns it.
 * Once loaded, the payload struct is no longer usable.
 */
char *hs_types_http_request_payload_to_string(struct HSHttpRequest *);

/**
 * Writes the entire payload to the requested file.
 * Once loaded, the payload struct is no longer usable.
 */
bool hs_types_http_request_payload_to_file(struct HSHttpRequest *, char * /* filename */);

#endif

