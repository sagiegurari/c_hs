#include "hs_socket.h"
#include <arpa/inet.h>
#include <stdlib.h>

struct HSSocketPlain
{
  int socket;
};

int _hs_socket_plain_get_socket(struct HSSocket *);
bool _hs_socket_plain_is_closed(struct HSSocket *);
ssize_t _hs_socket_plain_read(struct HSSocket *, void *, size_t);
ssize_t _hs_socket_plain_write(struct HSSocket *, const void *, size_t);
bool _hs_socket_plain_set_recv_timeout_in_seconds(struct HSSocket *, long);
void _hs_socket_plain_close(struct HSSocket *);
void _hs_socket_plain_release(struct HSSocket *);


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

struct HSSocket *hs_socket_plain_new(int socket)
{
  if (!socket)
  {
    return(NULL);
  }

  struct HSSocket *hssocket = malloc(sizeof(struct HSSocket));

  hssocket->is_closed                   = _hs_socket_plain_is_closed;
  hssocket->read                        = _hs_socket_plain_read;
  hssocket->write                       = _hs_socket_plain_write;
  hssocket->set_recv_timeout_in_seconds = _hs_socket_plain_set_recv_timeout_in_seconds;
  hssocket->close                       = _hs_socket_plain_close;
  hssocket->release                     = _hs_socket_plain_release;

  struct HSSocketPlain *plain_socket = malloc(sizeof(struct HSSocketPlain));
  plain_socket->socket = socket;
  hssocket->internal   = plain_socket;

  return(hssocket);
}

struct HSSocket *hs_socket_plain_accept(struct HSSocket *server_socket, struct sockaddr *address, socklen_t *address_size)
{
  int socket_fd = _hs_socket_plain_get_socket(server_socket);

  if (!socket_fd)
  {
    return(NULL);
  }

  int client_socket = accept(socket_fd, address, address_size);
  if (!client_socket)
  {
    return(NULL);
  }

  return(hs_socket_plain_new(client_socket));
}


int _hs_socket_plain_get_socket(struct HSSocket *hssocket)
{
  if (hssocket == NULL)
  {
    return(0);
  }

  struct HSSocketPlain *plain_socket = (struct HSSocketPlain *)hssocket->internal;

  if (plain_socket->socket <= 0)
  {
    return(0);
  }

  return(plain_socket->socket);
}


bool _hs_socket_plain_is_closed(struct HSSocket *hssocket)
{
  int socket = _hs_socket_plain_get_socket(hssocket);

  return(!socket);
}


ssize_t _hs_socket_plain_read(struct HSSocket *hssocket, void *buffer, size_t count)
{
  int socket = _hs_socket_plain_get_socket(hssocket);

  if (!socket)
  {
    return(-1);
  }

  return(read(socket, buffer, count));
}


ssize_t _hs_socket_plain_write(struct HSSocket *hssocket, const void *buffer, size_t count)
{
  int socket = _hs_socket_plain_get_socket(hssocket);

  if (!socket)
  {
    return(-1);
  }

  return(write(socket, buffer, count));
}


bool _hs_socket_plain_set_recv_timeout_in_seconds(struct HSSocket *hssocket, long recv_timeout_seconds)
{
  int socket = _hs_socket_plain_get_socket(hssocket);

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


void _hs_socket_plain_close(struct HSSocket *hssocket)
{
  int socket = _hs_socket_plain_get_socket(hssocket);

  if (!socket)
  {
    return;
  }

  close(socket);
}


void _hs_socket_plain_release(struct HSSocket *hssocket)
{
  if (hssocket == NULL)
  {
    return;
  }

  if (hssocket->internal != NULL)
  {
    free(hssocket->internal);
  }

  free(hssocket);
}

