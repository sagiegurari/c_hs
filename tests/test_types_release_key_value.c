#include "stringfn.h"
#include "test.h"


void test_impl()
{
  struct HSKeyValue *key_value = hs_types_new_key_value(stringfn_new_empty_string(), stringfn_new_empty_string());

  hs_types_release_key_value(key_value);
}


int main()
{
  test_run(test_impl);
}

