#include "fsio.h"
#include "hs_constants.h"
#include "hs_io.h"
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
  bool close_connection;
};


struct HSRoutesDirectoryServeContext
{
  char *base_directory;
  char *additional_head_content;
  bool (*filter)(char *, bool);
  char * (*render_directory_entry)(char *, char *, void *);
  char * (*render_file_entry)(char *, char *, void *);
  void *context;
};

enum HSServeFlowResponse _hs_routes_file_route_serve(struct HSRoute *, struct HSServeFlowParams *);
void _hs_routes_file_route_release(struct HSRoute *);
enum HSServeFlowResponse _hs_routes_directory_route_serve(struct HSRoute *, struct HSServeFlowParams *);
void _hs_routes_directory_route_release(struct HSRoute *);
void _hs_routes_directory_render_entry(struct StringBuffer *, char * (*render)(char *, char *, void *), char *, char *, void *);
void _hs_routes_directory_default_renderer(struct StringBuffer *, char *, char *);

struct HSRoute *hs_routes_fs_file_route_new(char *base_directory)
{
  return(hs_routes_fs_file_route_new_with_options(base_directory, NULL, true));
}

struct HSRoute *hs_routes_fs_file_route_new_with_options(char *base_directory, enum HSMimeType (*get_mime_type)(char *, enum HSMimeType), bool close_connection)
{
  struct HSRoute *route = hs_route_new();

  route->serve  = _hs_routes_file_route_serve;
  route->is_get = true;

  struct HSRoutesFileServeContext *context = malloc(sizeof(struct HSRoutesFileServeContext));
  context->base_directory   = base_directory;
  context->get_mime_type    = get_mime_type;
  context->close_connection = close_connection;
  route->extension          = context;

  route->release = _hs_routes_file_route_release;

  return(route);
}

struct HSRoute *hs_routes_fs_directory_route_new(char *base_directory)
{
  return(hs_routes_fs_directory_route_new_with_options(
           base_directory,
           NULL, // additional head content
           NULL, // filter
           NULL, // render directory
           NULL, // render file
           NULL  // context
           ));
}

struct HSRoute *hs_routes_fs_directory_route_new_with_media_support(char *base_directory)
{
  return(hs_routes_fs_directory_route_new_with_options(
           base_directory,
           NULL,                                                              // additional head content
           NULL,                                                              // filter
           NULL,                                                              // render directory
           hs_routes_fs_directory_route_render_file_entry_with_media_support, // render file
           NULL                                                               // context
           ));
}

struct HSRoute *hs_routes_fs_directory_route_new_with_options(char *base_directory,
                                                              char *additional_head_content,
                                                              bool (*filter)(char *, bool),
                                                              char * (*render_directory_entry)(char *, char *, void *),
                                                              char * (*render_file_entry)(char *, char *, void *),
                                                              void *context)
{
  struct HSRoute *route = hs_route_new();

  route->serve  = _hs_routes_directory_route_serve;
  route->is_get = true;

  struct HSRoutesDirectoryServeContext *route_context = malloc(sizeof(struct HSRoutesDirectoryServeContext));
  route_context->base_directory          = base_directory;
  route_context->additional_head_content = additional_head_content;
  route_context->filter                  = filter;
  route_context->render_directory_entry  = render_directory_entry;
  route_context->render_file_entry       = render_file_entry;
  route_context->context                 = context;
  route->extension                       = route_context;

  route->release = _hs_routes_directory_route_release;

  return(route);
}


char *hs_routes_fs_directory_route_render_file_entry_with_media_support(char *name, char *href, void *context)
{
  if (context != NULL)
  {
    // context not supported
    return(NULL);
  }

  struct StringBuffer *buffer = string_buffer_new();

  string_buffer_append_string(buffer, "<a class=\"entry\" href=\"");
  string_buffer_append_string(buffer, href);
  string_buffer_append_string(buffer, "\">");

  // handle some of the supported mime types
  enum HSMimeType mime_type = hs_constants_file_extension_to_mime_type(name);
  switch (mime_type)
  {
  case HS_MIME_TYPE_IMAGE_APNG:
  case HS_MIME_TYPE_IMAGE_AVIF:
  case HS_MIME_TYPE_IMAGE_GIF:
  case HS_MIME_TYPE_IMAGE_JPEG:
  case HS_MIME_TYPE_IMAGE_PNG:
  case HS_MIME_TYPE_IMAGE_SVG:
  case HS_MIME_TYPE_IMAGE_WEBP:
  case HS_MIME_TYPE_IMAGE_X_ICON:
  case HS_MIME_TYPE_IMAGE_TIFF:
  case HS_MIME_TYPE_IMAGE_X_MS_BMP:
    string_buffer_append_string(buffer, "<img src=\"");
    string_buffer_append_string(buffer, href);
    string_buffer_append_string(buffer, "\" alt=\"");
    string_buffer_append_string(buffer, name);
    string_buffer_append_string(buffer, "\" class=\"entry-image\"></img>");
    break;

  case HS_MIME_TYPE_VIDEO_WEBM:
  case HS_MIME_TYPE_VIDEO_3GPP:
  case HS_MIME_TYPE_VIDEO_MP2T:
  case HS_MIME_TYPE_VIDEO_MP4:
  case HS_MIME_TYPE_VIDEO_MPEG:
  case HS_MIME_TYPE_VIDEO_QUICKTIME:
  case HS_MIME_TYPE_VIDEO_X_FLV:
    string_buffer_append_string(buffer, "<video src=\"");
    string_buffer_append_string(buffer, href);
    string_buffer_append_string(buffer, "\" alt=\"");
    string_buffer_append_string(buffer, name);
    string_buffer_append_string(buffer, "\" class=\"entry-video\" autoplay=\"false\" controls loop=\"false\" preload=\"none\"></video>");
    break;

  default:
    string_buffer_append_string(buffer, name);
    break;
  }

  string_buffer_append_string(buffer, "</a><br>\n");

  char *html = string_buffer_to_string(buffer);
  string_buffer_release(buffer);

  return(html);
} /* hs_routes_fs_directory_route_render_file_entry_with_media_support */

