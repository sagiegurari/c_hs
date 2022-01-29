#include "hs_socket.h"
#include <arpa/inet.h>
#include <stdlib.h>

static int _hs_socket_get_socket(struct HSSocket *);
static int _hs_socket_server_accept(struct HSSocket *, struct sockaddr *, int);
static bool _hs_socket_is_closed(struct HSSocket *);
static bool _hs_socket_set_recv_timeout_in_seconds(struct HSSocket *, long);
static ssize_t _hs_socket_plain_read(struct HSSocket *, void *, size_t);
static ssize_t _hs_socket_plain_write(struct HSSocket *, const void *, size_t);
static void _hs_socket_plain_close(struct HSSocket *);
static void _hs_socket_plain_release(struct HSSocket *);


struct HSSocket
{
  bool    (*is_closed)(struct HSSocket *);
  ssize_t (*read)(struct HSSocket *, void * /* buffer */, size_t /* count */);
  ssize_t (*write)(struct HSSocket *, const void * /* buffer */, size_t /* count */);
  bool    (*set_recv_timeout_in_seconds)(struct HSSocket *, long);
  void    (*close)(struct HSSocket *);
  void    (*release)(struct HSSocket *);
  int     raw_socket; // should not be used directly
  void    *internal;
};


bool hs_socket_is_open(struct HSSocket *socket)
{
  if (socket == NULL || socket->is_closed == NULL)
  {
    return(false);
  }

  return(!socket->is_closed(socket));
}


void hs_socket_close(struct HSSocket *socket)
{
  if (socket == NULL)
  {
    return;
  }

  if (socket->close != NULL)
  {
    socket->close(socket);
  }
}


void hs_socket_close_and_release(struct HSSocket *socket)
{
  if (socket == NULL)
  {
    return;
  }

  hs_socket_close(socket);

  if (socket->release != NULL)
  {
    socket->release(socket);
  }
  else
  {
    free(socket);
  }
}


ssize_t hs_socket_read(struct HSSocket *socket, void *buffer, size_t count)
{
  if (socket == NULL || socket->read == NULL)
  {
    return(-1);
  }

  return(socket->read(socket, buffer, count));
}


ssize_t hs_socket_write(struct HSSocket *socket, const void *buffer, size_t count)
{
  if (socket == NULL || socket->write == NULL)
  {
    return(-1);
  }

  return(socket->write(socket, buffer, count));
}


bool    hs_socket_set_recv_timeout_in_seconds(struct HSSocket *socket, long recv_timeout_seconds)
{
  if (socket == NULL)
  {
    return(false);
  }

  return(socket->set_recv_timeout_in_seconds(socket, recv_timeout_seconds));
}

struct HSSocket *hs_socket_plain_new(int socket)
{
  if (!socket)
  {
    return(NULL);
  }

  struct HSSocket *hssocket = malloc(sizeof(struct HSSocket));
  if (hssocket == NULL)
  {
    return(NULL);
  }

  hssocket->is_closed                   = _hs_socket_is_closed;
  hssocket->read                        = _hs_socket_plain_read;
  hssocket->write                       = _hs_socket_plain_write;
  hssocket->set_recv_timeout_in_seconds = _hs_socket_set_recv_timeout_in_seconds;
  hssocket->close                       = _hs_socket_plain_close;
  hssocket->release                     = _hs_socket_plain_release;
  hssocket->raw_socket                  = socket;
  hssocket->internal                    = NULL;

  return(hssocket);
}

struct HSSocket *hs_socket_plain_accept(struct HSSocket *server_socket, struct sockaddr *address, int address_size)
{
  int client_socket = _hs_socket_server_accept(server_socket, address, address_size);

  if (!client_socket)
  {
    return(NULL);
  }

  return(hs_socket_plain_new(client_socket));
}


static int _hs_socket_get_socket(struct HSSocket *hssocket)
{
  if (hssocket == NULL)
  {
    return(0);
  }

  if (hssocket->raw_socket <= 0)
  {
    return(0);
  }

  return(hssocket->raw_socket);
}


static int _hs_socket_server_accept(struct HSSocket *server_socket, struct sockaddr *address, int address_size)
{
  int socket_fd = _hs_socket_get_socket(server_socket);

  if (!socket_fd)
  {
    return(0);
  }

  int client_socket = accept(socket_fd, address, (socklen_t *)&address_size);
  if (client_socket <= 0)
  {
    return(0);
  }

  return(client_socket);
}


static bool _hs_socket_is_closed(struct HSSocket *hssocket)
{
  int socket = _hs_socket_get_socket(hssocket);

  return(!socket);
}


