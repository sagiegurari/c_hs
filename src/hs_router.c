#include "fsio.h"
#include "hs_io.h"
#include "hs_parser.h"
#include "hs_router.h"
#include "stringfn.h"
#include "vector.h"
#include <stdlib.h>
#include <string.h>

struct HSRouter
{
  struct Vector *routes;
  bool          support_keep_alive;
};


static bool _hs_router_find_route_and_serve(struct HSRouter *, struct HSServeFlowParams *);
static bool _hs_router_serve(struct HSRouter *, struct HSServeFlowParams *, struct HSRoute *);
static enum HSServeFlowResponse _hs_router_as_route_serve(struct HSRoute *, struct HSServeFlowParams *);
static void                           _hs_router_as_route_release(struct HSRoute *);
static void _hs_router_run_callbacks(struct HSPostResponseCallbacks *);

struct HSRouter *hs_router_new(void)
{
  struct HSRouter *router = malloc(sizeof(struct HSRouter));

  router->routes             = vector_new();
  router->support_keep_alive = true;

  return(router);
}


void hs_router_release(struct HSRouter *router)
{
  if (router == NULL)
  {
    return;
  }

  if (router->routes != NULL)
  {
    size_t size = vector_size(router->routes);
    for (size_t index = 0; index < size; index++)
    {
      struct HSRoute *route = (struct HSRoute *)vector_get(router->routes, index);
      if (route != NULL)
      {
        hs_route_release_route(route);
      }
    }

    vector_release(router->routes);
  }

  hs_io_free(router);
}


void hs_router_set_keep_alive_support(struct HSRouter *router, bool enable)
{
  if (router == NULL)
  {
    return;
  }

  router->support_keep_alive = enable;
}


void hs_router_add_route(struct HSRouter *router, struct HSRoute *route)
{
  if (route != NULL)
  {
    vector_push(router->routes, route);
  }
}


bool hs_router_serve_forever(struct HSRouter *router, struct HSSocket *socket, void *context, bool (*should_stop)(struct HSRouter *, struct HSSocket *, size_t, void *))
{
  if (router == NULL || !hs_socket_is_open(socket))
  {
    return(false);
  }

  struct HSServerConnectionState *connection_state = hs_types_server_connection_state_new();
  connection_state->socket = socket;

  bool can_continue = true;
  if (should_stop != NULL)
  {
    can_continue = !should_stop(router, socket, connection_state->request_counter, context);
  }

  while (can_continue)
  {
    connection_state->request_counter++;
    can_continue = hs_router_serve_next(router, connection_state);
    if (can_continue && should_stop != NULL)
    {
      can_continue = !should_stop(router, socket, connection_state->request_counter, context);
    }
  }

  hs_types_server_connection_state_release(connection_state);

  return(true);
}


bool hs_router_serve_next(struct HSRouter *router, struct HSServerConnectionState *connection_state)
{
  if (router == NULL || connection_state == NULL || !hs_socket_is_open(connection_state->socket))
  {
    return(false);
  }

  struct HSHttpRequest *request = hs_parser_parse_request(connection_state->socket);
  if (request == NULL)
  {
    return(false);
  }

  struct HSServeFlowParams *params = hs_types_serve_flow_params_new_pre_populated(request);
  params->connection_state = connection_state;

  bool done         = hs_router_serve(router, params);
  bool can_continue = done && !params->router_state->closed_connection;

  hs_types_serve_flow_params_release(params);

  return(can_continue);
}


bool hs_router_serve(struct HSRouter *router, struct HSServeFlowParams *params)
{
  if (  router == NULL
     || params == NULL
     || params->request == NULL
     || params->response == NULL
     || params->connection_state == NULL
     || !hs_socket_is_open(params->connection_state->socket)
     || params->request->resource == NULL
     || params->router_state == NULL)
  {
    return(false);
  }

  if (params->router_state->done)
  {
    return(true);
  }

  // ensure request resource is valid
  size_t request_path_length = strlen(params->request->resource);
  if (!request_path_length || params->request->resource[0] != '/')
  {
    return(false);
  }

  bool done = _hs_router_find_route_and_serve(router, params);

  _hs_router_run_callbacks(params->callbacks);

  return(done);
} /* hs_router_serve */

