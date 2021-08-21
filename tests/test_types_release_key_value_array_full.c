#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSKeyValueArray *key_value_array = hs_types_new_key_value_array();

  key_value_array->count = 5;
  key_value_array->pairs = malloc(sizeof(struct HSKeyValue) * key_value_array->count);
  for (size_t index = 0; index < key_value_array->count; index++)
  {
    key_value_array->pairs[index] = hs_types_new_key_value(stringfn_new_empty_string(), stringfn_new_empty_string());
  }

  hs_types_release_key_value_array(key_value_array);
}


int main()
{
  test_run(test_impl);
}

