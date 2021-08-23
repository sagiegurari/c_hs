#include "stringfn.h"
#include "test.h"


void _test_release_callback(struct HSPostResponseCallback *callback)
{
  hs_io_free(callback->context);
}


void test_impl()
{
  struct HSRouteServeResponse *response = hs_route_new_serve_response();

  hs_route_release_serve_response(response);

  response                   = hs_route_new_serve_response();
  response->cookies->count   = 3;
  response->cookies->cookies = malloc(sizeof(struct HSCookie) * response->cookies->count);
  for (size_t index = 0; index < response->cookies->count; index++)
  {
    response->cookies->cookies[index]       = hs_types_new_cookie();
    response->cookies->cookies[index]->name = stringfn_new_empty_string();
  }

  response->headers->count = 5;
  response->headers->pairs = malloc(sizeof(struct HSKeyValue) * response->headers->count);
  for (size_t index = 0; index < response->headers->count; index++)
  {
    response->headers->pairs[index] = hs_types_new_key_value(stringfn_new_empty_string(), stringfn_new_empty_string());
  }

  response->content_string = stringfn_new_empty_string();
  response->content_file   = stringfn_new_empty_string();

  response->callback          = hs_route_new_post_response_callback();
  response->callback->context = stringfn_new_empty_string();
  response->callback->release = _test_release_callback;

  hs_route_release_serve_response(response);
}


int main()
{
  test_run(test_impl);
}

