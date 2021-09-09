# HS (HTTP Server)

[![CI](https://github.com/sagiegurari/c_hs/workflows/CI/badge.svg?branch=master)](https://github.com/sagiegurari/c_hs/actions)
[![Release](https://img.shields.io/github/v/release/sagiegurari/c_hs)](https://github.com/sagiegurari/c_hs/releases)
[![license](https://img.shields.io/github/license/sagiegurari/c_hs)](https://github.com/sagiegurari/c_hs/blob/master/LICENSE)

> Simple Embeddable HTTP Server for C.

* [Overview](#overview)
* [Usage](#usage)
* [Contributing](.github/CONTRIBUTING.md)
* [Release History](CHANGELOG.md)
* [License](#license)

<a name="overview"></a>
## Overview
This library provides both embeddable http server and HTTP parsing utility functions.<br>
I wrote it for some small personal projects that needed to expose integration via http.
The requirements were very basic and therefore this server is not really production ready.
It is lacking many features, such as:

* TLS
* HTTP 2
* No built in multi threaded support (but expandable to enable that).
* Configuration Based

It does however provide support for many core capabilities.

*Developed and tested only on linux*

<a name="usage"></a>
## Usage

```c
#include "hs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * This is a simple example which creates a http server and listens to
 * root path to return some custom html and to /fs/... paths to return
 * file system files or directory list.
 * If no path is matching, it will return 404.
 *
 * This example only shows basic capabilities and there are some more that
 * are available such as:
 * - Redirection
 * - File based response
 * - Post request/connection callbacks
 * - Custom headers support
 * - Connection closed/keep alive management
 * And more.
 * See the various header files for the complete API.
 */

enum HSServeFlowResponse _home_route_serve(struct HSRoute *, struct HSServeFlowParams *);
bool _fs_basic_auth(char *, void *);


int main(int argc, char *argv[])
{
  // This is our main server
  // It has a built in router
  struct HSServer *server = hs_server_new();

  server->accept_recv_timeout_seconds  = 60;
  server->request_recv_timeout_seconds = 30;

  int port = 8080;
  if (argc > 1)
  {
    port = atoi(argv[1]);
  }
  struct sockaddr_in address = hs_server_init_ipv4_address(port);

  // add powered by response header to all responses
  struct HSRoute *powered_by_route = hs_routes_powered_by_route_new(NULL);
  hs_router_add_route(server->router, powered_by_route);

  // let's make sure PUT/POST have their content-length header set
  hs_router_add_route(server->router, hs_routes_error_411_length_required_route_new());

  // Set a max on payload size
  hs_router_add_route(server->router, hs_routes_payload_limit_route_new(1024 * 1024 * 2));

  // This route will server as our top domain route and will return
  // a custom HTML that we are building in runtime (we can also point to a file).
  struct HSRoute *home_route = hs_route_new();
  home_route->path   = strdup("/");
  home_route->is_get = true;
  home_route->serve  = _home_route_serve;

  // Lets add this route to our main server router
  hs_router_add_route(server->router, home_route);

  // For all routes under /fs/ we will create a new sub router
  // which will serve all requests for that path and child paths
  // The sub routes in this fs router, will 'listen' to paths without
  // the /fs/ prefix
  struct HSRouter *fs_router = hs_router_new();

  // Protect FS access via basic auth
  struct HSRoute *basic_auth_route = hg_routes_auth_basic_new("My Realm", _fs_basic_auth, NULL);
  hs_router_add_route(fs_router, basic_auth_route);

  // Adding directory route that will handle any request that maps
  // into a directory location in our file system.
  struct HSRoute *fs_directory_route = hs_routes_fs_directory_route_new(".");
  fs_directory_route->is_parent_path = true; // enable to listen to all request sub paths
  hs_router_add_route(fs_router, fs_directory_route);

  // Adding file route that will handle any request that maps
  // into a file location on our file system.
  struct HSRoute *fs_file_route = hs_routes_fs_file_route_new(".");
  fs_file_route->is_parent_path = true;
  hs_router_add_route(fs_router, fs_file_route);

  // Lets make the file system router as a route that listens
  // to the /fs/ path and all child requests
  // Afterwards let's add it to the main server router
  struct HSRoute *fs_route = hs_router_as_route(fs_router);
  fs_route->path = strdup("/fs/");
  hs_router_add_route(server->router, fs_route);

  // Any request that doesn't map to what we support, should get a 404
  struct HSRoute *not_found_error_route = hs_routes_error_404_not_found_route_new();
  hs_router_add_route(server->router, not_found_error_route);

  // start listening and serving content.
  // The optional callbacks enable you to stop the server after
  // serving or after some time.
  printf("Start listening on port: %d\n", port);
  hs_server_serve(server, address, NULL, NULL, NULL);

  hs_server_release(server);

  return(0);
} /* main */


enum HSServeFlowResponse _home_route_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  if (route == NULL)
  {
    return(HS_SERVE_FLOW_RESPONSE_DONE);
  }

  params->response->code           = HS_HTTP_RESPONSE_CODE_OK;
  params->response->mime_type      = HS_MIME_TYPE_TEXT_HTML;
  params->response->content_string = strdup("<html>\n"
                                            "<head>\n"
                                            "<title>Example Home Page</title>\n"
                                            "</head>\n"
                                            "<body>\n"
                                            "<h1>Welcome To The Example Home Page</h1>\n"
                                            "<a href=\"/fs/\">Go To File System</a>\n"
                                            "</body>\n"
                                            "</html>");

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


bool _fs_basic_auth(char *auth_value, void *context)
{
  if (context != NULL)
  {
    return(false);
  }

  return(!strcmp(auth_value, "bXl1c2VyOm15cGFzc3dvcmQ=")); // myuser:mypassword
}
```

## Contributing
See [contributing guide](.github/CONTRIBUTING.md)

<a name="history"></a>
## Release History

See [Changelog](CHANGELOG.md)

<a name="license"></a>
## License
Developed by Sagie Gur-Ari and licensed under the Apache 2 open source license.
