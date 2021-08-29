#include "test.h"


void test_impl()
{
  struct HSKeyValueArray *key_value_array = hs_types_new_key_value_array(50);

  hs_types_release_key_value_array(key_value_array);
}


int main()
{
  test_run(test_impl);
}

