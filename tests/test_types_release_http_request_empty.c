#include "test.h"


void test_impl()
{
  struct HSHttpRequest *request = hs_types_new_http_request();

  hs_types_release_http_request(request);
}


int main()
{
  test_run(test_impl);
}

