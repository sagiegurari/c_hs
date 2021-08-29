#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSHttpRequest *request = hs_types_new_http_request();

  request->domain        = stringfn_new_empty_string();
  request->resource      = stringfn_new_empty_string();
  request->query_string  = stringfn_new_empty_string();
  request->user_agent    = stringfn_new_empty_string();
  request->authorization = stringfn_new_empty_string();
  request->payload       = hs_types_new_http_request_payload(NULL);

  request->cookies->count   = 3;
  request->cookies->cookies = malloc(sizeof(struct HSCookie) * request->cookies->count);
  for (size_t index = 0; index < request->cookies->count; index++)
  {
    request->cookies->cookies[index]       = hs_types_new_cookie();
    request->cookies->cookies[index]->name = stringfn_new_empty_string();
  }

  request->headers->count = 5;
  request->headers->pairs = malloc(sizeof(struct HSKeyValue) * request->headers->count);
  for (size_t index = 0; index < request->headers->count; index++)
  {
    request->headers->pairs[index] = hs_types_new_key_value(stringfn_new_empty_string(), stringfn_new_empty_string());
  }

  hs_types_release_http_request(request);
}


int main()
{
  test_run(test_impl);
}

