#include "test.h"
#include <stdlib.h>
#include <string.h>


char *_test_with_values(struct HSCookie *cookie, struct HSRoute *route)
{
  bool release_route = false;

  if (route == NULL)
  {
    route         = hs_routes_session_route_new_default();
    release_route = true;
  }
  struct HSServeFlowParams *params = hs_types_serve_flow_params_new();

  if (cookie != NULL)
  {
    hs_types_cookies_add(params->request->cookies, cookie);
  }

  enum HSServeFlowResponse response = route->serve(route, params);

  assert_num_equal(response, HS_SERVE_FLOW_RESPONSE_CONTINUE);

  assert_num_equal(params->callbacks->count, 1);
  struct HSPostResponseCallback *callback = params->callbacks->callbacks[0];
  assert_true(callback != NULL);

  assert_num_equal(hs_types_cookies_count(params->response->cookies), 1);
  struct HSCookie *session_cookie = hs_types_cookies_get(params->response->cookies, 0);
  assert_true(session_cookie != NULL);
  assert_string_equal(HS_DEFAULT_SESSION_COOKIE_NAME, session_cookie->name);
  assert_true(session_cookie->value != NULL);
  char *session_id = strdup(session_cookie->value);
  if (cookie != NULL)
  {
    assert_string_equal(session_id, cookie->value);
    hs_io_free(session_id);
  }

  callback->run(callback);

  hs_types_serve_flow_params_release(params);
  if (release_route)
  {
    hs_route_release_route(route);
  }

  return(session_id);
} /* _test_with_values */


void _test_flow(struct HSRoute *route)
{
  char *session_id = _test_with_values(NULL, route);

  assert_true(session_id != NULL);

  struct HSCookie *cookie = hs_types_cookie_new();
  cookie->name  = strdup(HS_DEFAULT_SESSION_COOKIE_NAME);
  cookie->value = strdup(session_id);
  _test_with_values(cookie, route);

  cookie        = hs_types_cookie_new();
  cookie->name  = strdup(HS_DEFAULT_SESSION_COOKIE_NAME);
  cookie->value = strdup(session_id);
  _test_with_values(cookie, route);

  hs_io_free(session_id);
}


void test_impl()
{
  _test_flow(NULL);

  struct HSRoute *route = hs_routes_session_route_new_default();
  _test_flow(route);
  hs_route_release_route(route);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

