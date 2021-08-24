#include "hs_io.h"
#include "hs_types.h"
#include "stringfn.h"
#include <stdio.h>
#include <stdlib.h>

struct HSHttpRequestPayload
{
  bool                          loaded;
  struct HSIOHttpRequestPayload *payload;
};

struct HSHttpRequest *hs_types_new_http_request()
{
  struct HSHttpRequest *request = malloc(sizeof(struct HSHttpRequest));

  request->method         = HS_HTTP_METHOD_GET;
  request->domain         = NULL;
  request->port           = -1;
  request->ssl            = false;
  request->resource       = NULL;
  request->query_string   = NULL;
  request->content_length = 0;
  request->connection     = HS_CONNECTION_TYPE_UNKNOWN;
  request->user_agent     = NULL;
  request->cookies        = hs_types_new_cookies();
  request->headers        = hs_types_new_key_value_array();
  request->payload        = NULL;

  // setup state
  request->state.done              = false;
  request->state.closed_connection = false;
  request->state.prevent_redirect  = false;
  request->state.prevent_serve     = false;
  request->state.base_path         = NULL;

  return(request);
}


void hs_types_release_http_request(struct HSHttpRequest *request)
{
  if (request == NULL)
  {
    return;
  }

  hs_io_free(request->domain);
  hs_io_free(request->resource);
  hs_io_free(request->query_string);
  hs_io_free(request->user_agent);

  hs_types_release_cookies(request->cookies);
  request->cookies = NULL;

  hs_types_release_key_value_array(request->headers);
  request->headers = NULL;

  if (request->payload != NULL)
  {
    hs_io_release_http_request_payload(request->payload->payload);
    request->payload->payload = NULL;
    hs_io_free(request->payload);
    request->payload = NULL;
  }

  hs_io_free(request->state.base_path);

  hs_io_free(request);
}

struct HSCookie *hs_types_new_cookie()
{
  struct HSCookie *cookie = malloc(sizeof(struct HSCookie));

  cookie->name      = NULL;
  cookie->value     = NULL;
  cookie->expires   = NULL;
  cookie->max_age   = -1;
  cookie->secure    = false;
  cookie->http_only = false;
  cookie->domain    = NULL;
  cookie->path      = NULL;
  cookie->same_site = HS_COOKIE_SAME_SITE_LAX;

  return(cookie);
}


void hs_types_release_cookie(struct HSCookie *cookie)
{
  if (cookie == NULL)
  {
    return;
  }

  hs_io_free(cookie->name);
  hs_io_free(cookie->value);
  hs_io_free(cookie->expires);
  hs_io_free(cookie->domain);
  hs_io_free(cookie->path);

  hs_io_free(cookie);
}

struct HSCookies *hs_types_new_cookies()
{
  struct HSCookies *cookies = malloc(sizeof(struct HSCookies));

  cookies->cookies = NULL;
  cookies->count   = 0;

  return(cookies);
}


void hs_types_release_cookies(struct HSCookies *cookies)
{
  if (cookies == NULL)
  {
    return;
  }

  if (cookies->count)
  {
    for (size_t index = 0; index < cookies->count; index++)
    {
      struct HSCookie *cookie = cookies->cookies[index];
      hs_types_release_cookie(cookie);
    }

    hs_io_free(cookies->cookies);
    cookies->cookies = NULL;
    cookies->count   = 0;
  }

  hs_io_free(cookies);
}

struct HSKeyValueArray *hs_types_new_key_value_array()
{
  struct HSKeyValueArray *array = malloc(sizeof(struct HSKeyValueArray));

  array->pairs = NULL;
  array->count = 0;

  return(array);
}


void hs_types_release_key_value_array(struct HSKeyValueArray *array)
{
  if (array == NULL)
  {
    return;
  }

  if (array->count)
  {
    for (size_t index = 0; index < array->count; index++)
    {
      struct HSKeyValue *key_value = array->pairs[index];
      hs_types_release_key_value(key_value);
    }

    hs_io_free(array->pairs);
    array->pairs = NULL;
    array->count = 0;
  }

  hs_io_free(array);
}


