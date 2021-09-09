#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSArrayDataPair *array = hs_types_array_data_pair_new();

  hs_types_array_data_pair_release(array);

  array = hs_types_array_data_pair_new();

  for (size_t index = 0; index < 10; index++)
  {
    hs_types_array_data_pair_add(array, stringfn_new_empty_string(), "test");
  }

  hs_types_array_data_pair_release(array);
}


int main()
{
  test_run(test_impl);
}

