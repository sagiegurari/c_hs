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
  hashtable_insert(session->data, "test1", "value1", NULL);
  hashtable_insert(session->data, "test2", "value2", NULL);
  hashtable_insert(session->data, "test3", "value3", NULL);
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