char *hs_types_get_value_for_key_from_array(struct HSKeyValueArray *array, char *key)
{
  if (array == NULL || key == NULL || !array->count)
  {
    return(NULL);
  }

  for (size_t index = 0; index < array->count; index++)
  {
    struct HSKeyValue *pair = array->pairs[index];

    if (pair != NULL && stringfn_equal(pair->key, key))
    {
      return(pair->value);
    }
  }

  return(NULL);
}

struct HSKeyValue *hs_types_new_key_value(char *key, char *value)
{
  struct HSKeyValue *key_value = malloc(sizeof(struct HSKeyValue));

  key_value->key   = key;
  key_value->value = value;

  return(key_value);
}


void hs_types_release_key_value(struct HSKeyValue *key_value)
{
  if (key_value == NULL)
  {
    return;
  }

  hs_io_free(key_value->key);
  hs_io_free(key_value->value);
  hs_io_free(key_value);
}

struct HSHttpRequestPayload *hs_types_new_http_request_payload(void *io_payload)
{
  struct HSIOHttpRequestPayload *io_payload_struct = (struct HSIOHttpRequestPayload *)io_payload;

  struct HSHttpRequestPayload   *payload = malloc(sizeof(struct HSHttpRequestPayload));

  payload->loaded  = false;
  payload->payload = io_payload_struct;

  return(payload);
}


bool hs_types_http_request_payload_is_loaded(struct HSHttpRequest *request)
{
  if (request == NULL || request->payload == NULL)
  {
    return(false);
  }

  return(request->payload->loaded);
}


char *hs_types_http_request_payload_to_string(struct HSHttpRequest *request)
{
  if (request == NULL || request->payload == NULL || request->payload->loaded)
  {
    return(NULL);
  }

  request->payload->loaded = true;

  struct StringBuffer *buffer = request->payload->payload->partial;
  size_t              length  = request->content_length;
  size_t              trim    = 0;
  if (buffer == NULL)
  {
    buffer = string_buffer_new();
  }
  else
  {
    size_t current_length = string_buffer_get_content_size(request->payload->payload->partial);
    if (current_length > length)
    {
      trim   = length;
      length = 0;
    }
    else if (current_length == length)
    {
      length = 0;
    }
    else
    {
      length = length - current_length;
    }
  }

  if (length)
  {
    hs_io_read_fully(request->payload->payload->socket, buffer, length);
  }

  char *content = string_buffer_to_string(buffer);
  if (trim > 0)
  {
    stringfn_mut_substring(content, 0, trim);
  }

  string_buffer_release(buffer);
  request->payload->payload->partial = NULL;

  return(content);
} /* hs_types_http_request_payload_to_string */


bool hs_types_http_request_payload_to_file(struct HSHttpRequest *request, char *filename)
{
  if (request == NULL || request->payload == NULL || request->payload->loaded || filename == NULL)
  {
    return(false);
  }

  request->payload->loaded = true;

  size_t length = request->content_length;
  if (!length)
  {
    return(true);
  }

  FILE *fp = fopen(filename, "w");
  if (fp == NULL)
  {
    return(false);
  }

  char *text = NULL;
  if (request->payload->payload->partial != NULL)
  {
    size_t current_length = string_buffer_get_content_size(request->payload->payload->partial);
    size_t trim           = 0;
    if (current_length > length)
    {
      trim   = length;
      length = 0;
    }
    else if (current_length == length)
    {
      length = 0;
    }
    else
    {
      length = length - current_length;
    }

    if (current_length)
    {
      text = string_buffer_to_string(request->payload->payload->partial);
      if (trim > 0)
      {
        stringfn_mut_substring(text, 0, trim);
      }
      string_buffer_release(request->payload->payload->partial);
      request->payload->payload->partial = NULL;

      if (fputs(text, fp) == EOF)
      {
        hs_io_free(text);
        fclose(fp);

        // prevent partially written file to be
        remove(filename);

        return(false);
      }
      hs_io_free(text);
    }
  }

  bool done = hs_io_read_and_write_to_file(request->payload->payload->socket, fp, length);

  fflush(fp);
  fclose(fp);

  if (!done)
  {
    remove(filename);
  }

  return(done);
} /* hs_types_http_request_payload_to_file */

