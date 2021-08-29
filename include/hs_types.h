#ifndef __HS_TYPES_H__
#define __HS_TYPES_H__

#include "hs_constants.h"
#include <stdbool.h>
#include <stddef.h>

struct HSKeyValue
{
  char *key;
  char *value;
};

struct HSKeyValueArray
{
  struct HSKeyValue **pairs;
  size_t            count;
  size_t            capacity;
};

struct HSCookie
{
  char                  *name;
  char                  *value;
  char                  *expires;
  int                   max_age;
  bool                  secure;
  bool                  http_only;
  char                  *domain;
  char                  *path;
  enum HSCookieSameSite same_site;
};

struct HSCookies
{
  struct HSCookie **cookies;
  size_t          count;
  size_t          capacity;
};

struct HSHttpRequestPayload;

struct HSRouterFlowState
{
  bool done;
  bool closed_connection;
  char *base_path;
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
  struct HSKeyValueArray      *headers;
  struct HSHttpRequestPayload *payload;
};

struct HSHttpResponse
{
  // The status code
  enum HSHttpResponseCode code;
  // Any cookies we want to set/delete
  struct HSCookies        *cookies;
  // Any extra headers to return
  struct HSKeyValueArray  *headers;
  // The content type header value, if the enum doesn't contain a relevant value
  // use the HS_MIME_TYPE_NONE and add the actual value manually to the headers array.
  enum HSMimeType         mime_type;
  // The content to return (one of the following)
  char                    *content_string;
  char                    *content_file;
};

struct HSPostResponseCallback
{
  void *context;
  void (*run)(struct HSPostResponseCallback *);
  void (*release)(struct HSPostResponseCallback *);
};

struct HSServeFlowParams
{
  struct HSHttpRequest          *request;
  struct HSHttpResponse         *response;
  int                           socket;
  // Optional callback after response is written
  struct HSPostResponseCallback *callback;
  // state, used internally by the router
  struct HSRouterFlowState      *router_state;
};

enum HSServeFlowResponse
{
  HS_SERVE_FLOW_RESPONSE_CONTINUE = 1,
  HS_SERVE_FLOW_RESPONSE_DONE     = 2,
};

/**
 * Creates and returns a new struct.
 */
struct HSServeFlowParams *hs_types_new_serve_flow_params(void);

/**
 * Creates and returns a new struct.
 */
struct HSServeFlowParams *hs_types_new_serve_flow_params_pre_populated(struct HSHttpRequest *);

/**
 * Frees all internal memory and struct.
 */
void hs_types_release_serve_flow_params(struct HSServeFlowParams *);

/**
 * Creates and returns a new http request struct.
 */
struct HSHttpRequest *hs_types_new_http_request(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_release_http_request(struct HSHttpRequest *);

/**
 * Creates and returns a new http response struct.
 */
struct HSHttpResponse *hs_types_new_http_response(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_release_http_response(struct HSHttpResponse *);

/**
 * Creates and returns the new struct.
 */
struct HSPostResponseCallback *hs_types_new_post_response_callback(void);

/**
 * Releases the struct memory, not including the context.
 * Optional release function (if defined) will be invoked.
 */
void hs_types_release_post_response_callback(struct HSPostResponseCallback *callback);

/**
 * Creates and returns a new state struct.
 */
struct HSRouterFlowState *hs_types_new_router_flow_state(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_release_router_flow_state(struct HSRouterFlowState *);

/**
 * Creates and returns a new cookie struct.
 */
struct HSCookie *hs_types_new_cookie(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_release_cookie(struct HSCookie *);

/**
 * Creates and returns a new cookies struct.
 */
struct HSCookies *hs_types_new_cookies(size_t /* capacity */);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_release_cookies(struct HSCookies *);

/**
 * Adds the cookie to the cookies structure.
 * If needed, a new internal array will be allocated with enough capacity.
 */
bool hs_types_cookies_add(struct HSCookies *, struct HSCookie *);

/**
 * Creates and returns a new key value array struct.
 */
struct HSKeyValueArray *hs_types_new_key_value_array(size_t /* capacity */);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_release_key_value_array(struct HSKeyValueArray *);

/**
 * Searches the array and returns the first value for the given key.
 * If not found or array/key are null, null will be returned.
 */
char *hs_types_key_value_array_get_by_key(struct HSKeyValueArray *, char *);

/**
 * Adds the key/value pair to the array.
 * If needed, a new internal pairs array will be allocated with enough capacity.
 */
bool hs_types_key_value_array_add(struct HSKeyValueArray *, char * /* key */, char * /* value */);

/**
 * Creates and returns a new key value pair struct.
 */
struct HSKeyValue *hs_types_new_key_value(char * /* key */, char * /* value */);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_release_key_value(struct HSKeyValue *);

/**
 * Internal function.
 */
struct HSHttpRequestPayload *hs_types_new_http_request_payload(void *);

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

