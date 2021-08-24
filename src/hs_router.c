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

bool _hs_router_serve(struct HSRouter *, struct HSHttpRequest *, int, struct HSRoute *);
struct HSRouteRedirectResponse *_hs_router_as_route_redirect(struct HSRoute *, struct HSHttpRequest *, int);
struct HSRouteServeResponse    *_hs_router_as_route_serve(struct HSRoute *, struct HSHttpRequest *, int);
void                           _hs_router_as_route_release(struct HSRoute *);
void _hs_router_run_callback(struct HSPostResponseCallback *);

struct HSRouter *hs_router_new()
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


bool hs_router_serve_forever(struct HSRouter *router, int socket, void *context, bool (*should_stop)(struct HSRouter *, int, int, void *))
{
  if (router == NULL || !socket)
  {
    return(false);
  }

  bool can_continue    = true;
  int  request_counter = 0;
  if (should_stop != NULL)
  {
    can_continue = !should_stop(router, socket, request_counter, context);
  }

  while (can_continue)
  {
    can_continue = hs_router_serve_next(router, socket);
    if (can_continue && should_stop != NULL)
    {
      request_counter++;
      can_continue = !should_stop(router, socket, request_counter, context);
    }
  }

  return(true);
}


bool hs_router_serve_next(struct HSRouter *router, int socket)
{
  if (router == NULL || !socket)
  {
    return(false);
  }

  struct HSHttpRequest *request = hs_parser_parse_request(socket);
  if (request == NULL)
  {
    return(false);
  }

  bool done         = hs_router_serve(router, request, socket);
  bool can_continue = done && !request->state.closed_connection;

  hs_types_release_http_request(request);

  return(can_continue);
}


bool hs_router_serve(struct HSRouter *router, struct HSHttpRequest *request, int socket)
{
  if (router == NULL || request == NULL || !socket || request->resource == NULL)
  {
    return(false);
  }

  // ensure request resource is valid
  size_t request_path_length = strlen(request->resource);
  if (!request_path_length || request->resource[0] != '/')
  {
    return(false);
  }

  size_t count = vector_size(router->routes);
  for (size_t index = 0; index < count; index++)
  {
    struct HSRoute *route = (struct HSRoute *)vector_get(router->routes, index);

    if (_hs_router_serve(router, request, socket, route))
    {
      return(true);
    }
  }

  return(false);
}

struct HSRoute *hs_router_as_route(struct HSRouter *router)
{
  if (router == NULL)
  {
    return(NULL);
  }

  struct HSRoute *route = hs_route_new_route();

  // enable for all method types as the routes will limit it
  route->is_get         = true;
  route->is_post        = true;
  route->is_put         = true;
  route->is_delete      = true;
  route->is_parent_path = true;

  route->redirect = _hs_router_as_route_redirect;
  route->serve    = _hs_router_as_route_serve;

  route->extension = router;
  route->release   = _hs_router_as_route_release;

  return(route);
}

struct StringBuffer *hs_router_write_common_response_header(enum HSHttpResponseCode code, struct HSKeyValueArray *headers, struct HSCookies *cookies, bool close_connection)
{
  struct StringBuffer *buffer = string_buffer_new();

  // write status line
  string_buffer_append_string(buffer, "HTTP/1.1 ");
  string_buffer_append_unsigned_int(buffer, code);
  string_buffer_append(buffer, ' ');
  string_buffer_append_unsigned_int(buffer, code);
  string_buffer_append_string(buffer, "\r\n");

  // write headers (may contain set cookie headers)
  if (headers != NULL && headers->count)
  {
    for (size_t index = 0; index < headers->count; index++)
    {
      struct HSKeyValue *header = (struct HSKeyValue *)headers->pairs[index];

      if (header != NULL && header->key != NULL && header->value != NULL)
      {
        string_buffer_append_string(buffer, header->key);
        string_buffer_append_string(buffer, ": ");
        string_buffer_append_string(buffer, header->value);
        string_buffer_append_string(buffer, "\r\n");
      }
    }
  }