struct HSRoute *hs_router_as_route(struct HSRouter *router)
{
  if (router == NULL)
  {
    return(NULL);
  }

  struct HSRoute *route = hs_route_new();

  // enable for all method types as the routes will limit it
  hs_route_set_all_methods(route, true);
  route->is_parent_path = true;

  route->serve = _hs_router_as_route_serve;

  route->extension = router;
  route->release   = _hs_router_as_route_release;

  return(route);
}

struct StringBuffer *hs_router_write_common_response_header(enum HSHttpResponseCode code, struct HSArrayStringPair *headers, struct HSCookies *cookies, bool close_connection)
{
  struct StringBuffer *buffer = stringbuffer_new();

  // write status line
  stringbuffer_append_string(buffer, "HTTP/1.1 ");
  stringbuffer_append_unsigned_int(buffer, code);
  stringbuffer_append(buffer, ' ');
  stringbuffer_append_unsigned_int(buffer, code);
  stringbuffer_append_string(buffer, "\r\n");

  // write headers (may contain set cookie headers)
  size_t count = hs_types_array_string_pair_count(headers);
  if (headers != NULL && count)
  {
    for (size_t index = 0; index < count; index++)
    {
      char *key   = hs_types_array_string_pair_get_key(headers, index);
      char *value = hs_types_array_string_pair_get_value(headers, index);

      if (key != NULL && value != NULL)
      {
        stringbuffer_append_string(buffer, key);
        stringbuffer_append_string(buffer, ": ");
        stringbuffer_append_string(buffer, value);
        stringbuffer_append_string(buffer, "\r\n");
      }
    }
  }

  // write set cookie headers
  count = hs_types_cookies_count(cookies);
  if (cookies != NULL && count)
  {
    for (size_t index = 0; index < count; index++)
    {
      struct HSCookie *cookie = hs_types_cookies_get(cookies, index);

      if (cookie != NULL && cookie->name != NULL && cookie->value != NULL)
      {
        stringbuffer_append_string(buffer, "Set-Cookie: ");
        stringbuffer_append_string(buffer, cookie->name);
        stringbuffer_append_string(buffer, "=");
        stringbuffer_append_string(buffer, cookie->value);

        // write cookie attributes
        if (cookie->expires != NULL)
        {
          stringbuffer_append_string(buffer, "; Expires=");
          stringbuffer_append_string(buffer, cookie->expires);
        }
        if (cookie->max_age >= 0)
        {
          stringbuffer_append_string(buffer, "; Max-Age=");
          stringbuffer_append_int(buffer, cookie->max_age);
        }
        if (cookie->secure)
        {
          stringbuffer_append_string(buffer, "; Secure");
        }
        if (cookie->http_only)
        {
          stringbuffer_append_string(buffer, "; HttpOnly");
        }
        if (cookie->domain != NULL)
        {
          stringbuffer_append_string(buffer, "; Domain=");
          stringbuffer_append_string(buffer, cookie->domain);
        }
        if (cookie->path != NULL)
        {
          stringbuffer_append_string(buffer, "; Path=");
          stringbuffer_append_string(buffer, cookie->path);
        }

        stringbuffer_append_string(buffer, "; SameSite=");
        switch (cookie->same_site)
        {
        case HS_COOKIE_SAME_SITE_NONE:
          stringbuffer_append_string(buffer, "None");
          break;

        case HS_COOKIE_SAME_SITE_LAX:
          stringbuffer_append_string(buffer, "Lax");
          break;

        case HS_COOKIE_SAME_SITE_STRICT:
          stringbuffer_append_string(buffer, "Strict");
          break;
        }

        stringbuffer_append_string(buffer, "\r\n");
      }
    }
  }

