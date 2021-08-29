#include "test.h"


void test_impl()
{
  struct HSHttpResponse *response = hs_types_new_http_response();

  hs_types_release_http_response(response);
}


int main()
{
  test_run(test_impl);
}

