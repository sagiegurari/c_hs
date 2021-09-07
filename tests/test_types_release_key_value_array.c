#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSKeyValueArray *key_value_array = hs_types_new_key_value_array(50);

  hs_types_release_key_value_array(key_value_array);

  key_value_array = hs_types_new_key_value_array(50);

  for (size_t index = 0; index < 10; index++)
  {
    hs_types_key_value_array_add(key_value_array, stringfn_new_empty_string(), stringfn_new_empty_string());
  }

  hs_types_release_key_value_array(key_value_array);
}


int main()
{
  test_run(test_impl);
}

