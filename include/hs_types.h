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
};

struct HSHttpRequestPayload;

struct HSHttpRequestState
{
  bool done;
  bool closed_connection;
  bool prevent_redirect;
  bool prevent_serve;
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
  size_t                      content_length;
  enum HSConnectionType       connection;
  struct HSCookies            *cookies;
  // all headers
  struct HSKeyValueArray      *headers;
  struct HSHttpRequestPayload *payload;
  // state, used internally by the router
  struct HSHttpRequestState   state;
};

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
struct HSCookies *hs_types_new_cookies(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_release_cookies(struct HSCookies *);

/**
 * Creates and returns a new key value array struct.
 */
struct HSKeyValueArray *hs_types_new_key_value_array(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_release_key_value_array(struct HSKeyValueArray *);

/**
 * Searches the array and returns the first value for the given key.
 * If not found or array/key are null, null will be returned.
 */
char *hs_types_get_value_for_key_from_array(struct HSKeyValueArray *, char *);

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
bool hs_types_http_request_payload_is_loaded(struct HSHttpRequestPayload *);

/**
 * Loads the entire payload to memory and returns it.
 * Once loaded, the payload struct is no longer usable.
 */
char *hs_types_http_request_payload_to_string(struct HSHttpRequestPayload *);

/**
 * Writes the entire payload to the requested file.
 * Once loaded, the payload struct is no longer usable.
 */
bool hs_types_http_request_payload_to_file(struct HSHttpRequestPayload *, char * /* filename */);

#endif

