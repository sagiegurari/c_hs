#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSHttpRequest *request = hs_types_new_http_request();

  hs_types_release_http_request(request);

  request = hs_types_new_http_request();

  request->domain        = stringfn_new_empty_string();
  request->resource      = stringfn_new_empty_string();
  request->query_string  = stringfn_new_empty_string();
  request->user_agent    = stringfn_new_empty_string();
  request->authorization = stringfn_new_empty_string();
  request->payload       = hs_types_new_http_request_payload(NULL);

  for (size_t index = 0; index < 3; index++)
  {
    struct HSCookie *cookie = hs_types_cookie_new();
    cookie->name  = stringfn_new_empty_string();
    cookie->value = stringfn_new_empty_string();
    hs_types_cookies_add(request->cookies, cookie);
  }

  for (size_t index = 0; index < 5; index++)
  {
    hs_types_array_string_pair_add(request->headers, stringfn_new_empty_string(), stringfn_new_empty_string());
  }

  hs_types_release_http_request(request);
}


int main()
{
  test_run(test_impl);
}

