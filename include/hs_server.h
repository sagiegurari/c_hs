#ifndef HS_SERVER_H
#define HS_SERVER_H

#include "hs_socket.h"
#include <netinet/ip.h>
#include <sys/time.h>

struct HSServer;
struct HSServerInternal;

struct HSServerConnectionHandler
{
  void (*init)(struct HSServerConnectionHandler *);
  void (*on_connection)(struct HSServerConnectionHandler *, struct HSServer *, struct HSSocket *, void * /* context */, bool (*should_stop_server)(struct HSServer *, void * /* context */), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t /* request counter */, void * /* context */));
  void (*stop_connections)(struct HSServerConnectionHandler *);
  void (*release)(struct HSServerConnectionHandler *);
  void *extension;
};

struct HSServerSSLInfo
{
  // external files with the key and certificate (default NULL)
  // The strings will be released with the server.
  char *private_key_pem_file;
  char *certificate_pem_file;
};

struct HSServer
{
  struct HSRouter                  *router;
  time_t                           accept_recv_timeout_seconds;
  time_t                           request_recv_timeout_seconds;
  struct HSServerConnectionHandler *connection_handler;

  // If populated, it will enable TLS support.
  // However, if HS_SSL_SUPPORTED is undefined while this struct is populated,
  // it means the library was compiled without SSL support and the
  // hs_server_serve function will return false to avoid security issue.
  struct HSServerSSLInfo  *ssl_info;

  // server functions should not be invoked directly, instead use the hs_server_xxx functions.
  struct HSSocket         * (*create_socket_and_listen)(struct HSServer *, struct sockaddr_in *);
  struct HSSocket         * (*accept)(struct HSServer *, struct HSSocket *, struct sockaddr *, int /* address size */);
  void                    (*listen_loop)(struct HSServer *, struct HSSocket *, struct sockaddr_in, void * /* context */, bool (*should_stop_server)(struct HSServer *, void * /* context */), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t /* request counter */, void * /* context */));
  struct HSServerInternal *internal;
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
 * Creates a new fully initialized server and returns it.
 * The server will run all requests via thread pool.
 */
struct HSServer *hs_server_new_multi_thread(size_t /* thread pool size */);

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
bool hs_server_serve(struct HSServer *, struct sockaddr_in, void * /* context */, bool (*should_stop_server)(struct HSServer *, void * /* context */), bool (*should_stop_for_connection)(struct HSRouter *, struct HSSocket *, size_t /* request counter */, void * /* context */));

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
 * This function can be set for the server->create_socket_and_listen and should
 * not be invoked directly.
 */
struct HSSocket *hs_server_create_socket_and_listen(struct HSServer *, struct sockaddr_in *);

/**
 * Simple utility function to create address for the given port.
 */
struct sockaddr_in hs_server_init_ipv4_address(uint16_t /* port */);

#ifdef HS_SSL_SUPPORTED

#endif

#endif

