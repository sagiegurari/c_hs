#include "stringfn.h"
#include "test.h"


void _test_set_cookies(struct HSCookies *cookies)
{
  cookies->count   = 3;
  cookies->cookies = malloc(sizeof(struct HSCookie) * cookies->count);
  for (size_t index = 0; index < cookies->count; index++)
  {
    cookies->cookies[index]       = hs_types_new_cookie();
    cookies->cookies[index]->name = stringfn_new_empty_string();
  }
}


void _test_set_headers(struct HSKeyValueArray *headers)
{
  headers->count = 5;
  headers->pairs = malloc(sizeof(struct HSKeyValue) * headers->count);
  for (size_t index = 0; index < headers->count; index++)
  {
    headers->pairs[index] = hs_types_new_key_value(stringfn_new_empty_string(), stringfn_new_empty_string());
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

  _test_set_headers(params->request->headers);
  _test_set_headers(params->response->headers);

  params->request->domain        = stringfn_new_empty_string();
  params->request->resource      = stringfn_new_empty_string();
  params->request->query_string  = stringfn_new_empty_string();
  params->request->user_agent    = stringfn_new_empty_string();
  params->request->authorization = stringfn_new_empty_string();
  params->request->payload       = hs_types_new_http_request_payload(NULL);

  params->response->content_string = stringfn_new_empty_string();
  params->response->content_file   = stringfn_new_empty_string();

  params->callback->context = stringfn_new_empty_string();
  params->callback->release = _test_release_callback;

  params->router_state->base_path = stringfn_new_empty_string();

  hs_types_release_serve_flow_params(params);
}


int main()
{
  test_run(test_impl);
}

