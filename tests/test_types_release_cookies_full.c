#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSCookies *cookies = hs_types_new_cookies();

  cookies->count   = 3;
  cookies->cookies = malloc(sizeof(struct HSCookie) * cookies->count);
  for (size_t index = 0; index < cookies->count; index++)
  {
    cookies->cookies[index]       = hs_types_new_cookie();
    cookies->cookies[index]->name = stringfn_new_empty_string();
  }

  hs_types_release_cookies(cookies);
}


int main()
{
  test_run(test_impl);
}

