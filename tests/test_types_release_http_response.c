#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSHttpResponse *response = hs_types_http_response_new();

  hs_types_http_response_release(response);

  response = hs_types_http_response_new();

  for (size_t index = 0; index < 3; index++)
  {
    struct HSCookie *cookie = hs_types_cookie_new();
    cookie->name  = stringfn_new_empty_string();
    cookie->value = stringfn_new_empty_string();
    hs_types_cookies_add(response->cookies, cookie);
  }

  for (size_t index = 0; index < 5; index++)
  {
    hs_types_array_string_pair_add(response->headers, stringfn_new_empty_string(), stringfn_new_empty_string());
  }

  response->content_string = stringfn_new_empty_string();
  response->content_file   = stringfn_new_empty_string();

  hs_types_http_response_release(response);
}


int main()
{
  test_run(test_impl);
}

