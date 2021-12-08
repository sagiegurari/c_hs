#include "test.h"
#include <stdio.h>

#ifdef HS_SSL_SUPPORTED
#include "fsio.h"
#include "stringfn.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

int                      global_request_counter    = 0;
int                      global_connection_counter = 0;

enum HSServeFlowResponse _test_redirect(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);

  params->response->code = HS_HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT;
  hs_types_array_string_pair_add(params->response->headers, strdup("Location"), strdup("/mylocation"));

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


enum HSServeFlowResponse _test_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);

  params->response->code           = HS_HTTP_RESPONSE_CODE_OK;
  params->response->mime_type      = HS_MIME_TYPE_TEXT_HTML;
  params->response->content_string = strdup("my html");

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


bool _test_should_stop_server(struct HSServer *server, void *context)
{
  if (server == NULL || context == NULL)
  {
    test_fail();
    return(true);
  }

  global_connection_counter++;

  assert_string_equal((char *)context, "test context");

  return(global_connection_counter > 2);
}


bool _test_should_stop_router(struct HSRouter *router, struct HSSocket *socket, size_t counter, void *context)
{
  if (router == NULL || !hs_socket_is_open(socket) || context == NULL)
  {
    test_fail();
    return(true);
  }

  assert_true(counter <= 3);
  assert_string_equal((char *)context, "test context");

  global_request_counter++;

  return(false);
} /* _test_should_stop */


void test_impl()
{
  char *filename = "./test_server_serve_tls.txt";

  fsio_remove(filename);

  int pid = fork();
  assert_true(pid >= 0);

  if (pid)
  {
    struct HSServer *server     = hs_server_new_single_thread();
    struct HSRouter *sub_router = hs_router_new();
    struct HSRoute  *route      = hs_route_new();

    server->accept_recv_timeout_seconds    = 5;
    server->request_recv_timeout_seconds   = 5;
    server->ssl_info->private_key_pem_file = strdup("./key.pem");
    server->ssl_info->certificate_pem_file = strdup("./cert.pem");
    struct sockaddr_in address = hs_server_init_ipv4_address(7005);

    hs_router_add_route(server->router, hs_routes_error_411_length_required_route_new());
    hs_router_add_route(server->router, hs_routes_payload_limit_route_new(1024));

    route->is_get = true;
    route->path   = strdup("/gohome");
    route->serve  = _test_redirect;
    hs_router_add_route(sub_router, route);

    route         = hs_route_new();
    route->is_get = true;
    route->path   = strdup("/index.html");
    route->serve  = _test_serve;
    hs_router_add_route(sub_router, route);

    route       = hs_router_as_route(sub_router);
    route->path = strdup("/admin/");

    hs_router_add_route(server->router, route);
    route = hs_routes_error_404_not_found_route_new();
    hs_router_add_route(server->router, route);

    bool done = hs_server_serve(server, address, "test context", _test_should_stop_server, _test_should_stop_router);
    assert_true(done);

    char *content = fsio_read_text_file(filename);
    fsio_remove(filename);

    assert_string_equal(content, "GET /test HTTP/1.0\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "HTTP/1.1 404 404\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "GET /admin HTTP/1.0\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "HTTP/1.1 404 404\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "GET /admin/ HTTP/1.0\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "HTTP/1.1 404 404\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "GET /admin/gohome HTTP/1.0\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "HTTP/1.1 307 307\r\n"
                        "Location: /mylocation\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "GET /admin/index.html HTTP/1.0\r\n"
                        "Connection: close\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "HTTP/1.1 200 200\r\n"
                        "Connection: close\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: 7\r\n"
                        "\r\n"
                        "my html");
    hs_io_free(content);
    assert_num_equal(global_connection_counter, 3);
    assert_num_equal(global_request_counter, 6);

    hs_server_release(server);
  }
  else
  {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    int                socket_fd = 0;
    ssize_t            read_size = -1;
    struct sockaddr_in serv_addr;
    char               buffer[1024] = { 0 };
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      test_fail();
      return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(7005);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
      test_fail();
      return;
    }

    sleep(1); // wait for server to startup
    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
      test_fail();
      return;
    }

    SSL_CTX *ssl_context = SSL_CTX_new(SSLv23_method());
    SSL     *ssl         = SSL_new(ssl_context);
    SSL_set_fd(ssl, socket_fd);
    SSL_connect(ssl);

    char *request = "GET /test HTTP/1.0\r\n"
                    "Connection: keep-alive\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n";
    fsio_append_text_file(filename, request);
    SSL_write(ssl, request, strlen(request));
    sleep(1);
    read_size         = SSL_read(ssl, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);
    request = "GET /admin HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    fsio_append_text_file(filename, request);
    SSL_write(ssl, request, strlen(request));
    sleep(1);
    read_size         = SSL_read(ssl, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);
    request = "GET /admin/ HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    fsio_append_text_file(filename, request);
    SSL_write(ssl, request, strlen(request));
    sleep(1);
    read_size         = SSL_read(ssl, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);

    close(socket_fd);
    SSL_free(ssl);

    sleep(1);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      test_fail();
      return;
    }
    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
      test_fail();
      return;
    }

    ssl = SSL_new(ssl_context);
    SSL_set_fd(ssl, socket_fd);
    SSL_connect(ssl);

    request = "GET /admin/gohome HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    fsio_append_text_file(filename, request);
    SSL_write(ssl, request, strlen(request));
    sleep(1);
    read_size         = SSL_read(ssl, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);
    request = "GET /admin/index.html HTTP/1.0\r\n"
              "Connection: close\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    fsio_append_text_file(filename, request);
    SSL_write(ssl, request, strlen(request));
    read_size         = SSL_read(ssl, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);
    read_size         = SSL_read(ssl, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);

    close(socket_fd);
    SSL_free(ssl);

    SSL_CTX_free(ssl_context);

    ERR_free_strings();
    EVP_cleanup();
  }
} /* test_impl */
#else


void test_impl()
{
  printf("TLS testing not supported\n");
}
#endif


int main()
{
  test_run(test_impl);
}

