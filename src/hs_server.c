#include "hs_io.h"
#include "hs_openssl.h"
#include "hs_router.h"
#include "hs_server.h"
#include "vector.h"
#include <stdlib.h>

struct HSServerInternal
{
  bool    running;
  bool    stop_requested;
#ifdef HS_SSL_SUPPORTED
  SSL_CTX *ssl_context;
#endif
};

static struct HSSocket *_hs_server_plain_socket_accept(struct HSServer *, struct HSSocket *, struct sockaddr *, int);
static void _hs_server_socket_listen_loop(struct HSServer *, struct HSSocket *, struct sockaddr_in, void *, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t, void *));
static void _hs_server_single_thread_on_connection(struct HSServer *, struct HSSocket *, void *, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t, void *));

#ifdef HS_SSL_SUPPORTED

static struct HSSocket *_hs_server_ssl_socket_accept(struct HSServer *, struct HSSocket *, struct sockaddr *, int);

#endif

struct HSServer *hs_server_new()
{
  struct HSServer *server = malloc(sizeof(struct HSServer));

  server->router                       = hs_router_new();
  server->accept_recv_timeout_seconds  = -1;
  server->request_recv_timeout_seconds = -1;
  server->create_socket_and_listen     = hs_server_create_socket_and_listen;
  server->listen_loop                  = _hs_server_socket_listen_loop;

  server->connection_handler = hs_server_connection_handler_new();

  server->ssl_info                       = malloc(sizeof(struct HSServerSSLInfo));
  server->ssl_info->private_key_pem_file = NULL;
  server->ssl_info->certificate_pem_file = NULL;

  server->internal                 = malloc(sizeof(struct HSServerInternal));
  server->internal->running        = false;
  server->internal->stop_requested = false;

#ifdef HS_SSL_SUPPORTED
  server->internal->ssl_context = NULL;
#endif

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

  if (server->ssl_info != NULL)
  {
    hs_io_free(server->ssl_info->private_key_pem_file);
    hs_io_free(server->ssl_info->certificate_pem_file);
    hs_io_free(server->ssl_info);
  }

  if (server->internal != NULL)
  {
#ifdef HS_SSL_SUPPORTED
    hs_openssl_context_release(server->internal->ssl_context);
    server->internal->ssl_context = NULL;
#endif

    hs_io_free(server->internal);
  }

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
     || server->connection_handler->on_connection == NULL
     || server->internal->running)
  {
    return(false);
  }

  bool use_ssl = false;
  if (  server->ssl_info != NULL
     && (  server->ssl_info->private_key_pem_file != NULL
        && server->ssl_info->certificate_pem_file != NULL))
  {
    use_ssl = true;
  }
  if (use_ssl)
  {
#ifdef HS_SSL_SUPPORTED
    server->accept = _hs_server_ssl_socket_accept;
    hs_openssl_init();
    server->internal->ssl_context = hs_openssl_context_create(server->ssl_info->private_key_pem_file, server->ssl_info->certificate_pem_file);

    if (server->internal->ssl_context == NULL)
    {
      hs_openssl_cleanup();
      return(false);
    }
#else
    // SSL was requested but the server was not compiled with it
    return(false);
#endif
  }
  else
  {
    server->accept = _hs_server_plain_socket_accept;
  }

  server->internal->running        = true;
  server->internal->stop_requested = false;

  // start listening
  struct HSSocket *socket = server->create_socket_and_listen(server, &address);
  if (socket == NULL)
  {
#ifdef HS_SSL_SUPPORTED
    if (use_ssl)
    {
      hs_openssl_context_release(server->internal->ssl_context);
      server->internal->ssl_context = NULL;
      hs_openssl_cleanup();
    }
#endif

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

#ifdef HS_SSL_SUPPORTED
  if (use_ssl)
  {
    hs_openssl_context_release(server->internal->ssl_context);
    server->internal->ssl_context = NULL;
    hs_openssl_cleanup();
  }
#endif

  server->internal->running = false;

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


struct HSSocket *hs_server_create_socket_and_listen(struct HSServer *server, struct sockaddr_in *address)
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

  if (!hs_socket_set_recv_timeout_in_seconds(hssocket, server->accept_recv_timeout_seconds))
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
} /* hs_server_create_socket_and_listen */


struct sockaddr_in hs_server_init_ipv4_address(uint16_t port)
{
  struct sockaddr_in address;

  address.sin_family      = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port        = htons(port);

  return(address);
}

static struct HSSocket *_hs_server_plain_socket_accept(struct HSServer *server, struct HSSocket *server_socket, struct sockaddr *address, int address_size)
{
  if (  server == NULL
     || server_socket == NULL)
  {
    return(NULL);
  }

  return(hs_socket_plain_accept(server_socket, address, address_size));
}


static void _hs_server_socket_listen_loop(struct HSServer *server, struct HSSocket *server_socket, struct sockaddr_in address, void *context, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t, void *))
{
  // listen loop
  int address_size = sizeof(address);

  do
  {
    struct HSSocket *client_socket = server->accept(server, server_socket, (struct sockaddr *)&address, address_size);
    if (client_socket != NULL && hs_socket_set_recv_timeout_in_seconds(client_socket, server->request_recv_timeout_seconds))
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


static void _hs_server_single_thread_on_connection(struct HSServer *server, struct HSSocket *socket, void *context, bool (*should_stop_server)(struct HSServer *, void *), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t, void *))
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

#ifdef HS_SSL_SUPPORTED
#include <openssl/err.h>

static struct HSSocket *_hs_server_ssl_socket_accept(struct HSServer *server, struct HSSocket *server_socket, struct sockaddr *address, int address_size)
{
  if (  server == NULL
     || server_socket == NULL
     || server->internal->ssl_context == NULL)
  {
    return(NULL);
  }

  return(hs_socket_ssl_accept(server_socket, address, address_size, server->internal->ssl_context));
}

#endif

