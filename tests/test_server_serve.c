#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

int                            global_request_counter    = 0;
int                            global_connection_counter = 0;

struct HSRouteRedirectResponse *_test_redirect(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  assert_true(route != NULL);
  assert_true(request != NULL);
  assert_true(socket);

  struct HSRouteRedirectResponse *response = hs_route_new_redirect_response();

  response->path = strdup("/mylocation");

  return(response);
}


struct HSRouteServeResponse *_test_serve(struct HSRoute *route, struct HSHttpRequest *request, int socket)
{
  assert_true(route != NULL);
  assert_true(request != NULL);
  assert_true(socket);

  struct HSRouteServeResponse *response = hs_route_new_serve_response();

  response->code           = HS_HTTP_RESPONSE_CODE_OK;
  response->content_string = strdup("my html");

  return(response);
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


bool _test_should_stop_router(struct HSRouter *router, int socket, int counter, void *context)
{
  if (router == NULL || !socket || context == NULL)
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
  char *filename = "./test_server_serve.txt";

  fsio_remove(filename);

  int pid = fork();
  assert_true(pid >= 0);

  if (pid)
  {
    struct HSServer *server     = hs_server_new();
    struct HSRouter *sub_router = hs_router_new();
    struct HSRoute  *route      = hs_route_new_route();

    server->accept_recv_timeout_seconds  = 5;
    server->request_recv_timeout_seconds = 5;
    struct sockaddr_in address = hs_server_init_ipv4_address(7005);

    route->is_get   = true;
    route->path     = strdup("/gohome");
    route->redirect = _test_redirect;
    hs_router_add_route(sub_router, route);

    route         = hs_route_new_route();
    route->is_get = true;
    route->path   = strdup("/index.html");
    route->serve  = _test_serve;
    hs_router_add_route(sub_router, route);

    route       = hs_router_as_route(sub_router);
    route->path = strdup("/admin/");

    hs_router_add_route(server->router, route);
    route = hs_routes_new_404_route();
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
                        "Content-Type: text/html\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "GET /admin HTTP/1.0\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "HTTP/1.1 404 404\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "GET /admin/ HTTP/1.0\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "HTTP/1.1 404 404\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "GET /admin/gohome HTTP/1.0\r\n"
                        "Connection: keep-alive\r\n"
                        "Content-Length: 0\r\n"
                        "\r\n"
                        "HTTP/1.1 307 307\r\n"
                        "Connection: keep-alive\r\n"
                        "Location: /mylocation\r\n"
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

    char *request = "GET /test HTTP/1.0\r\n"
                    "Connection: keep-alive\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n";
    fsio_append_text_file(filename, request);
    send(socket_fd, request, strlen(request), 0);
    sleep(1);
    read_size         = read(socket_fd, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);
    request = "GET /admin HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    fsio_append_text_file(filename, request);
    send(socket_fd, request, strlen(request), 0);
    sleep(1);
    read_size         = read(socket_fd, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);
    request = "GET /admin/ HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    fsio_append_text_file(filename, request);
    send(socket_fd, request, strlen(request), 0);
    sleep(1);
    read_size         = read(socket_fd, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);

    close(socket_fd);

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

    request = "GET /admin/gohome HTTP/1.0\r\n"
              "Connection: keep-alive\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    fsio_append_text_file(filename, request);
    send(socket_fd, request, strlen(request), 0);
    sleep(1);
    read_size         = read(socket_fd, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);
    request = "GET /admin/index.html HTTP/1.0\r\n"
              "Connection: close\r\n"
              "Content-Length: 0\r\n"
              "\r\n";
    fsio_append_text_file(filename, request);
    send(socket_fd, request, strlen(request), 0);
    sleep(1);
    read_size         = read(socket_fd, buffer, 1024);
    buffer[read_size] = '\0';
    fsio_append_text_file(filename, buffer);

    close(socket_fd);
  }
} /* test_impl */


int main()
{
  test_run(test_impl);
}

