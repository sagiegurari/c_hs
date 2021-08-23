#include "stringfn.h"
#include "test.h"


void _test_release_callback(struct HSPostResponseCallback *callback)
{
  hs_io_free(callback->context);
}


void test_impl()
{
  struct HSPostResponseCallback *callback = hs_route_new_post_response_callback();

  hs_route_release_post_response_callback(callback);

  callback          = hs_route_new_post_response_callback();
  callback->context = stringfn_new_empty_string();
  callback->release = _test_release_callback;

  hs_route_release_post_response_callback(callback);
}


int main()
{
  test_run(test_impl);
}

