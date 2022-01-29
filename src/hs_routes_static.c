#include "hs_io.h"
#include "hs_routes_static.h"
#include <stdlib.h>
#include <string.h>

static enum HSServeFlowResponse _hs_routes_static_serve(struct HSRoute *, struct HSServeFlowParams *);
static void _hs_routes_static_release(struct HSRoute *);

struct HsRoutesStaticContext
{
  char            *content;
  enum HSMimeType mime_type;
};

struct HSRoute *hs_routes_static_route_new(char *content, enum HSMimeType mime_type)
{
  if (content == NULL)
  {
    return(NULL);
  }

  struct HsRoutesStaticContext *context = malloc(sizeof(struct HsRoutesStaticContext));
  context->content   = content;
  context->mime_type = mime_type;

  struct HSRoute *route = hs_route_new();
  route->is_get    = true;
  route->serve     = _hs_routes_static_serve;
  route->release   = _hs_routes_static_release;
  route->extension = context;

  return(route);
}

struct HSRoute *hs_routes_static_text_route_new(char *text)
{
  return(hs_routes_static_route_new(text, HS_MIME_TYPE_TEXT_PLAIN));
}

struct HSRoute *hs_routes_static_html_route_new(char *html)
{
  return(hs_routes_static_route_new(html, HS_MIME_TYPE_TEXT_HTML));
}

struct HSRoute *hs_routes_static_css_route_new(char *css_text)
{
  return(hs_routes_static_route_new(css_text, HS_MIME_TYPE_TEXT_CSS));
}

struct HSRoute *hs_routes_static_js_route_new(char *js_text)
{
  return(hs_routes_static_route_new(js_text, HS_MIME_TYPE_TEXT_JAVASCRIPT));
}

static enum HSServeFlowResponse _hs_routes_static_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL || route->extension == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HsRoutesStaticContext *context = (struct HsRoutesStaticContext *)route->extension;

  params->response->code           = HS_HTTP_RESPONSE_CODE_OK;
  params->response->content_string = strdup(context->content);
  params->response->mime_type      = context->mime_type;

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


static void _hs_routes_static_release(struct HSRoute *route)
{
  if (route == NULL || route->extension == NULL)
  {
    return;
  }

  struct HsRoutesStaticContext *context = (struct HsRoutesStaticContext *)route->extension;

  hs_io_free(context->content);
  hs_io_free(context);
}

