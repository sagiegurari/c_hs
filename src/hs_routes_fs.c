#include "fsio.h"
#include "hs_io.h"
#include "hs_routes_common.h"
#include "hs_routes_fs.h"
#include "stringfn.h"
#include <dirent.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

struct HSRoutesFileServeContext
{
  char *base_directory;
  enum HSMimeType (*get_mime_type)(char *, enum HSMimeType);
};


struct HSRoutesDirectoryServeContext
{
  char *base_directory;
  char *additional_head_content;
};

enum HSServeFlowResponse _hs_routes_file_serve(struct HSRoute *, struct HSServeFlowParams *);
enum HSServeFlowResponse _hs_routes_directory_serve(struct HSRoute *, struct HSServeFlowParams *);

struct HSRoute           *hs_routes_fs_file_route_new(char *base_directory)
{
  return(hs_routes_fs_file_route_new_with_options(base_directory, NULL));
}

struct HSRoute *hs_routes_fs_file_route_new_with_options(char *base_directory, enum HSMimeType (*get_mime_type)(char *, enum HSMimeType))
{
  struct HSRoute *route = hs_route_new();

  route->serve  = _hs_routes_file_serve;
  route->is_get = true;

  struct HSRoutesFileServeContext *context = malloc(sizeof(struct HSRoutesFileServeContext));
  context->base_directory = base_directory;
  context->get_mime_type  = get_mime_type;
  route->extension        = context;

  route->release = hs_routes_common_extension_release;

  return(route);
}

struct HSRoute *hs_routes_fs_directory_route_new(char *base_directory)
{
  return(hs_routes_fs_directory_route_new_with_options(base_directory, NULL));
}

struct HSRoute *hs_routes_fs_directory_route_new_with_options(char *base_directory, char *additional_head_content)
{
  struct HSRoute *route = hs_route_new();

  route->serve  = _hs_routes_directory_serve;
  route->is_get = true;

  struct HSRoutesDirectoryServeContext *context = malloc(sizeof(struct HSRoutesDirectoryServeContext));
  context->base_directory          = base_directory;
  context->additional_head_content = additional_head_content;
  route->extension                 = context;

  route->release = hs_routes_common_extension_release;

  return(route);
}

enum HSServeFlowResponse _hs_routes_file_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (  route == NULL
     || params == NULL
     || params->request == NULL
     || params->response == NULL
     || params->request->resource == NULL
     || route->extension == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  if (strstr(params->request->resource, "..") != NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HSRoutesFileServeContext *context = (struct HSRoutesFileServeContext *)route->extension;

  char                            *path = fsio_join_paths(context->base_directory, params->request->resource);
  if (path == NULL || !fsio_file_exists(path))
  {
    hs_io_free(path);
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  params->response->code         = HS_HTTP_RESPONSE_CODE_OK;
  params->response->content_file = path;
  params->response->mime_type    = hs_constants_file_extension_to_mime_type(path);

  if (context->get_mime_type != NULL)
  {
    params->response->mime_type = context->get_mime_type(path, params->response->mime_type);
  }

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


enum HSServeFlowResponse _hs_routes_directory_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (  route == NULL
     || params == NULL
     || params->request == NULL
     || params->response == NULL
     || params->request->resource == NULL
     || route->extension == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  if (strstr(params->request->resource, "..") != NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  struct HSRoutesDirectoryServeContext *context = (struct HSRoutesDirectoryServeContext *)route->extension;

  char                                 *path = fsio_join_paths(context->base_directory, params->request->resource);
  if (path == NULL || !fsio_dir_exists(path))
  {
    hs_io_free(path);
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  DIR *directory = opendir(path);
  if (directory == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_CONTINUE);
  }

  char                *path_clone = strdup(path);
  char                *dir_name   = basename(path_clone);

  struct dirent       *entry;
  struct StringBuffer *html_buffer = string_buffer_new();
  string_buffer_append_string(html_buffer, "<!DOCTYPE html>\n"
                              "<html>\n"
                              "<head>\n"
                              "<title>");
  string_buffer_append_string(html_buffer, dir_name);
  string_buffer_append_string(html_buffer, "</title>\n"
                              "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, viewport-fit=cover\">\n");
  if (params->router_state->base_path != NULL)
  {
    string_buffer_append_string(html_buffer, "<base href=\"");
    string_buffer_append_string(html_buffer, params->router_state->base_path);
    string_buffer_append_string(html_buffer, "<\">\n");
  }
  if (context->additional_head_content != NULL)
  {
    string_buffer_append_string(html_buffer, context->additional_head_content);
  }
  string_buffer_append_string(html_buffer, "</head>\n"
                              "<body>\n");
  string_buffer_append_string(html_buffer, "<h1 class=\"header\">Directory: ");
  string_buffer_append_string(html_buffer, dir_name);
  string_buffer_append_string(html_buffer, "</h1>\n");
  hs_io_free(path_clone);

  while ((entry = readdir(directory)))
  {
    if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
    {
      // skip special directories
      continue;
    }

    char *entry_path = fsio_join_paths(path, entry->d_name);

    if (fsio_path_exists(entry_path))
    {
      string_buffer_append_string(html_buffer, "<a class=\"entry\" href=\"");
      string_buffer_append_string(html_buffer, entry_path);
      string_buffer_append_string(html_buffer, "\">");
      string_buffer_append_string(html_buffer, entry->d_name);
      string_buffer_append_string(html_buffer, "</a><br>\n");
    }

    hs_io_free(entry_path);
  }

  closedir(directory);

  string_buffer_append_string(html_buffer, "</body>\n"
                              "</html>");

  char *html = string_buffer_to_string(html_buffer);
  string_buffer_release(html_buffer);

  params->response->code           = HS_HTTP_RESPONSE_CODE_OK;
  params->response->content_string = html;

  return(HS_SERVE_FLOW_RESPONSE_DONE);
} /* _hs_routes_directory_serve */

