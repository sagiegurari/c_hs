#include "test.h"


void test_impl()
{
  struct HSCookie *cookie = hs_types_new_cookie();

  hs_types_release_cookie(cookie);
}


int main()
{
  test_run(test_impl);
}

