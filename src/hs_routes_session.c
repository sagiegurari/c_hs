#include "fsio.h"
#include "hs_io.h"
#include "hs_routes_common.h"
#include "hs_routes_session.h"
#include "ini.h"
#include "stringfn.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HS_ROUTES_SESSION_INI_SECTION_NAME    "session"

static enum HSServeFlowResponse _hs_routes_session_route_serve(struct HSRoute *, struct HSServeFlowParams *);
static void _hs_routes_session_route_release(struct HSRoute *);
static void _hs_routes_session_callback_run(struct HSPostResponseCallback *);
static void _hs_routes_session_callback_release(struct HSPostResponseCallback *);
static char *_hs_routes_session_get_file_for_session_id(char *);

struct HSSessionRouteContext
{
  char *cookie_name;
  char *session_name;
  char * (*generate_cookie_id)(void *);
  char * (*to_string)(struct HSSession *);
  void (*from_string)(struct HSSession *, char *);
  char * (*read_from_storage)(char *, void *);
  bool (*write_to_storage)(char *, char *, void *);
  void (*release_context)(void *);
  void *context;
};

struct HSSessionCallbackContext
{
  char                          *session_id;
  struct  HSSessionRouteContext *route_context;
  struct HSRouteFlowState       *route_state;
};

struct HSRoute *hs_routes_session_route_new(char *cookie_name, char *session_name, char * (*generate_cookie_id)(void *), char * (*to_string)(struct HSSession *), void (*from_string)(struct HSSession *, char *), char * (*read_from_storage)(char *, void *), bool (*write_to_storage)(char *, char *, void *), void (*release_context)(void *), void *route_context)
{
  if (  cookie_name == NULL
     || session_name == NULL
     || generate_cookie_id == NULL
     || to_string == NULL
     || from_string == NULL
     || read_from_storage == NULL
     || write_to_storage == NULL)
  {
    return(NULL);
  }

  struct HSRoute               *route = hs_routes_common_serve_all_route_new();

  struct HSSessionRouteContext *context = malloc(sizeof(struct HSSessionRouteContext));
  context->cookie_name        = cookie_name;
  context->session_name       = session_name;
  context->generate_cookie_id = generate_cookie_id;
  context->to_string          = to_string;
  context->from_string        = from_string;
  context->read_from_storage  = read_from_storage;
  context->write_to_storage   = write_to_storage;
  context->release_context    = release_context;
  context->context            = route_context;
  route->extension            = context;

  route->serve   = _hs_routes_session_route_serve;
  route->release = _hs_routes_session_route_release;

  return(route);
}

struct HSRoute *hs_routes_session_route_new_default()
{
  return(hs_routes_session_route_new(strdup(HS_DEFAULT_SESSION_COOKIE_NAME),
                                     strdup(HS_DEFAULT_SESSION_STATE_NAME),
                                     hs_routes_session_route_generate_cookie_id,
                                     hs_routes_session_route_session_to_string,
                                     hs_routes_session_route_session_from_string,
                                     hs_routes_session_route_session_read_from_file_based_storage,
                                     hs_routes_session_route_session_write_to_file_based_storage,
                                     NULL,
                                     NULL));
}

struct HSSession *hs_routes_session_new_session()
{
  struct HSSession *session = malloc(sizeof(struct HSSession));

  session->id           = NULL;
  session->string_pairs = hs_types_array_string_pair_new();

  return(session);
}


void hs_routes_session_release_session(struct HSSession *session)
{
  if (session == NULL)
  {
    return;
  }

  hs_io_free(session->id);
  hs_types_array_string_pair_release(session->string_pairs);

  hs_io_free(session);
}


char *hs_routes_session_route_generate_cookie_id(void *context)
{
  // context is not supported. wrap the function and pass NULL if needed.
  if (context != NULL)
  {
    return(NULL);
  }

  struct StringBuffer *buffer = stringbuffer_new();

  stringbuffer_append_int(buffer, rand());
  stringbuffer_append_long(buffer, time(NULL));
  stringbuffer_append_int(buffer, rand());
  stringbuffer_append_string(buffer, "SID");

  char *cookie_id = stringbuffer_to_string(buffer);
  stringbuffer_release(buffer);

  return(cookie_id);
}


