#include "hs_io.h"
#include "hs_routes_common.h"


void hs_routes_common_extension_release(struct HSRoute *route)
{
  if (route == NULL)
  {
    return;
  }

  hs_io_free(route->extension);
}

struct HSRoute *hs_routes_common_serve_all_route_new(void)
{
  struct HSRoute *route = hs_route_new();

  hs_route_set_all_methods(route, true);
  route->is_parent_path = true;

  return(route);
}