  if (close_connection)
  {
    stringbuffer_append_string(buffer, "Connection: close\r\n");
  }
  else
  {
    stringbuffer_append_string(buffer, "Connection: keep-alive\r\n");
  }

  return(buffer);
} /* hs_router_write_common_response_header */


char *hs_router_remove_path_prefix(struct HSRoute *route, struct HSHttpRequest *request)
{
  if (route->path == NULL)
  {
    return(NULL);
  }

  size_t route_path_length = strlen(route->path);
  if (route_path_length <= 1)
  {
    return(request->resource);
  }

  char   *original_path = request->resource;

  size_t request_path_length = strlen(request->resource);
  if (request_path_length == route_path_length)
  {
    request->resource = "/";
  }
  else
  {
    bool ends_with_separator = route->path[route_path_length - 1] == '/';

    request->resource = request->resource + route_path_length - 1;
    if (!ends_with_separator)
    {
      request->resource++;
    }
  }

  return(original_path);
}


static bool _hs_router_find_route_and_serve(struct HSRouter *router, struct HSServeFlowParams *params)
{
  if (router == NULL || params == NULL)
  {
    return(false);
  }

  size_t count = vector_size(router->routes);
  bool   done  = false;
  for (size_t index = 0; index < count; index++)
  {
    struct HSRoute *route = (struct HSRoute *)vector_get(router->routes, index);

    if (_hs_router_serve(router, params, route))
    {
      done = true;
      break;
    }
  }

  return(done);
}


static bool _hs_router_serve(struct HSRouter *router, struct HSServeFlowParams *params, struct HSRoute *route)
{
  if (  router == NULL
     || params->request == NULL
     || params->response == NULL
     || params->connection_state == NULL
     || !hs_socket_is_open(params->connection_state->socket)
     || params->request->resource == NULL
     || params->router_state == NULL
     || params->router_state->closed_connection)
  {
    return(false);
  }

  if (params->router_state->done)
  {
    return(true);
  }

  if (!hs_route_is_allowed_for_method(route, params->request))
  {
    return(false);
  }

  if (!hs_route_is_supported_path(route, params->request))
  {
    return(false);
  }

  if (route->serve == NULL)
  {
    return(false);
  }

  enum HSServeFlowResponse serve_response = route->serve(route, params);
  if (serve_response == HS_SERVE_FLOW_RESPONSE_CONTINUE || params->router_state->done)
  {
    return(params->router_state->done);
  }

  // if close or unknown, add the close response header
  bool close_connection = !router->support_keep_alive
                          || params->request->connection == HS_CONNECTION_TYPE_CLOSE
                          || params->request->connection == HS_CONNECTION_TYPE_UNKNOWN;

  struct StringBuffer *header_buffer = hs_router_write_common_response_header(params->response->code,
                                                                              params->response->headers,
                                                                              params->response->cookies,
                                                                              close_connection);

  // check if route has set the header directly
  char *content_type_header = hs_types_array_string_pair_get_by_key(params->response->headers, "Content-Type");
  if (content_type_header == NULL)
  {
    // no header set, mime type is unknown but response is a file, attempt to get mime type from file name/extension
    if (params->response->mime_type == HS_MIME_TYPE_NONE && params->response->content_file != NULL)
    {
      params->response->mime_type = hs_constants_file_extension_to_mime_type(params->response->content_file);
    }

    const char *mime_type = hs_constants_mime_type_to_string(params->response->mime_type);
    if (mime_type != NULL)
    {
      stringbuffer_append_string(header_buffer, "Content-Type: ");
      stringbuffer_append_string(header_buffer, (char *)mime_type);
      stringbuffer_append_string(header_buffer, "\r\n");
    }
  }

  bool has_content = true;
  if (params->response->content_string != NULL)
  {
    size_t content_length = strlen(params->response->content_string);
    stringbuffer_append_string(header_buffer, "Content-Length: ");
    stringbuffer_append_unsigned_long(header_buffer, content_length);
    stringbuffer_append_string(header_buffer, "\r\n\r\n");

    if (!content_length)
    {
      has_content = false;
    }
  }
  else if (params->response->content_file != NULL)
  {
    long content_length = fsio_file_size(params->response->content_file);
    if (content_length)
    {
      stringbuffer_append_string(header_buffer, "Content-Length: ");
      stringbuffer_append_long(header_buffer, content_length);
      stringbuffer_append_string(header_buffer, "\r\n\r\n");
    }
    else
    {
      // file validations should be done by the route, so we are keeping response status code
      // as is and just not returning any content.
      has_content = false;
      stringbuffer_append_string(header_buffer, "Content-Length: 0\r\n\r\n");
    }
  }
  else
  {
    has_content = false;
    stringbuffer_append_string(header_buffer, "Content-Length: 0\r\n\r\n");
  }

  char   *header_string = stringbuffer_to_string(header_buffer);
  size_t length         = stringbuffer_get_content_size(header_buffer);
  hs_io_write_string_to_socket(params->connection_state->socket, header_string, length);
  hs_io_free(header_string);

  if (has_content)
  {
    if (params->response->content_string != NULL)
    {
      hs_io_write_string_to_socket(params->connection_state->socket, params->response->content_string, strlen(params->response->content_string));
    }
    else
    {
      hs_io_write_file_to_socket(params->connection_state->socket, params->response->content_file);
    }
  }

  stringbuffer_release(header_buffer);

  // if there is still payload left to read, no matter what we sent to the header
  // we will still close the connection instead of reading all that payload for nothing
  if (params->request->content_length && !hs_types_http_request_payload_is_loaded(params->request))
  {
    close_connection = true;
  }

  if (close_connection)
  {
    hs_socket_close(params->connection_state->socket);
    params->connection_state->socket        = NULL;
    params->router_state->closed_connection = true;
  }

  return(true);
} /* _hs_router_serve */

