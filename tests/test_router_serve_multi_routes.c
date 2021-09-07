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

  struct HSSession *session = (struct HSSession *)hs_types_route_flow_state_get_data_by_key(params->route_state, HS_DEFAULT_SESSION_STATE_NAME);
  assert_true(session != NULL);
  assert_true(session->id != NULL);
  if (global_session_id == NULL)
  {
    global_session_id = strdup(session->id);
    hs_types_key_value_array_add(session->string_pairs, strdup("counter"), stringfn_format("%d", global_counter));
  }
  else
  {
    assert_string_equal(global_session_id, session->id);
    assert_num_equal(atoi(hs_types_key_value_array_get_by_key(session->string_pairs, "counter")), global_counter);
    hs_io_free(session->string_pairs->pairs[0]->value);
    global_counter++;
    session->string_pairs->pairs[0]->value = stringfn_format("%d", global_counter);
  }

  return(HS_SERVE_FLOW_RESPONSE_DONE);
}


void _test_with_values(struct HSRouter *router, struct HSRoute *route, bool is_get, bool is_post, bool closed, char *expected_response)
{
  route->is_get  = is_get;
  route->is_post = is_post;

  char                     *filename = "./test_router_serve_multi_routes.txt";
  fsio_create_empty_file(filename);
  int                      socket = open(filename, O_WRONLY);

  struct HSServeFlowParams *params = hs_types_new_serve_flow_params();
  params->socket            = socket;
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

  close(socket);

  if (expected_response != NULL)
  {
    char *content = fsio_read_text_file(filename);

    char *updated_response = stringfn_format(expected_response, global_session_id);
    assert_string_equal(content, updated_response);

    hs_io_free(updated_response);
    hs_io_free(content);
  }

  fsio_remove(filename);
  hs_types_release_serve_flow_params(params);
} /* _test_with_values */


void test_impl()
{
  struct HSRouter *router = hs_router_new();

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

  hs_router_add_route(router, hs_routes_error_404_route_new());

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

