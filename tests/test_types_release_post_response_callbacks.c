#include "stringfn.h"
#include "test.h"


void _test_release(struct HSPostResponseCallback *callback)
{
  hs_io_free(callback->context);
}


void test_impl()
{
  struct HSPostResponseCallbacks *callbacks = hs_types_post_response_callbacks_new(1);

  for (size_t index = 0; index < 10; index++)
  {
    struct HSPostResponseCallback *callback = hs_types_post_response_callback_new();
    callback->context = stringfn_new_empty_string();
    callback->release = _test_release;
    hs_types_post_response_callbacks_add(callbacks, callback);
  }

  hs_types_post_response_callbacks_release(callbacks);
}


int main()
{
  test_run(test_impl);
}

