#ifndef HS_ROUTES_FS_H
#define HS_ROUTES_FS_H

#include "hs_route.h"

/**
 * Returns the file content with the relevant mime type.
 * It will ignore non file paths or paths that do not exist.
 * The base directory value will be released with the route.
 */
struct HSRoute *hs_routes_fs_file_route_new(char * /* base directory */);

/**
 * Returns the file content with the relevant mime type.
 * It will ignore non file paths or paths that do not exist.
 * The base directory value will be released with the route.
 */
struct HSRoute *hs_routes_fs_file_route_new_with_options(char * /* base directory */, enum HSMimeType (*get_mime_type)(char * /* file */, enum HSMimeType /* detected mime type */), bool /* close connection */);

/**
 * Returns the directory content.
 * It will ignore non file paths or paths that do not exist.
 * The base directory value will be released with the route.
 */
struct HSRoute *hs_routes_fs_directory_route_new(char * /* base directory */);

/**
 * Returns the directory content.
 * It will ignore non file paths or paths that do not exist.
 * For media entities, such as images and videos, the relevant html elements will be created.
 * The base directory value will be released with the route.
 */
struct HSRoute *hs_routes_fs_directory_route_new_with_media_support(char * /* base directory */);

/**
 * Returns the directory content.
 * It will ignore non file paths or paths that do not exist.
 * The base directory and optional additional_head_content value will be released with the route.
 * The optional context will not be released with the route.
 */
struct HSRoute *hs_routes_fs_directory_route_new_with_options(char * /* base directory */, char * /* additional_head_content */, bool (*filter)(char * /* name */, bool /* is directory */), char * (*render_directory_entry)(char * /* name */, char * /* href */, void * /* context */), char * (*render_file_entry)(char * /* name */, char * /* href */, void * /* context */), void *context);

/**
 * The render_file_entry used by the directory with media support route as standalone function.
 */
char *hs_routes_fs_directory_route_render_file_entry_with_media_support(char * /* name */, char * /* href */, void * /* context */);

#endif

