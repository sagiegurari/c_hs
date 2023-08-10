#include "hs_external_libs.h"
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

struct HSServeFlowParams *hs_types_serve_flow_params_new()
{
  struct HSHttpRequest *request = hs_types_http_request_new();

  return(hs_types_serve_flow_params_new_pre_populated(request));
}

struct HSServeFlowParams *hs_types_serve_flow_params_new_pre_populated(struct HSHttpRequest *request)
{
  struct HSServeFlowParams *params = malloc(sizeof(struct HSServeFlowParams));

  params->request          = request;
  params->response         = hs_types_http_response_new();
  params->callbacks        = hs_types_post_response_callbacks_new(1);
  params->route_state      = hs_types_route_flow_state_new();
  params->router_state     = hs_types_router_flow_state_new();
  params->connection_state = NULL;

  return(params);
}


void hs_types_serve_flow_params_release(struct HSServeFlowParams *params)
{
  if (params == NULL)
  {
    return;
  }

  hs_types_http_request_release(params->request);
  hs_types_http_response_release(params->response);
  hs_types_post_response_callbacks_release(params->callbacks);
  hs_types_route_flow_state_release(params->route_state);
  hs_types_router_flow_state_release(params->router_state);

  hs_io_free(params);
}

struct HSHttpRequest *hs_types_http_request_new()
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
  request->headers        = hs_types_array_string_pair_new();
  request->payload        = NULL;

  return(request);
}


void hs_types_http_request_release(struct HSHttpRequest *request)
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

  hs_types_array_string_pair_release(request->headers);

  if (request->payload != NULL)
  {
    hs_io_release_http_request_payload(request->payload->payload);
    request->payload->payload = NULL;
    hs_io_free(request->payload);
  }

  hs_io_free(request);
}


struct HSHttpResponse *hs_types_http_response_new()
{
  struct HSHttpResponse *response = malloc(sizeof(struct HSHttpResponse));

  response->code           = HS_HTTP_RESPONSE_CODE_OK;
  response->cookies        = hs_types_cookies_new();
  response->headers        = hs_types_array_string_pair_new();
  response->mime_type      = HS_MIME_TYPE_NONE;
  response->content_string = NULL;
  response->content_file   = NULL;

  return(response);
}


void hs_types_http_response_release(struct HSHttpResponse *response)
{
  if (response == NULL)
  {
    return;
  }

  hs_io_free(response->content_string);
  hs_io_free(response->content_file);
  hs_types_cookies_release(response->cookies);
  hs_types_array_string_pair_release(response->headers);

  hs_io_free(response);
}

struct HSPostResponseCallback *hs_types_post_response_callback_new()
{
  struct HSPostResponseCallback *callback = malloc(sizeof(struct HSPostResponseCallback));

  callback->context = NULL;
  callback->run     = NULL;
  callback->release = NULL;

  return(callback);
}


void hs_types_post_response_callback_release(struct HSPostResponseCallback *callback)
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

struct HSPostResponseCallbacks *hs_types_post_response_callbacks_new(size_t capacity)
{
  struct HSPostResponseCallbacks *callbacks = malloc(sizeof(struct HSPostResponseCallbacks));

  callbacks->count    = 0;
  callbacks->capacity = capacity;

  callbacks->callbacks = malloc(sizeof(struct HSPostResponseCallback *) * callbacks->capacity);

  return(callbacks);
}


void hs_types_post_response_callbacks_release(struct HSPostResponseCallbacks *callbacks)
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
      hs_types_post_response_callback_release(callback);
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

struct HSRouterFlowState *hs_types_router_flow_state_new()
{
  struct HSRouterFlowState *state = malloc(sizeof(struct HSRouterFlowState));

  state->done              = false;
  state->closed_connection = false;
  state->base_path         = NULL;

  return(state);
}


void hs_types_router_flow_state_release(struct HSRouterFlowState *state)
{
  if (state == NULL)
  {
    return;
  }

  hs_io_free(state->base_path);
  hs_io_free(state);
}

struct HSRouteFlowState *hs_types_route_flow_state_new()
{
  struct HSRouteFlowState *state = malloc(sizeof(struct HSRouteFlowState));

  state->string_pairs = hs_types_array_string_pair_new();
  state->data         = hashtable_new();

  return(state);
}


void hs_types_route_flow_state_release(struct HSRouteFlowState *state)
{
  if (state == NULL)
  {
    return;
  }

  hs_types_array_string_pair_release(state->string_pairs);
  hashtable_release(state->data);

  hs_io_free(state);
}

struct HSServerConnectionState *hs_types_server_connection_state_new()
{
  struct HSServerConnectionState *state = malloc(sizeof(struct HSServerConnectionState));

  state->socket          = NULL;
  state->request_counter = 0;
  state->creation_time   = time(NULL);

  return(state);
}


void hs_types_server_connection_state_release(struct HSServerConnectionState *state)
{
  hs_io_free(state);
}

struct HSHttpRequestPayload *hs_types_http_request_new_payload(void *io_payload)
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
    buffer = stringbuffer_new();
  }
  else
  {
    size_t current_length = stringbuffer_get_content_size(request->payload->payload->partial);
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

  char *content = stringbuffer_to_string(buffer);
  if (trim > 0)
  {
    stringfn_mut_substring(content, 0, trim);
  }

  stringbuffer_release(buffer);
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

  if (request->payload->payload->partial != NULL)
  {
    size_t current_length = stringbuffer_get_content_size(request->payload->payload->partial);
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
      char *text = stringbuffer_to_string(request->payload->payload->partial);
      if (trim > 0)
      {
        stringfn_mut_substring(text, 0, trim);
      }
      stringbuffer_release(request->payload->payload->partial);
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

