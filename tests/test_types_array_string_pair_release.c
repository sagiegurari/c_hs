#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSArrayStringPair *array = hs_types_array_string_pair_new();

  hs_types_array_string_pair_release(array);

  array = hs_types_array_string_pair_new();

  for (size_t index = 0; index < 10; index++)
  {
    hs_types_array_string_pair_add(array, stringfn_new_empty_string(), stringfn_new_empty_string());
  }

  hs_types_array_string_pair_release(array);
}


int main()
{
  test_run(test_impl);
}