  // write set cookie headers
  if (cookies != NULL && cookies->count)
  {
    for (size_t index = 0; index < cookies->count; index++)
    {
      struct HSCookie *cookie = (struct HSCookie *)cookies->cookies[index];

      if (cookie != NULL && cookie->name != NULL && cookie->value != NULL)
      {
        string_buffer_append_string(buffer, "Set-Cookie: ");
        string_buffer_append_string(buffer, cookie->name);
        string_buffer_append_string(buffer, "=");
        string_buffer_append_string(buffer, cookie->value);

        // write cookie attributes
        if (cookie->expires != NULL)
        {
          string_buffer_append_string(buffer, "; Expires=");
          string_buffer_append_string(buffer, cookie->expires);
        }
        if (cookie->max_age >= 0)
        {
          string_buffer_append_string(buffer, "; Max-Age=");
          string_buffer_append_int(buffer, cookie->max_age);
        }
        if (cookie->secure)
        {
          string_buffer_append_string(buffer, "; Secure");
        }
        if (cookie->http_only)
        {
          string_buffer_append_string(buffer, "; HttpOnly");
        }
        if (cookie->domain != NULL)
        {
          string_buffer_append_string(buffer, "; Domain=");
          string_buffer_append_string(buffer, cookie->domain);
        }
        if (cookie->path != NULL)
        {
          string_buffer_append_string(buffer, "; Path=");
          string_buffer_append_string(buffer, cookie->path);
        }

        string_buffer_append_string(buffer, "; SameSite=");
        switch (cookie->same_site)
        {
        case HS_COOKIE_SAME_SITE_NONE:
          string_buffer_append_string(buffer, "None");
          break;

        case HS_COOKIE_SAME_SITE_LAX:
          string_buffer_append_string(buffer, "Lax");
          break;

        case HS_COOKIE_SAME_SITE_STRICT:
          string_buffer_append_string(buffer, "Strict");
          break;
        }

        string_buffer_append_string(buffer, "\r\n");
      }
    }
  }

