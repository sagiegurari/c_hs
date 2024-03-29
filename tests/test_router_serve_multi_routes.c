#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char                     *global_session_id = NULL;
int                      global_counter     = 1;

enum HSServeFlowResponse _test_serve(struct HSRoute *route, struct HSServeFlowParams *params)
{
  assert_true(route != NULL);
  assert_true(params != NULL);

  params->response->code = HS_HTTP_RESPONSE_CODE_OK;

  struct HSSession *session = (struct HSSession *)hashtable_get(params->route_state->data, HS_DEFAULT_SESSION_STATE_NAME);
  assert_true(session != NULL);
  assert_true(session->id != NULL);
  if (global_session_id == NULL)
  {
    global_session_id = strdup(session->id);
    hashtable_insert(session->data, strdup("counter"), stringfn_format("%d", global_counter), hs_io_release_hashtable_key_and_value);
  }
  else
  {
    assert_string_equal(global_session_id, session->id);
    assert_num_equal(atoi(hashtable_get(session->data, "counter")), global_counter);
    global_counter++;
    hashtable_insert(session->data, strdup("counter"), stringfn_format("%d", global_counter), hs_io_release_hashtable_key_and_value);
  }

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


void _test_with_values(struct HSRouter *router, struct HSRoute *route, bool is_get, bool is_post, bool closed, char *expected_response)
{
  route->is_get  = is_get;
  route->is_post = is_post;

  char                           *filename = "./test_router_serve_multi_routes.txt";
  fsio_create_empty_file(filename);
  int                            socket = open(filename, O_WRONLY);

  struct HSSocket                *hssocket         = hs_socket_plain_new(socket);
  struct HSServerConnectionState *connection_state = hs_types_server_connection_state_new();
  connection_state->socket = hssocket;
  struct HSServeFlowParams       *params = hs_types_serve_flow_params_new();
  params->connection_state  = connection_state;
  params->request->resource = strdup("/test");
  params->request->method   = HS_HTTP_METHOD_GET;

  if (global_session_id != NULL)
  {
    struct HSCookie *cookie = hs_types_cookie_new();
    cookie->name  = strdup("sc");
    cookie->value = strdup(global_session_id);
    hs_types_cookies_add(params->request->cookies, cookie);
  }

  params->router_state->closed_connection = closed;
  bool done = hs_router_serve(router, params);

  if (expected_response == NULL || closed)
  {
    assert_true(!done);
  }
  else
  {
    assert_true(done);
  }

  hs_socket_close_and_release(hssocket);

  if (expected_response != NULL)
  {
    char *content = fsio_read_text_file(filename);

    char *updated_response = stringfn_format(expected_response, global_session_id);
    assert_string_equal(content, updated_response);

    hs_io_free(updated_response);
    hs_io_free(content);
  }

  fsio_remove(filename);
  hs_types_serve_flow_params_release(params);
  hs_types_server_connection_state_release(connection_state);
} /* _test_with_values */


void test_impl()
{
  struct HSRouter *router = hs_router_new();

  hs_router_add_route(router, hs_routes_error_411_length_required_route_new());
  hs_router_add_route(router, hs_routes_payload_limit_route_new(1024));
  hs_router_add_route(router, hs_routes_session_route_new_default());
  hs_router_add_route(router, hs_routes_powered_by_route_new(NULL));

  struct HSRoute *route = hs_route_new();

  route->path   = strdup("/test");
  route->serve  = _test_serve;
  route->is_get = true;
  hs_router_add_route(router, route);

  _test_with_values(router, route, true, false, false, "HTTP/1.1 200 200\r\n"
                    "X-Powered-By: CHS\r\n"
                    "Set-Cookie: sc=%s; Max-Age=63072000; Secure; HttpOnly; SameSite=Strict\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, route, false, true, false, NULL);

  _test_with_values(router, route, true, false, false, "HTTP/1.1 200 200\r\n"
                    "X-Powered-By: CHS\r\n"
                    "Set-Cookie: sc=%s; Max-Age=63072000; Secure; HttpOnly; SameSite=Strict\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, route, false, true, false, NULL);

  hs_router_add_route(router, hs_routes_error_404_not_found_route_new());

  _test_with_values(router, route, false, false, false, "HTTP/1.1 404 404\r\n"
                    "X-Powered-By: CHS\r\n"
                    "Set-Cookie: sc=%s; Max-Age=63072000; Secure; HttpOnly; SameSite=Strict\r\n"
                    "Connection: close\r\n"
                    "Content-Length: 0\r\n"
                    "\r\n");

  _test_with_values(router, route, true, true, true, "");

  hs_router_release(router);
  hs_io_free(global_session_id);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

