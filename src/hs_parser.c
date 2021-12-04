#include "hs_io.h"
#include "hs_parser.h"
#include "hs_types.h"
#include "string_buffer.h"
#include "stringfn.h"
#include <stdlib.h>
#include <string.h>

struct HSHttpRequest *_hs_parser_create_request_from_path(char *url_or_resource, bool force_protocol);

struct HSHttpRequest *hs_parser_create_request_from_resource(char *resource)
{
  return(_hs_parser_create_request_from_path(resource, false));
}

struct HSHttpRequest *hs_parser_create_request_from_url(char *url)
{
  return(_hs_parser_create_request_from_path(url, true));
}

struct HSHttpRequest *hs_parser_parse_request_line(char *line)
{
  if (line == NULL)
  {
    return(NULL);
  }

  struct StringFNStrings words    = stringfn_split_words(line);
  struct HSHttpRequest   *request = NULL;

  if (words.count == 3)
  {
    request = hs_parser_create_request_from_resource(words.strings[1]);

    if (request != NULL)
    {
      request->method = hs_parser_parse_method(words.strings[0]);
    }
  }

  stringfn_release_strings_struct(words);

  return(request);
}


char **hs_parser_parse_header(char *line)
{
  if (line == NULL)
  {
    return(NULL);
  }

  size_t length = strlen(line);

  if (length < 3)
  {
    return(NULL);
  }

  size_t end_index = 0;
  for (size_t index = 0; index < length - 1; index++)
  {
    if (line[index] == ':' && line[index + 1] == ' ')
    {
      end_index = index;
    }
  }

  if (!end_index)
  {
    return(NULL);
  }

  char *key = stringfn_substring(line, 0, end_index);
  stringfn_mut_to_lowercase(key);

  char *value = NULL;
  if (length > (end_index + 2))
  {
    value = stringfn_substring(line, (int)end_index + 2, 0);
  }

  char **header = malloc(sizeof(char *) * 2);
  header[0] = key;
  header[1] = value;

  return(header);
} /* hs_parser_parse_header */


bool hs_parser_parse_cookie_header(struct HSCookies *cookies, char *cookie_line)
{
  if (cookies == NULL || cookie_line == NULL)
  {
    return(false);
  }

  size_t length = strlen(cookie_line);
  bool   added  = false;
  if (length)
  {
    struct StringBuffer *buffer = string_buffer_new_with_options(length, true);

    char                *name            = NULL;
    char                *value           = NULL;
    bool                looking_for_name = true;
    for (size_t index = 0; index < length; index++)
    {
      char character = cookie_line[index];

      if (looking_for_name)
      {
        if (character == '=')
        {
          looking_for_name = false;
          name             = string_buffer_to_string(buffer);
          string_buffer_clear(buffer);
        }
        else if (character != ' ' || !string_buffer_is_empty(buffer))
        {
          string_buffer_append(buffer, character);
        }
      }
      else
      {
        if (character == ';' && (index == length - 1 || cookie_line[index + 1] == ' '))
        {
          looking_for_name = true;
          value            = string_buffer_to_string(buffer);
          string_buffer_clear(buffer);

          struct HSCookie *cookie = hs_types_cookie_new();
          cookie->name  = name;
          cookie->value = value;
          added         = hs_types_cookies_add(cookies, cookie);

          name  = NULL;
          value = NULL;
        }
        else
        {
          string_buffer_append(buffer, character);
        }
      }
    }

    if (!looking_for_name && !string_buffer_is_empty(buffer))
    {
      looking_for_name = true;
      value            = string_buffer_to_string(buffer);

      struct HSCookie *cookie = hs_types_cookie_new();
      cookie->name  = name;
      cookie->value = value;
      added         = hs_types_cookies_add(cookies, cookie);
    }
    else
    {
      hs_io_free(name);
    }

    string_buffer_release(buffer);
  }

  return(added);
} /* hs_parser_parse_cookie_header */


void hs_parser_set_header(struct HSHttpRequest *request, char *key, char *value)
{
  if (request == NULL || key == NULL || value == NULL)
  {
    return;
  }

  if (stringfn_equal(key, "cookie"))
  {
    hs_parser_parse_cookie_header(request->cookies, value);
  }
  else if (stringfn_equal(key, "content-length"))
  {
    request->content_length = (size_t)atoi(value);
  }
  else if (stringfn_equal(key, "connection"))
  {
    if (stringfn_equal(value, "close"))
    {
      request->connection = HS_CONNECTION_TYPE_CLOSE;
    }
    else if (stringfn_starts_with(value, "keep-alive"))
    {
      request->connection = HS_CONNECTION_TYPE_KEEP_ALIVE;
    }
  }
  else if (stringfn_equal(key, "user-agent"))
  {
    request->user_agent = strdup(value);
  }
  else if (stringfn_equal(key, "authorization"))
  {
    request->authorization = strdup(value);
  }
}

