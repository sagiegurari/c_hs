#ifndef __HS_ROUTES_FS_H__
#define __HS_ROUTES_FS_H__

#include "hs_route.h"

/**
 * Returns the file content with the relevant mime type.
 * It will ignore non file paths or paths that do not exist.
 */
struct HSRoute *hs_routes_fs_file_route_new(char * /* base directory */);

/**
 * Returns the file content with the relevant mime type.
 * It will ignore non file paths or paths that do not exist.
 */
struct HSRoute *hs_routes_fs_file_route_new_with_options(char * /* base directory */, enum HSMimeType (*get_mime_type)(char * /* file */, enum HSMimeType /* detected mime type */));

/**
 * Returns the directory content.
 * It will ignore non file paths or paths that do not exist.
 */
struct HSRoute *hs_routes_fs_directory_route_new(char * /* base directory */);

/**
 * Returns the directory content.
 * It will ignore non file paths or paths that do not exist.
 * For media entities, such as images and videos, the relevant html elements will be created.
 */
struct HSRoute *hs_routes_fs_directory_route_new_with_media_support(char * /* base directory */);

/**
 * Returns the directory content.
 * It will ignore non file paths or paths that do not exist.
 * The optional additional_head_content and context will not be released with the route.
 */
struct HSRoute *hs_routes_fs_directory_route_new_with_options(char * /* base directory */, char * /* additional_head_content */, bool (*filter)(char * /* name */, bool /* is directory */), char * (*render_directory_entry)(char * /* name */, char * /* href */, void * /* context */), char * (*render_file_entry)(char * /* name */, char * /* href */, void * /* context */), void *context);

#endif