  if (close_connection)
  {
    string_buffer_append_string(buffer, "Connection: close\r\n");
  }
  else
  {
    string_buffer_append_string(buffer, "Connection: keep-alive\r\n");
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


bool _hs_router_serve(struct HSRouter *router, struct HSHttpRequest *request, int socket, struct HSRoute *route)
{
  if (router == NULL || request == NULL || !socket || route == NULL)
  {
    return(false);
  }

  // already served
  if (request->state.closed_connection)
  {
    return(true);
  }

  if (!hs_route_is_allowed_for_method(route, request))
  {
    return(false);
  }

  if (!hs_route_is_supported_path(route, request))
  {
    return(false);
  }

  // if close or unknown, add the close response header
  bool close_connection = !router->support_keep_alive
                          || request->connection == HS_CONNECTION_TYPE_CLOSE
                          || request->connection == HS_CONNECTION_TYPE_UNKNOWN;

  if (route->redirect != NULL && !request->state.prevent_redirect)
  {
    struct HSRouteRedirectResponse *redirection_response = route->redirect(route, request, socket);
    if (redirection_response != NULL)
    {
      if (redirection_response->code < 300 || redirection_response->code >= 400)
      {
        redirection_response->code = HS_HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT;
      }

      struct StringBuffer *header_buffer = hs_router_write_common_response_header(redirection_response->code,
                                                                                  redirection_response->headers,
                                                                                  redirection_response->cookies, close_connection);

      if (redirection_response->path != NULL)
      {
        // write redirection target location
        string_buffer_append_string(header_buffer, "Location: ");
        string_buffer_append_string(header_buffer, redirection_response->path);
        string_buffer_append_string(header_buffer, "\r\n");
      }
      string_buffer_append_string(header_buffer, "Content-Length: 0\r\n\r\n");

      char *header_string = string_buffer_to_string(header_buffer);
      hs_io_write_string_to_socket(socket, header_string);
      string_buffer_release(header_buffer);
      hs_io_free(header_string);

      if (close_connection)
      {
        hs_io_close(socket);
        request->state.closed_connection = true;
      }

      _hs_router_run_callback(redirection_response->callback);
      hs_route_release_redirect_response(redirection_response);

      return(true);
    }
  }

  if (route->serve != NULL && !request->state.prevent_serve)
  {
    struct HSRouteServeResponse *serve_response = route->serve(route, request, socket);
    if (serve_response != NULL)
    {
      struct StringBuffer *header_buffer = hs_router_write_common_response_header(serve_response->code,
                                                                                  serve_response->headers,
                                                                                  serve_response->cookies, close_connection);

      const char *mime_type = hs_constants_mime_type_to_string(serve_response->mime_type);
      if (mime_type != NULL)
      {
        string_buffer_append_string(header_buffer, "Content-Type: ");
        string_buffer_append_string(header_buffer, (char *)mime_type);
        string_buffer_append_string(header_buffer, "\r\n");
      }

      if (serve_response->content_string != NULL || serve_response->content_file != NULL)
      {
        bool has_content = true;
        if (serve_response->content_string != NULL)
        {
          size_t content_length = strlen(serve_response->content_string);
          string_buffer_append_string(header_buffer, "Content-Length: ");
          string_buffer_append_unsigned_long(header_buffer, content_length);
          string_buffer_append_string(header_buffer, "\r\n\r\n");

          if (!content_length)
          {
            has_content = false;
          }
        }
        else if (serve_response->content_file != NULL)
        {
          long content_length = fsio_file_size(serve_response->content_file);
          if (content_length)
          {
            string_buffer_append_string(header_buffer, "Content-Length: ");
            string_buffer_append_long(header_buffer, content_length);
            string_buffer_append_string(header_buffer, "\r\n\r\n");
          }
          else
          {
            // file validations should be done by the route, so we are keeping response status code
            // as is and just not returning any content.
            has_content = false;
            string_buffer_append_string(header_buffer, "Content-Length: 0\r\n\r\n");
          }
        }
        else
        {
          has_content = false;
          string_buffer_append_string(header_buffer, "Content-Length: 0\r\n\r\n");
        }

        char *header_string = string_buffer_to_string(header_buffer);
        hs_io_write_string_to_socket(socket, header_string);
        hs_io_free(header_string);

        if (has_content)
        {
          if (serve_response->content_string != NULL)
          {
            hs_io_write_string_to_socket(socket, serve_response->content_string);
          }
          else
          {
            hs_io_write_file_to_socket(socket, serve_response->content_file);
          }
        }
      }
      else
      {
        string_buffer_append_string(header_buffer, "Content-Length: 0\r\n\r\n");
        char *header_string = string_buffer_to_string(header_buffer);
        hs_io_write_string_to_socket(socket, header_string);
        hs_io_free(header_string);
      }

      string_buffer_release(header_buffer);

      if (close_connection)
      {
        hs_io_close(socket);
        request->state.closed_connection = true;
      }

      _hs_router_run_callback(serve_response->callback);
      hs_route_release_serve_response(serve_response);

      return(true);
    }
  }

  // if the route wrote the response on its own (router as route)
  // done=true so we may have gotten null responses, but we actually
  // handled it and need to stop looping.
  return(request->state.done);
}   /* _hs_router_serve */


struct HSRouteRedirectResponse *_hs_router_as_route_redirect(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  if (  route == NULL
     || request == NULL
     || !socket
     || route->extension == NULL
     || request->state.prevent_redirect
     || request->state.closed_connection)
  {
    return(NULL);
  }

  char            *original_base_path = request->state.base_path;
  request->state.base_path = fsio_join_paths(original_base_path, route->path);
  char            *original_path = hs_router_remove_path_prefix(route, request);

  struct HSRouter *router = (struct HSRouter *)route->extension;

  bool            prevent_serve = request->state.prevent_serve;
  request->state.prevent_serve = true;
  request->state.done          = hs_router_serve(router, request, socket);
  request->state.prevent_serve = prevent_serve;

  request->resource = original_path;
  hs_io_free(request->state.base_path);
  request->state.base_path = original_base_path;

  return(NULL);
}

struct HSRouteServeResponse *_hs_router_as_route_serve(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  if (  route == NULL
     || request == NULL
     || !socket
     || route->extension == NULL
     || request->state.prevent_serve
     || request->state.closed_connection)
  {
    return(NULL);
  }

  char            *original_base_path = request->state.base_path;
  request->state.base_path = fsio_join_paths(original_base_path, route->path);
  char            *original_path = hs_router_remove_path_prefix(route, request);

  struct HSRouter *router = (struct HSRouter *)route->extension;

  bool            prevent_redirect = request->state.prevent_redirect;
  request->state.prevent_redirect = true;
  request->state.done             = hs_router_serve(router, request, socket);
  request->state.prevent_redirect = prevent_redirect;

  request->resource = original_path;
  hs_io_free(request->state.base_path);
  request->state.base_path = original_base_path;

  return(NULL);
}


void _hs_router_as_route_release(struct HSRoute *route)
{
  if (route == NULL || route->extension == NULL)
  {
    return;
  }

  struct HSRouter *router = (struct HSRouter *)route->extension;

  hs_router_release(router);
}


void _hs_router_run_callback(struct HSPostResponseCallback *callback)
{
  if (callback == NULL || callback->run == NULL)
  {
    return;
  }

  callback->run(callback);
}

