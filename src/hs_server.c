#include "hs_io.h"
#include "hs_router.h"
#include "hs_server.h"
#include "vector.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>

struct HSServerInternal
{
  bool stop_requested;
};

bool _hs_server_set_recv_timeout_in_seconds(int, long);
void _hs_server_single_thread_on_connection(struct HSServer *, int, void *, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, int, size_t, void *));

struct HSServer *hs_server_new()
{
  struct HSServer *server = malloc(sizeof(struct HSServer));

  server->router                       = hs_router_new();
  server->accept_recv_timeout_seconds  = -1;
  server->request_recv_timeout_seconds = -1;
  server->create_socket_and_listen     = hs_server_create_socket_and_listen;

  server->connection_handler = hs_server_connection_handler_new();

  server->internal                 = malloc(sizeof(struct HSServerInternal));
  server->internal->stop_requested = false;

  // by default setup single thread implementation
  server->connection_handler->on_connection = _hs_server_single_thread_on_connection;

  return(server);
}

struct HSServer *hs_server_new_single_thread()
{
  return(hs_server_new());
}


void hs_server_release(struct HSServer *server)
{
  if (server == NULL)
  {
    return;
  }

  hs_router_release(server->router);
  hs_server_connection_handler_release(server->connection_handler);

  hs_io_free(server->internal);

  hs_io_free(server);
}


bool hs_server_serve(struct HSServer *server, struct sockaddr_in address, void *context, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, int, size_t, void *))
{
  if (  server == NULL
     || server->internal == NULL
     || server->router == NULL
     || server->create_socket_and_listen == NULL
     || server->connection_handler == NULL
     || server->connection_handler->on_connection == NULL)
  {
    return(false);
  }

  server->internal->stop_requested = false;

  // start listening
  int socket_fd = server->create_socket_and_listen(server, &address);
  if (!socket_fd)
  {
    return(false);
  }

  if (server->connection_handler->init != NULL)
  {
    server->connection_handler->init(server->connection_handler);
  }

  // listen loop
  int address_size = sizeof(address);
  do
  {
    int client_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t *)&address_size);

    if (client_socket && _hs_server_set_recv_timeout_in_seconds(client_socket, server->request_recv_timeout_seconds))
    {
      server->connection_handler->on_connection(server, client_socket, context, should_stop_server, should_stop_for_connection);
    }
    else
    {
      hs_io_close(client_socket);

      if (should_stop_server != NULL && !server->internal->stop_requested)
      {
        server->internal->stop_requested = should_stop_server(server, context);
      }
    }
  } while (!server->internal->stop_requested);

  hs_io_close(socket_fd);

  if (server->connection_handler->stop_connections != NULL)
  {
    server->connection_handler->stop_connections(server->connection_handler);
  }

  return(true);
} /* hs_server_serve */

struct HSServerConnectionHandler *hs_server_connection_handler_new()
{
  struct HSServerConnectionHandler *handler = malloc(sizeof(struct HSServerConnectionHandler));

  handler->init             = NULL;
  handler->on_connection    = NULL;
  handler->stop_connections = NULL;
  handler->release          = NULL;

  return(handler);
}


void hs_server_connection_handler_release(struct HSServerConnectionHandler *handler)
{
  if (handler == NULL)
  {
    return;
  }

  if (handler->stop_connections != NULL)
  {
    handler->stop_connections(handler);
  }

  if (handler->release != NULL)
  {
    handler->release(handler);
  }

  hs_io_free(handler);
}


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


struct sockaddr_in hs_server_init_ipv4_address(uint16_t port)
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


void _hs_server_single_thread_on_connection(struct HSServer *server, int socket, void *context, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, int, size_t, void *))
{
  if (  server == NULL
     || server->router == NULL
     || !socket)
  {
    return;
  }

  hs_router_serve_forever(server->router, socket, context, should_stop_for_connection);

  hs_io_close(socket);

  if (should_stop_server != NULL && !server->internal->stop_requested)
  {
    server->internal->stop_requested = should_stop_server(server, context);
  }
}