struct HSHttpRequest *hs_parser_parse_request(struct HSSocket *socket)
{
  if (socket == NULL)
  {
    return(NULL);
  }

  // read request line
  struct StringBuffer *work_buffer = string_buffer_new();
  char                *line        = hs_io_read_line(socket, work_buffer);
  if (line == NULL)
  {
    string_buffer_release(work_buffer);
    return(NULL);
  }

  struct HSHttpRequest *request = hs_parser_parse_request_line(line);
  hs_io_free(line);

  do
  {
    line = hs_io_read_line(socket, work_buffer);
    if (line != NULL)
    {
      char **header = hs_parser_parse_header(line);
      hs_io_free(line);

      if (header != NULL)
      {
        hs_types_array_string_pair_add(request->headers, header[0], header[1]);
        hs_parser_set_header(request, header[0], header[1]);
        hs_io_free(header);
      }
    }
  } while (line != NULL);

  struct HSIOHttpRequestPayload *io_payload = hs_io_new_http_request_payload(socket, work_buffer);
  request->payload = hs_types_http_request_new_payload(io_payload);

  return(request);
}   /* hs_parser_parse_request */

enum HSHttpMethod hs_parser_parse_method(char *method_string)
{
  if (method_string == NULL)
  {
    return(HS_HTTP_METHOD_UNKNOWN);
  }
  if (stringfn_equal(method_string, "GET"))
  {
    return(HS_HTTP_METHOD_GET);
  }
  if (stringfn_equal(method_string, "POST"))
  {
    return(HS_HTTP_METHOD_POST);
  }
  if (stringfn_equal(method_string, "PUT"))
  {
    return(HS_HTTP_METHOD_PUT);
  }
  if (stringfn_equal(method_string, "DELETE"))
  {
    return(HS_HTTP_METHOD_DELETE);
  }
  if (stringfn_equal(method_string, "HEAD"))
  {
    return(HS_HTTP_METHOD_HEAD);
  }
  if (stringfn_equal(method_string, "CONNECT"))
  {
    return(HS_HTTP_METHOD_CONNECT);
  }
  if (stringfn_equal(method_string, "OPTIONS"))
  {
    return(HS_HTTP_METHOD_OPTIONS);
  }
  if (stringfn_equal(method_string, "TRACE"))
  {
    return(HS_HTTP_METHOD_TRACE);
  }
  if (stringfn_equal(method_string, "PATCH"))
  {
    return(HS_HTTP_METHOD_PATCH);
  }

  return(HS_HTTP_METHOD_UNKNOWN);
}   /* _hs_parser_parse_method */

struct HSArrayStringPair *hs_parser_parse_query_string(char *query_string)
{
  if (query_string == NULL)
  {
    return(NULL);
  }

  size_t length = strlen(query_string);
  if (!length)
  {
    return(NULL);
  }

  struct StringBuffer      *buffer = string_buffer_new();
  struct HSArrayStringPair *array  = hs_types_array_string_pair_new();

  char                     *key         = NULL;
  char                     *value       = NULL;
  bool                     look_for_key = true;
  for (size_t index = 0; index < length; index++)
  {
    char character = query_string[index];

    if (look_for_key)
    {
      if (character == '&')
      {
        key = string_buffer_to_string(buffer);
        string_buffer_clear(buffer);

        hs_types_array_string_pair_add(array, key, NULL);

        key = NULL;
      }
      else if (character == '=')
      {
        key = string_buffer_to_string(buffer);
        string_buffer_clear(buffer);
        look_for_key = false;
      }
      else
      {
        string_buffer_append(buffer, character);
      }
    }
    else
    {
      if (character == '&')
      {
        if (!string_buffer_is_empty(buffer))
        {
          value = string_buffer_to_string(buffer);
          string_buffer_clear(buffer);
        }

        hs_types_array_string_pair_add(array, key, value);

        key          = NULL;
        value        = NULL;
        look_for_key = true;
      }
      else
      {
        string_buffer_append(buffer, character);
      }
    }
  }

