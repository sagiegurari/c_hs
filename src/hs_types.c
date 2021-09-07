#include "hs_io.h"
#include "hs_types.h"
#include "stringfn.h"
#include <stdio.h>
#include <stdlib.h>

#define HS_TYPES_DEFAULT_HEADERS_CAPACITY                          50
#define HS_TYPES_DEFAULT_POST_RESPONSE_CALLBACKS_CAPACITY          10
#define HS_TYPES_DEFAULT_ROUTE_FLOW_STATE_STRING_PAIRS_CAPACITY    20
#define HS_TYPES_DEFAULT_ROUTE_FLOW_STATE_DATA_CAPACITY            20

struct HSHttpRequestPayload
{
  bool                          loaded;
  struct HSIOHttpRequestPayload *payload;
};

struct HSServeFlowParams *hs_types_new_serve_flow_params()
{
  struct HSHttpRequest *request = hs_types_new_http_request();

  return(hs_types_new_serve_flow_params_pre_populated(request));
}

struct HSServeFlowParams *hs_types_new_serve_flow_params_pre_populated(struct HSHttpRequest *request)
{
  struct HSServeFlowParams *params = malloc(sizeof(struct HSServeFlowParams));

  params->request      = request;
  params->response     = hs_types_new_http_response();
  params->socket       = 0;
  params->callbacks    = hs_types_new_post_response_callbacks(HS_TYPES_DEFAULT_POST_RESPONSE_CALLBACKS_CAPACITY);
  params->route_state  = hs_types_new_route_flow_state(HS_TYPES_DEFAULT_ROUTE_FLOW_STATE_DATA_CAPACITY);
  params->router_state = hs_types_new_router_flow_state();

  return(params);
}


void hs_types_release_serve_flow_params(struct HSServeFlowParams *params)
{
  if (params == NULL)
  {
    return;
  }

  hs_types_release_http_request(params->request);
  hs_types_release_http_response(params->response);
  hs_types_release_post_response_callbacks(params->callbacks);
  hs_types_release_route_flow_state(params->route_state);
  hs_types_release_router_flow_state(params->router_state);

  hs_io_free(params);
}

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
  request->authorization  = NULL;
  request->cookies        = hs_types_cookies_new();
  request->headers        = hs_types_new_key_value_array(HS_TYPES_DEFAULT_HEADERS_CAPACITY);
  request->payload        = NULL;

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
  hs_io_free(request->authorization);

  hs_types_cookies_release(request->cookies);

  hs_types_release_key_value_array(request->headers);

  if (request->payload != NULL)
  {
    hs_io_release_http_request_payload(request->payload->payload);
    request->payload->payload = NULL;
    hs_io_free(request->payload);
  }

  hs_io_free(request);
}


struct HSHttpResponse *hs_types_new_http_response()
{
  struct HSHttpResponse *response = malloc(sizeof(struct HSHttpResponse));

  response->code           = HS_HTTP_RESPONSE_CODE_OK;
  response->cookies        = hs_types_cookies_new();
  response->headers        = hs_types_new_key_value_array(HS_TYPES_DEFAULT_HEADERS_CAPACITY);
  response->mime_type      = HS_MIME_TYPE_NONE;
  response->content_string = NULL;
  response->content_file   = NULL;

  return(response);
}


void hs_types_release_http_response(struct HSHttpResponse *response)
{
  if (response == NULL)
  {
    return;
  }

  hs_io_free(response->content_string);
  hs_io_free(response->content_file);
  hs_types_cookies_release(response->cookies);
  hs_types_release_key_value_array(response->headers);

  hs_io_free(response);
}

struct HSPostResponseCallback *hs_types_new_post_response_callback()
{
  struct HSPostResponseCallback *callback = malloc(sizeof(struct HSPostResponseCallback));

  callback->context = NULL;
  callback->run     = NULL;
  callback->release = NULL;

  return(callback);
}


void hs_types_release_post_response_callback(struct HSPostResponseCallback *callback)
{
  if (callback == NULL)
  {
    return;
  }

  if (callback->release != NULL)
  {
    callback->release(callback);
  }

  hs_io_free(callback);
}

struct HSPostResponseCallbacks *hs_types_new_post_response_callbacks(size_t capacity)
{
  struct HSPostResponseCallbacks *callbacks = malloc(sizeof(struct HSPostResponseCallbacks));

  callbacks->count    = 0;
  callbacks->capacity = capacity;

  callbacks->callbacks = malloc(sizeof(struct HSPostResponseCallback *) * callbacks->capacity);

  return(callbacks);
}


void hs_types_release_post_response_callbacks(struct HSPostResponseCallbacks *callbacks)
{
  if (callbacks == NULL)
  {
    return;
  }

  if (callbacks->count)
  {
    for (size_t index = 0; index < callbacks->count; index++)
    {
      struct HSPostResponseCallback *callback = callbacks->callbacks[index];
      hs_types_release_post_response_callback(callback);
    }
  }
  hs_io_free(callbacks->callbacks);

  hs_io_free(callbacks);
}


