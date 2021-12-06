#ifndef __HS_SOCKET_H__
#define __HS_SOCKET_H__

#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>

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

bool hs_socket_is_open(struct HSSocket *);
void hs_socket_close(struct HSSocket *);
void hs_socket_close_and_release(struct HSSocket *);

struct HSSocket *hs_socket_plain_new(int /* socket */);
struct HSSocket *hs_socket_plain_accept(struct HSSocket *, struct sockaddr *, int /* address size */);

#ifdef HS_SSL_SUPPORTED

#include <openssl/ssl.h>

struct HSSocket *hs_socket_ssl_new(int /* socket */, SSL_CTX *);
struct HSSocket *hs_socket_ssl_accept(struct HSSocket *, struct sockaddr *, int /* address size */, SSL_CTX *);

#endif

#endif