  if (look_for_key)
  {
    if (!string_buffer_is_empty(buffer))
    {
      key = string_buffer_to_string(buffer);
      hs_types_array_string_pair_add(array, key, NULL);
    }
  }
  else
  {
    if (!string_buffer_is_empty(buffer))
    {
      value = string_buffer_to_string(buffer);
    }

    hs_types_array_string_pair_add(array, key, value);
  }

  string_buffer_release(buffer);

  return(array);
} /* hs_parser_parse_query_string */

enum HSHttpProtocol hs_parser_parse_protocol_from_url(char *url)
{
  if (url == NULL)
  {
    return(HS_HTTP_PROTOCOL_UNKNOWN);
  }

  size_t length = strlen(url);
  if (length < 7)
  {
    return(HS_HTTP_PROTOCOL_UNKNOWN);
  }

  size_t prefix_length = length >= 8 ? 8 : 7;
  char   *prefix       = stringfn_substring(url, 0, prefix_length);
  stringfn_mut_to_lowercase(prefix);

  enum HSHttpProtocol protocol = HS_HTTP_PROTOCOL_UNKNOWN;
  if (stringfn_starts_with(prefix, "https://"))
  {
    protocol = HS_HTTP_PROTOCOL_HTTPS;
  }
  if (stringfn_starts_with(prefix, "http://"))
  {
    protocol = HS_HTTP_PROTOCOL_HTTP;
  }

  hs_io_free(prefix);

  return(protocol);
}

struct HSHttpRequest *_hs_parser_create_request_from_path(char *url_or_resource, bool force_protocol)
{
  if (url_or_resource == NULL)
  {
    return(NULL);
  }

  enum HSHttpProtocol protocol = hs_parser_parse_protocol_from_url(url_or_resource);
  bool                ssl      = protocol == HS_HTTP_PROTOCOL_HTTPS;
  if (force_protocol && protocol == HS_HTTP_PROTOCOL_UNKNOWN)
  {
    return(NULL);
  }

  size_t offset = 0;
  switch (protocol)
  {
  case HS_HTTP_PROTOCOL_HTTP:
    offset = 7;
    break;

  case HS_HTTP_PROTOCOL_HTTPS:
    offset = 8;
    break;

  case HS_HTTP_PROTOCOL_UNKNOWN:
    offset = 0;
    break;
  }

  char *url_clone;
  if (offset)
  {
    url_clone = stringfn_substring(url_or_resource, (int)offset, 0);
  }
  else
  {
    url_clone = strdup(url_or_resource);
  }

  char *found        = strchr(url_clone, '?');
  char *query_string = NULL;
  if (found != NULL)
  {
    if (strlen(found) > 1)
    {
      query_string = strdup(&found[1]);
    }
    *found = '\0';
  }

  char *port_separator = strchr(url_clone, ':');
  char *path_separator = strchr(url_clone, '/');
  if (port_separator != NULL && path_separator != NULL)
  {
    if (port_separator < path_separator)
    {
      found = port_separator;
    }
    else
    {
      found = NULL;
    }
  }
  else
  {
    found = port_separator;
  }

  char *domain   = NULL;
  char *resource = NULL;
  int  port      = -1;
  if (found != NULL)
  {
    *found = '\0';
    domain = strdup(url_clone);
    *found = ':';

    if (strlen(found) > 1)
    {
      found = found + 1;
      char *next_found = strchr(found, '/');

      if (next_found != NULL)
      {
        *next_found = '\0';
        port        = atoi(found);
        *next_found = '/';
        found       = next_found;
        if (strlen(found) > 1)
        {
          resource = strdup(found);
        }
        else
        {
          resource = strdup("/");
        }
      }
      else
      {
        port     = atoi(found);
        resource = strdup("/");
      }

      hs_io_free(url_clone);
    }
  }
  else
  {
    found = strchr(url_clone, '/');

    if (found != NULL)
    {
      *found = '\0';
      domain = strdup(url_clone);
      *found = '/';
      if (strlen(found) > 1)
      {
        resource = strdup(found);
      }
      else
      {
        resource = strdup("/");
      }
    }
    else
    {
      domain   = strdup(url_clone);
      resource = strdup("/");
    }

    hs_io_free(url_clone);
  }

  struct HSHttpRequest *request = hs_types_http_request_new();
  request->method       = HS_HTTP_METHOD_UNKNOWN;
  request->domain       = domain;
  request->port         = port;
  request->ssl          = ssl;
  request->resource     = resource;
  request->query_string = query_string;

  return(request);
}   /* _hs_parser_create_request_from_path */