static bool _hs_socket_set_recv_timeout_in_seconds(struct HSSocket *hssocket, long recv_timeout_seconds)
{
  int socket = _hs_socket_get_socket(hssocket);

  if (!socket)
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


static ssize_t _hs_socket_plain_read(struct HSSocket *hssocket, void *buffer, size_t count)
{
  int socket = _hs_socket_get_socket(hssocket);

  if (!socket)
  {
    return(-1);
  }

  return(read(socket, buffer, count));
}


static ssize_t _hs_socket_plain_write(struct HSSocket *hssocket, const void *buffer, size_t count)
{
  int socket = _hs_socket_get_socket(hssocket);

  if (!socket)
  {
    return(-1);
  }

  return(write(socket, buffer, count));
}


static void _hs_socket_plain_close(struct HSSocket *hssocket)
{
  int socket = _hs_socket_get_socket(hssocket);

  if (!socket)
  {
    return;
  }

  close(socket);
}


static void _hs_socket_plain_release(struct HSSocket *hssocket)
{
  if (hssocket == NULL)
  {
    return;
  }

  free(hssocket);
}

#ifdef HS_SSL_SUPPORTED
ssize_t _hs_socket_ssl_read(struct HSSocket *, void *, size_t);
ssize_t _hs_socket_ssl_write(struct HSSocket *, const void *, size_t);
void _hs_socket_ssl_close(struct HSSocket *);
void _hs_socket_ssl_release(struct HSSocket *);
void _hs_socket_ssl_init();
void _hs_socket_ssl_cleanup();

struct HSSocket *hs_socket_ssl_new(int socket, SSL_CTX *ssl_context)
{
  if (!socket || ssl_context == NULL)
  {
    return(NULL);
  }

  SSL *ssl = SSL_new(ssl_context);
  if (ssl == NULL)
  {
    return(NULL);
  }
  SSL_set_fd(ssl, socket);

  struct HSSocket *hssocket = malloc(sizeof(struct HSSocket));

  hssocket->is_closed                   = _hs_socket_is_closed;
  hssocket->read                        = _hs_socket_ssl_read;
  hssocket->write                       = _hs_socket_ssl_write;
  hssocket->set_recv_timeout_in_seconds = _hs_socket_set_recv_timeout_in_seconds;
  hssocket->close                       = _hs_socket_ssl_close;
  hssocket->release                     = _hs_socket_ssl_release;
  hssocket->raw_socket                  = socket;

  hssocket->internal = ssl;

  return(hssocket);
}

struct HSSocket *hs_socket_ssl_accept(struct HSSocket *server_socket, struct sockaddr *address, int address_size, SSL_CTX *ssl_context)
{
  int client_socket = _hs_socket_server_accept(server_socket, address, address_size);

  if (!client_socket)
  {
    return(NULL);
  }

  struct HSSocket *hssocket = hs_socket_ssl_new(client_socket, ssl_context);
  if (hssocket == NULL)
  {
    return(NULL);
  }

  SSL *ssl = (SSL *)hssocket->internal;
  if (SSL_accept(ssl) <= 0)
  {
    hs_socket_close_and_release(hssocket);
    return(NULL);
  }

  return(hssocket);
}


SSL *_hs_socket_get_ssl(struct HSSocket *hssocket)
{
  int socket = _hs_socket_get_socket(hssocket);

  if (!socket)
  {
    return(NULL);
  }

  return((SSL *)hssocket->internal);
}


ssize_t _hs_socket_ssl_read(struct HSSocket *hssocket, void *buffer, size_t count)
{
  SSL *ssl = _hs_socket_get_ssl(hssocket);

  if (ssl == NULL)
  {
    return(-1);
  }

  int read = SSL_read(ssl, buffer, (int)count);

  if (read <= 0)
  {
    return(-1);
  }

  return((ssize_t)read);
}


ssize_t _hs_socket_ssl_write(struct HSSocket *hssocket, const void *buffer, size_t count)
{
  SSL *ssl = _hs_socket_get_ssl(hssocket);

  if (ssl == NULL)
  {
    return(-1);
  }

  int written = SSL_write(ssl, buffer, (int)count);

  if (written <= 0)
  {
    return(-1);
  }

  return((ssize_t)written);
}


void _hs_socket_ssl_close(struct HSSocket *hssocket)
{
  int socket = _hs_socket_get_socket(hssocket);

  if (!socket)
  {
    return;
  }

  SSL *ssl = _hs_socket_get_ssl(hssocket);
  if (ssl != NULL)
  {
    SSL_shutdown(ssl);
  }

  close(socket);
}


void _hs_socket_ssl_release(struct HSSocket *hssocket)
{
  if (hssocket == NULL)
  {
    return;
  }

  SSL *ssl = _hs_socket_get_ssl(hssocket);
  if (ssl != NULL)
  {
    SSL_free(ssl);
  }

  free(hssocket);
}
#endif

