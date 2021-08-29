#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSCookies *cookies = hs_types_new_cookies(1);

  for (size_t index = 0; index < cookies->count; index++)
  {
    struct HSCookie *cookie = hs_types_new_cookie();
    cookie->name = stringfn_new_empty_string();
    hs_types_cookies_add(cookies, cookie);
  }

  hs_types_release_cookies(cookies);
}


int main()
{
  test_run(test_impl);
}

