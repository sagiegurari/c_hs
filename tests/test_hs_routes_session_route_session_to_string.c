#include "test.h"
#include <stdlib.h>
#include <string.h>


void test_impl()
{
  struct HSSession *session        = NULL;
  char             *session_string = hs_routes_session_route_session_to_string(session);

  assert_true(session_string == NULL);

  session        = hs_routes_session_new_session();
  session_string = hs_routes_session_route_session_to_string(session);
  assert_string_equal(session_string, "");
  hs_routes_session_release_session(session);
  hs_io_free(session_string);

  session = hs_routes_session_new_session();
  hs_types_array_string_pair_add(session->string_pairs, strdup("test1"), strdup("value1"));
  hs_types_array_string_pair_add(session->string_pairs, strdup("test2"), strdup("value2"));
  hs_types_array_string_pair_add(session->string_pairs, strdup("test3"), strdup("value3"));
  session_string = hs_routes_session_route_session_to_string(session);
  assert_string_equal(session_string, "[session]\n"
                      "test1=value1\n"
                      "test2=value2\n"
                      "test3=value3\n\n");
  hs_routes_session_release_session(session);
  hs_io_free(session_string);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

