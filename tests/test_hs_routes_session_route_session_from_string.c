#include "test.h"
#include <stdlib.h>
#include <string.h>


void test_impl()
{
  struct HSSession *session = NULL;

  hs_routes_session_route_session_from_string(session, "[session]\n"
                                              "test1=value1\n"
                                              "test2=value2\n"
                                              "test3=value3\n\n");

  session = hs_routes_session_new_session();
  hs_routes_session_route_session_from_string(session, "[session]\n"
                                              "test1=value1\n"
                                              "test2=value2\n"
                                              "test3=value3\n\n");
  assert_num_equal(hashtable_size(session->data), 3);
  assert_string_equal(hashtable_get(session->data, "test1"), "value1");
  assert_string_equal(hashtable_get(session->data, "test2"), "value2");
  assert_string_equal(hashtable_get(session->data, "test3"), "value3");
  hs_routes_session_release_session(session);

  session = hs_routes_session_new_session();
  hs_routes_session_route_session_from_string(session, "bad=1\n\n"
                                              "[bad]\n"
                                              "k1=v1\n\n"
                                              "[session]\n"
                                              "test1=value1\n"
                                              "test2=value2\n"
                                              "test3=value3\n\n"
                                              "[bad2]\n"
                                              "k1=v1\n\n"
                                              );
  assert_num_equal(hashtable_size(session->data), 3);
  assert_string_equal(hashtable_get(session->data, "test1"), "value1");
  assert_string_equal(hashtable_get(session->data, "test2"), "value2");
  assert_string_equal(hashtable_get(session->data, "test3"), "value3");
  hs_routes_session_release_session(session);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

