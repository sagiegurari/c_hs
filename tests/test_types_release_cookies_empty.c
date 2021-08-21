#include "test.h"


void test_impl()
{
  struct HSCookies *cookies = hs_types_new_cookies();

  hs_types_release_cookies(cookies);
}


int main()
{
  test_run(test_impl);
}

