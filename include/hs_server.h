#ifndef __HS_SERVER_H__
#define __HS_SERVER_H__

#include <netinet/ip.h>
#include <sys/time.h>

struct HSServer
{
  struct HSRouter *router;
  time_t          accept_recv_timeout_seconds;
  time_t          request_recv_timeout_seconds;
  int             (*create_socket_and_listen)(struct HSServer *, struct sockaddr_in *);
  void            (*on_connection)(struct HSServer *, int /* socket */, void * /* context */, bool (*should_stop)(struct HSRouter *, int /* socket */, int /* request counter */, void * /* context */));
};

/**
 * Creates a new server and returns it.
 */
struct HSServer *hs_server_new(void);

/**
 * Frees all memory used by the server.
 */
void hs_server_release(struct HSServer *);

/**
 * This is the main server function that initializes the socket and starts
 * listening. Any connection will be handled by the internal router.
 * After each timeout or accept, the callback will be called to check if
 * to continue handling or to stop (return true to stop).
 * Once stop is requested via callback, the socket will be closed and this
 * function can be invoked again.
 */
bool hs_server_serve(struct HSServer *, struct sockaddr_in, void * /* context */, bool (*should_stop_server)(struct HSServer *, void * /* context */), bool (*should_stop_router)(struct HSRouter *, int /* socket */, int /* request counter */, void * /* context */));

/**
 * Basic implementation of creating the server socket, binding and listening
 * to new incoming connections.
 */
int hs_server_create_socket_and_listen(struct HSServer *, struct sockaddr_in *);

/**
 * Basic single threaded implementation of the server on_connection which
 * simply serves the requests via router on the same thread until socket
 * is closed.
 * Since its single threaded and will block other clients from being served,
 * you can set the router to not support keep alive meaning it will in
 * practice, only one request will be handled.
 */
void hs_server_on_connection(struct HSServer *, int /* socket */, void * /* context */, bool (*should_stop)(struct HSRouter *, int /* socket */, int /* request counter */, void * /* context */));

/**
 * Simple utility function to create address for the given port.
 */
struct sockaddr_in hs_server_init_ipv4_address(uint16_t /* port */);

#endif

