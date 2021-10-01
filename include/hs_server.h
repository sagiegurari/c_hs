#ifndef __HS_SERVER_H__
#define __HS_SERVER_H__

#include <netinet/ip.h>
#include <sys/time.h>

struct HSServer;
struct HSServerInternal;

struct HSServerConnectionHandler
{
  void (*init)(struct HSServerConnectionHandler *);
  void (*on_connection)(struct HSServer *, int /* socket */, void * /* context */, bool (*should_stop_server)(struct HSServer *, void * /* context */), bool (*should_stop_for_connection)(struct HSRouter *, int /* socket */, int /* request counter */, void * /* context */));
  void (*stop_connections)(struct HSServerConnectionHandler *);
  void (*release)(struct HSServerConnectionHandler *);
  void *extension;
};

struct HSServer
{
  struct HSRouter                  *router;
  time_t                           accept_recv_timeout_seconds;
  time_t                           request_recv_timeout_seconds;
  int                              (*create_socket_and_listen)(struct HSServer *, struct sockaddr_in *);
  struct HSServerConnectionHandler *connection_handler;
  struct HSServerInternal          *internal;
};

/**
 * Creates a new server and returns it.
 * The server is by default setup as single thread implementation
 * but can be mutated after this call to allow custom connection handler
 * functions.
 */
struct HSServer *hs_server_new(void);

/**
 * Creates a new fully initialized server and returns it.
 * The server will run all requests on the current thread.
 */
struct HSServer *hs_server_new_single_thread();

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
bool hs_server_serve(struct HSServer *, struct sockaddr_in, void * /* context */, bool (*should_stop_server)(struct HSServer *, void * /* context */), bool (*should_stop_for_connection)(struct HSRouter *, int /* socket */, int /* request counter */, void * /* context */));

/**
 * Returns new connection handler.
 */
struct HSServerConnectionHandler *hs_server_connection_handler_new();

/**
 * Releases the connection handler by calling the release function (if exists)
 * and clears any memory.
 */
void hs_server_connection_handler_release(struct HSServerConnectionHandler *);

/**
 * Basic implementation of creating the server socket, binding and listening
 * to new incoming connections.
 * This function can be set for the server->create_socket_and_listen.
 */
int hs_server_create_socket_and_listen(struct HSServer *, struct sockaddr_in *);

/**
 * Simple utility function to create address for the given port.
 */
struct sockaddr_in hs_server_init_ipv4_address(uint16_t /* port */);

#endif

