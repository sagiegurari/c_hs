#ifndef __HS_ROUTES_H__
#define __HS_ROUTES_H__

#include "hs_route.h"

/**
 * Simple route which returns 404 when invoked.
 */
struct HSRoute *hs_routes_new_404_route(void);

/**
 * Returns the file content with the relevant mime type.
 * It will ignore non file paths or paths that do not exist.
 */
struct HSRoute *hs_routes_new_file_route(char * /* base directory */);

/**
 * Returns the directory content.
 * It will ignore non file paths or paths that do not exist.
 */
struct HSRoute *hs_routes_new_directory_route(char * /* base directory */);

#endif

