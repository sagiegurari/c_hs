#include "hs_io.h"
#include "hs_router.h"
#include "hs_server.h"
#include "vector.h"
#include <stdlib.h>

struct HSServerInternal
{
  bool stop_requested;
};

void _hs_server_plain_socket_listen_loop(struct HSServer *, struct HSSocket *, struct sockaddr_in, void *, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t, void *));
void _hs_server_single_thread_on_connection(struct HSServer *, struct HSSocket *, void *, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t, void *));

struct HSServer *hs_server_new()
{
  struct HSServer *server = malloc(sizeof(struct HSServer));

  server->router                       = hs_router_new();
  server->accept_recv_timeout_seconds  = -1;
  server->request_recv_timeout_seconds = -1;
  server->create_socket_and_listen     = hs_server_create_plain_socket_and_listen;
  server->listen_loop                  = _hs_server_plain_socket_listen_loop;

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


bool hs_server_serve(struct HSServer *server, struct sockaddr_in address, void *context, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t, void *))
{
  if (  server == NULL
     || server->internal == NULL
     || server->router == NULL
     || server->create_socket_and_listen == NULL
     || server->listen_loop == NULL
     || server->connection_handler == NULL
     || server->connection_handler->on_connection == NULL)
  {
    return(false);
  }

  server->internal->stop_requested = false;

  // start listening
  struct HSSocket *socket = server->create_socket_and_listen(server, &address);
  if (socket == NULL)
  {
    return(false);
  }

  if (server->connection_handler->init != NULL)
  {
    server->connection_handler->init(server->connection_handler);
  }

  // listen loop
  server->listen_loop(server, socket, address, context, should_stop_server, should_stop_for_connection);

  // stop listening
  hs_socket_close_and_release(socket);

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


struct HSSocket *hs_server_create_plain_socket_and_listen(struct HSServer *server, struct sockaddr_in *address)
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

  struct HSSocket *hssocket = hs_socket_plain_new(socket_fd);
  if (hssocket == NULL)
  {
    close(socket_fd);
    return(0);
  }

  int opt = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
  {
    hs_socket_close_and_release(hssocket);
    return(0);
  }

  if (!hssocket->set_recv_timeout_in_seconds(hssocket, server->accept_recv_timeout_seconds))
  {
    hs_socket_close_and_release(hssocket);
    return(0);
  }

  if (bind(socket_fd, (struct sockaddr *)address, sizeof(*address)) < 0)
  {
    hs_socket_close_and_release(hssocket);
    return(0);
  }

  if (listen(socket_fd, 1) < 0)
  {
    hs_socket_close_and_release(hssocket);
    return(0);
  }

  return(hssocket);
} /* hs_server_create_plain_socket_and_listen */


struct sockaddr_in hs_server_init_ipv4_address(uint16_t port)
{
  struct sockaddr_in address;

  address.sin_family      = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port        = htons(port);

  return(address);
}


void _hs_server_plain_socket_listen_loop(struct HSServer *server, struct HSSocket *server_socket, struct sockaddr_in address, void *context, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t, void *))
{
  // listen loop
  int address_size = sizeof(address);

  do
  {
    struct HSSocket *client_socket = hs_socket_plain_accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&address_size);

    if (client_socket && client_socket->set_recv_timeout_in_seconds(client_socket, server->request_recv_timeout_seconds))
    {
      server->connection_handler->on_connection(server, client_socket, context, should_stop_server, should_stop_for_connection);
    }
    else
    {
      if (client_socket != NULL)
      {
        hs_socket_close_and_release(client_socket);
      }

      if (should_stop_server != NULL && !server->internal->stop_requested)
      {
        server->internal->stop_requested = should_stop_server(server, context);
      }
    }
  } while (!server->internal->stop_requested);
}


void _hs_server_single_thread_on_connection(struct HSServer *server, struct HSSocket *socket, void *context, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t, void *))
{
  if (  server == NULL
     || server->router == NULL
     || !socket)
  {
    return;
  }

  hs_router_serve_forever(server->router, socket, context, should_stop_for_connection);

  hs_socket_close_and_release(socket);

  if (should_stop_server != NULL && !server->internal->stop_requested)
  {
    server->internal->stop_requested = should_stop_server(server, context);
  }
}

