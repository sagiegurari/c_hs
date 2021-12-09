#ifndef __HS_SOCKET_H__
#define __HS_SOCKET_H__

#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>

struct HSSocket;

bool hs_socket_is_open(struct HSSocket *);
void hs_socket_close(struct HSSocket *);
void hs_socket_close_and_release(struct HSSocket *);

ssize_t hs_socket_read(struct HSSocket *, void * /* buffer */, size_t /* count */);
ssize_t hs_socket_write(struct HSSocket *, const void * /* buffer */, size_t /* count */);
bool    hs_socket_set_recv_timeout_in_seconds(struct HSSocket *, long);

struct HSSocket *hs_socket_plain_new(int /* socket */);
struct HSSocket *hs_socket_plain_accept(struct HSSocket *, struct sockaddr *, int /* address size */);

#ifdef HS_SSL_SUPPORTED

#include <openssl/ssl.h>

struct HSSocket *hs_socket_ssl_new(int /* socket */, SSL_CTX *);
struct HSSocket *hs_socket_ssl_accept(struct HSSocket *, struct sockaddr *, int /* address size */, SSL_CTX *);

#endif

#endif

