#include "fsio.h"
#include "hs_io.h"
#include "hs_routes_favicon.h"
#include <stdlib.h>
#include <string.h>

static enum HSServeFlowResponse _hs_routes_favicon_route_serve(struct HSRoute *, struct HSServeFlowParams *);
static void _hs_routes_favicon_route_release(struct HSRoute *);

struct HSRouteFaviconContext
{
  char *path;
  char *cache_control_header;
};

struct HSRoute *hs_routes_favicon_route_new(char *path, int max_age_seconds)
{
  if (path == NULL)
  {
    return(NULL);
  }

  struct HSRoute *route = hs_route_new();

  route->serve   = _hs_routes_favicon_route_serve;
  route->release = _hs_routes_favicon_route_release;
  route->is_get  = true;
  route->path    = strdup("/favicon.ico");

  struct HSRouteFaviconContext *context = malloc(sizeof(struct HSRouteFaviconContext));
  context->path                 = path;
  context->cache_control_header = NULL;
  if (max_age_seconds > 0)
  {
    struct StringBuffer *buffer = string_buffer_new();
    string_buffer_append_string(buffer, "public, max-age=");
    string_buffer_append_int(buffer, max_age_seconds);
    context->cache_control_header = string_buffer_to_string(buffer);
    string_buffer_release(buffer);
  }
  route->extension = context;

  return(route);
}

static enum HSServeFlowResponse _hs_routes_favicon_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || route->extension == NULL || params == NULL || params->response == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HSRouteFaviconContext *context = (struct HSRouteFaviconContext *)route->extension;

  if (!fsio_file_exists(context->path))
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  params->response->code         = HS_HTTP_RESPONSE_CODE_OK;
  params->response->content_file = strdup(context->path);

  if (context->cache_control_header != NULL)
  {
    hs_types_array_string_pair_add(params->response->headers, strdup("Cache-Control"), strdup(context->cache_control_header));
  }

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


static void _hs_routes_favicon_route_release(struct HSRoute *route)
{
  if (route == NULL)
  {
    return;
  }

  struct HSRouteFaviconContext *context = (struct HSRouteFaviconContext *)route->extension;

  hs_io_free(context->path);
  hs_io_free(context->cache_control_header);
  hs_io_free(context);
}