char *hs_routes_session_route_session_to_string(struct HSSession *session)
{
  if (session == NULL || session->string_pairs == NULL)
  {
    return(NULL);
  }

  size_t count = hs_types_array_string_pair_count(session->string_pairs);
  if (!count)
  {
    return(strdup(""));
  }

  struct IniKeyValue **key_value_pairs = malloc(sizeof(struct IniKeyValue *) * count);
  for (size_t index = 0; index < count; index++)
  {
    key_value_pairs[index]        = malloc(sizeof(struct IniKeyValue));
    key_value_pairs[index]->key   = strdup(hs_types_array_string_pair_get_key(session->string_pairs, index));
    key_value_pairs[index]->value = strdup(hs_types_array_string_pair_get_value(session->string_pairs, index));
  }

  struct IniSection *section = malloc(sizeof(struct IniSection));
  section->name            = strdup(HS_ROUTES_SESSION_INI_SECTION_NAME);
  section->key_value_pairs = key_value_pairs;
  section->count           = count;
  struct IniSection **sections = malloc(sizeof(struct IniSection *));
  sections[0] = section;

  struct Ini *ini = malloc(sizeof(struct Ini));
  ini->sections = sections;
  ini->count    = 1;

  char *session_string = ini_to_string(ini);

  ini_release_ini_structure(ini);

  return(session_string);
}


void hs_routes_session_route_session_from_string(struct HSSession *session, char *session_string)
{
  if (session == NULL || session_string == NULL || session->string_pairs == NULL)
  {
    return;
  }

  struct Ini *ini = ini_parse_string(session_string, NULL);
  if (ini == NULL)
  {
    return;
  }

  if (ini->count)
  {
    for (size_t section_index = 0; section_index < ini->count; section_index++)
    {
      struct IniSection *section = ini->sections[section_index];

      if (section != NULL && stringfn_equal(section->name, HS_ROUTES_SESSION_INI_SECTION_NAME))
      {
        for (size_t pair_index = 0; pair_index < section->count; pair_index++)
        {
          struct IniKeyValue *ini_key_value = section->key_value_pairs[pair_index];
          if (ini_key_value != NULL && ini_key_value->key != NULL && ini_key_value->value != NULL)
          {
            hs_types_array_string_pair_add(session->string_pairs, strdup(ini_key_value->key), strdup(ini_key_value->value));
          }
        }

        break;
      }
    }
  }

  ini_release_ini_structure(ini);
}


char *hs_routes_session_route_session_read_from_file_based_storage(char *session_id, void *context)
{
  // context is not supported. wrap the function and pass NULL if needed.
  if (session_id == NULL || context != NULL)
  {
    return(NULL);
  }

  char *file = _hs_routes_session_get_file_for_session_id(session_id);
  if (file == NULL)
  {
    return(NULL);
  }

  char *session_string = fsio_read_text_file(file);

  hs_io_free(file);

  return(session_string);
}


bool hs_routes_session_route_session_write_to_file_based_storage(char *session_id, char *session_string, void *context)
{
  // context is not supported. wrap the function and pass NULL if needed.
  if (session_id == NULL || context != NULL)
  {
    return(false);
  }

  char *file = _hs_routes_session_get_file_for_session_id(session_id);
  if (file == NULL)
  {
    return(false);
  }

  if (session_string == NULL && fsio_file_exists(file))
  {
    fsio_remove(file);
    hs_io_free(file);
    return(true);
  }

  bool done = fsio_write_text_file(file, session_string);
  hs_io_free(file);

  return(done);
}

struct HSCookie *hs_routes_session_new_cookie(char *name, char *session_id)
{
  if (name == NULL || session_id == NULL)
  {
    return(NULL);
  }

  struct HSCookie *cookie = hs_types_cookie_new();
  cookie->name      = name;
  cookie->value     = session_id;
  cookie->secure    = true;
  cookie->http_only = true;
  cookie->same_site = HS_COOKIE_SAME_SITE_STRICT;
  cookie->max_age   = 2 * 365 * 24 * 60 * 60;

  return(cookie);
}

