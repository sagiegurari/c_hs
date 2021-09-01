#ifndef __HS_ROUTES_SESSION_H__
#define __HS_ROUTES_SESSION_H__

#include "hs_route.h"

#define HS_DEFAULT_SESSION_COOKIE_NAME    "sc"
#define HS_DEFAULT_SESSION_STATE_NAME     "http-session"

struct HSSession
{
  char                   *id;
  struct HSKeyValueArray *string_pairs;
};

/**
 * The session route enables to initialize a session, store/read it and save its ID
 * in a specific cookie to persist it for future requests.
 * All provided data will be released when the route is released.
 */
struct HSRoute *hs_routes_session_route_new(char * /* cookie name */, char * /* session name in route context */, char * (*generate_cookie_id)(void * /* context */), char * (*to_string)(struct HSSession *), void (*from_string)(struct HSSession *, char *), char * (*read_from_storage)(char * /* ID */, void * /* context */), bool (*write_to_storage)(char * /* ID */, char * /* serialized session */, void * /* context */), void (*release_context)(void *), void * /* context */);

/**
 * The session route with file based storage and ini serialization format.
 */
struct HSRoute *hs_routes_session_route_new_default();

/**
 * Creates a new session struct.
 */
struct HSSession *hs_routes_session_new_session();

/**
 * Releases the session and all its content.
 */
void hs_routes_session_release_session(struct HSSession *);

/**
 * Simple implementation to the generate_cookie_id function.
 */
char *hs_routes_session_route_generate_cookie_id(void *);

/**
 * ini based implementation.
 */
char *hs_routes_session_route_session_to_string(struct HSSession *);

/**
 * ini based implementation.
 */
void hs_routes_session_route_session_from_string(struct HSSession *, char *);

/**
 * Reads the session content from a file under sessions directory.
 * Files are named after session ID.
 */
char *hs_routes_session_route_session_read_from_file_based_storage(char *, void *);

/**
 * Writes the session to a file under sessions directory.
 * Files are named after session ID.
 */
bool hs_routes_session_route_session_write_to_file_based_storage(char *, char *, void *);

/**
 * Creates a cookie for the given session.
 * All provided data will be automatically released.
 */
struct HSCookie *hs_routes_session_new_cookie(char * /* name */, char * /* session_id */);

#endif

