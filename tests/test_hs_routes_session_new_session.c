#include "test.h"
#include <stdlib.h>
#include <string.h>


void test_impl()
{
  struct HSSession *session = hs_routes_session_new_session();

  hs_routes_session_release_session(session);

  session = hs_routes_session_new_session();
  hs_types_key_value_array_add(session->string_pairs, strdup("key"), strdup("value"));
  hs_routes_session_release_session(session);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

