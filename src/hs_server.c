#include "hs_io.h"
#include "hs_router.h"
#include "hs_server.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>

bool _hs_server_set_recv_timeout_in_seconds(int, long);

struct HSServer *hs_server_new()
{
  struct HSServer *server = malloc(sizeof(struct HSServer));

  server->router                       = hs_router_new();
  server->accept_recv_timeout_seconds  = -1;
  server->request_recv_timeout_seconds = -1;
  server->create_socket_and_listen     = hs_server_create_socket_and_listen;
  server->on_connection                = hs_server_on_connection;

  return(server);
}


void hs_server_release(struct HSServer *server)
{
  if (server == NULL)
  {
    return;
  }

  hs_router_release(server->router);

  hs_io_free(server);
}


bool hs_server_serve(struct HSServer *server, struct sockaddr_in address, void *context, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_router)(struct HSRouter *, int, int, void *))
{
  if (  server == NULL
     || server->router == NULL
     || server->create_socket_and_listen == NULL
     || server->on_connection == NULL)
  {
    return(false);
  }

  // start listening
  int socket_fd = server->create_socket_and_listen(server, &address);
  if (!socket_fd)
  {
    return(false);
  }

  // listen loop
  bool stop         = false;
  int  address_size = sizeof(address);
  do
  {
    int client_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t *)&address_size);

    if (_hs_server_set_recv_timeout_in_seconds(client_socket, server->request_recv_timeout_seconds))
    {
      server->on_connection(server, client_socket, context, should_stop_router);
    }
    hs_io_close(client_socket);

    if (should_stop_server != NULL)
    {
      stop = should_stop_server(server, context);
    }
  } while (!stop);

  hs_io_close(socket_fd);

  return(true);
} /* hs_server_serve */


int hs_server_create_socket_and_listen(struct HSServer *server, struct sockaddr_in *address)
{
  if (server == NULL || address == NULL)
  {
    return(0);
  }

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == 0 || socket_fd == -1)
  {
    return(0);
  }

  int opt = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
  {
    hs_io_close(socket_fd);
    return(0);
  }

  if (!_hs_server_set_recv_timeout_in_seconds(socket_fd, server->accept_recv_timeout_seconds))
  {
    hs_io_close(socket_fd);
    return(0);
  }

  if (bind(socket_fd, (struct sockaddr *)address, sizeof(*address)) < 0)
  {
    hs_io_close(socket_fd);
    return(0);
  }

  if (listen(socket_fd, 1) < 0)
  {
    hs_io_close(socket_fd);
    return(0);
  }

  return(socket_fd);
} /* hs_server_create_socket_and_listen */


void hs_server_on_connection(struct HSServer *server, int socket, void *context, bool (*should_stop)(struct HSRouter *, int, int, void *))
{
  hs_router_serve_forever(server->router, socket, context, should_stop);
}


struct sockaddr_in hs_server_init_ipv4_address(int port)
{
  struct sockaddr_in address;

  address.sin_family      = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port        = htons(port);

  return(address);
}


bool _hs_server_set_recv_timeout_in_seconds(int socket, long recv_timeout_seconds)
{
  if (socket <= 0)
  {
    return(false);
  }

  if (recv_timeout_seconds <= 0)
  {
    return(true);
  }

  struct timeval timeout;
  timeout.tv_sec  = recv_timeout_seconds;
  timeout.tv_usec = 0;

  return(!setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)));
}

