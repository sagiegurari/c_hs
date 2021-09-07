#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSCookie *cookie = hs_types_cookie_new();

  hs_types_cookie_release(cookie);

  cookie = hs_types_cookie_new();

  cookie->name    = stringfn_new_empty_string();
  cookie->value   = stringfn_new_empty_string();
  cookie->expires = stringfn_new_empty_string();
  cookie->domain  = stringfn_new_empty_string();
  cookie->path    = stringfn_new_empty_string();

  hs_types_cookie_release(cookie);
}


int main()
{
  test_run(test_impl);
}