static enum HSServeFlowResponse _hs_router_as_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (  route == NULL
     || params->request == NULL
     || params->response == NULL
     || params->connection_state == NULL
     || !hs_socket_is_open(params->connection_state->socket)
     || params->request->resource == NULL
     || params->router_state == NULL
     || params->router_state->closed_connection)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  if (params->router_state->done)
  {
    return(HS_SERVE_FLOW_RESPONSE_DONE);
  }

  char            *original_base_path = params->router_state->base_path;
  params->router_state->base_path = fsio_join_paths(original_base_path, route->path);
  char            *original_path = hs_router_remove_path_prefix(route, params->request);

  struct HSRouter *router = (struct HSRouter *)route->extension;

  params->router_state->done = _hs_router_find_route_and_serve(router, params);

  params->request->resource = original_path;
  hs_io_free(params->router_state->base_path);
  params->router_state->base_path = original_base_path;

  if (params->router_state->done)
  {
    return(HS_SERVE_FLOW_RESPONSE_DONE);
  }

  return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
}


static void _hs_router_as_route_release(struct HSRoute *route)
{
  if (route == NULL || route->extension == NULL)
  {
    return;
  }

  struct HSRouter *router = (struct HSRouter *)route->extension;

  hs_router_release(router);
}


static void _hs_router_run_callbacks(struct HSPostResponseCallbacks *callbacks)
{
  if (callbacks == NULL || !callbacks->count)
  {
    return;
  }

  // run in the reverse order (first route callback is called last)
  for (size_t index = callbacks->count; index > 0; index--)
  {
    struct HSPostResponseCallback *callback = callbacks->callbacks[index - 1];
    if (callback != NULL && callback->run != NULL)
    {
      callback->run(callback);
    }
  }
}

