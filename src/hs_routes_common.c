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

