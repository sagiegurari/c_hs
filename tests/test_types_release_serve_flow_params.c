#include "stringfn.h"
#include "test.h"


void _test_set_cookies(struct HSCookies *cookies)
{
  for (size_t index = 0; index < 3; index++)
  {
    struct HSCookie *cookie = hs_types_cookie_new();
    cookie->name  = stringfn_new_empty_string();
    cookie->value = stringfn_new_empty_string();
    hs_types_cookies_add(cookies, cookie);
  }
}


void _test_set_strings(struct HSArrayStringPair *array)
{
  for (size_t index = 0; index < 5; index++)
  {
    bool added = hs_types_array_string_pair_add(array, stringfn_new_empty_string(), stringfn_new_empty_string());
    assert_true(added);
  }
}


void _test_set_data(struct HSArrayDataPair *array)
{
  for (size_t index = 0; index < 5; index++)
  {
    bool added = hs_types_array_data_pair_add(array, stringfn_new_empty_string(), "test");
    assert_true(added);
  }
}


void _test_release_callback(struct HSPostResponseCallback *callback)
{
  hs_io_free(callback->context);
}


void test_impl()
{
  struct HSServeFlowParams *params = hs_types_new_serve_flow_params();

  hs_types_release_serve_flow_params(params);

  params = hs_types_new_serve_flow_params();

  _test_set_cookies(params->request->cookies);
  _test_set_cookies(params->response->cookies);

  _test_set_strings(params->request->headers);
  _test_set_strings(params->response->headers);
  _test_set_strings(params->route_state->string_pairs);
  _test_set_data(params->route_state->data_pairs);

  params->request->domain        = stringfn_new_empty_string();
  params->request->resource      = stringfn_new_empty_string();
  params->request->query_string  = stringfn_new_empty_string();
  params->request->user_agent    = stringfn_new_empty_string();
  params->request->authorization = stringfn_new_empty_string();
  params->request->payload       = hs_types_new_http_request_payload(NULL);

  params->response->content_string = stringfn_new_empty_string();
  params->response->content_file   = stringfn_new_empty_string();

  struct HSPostResponseCallback *callback = hs_types_new_post_response_callback();
  callback->context = stringfn_new_empty_string();
  callback->release = _test_release_callback;
  hs_types_post_response_callbacks_add(params->callbacks, callback);

  params->router_state->base_path = stringfn_new_empty_string();

  hs_types_release_serve_flow_params(params);
}


int main()
{
  test_run(test_impl);
}