bool hs_types_post_response_callbacks_add(struct HSPostResponseCallbacks *callbacks, struct HSPostResponseCallback *callback)
{
  if (callbacks == NULL || callback == NULL)
  {
    return(false);
  }

  if (callbacks->count >= callbacks->capacity)
  {
    struct HSPostResponseCallback **old_callbacks = callbacks->callbacks;
    callbacks->capacity  = callbacks->capacity * 2;
    callbacks->callbacks = malloc(sizeof(struct HSPostResponseCallback *) * callbacks->capacity);

    for (size_t index = 0; index < callbacks->count; index++)
    {
      callbacks->callbacks[index] = old_callbacks[index];
    }

    hs_io_free(old_callbacks);
  }

  callbacks->callbacks[callbacks->count] = callback;
  callbacks->count++;

  return(true);
}

struct HSRouterFlowState *hs_types_new_router_flow_state()
{
  struct HSRouterFlowState *state = malloc(sizeof(struct HSRouterFlowState));

  state->done              = false;
  state->closed_connection = false;
  state->base_path         = NULL;

  return(state);
}


void hs_types_release_router_flow_state(struct HSRouterFlowState *state)
{
  if (state == NULL)
  {
    return;
  }

  hs_io_free(state->base_path);
  hs_io_free(state);
}

struct HSRouteFlowState *hs_types_new_route_flow_state(size_t capacity)
{
  struct HSRouteFlowState *state = malloc(sizeof(struct HSRouteFlowState));

  state->string_pairs = hs_types_new_key_value_array(HS_TYPES_DEFAULT_ROUTE_FLOW_STATE_STRING_PAIRS_CAPACITY);

  state->data_capacity = capacity;
  state->data_count    = 0;
  state->data          = malloc(sizeof(void *) * state->data_capacity);
  state->data_keys     = malloc(sizeof(char *) * state->data_capacity);

  return(state);
}


void hs_types_release_route_flow_state(struct HSRouteFlowState *state)
{
  if (state == NULL)
  {
    return;
  }

  hs_types_release_key_value_array(state->string_pairs);
  hs_io_free(state->data);

  for (size_t index = 0; index < state->data_count; index++)
  {
    hs_io_free(state->data_keys[index]);
  }
  hs_io_free(state->data_keys);

  hs_io_free(state);
}


void hs_types_route_flow_state_add_data(struct HSRouteFlowState *state, char *key, void *data)
{
  if (state == NULL || key == NULL || data == NULL)
  {
    return;
  }

  if (state->data_count >= state->data_capacity)
  {
    void **old_data      = state->data;
    char **old_data_keys = state->data_keys;
    state->data_capacity = state->data_capacity * 2;
    state->data          = malloc(sizeof(void *) * state->data_capacity);
    state->data_keys     = malloc(sizeof(char *) * state->data_capacity);

    for (size_t index = 0; index < state->data_count; index++)
    {
      state->data[index]      = old_data[index];
      state->data_keys[index] = old_data_keys[index];
    }

    hs_io_free(old_data);
    hs_io_free(old_data_keys);
  }

  state->data[state->data_count]      = data;
  state->data_keys[state->data_count] = key;
  state->data_count++;
}


void *hs_types_route_flow_state_get_data_by_key(struct HSRouteFlowState *state, char *key)
{
  if (state == NULL || key == NULL)
  {
    return(NULL);
  }

  for (size_t index = 0; index < state->data_count; index++)
  {
    if (state->data_keys[index] != NULL && stringfn_equal(state->data_keys[index], key))
    {
      return(state->data[index]);
    }
  }

  return(NULL);
}

struct HSKeyValueArray *hs_types_new_key_value_array(size_t capacity)
{
  struct HSKeyValueArray *array = malloc(sizeof(struct HSKeyValueArray));

  array->count    = 0;
  array->capacity = capacity;

  array->pairs = malloc(sizeof(struct HSKeyValue *) * array->capacity);

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
  }

  hs_io_free(array->pairs);

  hs_io_free(array);
}


char *hs_types_key_value_array_get_by_key(struct HSKeyValueArray *array, char *key)
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


bool hs_types_key_value_array_add(struct HSKeyValueArray *array, char *key, char *value)
{
  if (array == NULL || key == NULL)
  {
    return(false);
  }

  struct HSKeyValue *key_value = hs_types_new_key_value(key, value);
  if (key_value == NULL)
  {
    return(false);
  }

  if (array->count >= array->capacity)
  {
    struct HSKeyValue **old_pairs = array->pairs;
    array->capacity = array->capacity * 2;
    array->pairs    = malloc(sizeof(struct HSKeyValue *) * array->capacity);

    for (size_t index = 0; index < array->count; index++)
    {
      array->pairs[index] = old_pairs[index];
    }

    hs_io_free(old_pairs);
  }

  array->pairs[array->count] = key_value;
  array->count++;

  return(true);
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

