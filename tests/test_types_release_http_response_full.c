#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSHttpResponse *response = hs_types_new_http_response();

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

  hs_types_release_http_response(response);
}


int main()
{
  test_run(test_impl);
}