static enum HSServeFlowResponse _hs_routes_session_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (  route == NULL
     || route->extension == NULL
     || params == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HSSessionRouteContext *context = (struct HSSessionRouteContext *)route->extension;

  // find the cookie which stores the session ID
  struct HSCookie *cookie = hs_types_cookies_get_by_name(params->request->cookies, context->cookie_name);

  char            *session_id = NULL;
  if (cookie != NULL)
  {
    session_id = cookie->value;
  }
  if (session_id != NULL)
  {
    session_id = strdup(session_id);
  }
  else
  {
    session_id = context->generate_cookie_id(context->context);
  }

  struct HSSession *session        = hs_routes_session_new_session();
  char             *session_string = context->read_from_storage(session_id, context->context);
  if (session_string != NULL)
  {
    context->from_string(session, session_string);
    hs_io_free(session_string);
  }
  else
  {
    // in case we can't find the session for that ID, lets generate a new ID
    hs_io_free(session_id);
    session_id = context->generate_cookie_id(context->context);
  }
  session->id = strdup(session_id);

  // store the session ID in a cookie or refresh existing cookie
  cookie = hs_routes_session_new_cookie(strdup(context->cookie_name), strdup(session_id));
  if (cookie != NULL)
  {
    hs_types_cookies_add(params->response->cookies, cookie);
  }

  // add session to route data
  hs_types_array_data_pair_add(params->route_state->data_pairs, strdup(context->session_name), session);

  // setup post response callback to write the session back to storage
  struct HSSessionCallbackContext *callback_context = malloc(sizeof(struct HSSessionCallbackContext));
  callback_context->session_id    = session_id;
  callback_context->route_context = context;
  callback_context->route_state   = params->route_state;
  struct HSPostResponseCallback *callback = hs_types_post_response_callback_new();
  callback->run     = _hs_routes_session_callback_run;
  callback->release = _hs_routes_session_callback_release;
  callback->context = callback_context;
  hs_types_post_response_callbacks_add(params->callbacks, callback);

  return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
} /* _hs_routes_session_route_serve */


static void _hs_routes_session_route_release(struct HSRoute *route)
{
  if (route == NULL || route->extension == NULL)
  {
    return;
  }

  struct HSSessionRouteContext *context = (struct HSSessionRouteContext *)route->extension;

  if (context->release_context != NULL && context->context != NULL)
  {
    context->release_context(context->context);
  }

  hs_io_free(context->cookie_name);
  hs_io_free(context->session_name);
  hs_io_free(context);
}


static void _hs_routes_session_callback_run(struct HSPostResponseCallback *callback)
{
  if (callback == NULL || callback->context == NULL)
  {
    return;
  }

  struct HSSessionCallbackContext *context = (struct HSSessionCallbackContext *)callback->context;
  if (  context->session_id == NULL
     || context->route_context == NULL
     || context->route_state == NULL
     || context->route_context->session_name == NULL)
  {
    return;
  }

  void *data = hs_types_array_data_pair_get_by_key(context->route_state->data_pairs, context->route_context->session_name);
  if (data == NULL)
  {
    return;
  }

  struct HSSession *session        = (struct HSSession *)data;
  char             *session_string = context->route_context->to_string(session);

  hs_routes_session_release_session(session);

  if (session_string == NULL)
  {
    return;
  }

  context->route_context->write_to_storage(context->session_id, session_string, context->route_context->context);
  hs_io_free(session_string);
}


static void _hs_routes_session_callback_release(struct HSPostResponseCallback *callback)
{
  if (callback == NULL || callback->context == NULL)
  {
    return;
  }

  struct HSSessionCallbackContext *context = (struct HSSessionCallbackContext *)callback->context;

  hs_io_free(context->session_id);
  hs_io_free(context);
}


static char *_hs_routes_session_get_file_for_session_id(char *session_id)
{
  if (session_id == NULL)
  {
    return(NULL);
  }

  struct StringBuffer *buffer = stringbuffer_new();
  stringbuffer_append_string(buffer, "./sessions/");
  stringbuffer_append_string(buffer, session_id);
  char *file = stringbuffer_to_string(buffer);
  stringbuffer_release(buffer);

  return(file);
}

