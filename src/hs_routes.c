#include "fsio.h"
#include "hs_io.h"
#include "hs_routes.h"
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

struct HSRouteServeResponse *_hs_routes_404_serve(struct HSRoute *, struct HSHttpRequest *, int);
struct HSRouteServeResponse *_hs_routes_file_serve(struct HSRoute *, struct HSHttpRequest *, int);
struct HSRouteServeResponse *_hs_routes_directory_serve(struct HSRoute *, struct HSHttpRequest *, int);
void _hs_routes_fs_release(struct HSRoute *);

struct HSRoute *hs_routes_new_404_route()
{
  struct HSRoute *route = hs_route_new_route();

  route->serve     = _hs_routes_404_serve;
  route->is_get    = true;
  route->is_post   = true;
  route->is_put    = true;
  route->is_delete = true;

  return(route);
}

struct HSRoute *hs_routes_new_file_route(char *base_directory)
{
  return(hs_routes_new_file_route_with_options(base_directory, NULL));
}

struct HSRoute *hs_routes_new_file_route_with_options(char *base_directory, enum HSMimeType (*get_mime_type)(char *, enum HSMimeType))
{
  struct HSRoute *route = hs_route_new_route();

  route->serve  = _hs_routes_file_serve;
  route->is_get = true;

  struct HSRoutesFileServeContext *context = malloc(sizeof(struct HSRoutesFileServeContext));
  context->base_directory = base_directory;
  context->get_mime_type  = get_mime_type;
  route->extension        = context;

  route->release = _hs_routes_fs_release;

  return(route);
}

struct HSRoute *hs_routes_new_directory_route(char *base_directory)
{
  return(hs_routes_new_directory_route_with_options(base_directory, NULL));
}

struct HSRoute *hs_routes_new_directory_route_with_options(char *base_directory, char *additional_head_content)
{
  struct HSRoute *route = hs_route_new_route();

  route->serve  = _hs_routes_directory_serve;
  route->is_get = true;

  struct HSRoutesDirectoryServeContext *context = malloc(sizeof(struct HSRoutesDirectoryServeContext));
  context->base_directory          = base_directory;
  context->additional_head_content = additional_head_content;
  route->extension                 = context;

  route->release = _hs_routes_fs_release;

  return(route);
}

struct HSRouteServeResponse *_hs_routes_404_serve(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  if (route == NULL || request == NULL || !socket)
  {
    return(NULL);
  }

  struct HSRouteServeResponse *response = hs_route_new_serve_response();
  response->code = HS_HTTP_RESPONSE_CODE_NOT_FOUND;

  return(response);
}

struct HSRouteServeResponse *_hs_routes_file_serve(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  if (route == NULL || request == NULL || !socket || request->resource == NULL)
  {
    return(NULL);
  }

  if (strstr(request->resource, "..") != NULL)
  {
    return(NULL);
  }

  struct HSRoutesFileServeContext *context = (struct HSRoutesFileServeContext *)route->extension;

  char                            *path = fsio_join_paths(context->base_directory, request->resource);
  if (path == NULL)
  {
    return(NULL);
  }

  if (!fsio_file_exists(path))
  {
    hs_io_free(path);
    return(NULL);
  }

  struct HSRouteServeResponse *response = hs_route_new_serve_response();
  response->code         = HS_HTTP_RESPONSE_CODE_OK;
  response->content_file = path;
  response->mime_type    = hs_constants_file_extension_to_mime_type(path);

  if (context->get_mime_type != NULL)
  {
    response->mime_type = context->get_mime_type(path, response->mime_type);
  }

  return(response);
}


struct HSRouteServeResponse *_hs_routes_directory_serve(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  if (route == NULL || request == NULL || !socket || request->resource == NULL)
  {
    return(NULL);
  }

  if (strstr(request->resource, "..") != NULL)
  {
    return(NULL);
  }

  struct HSRoutesDirectoryServeContext *context = (struct HSRoutesDirectoryServeContext *)route->extension;

  char                                 *path = fsio_join_paths(context->base_directory, request->resource);
  if (path == NULL)
  {
    return(NULL);
  }

  if (!fsio_dir_exists(path))
  {
    hs_io_free(path);
    return(NULL);
  }

  DIR *directory = opendir(path);
  if (directory == NULL)
  {
    return(NULL);
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
  if (request->state.base_path != NULL)
  {
    string_buffer_append_string(html_buffer, "<base href=\"");
    string_buffer_append_string(html_buffer, request->state.base_path);
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

  struct HSRouteServeResponse *response = hs_route_new_serve_response();
  response->code           = HS_HTTP_RESPONSE_CODE_OK;
  response->content_string = html;

  return(response);
} /* _hs_routes_directory_serve */


void _hs_routes_fs_release(struct HSRoute *route)
{
  if (route == NULL)
  {
    return;
  }

  hs_io_free(route->extension);
}

