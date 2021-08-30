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
 * The optional additional_head_content will not be released with the route.
 */
struct HSRoute *hs_routes_fs_directory_route_new_with_options(char * /* base directory */, char * /* additional_head_content */);

#endif

