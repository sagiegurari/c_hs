#include "stringfn.h"
#include "test.h"
#include <string.h>


void test_impl()
{
  struct HSCookies *cookies = hs_types_cookies_new();

  hs_types_cookies_release(cookies);

  cookies = hs_types_cookies_new();

  struct HSCookie *cookie = NULL;
  for (size_t index = 0; index < 5; index++)
  {
    cookie        = hs_types_cookie_new();
    cookie->name  = stringfn_new_empty_string();
    cookie->value = stringfn_new_empty_string();
    hs_types_cookies_add(cookies, cookie);
  }

  cookie        = hs_types_cookie_new();
  cookie->name  = strdup("test");
  cookie->value = stringfn_new_empty_string();
  hs_types_cookies_add(cookies, cookie);

  for (size_t index = 0; index < 5; index++)
  {
    cookie        = hs_types_cookie_new();
    cookie->name  = stringfn_new_empty_string();
    cookie->value = stringfn_new_empty_string();
    hs_types_cookies_add(cookies, cookie);
  }

  assert_num_equal(11, hs_types_cookies_count(cookies));
  hs_types_cookies_remove_by_name(cookies, "test");
  assert_num_equal(10, hs_types_cookies_count(cookies));

  hs_types_cookies_release(cookies);
}


int main()
{
  test_run(test_impl);
}