enum HSServeFlowResponse _hs_routes_file_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
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

  // force close connection to enable new file request on other connections for single threaded servers
  if (context->close_connection)
  {
    params->request->connection = HS_CONNECTION_TYPE_CLOSE;
  }

  return(HS_SERVE_FLOW_RESPONSE_DONE);
} /* _hs_routes_file_serve */


void _hs_routes_file_route_release(struct HSRoute *route)
{
  if (route == NULL || route->extension == NULL)
  {
    return;
  }

  struct HSRoutesFileServeContext *context = (struct HSRoutesFileServeContext *)route->extension;

  hs_io_free(context->base_directory);
  hs_io_free(context);
}

enum HSServeFlowResponse _hs_routes_directory_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
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

  char                *path_clone = strdup(path);
  char                *dir_name   = basename(path_clone);

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

  struct dirent **file_list    = NULL;
  int           file_list_size = scandir(path, &file_list, 0, alphasort);
  if (file_list_size > 0)
  {
    struct dirent       *entry;
    struct StringBuffer *dir_buffer   = string_buffer_new();
    struct StringBuffer *files_buffer = string_buffer_new();
    for (int index = 0; index < file_list_size; index++)
    {
      entry = file_list[index];
      if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
      {
        // skip special directories
        hs_io_free(entry);
        continue;
      }

      char *entry_path  = fsio_join_paths(path, entry->d_name);
      bool is_directory = fsio_dir_exists(entry_path);
      bool filter_out   = false;
      if (context->filter != NULL)
      {
        filter_out = context->filter(entry->d_name, is_directory);
      }

      if (!filter_out)
      {
        char *href          = fsio_join_paths(params->request->resource, entry->d_name);
        char *href_relative = href + 1;

        if (is_directory)
        {
          _hs_routes_directory_render_entry(dir_buffer, context->render_directory_entry, entry->d_name, href_relative, context->context);
        }
        else if (fsio_file_exists(entry_path))
        {
          _hs_routes_directory_render_entry(files_buffer, context->render_file_entry, entry->d_name, href_relative, context->context);
        }

        hs_io_free(href);
      }

      hs_io_free(entry_path);
      hs_io_free(entry);
    }

    hs_io_free(file_list);

    char *entries_html = string_buffer_to_string(dir_buffer);
    string_buffer_release(dir_buffer);
    string_buffer_append_string(html_buffer, entries_html);
    hs_io_free(entries_html);
    entries_html = string_buffer_to_string(files_buffer);
    string_buffer_release(files_buffer);
    string_buffer_append_string(html_buffer, entries_html);
    hs_io_free(entries_html);
  }

  string_buffer_append_string(html_buffer, "</body>\n"
                              "</html>");

  char *html = string_buffer_to_string(html_buffer);
  string_buffer_release(html_buffer);

  params->response->code           = HS_HTTP_RESPONSE_CODE_OK;
  params->response->content_string = html;
  params->response->mime_type      = HS_MIME_TYPE_TEXT_HTML;

  return(HS_SERVE_FLOW_RESPONSE_DONE);
} /* _hs_routes_directory_serve */


void _hs_routes_directory_route_release(struct HSRoute *route)
{
  if (route == NULL || route->extension == NULL)
  {
    return;
  }

  struct HSRoutesDirectoryServeContext *context = (struct HSRoutesDirectoryServeContext *)route->extension;

  hs_io_free(context->base_directory);
  hs_io_free(context->additional_head_content);
  hs_io_free(context);
}


void _hs_routes_directory_render_entry(struct StringBuffer *buffer, char * (*render_entry)(char *, char *, void *), char *name, char *href, void *context)
{
  char *entry_html = NULL;

  if (render_entry != NULL)
  {
    entry_html = render_entry(name, href, context);
  }

  if (entry_html != NULL)
  {
    string_buffer_append_string(buffer, entry_html);
    hs_io_free(entry_html);
  }
  else
  {
    _hs_routes_directory_default_renderer(buffer, name, href);
  }
}


void _hs_routes_directory_default_renderer(struct StringBuffer *buffer, char *name, char *href)
{
  string_buffer_append_string(buffer, "<a class=\"entry\" href=\"");
  string_buffer_append_string(buffer, href);
  string_buffer_append_string(buffer, "\">");
  string_buffer_append_string(buffer, name);
  string_buffer_append_string(buffer, "</a><br>\n");
}

