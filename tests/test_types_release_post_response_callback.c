#include "stringfn.h"
#include "test.h"


void _test_release_callback(struct HSPostResponseCallback *callback)
{
  hs_io_free(callback->context);
}


void test_impl()
{
  struct HSPostResponseCallback *callback = hs_types_post_response_callback_new();

  hs_types_post_response_callback_release(callback);

  callback          = hs_types_post_response_callback_new();
  callback->context = stringfn_new_empty_string();
  callback->release = _test_release_callback;

  hs_types_post_response_callback_release(callback);
}


int main()
{
  test_run(test_impl);
}

