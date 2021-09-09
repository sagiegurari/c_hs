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
  assert_num_equal(hs_types_array_string_pair_count(session->string_pairs), 3);
  assert_string_equal(hs_types_array_string_pair_get_key(session->string_pairs, 0), "test1");
  assert_string_equal(hs_types_array_string_pair_get_value(session->string_pairs, 0), "value1");
  assert_string_equal(hs_types_array_string_pair_get_key(session->string_pairs, 1), "test2");
  assert_string_equal(hs_types_array_string_pair_get_value(session->string_pairs, 1), "value2");
  assert_string_equal(hs_types_array_string_pair_get_key(session->string_pairs, 2), "test3");
  assert_string_equal(hs_types_array_string_pair_get_value(session->string_pairs, 2), "value3");
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
  assert_num_equal(hs_types_array_string_pair_count(session->string_pairs), 3);
  assert_string_equal(hs_types_array_string_pair_get_key(session->string_pairs, 0), "test1");
  assert_string_equal(hs_types_array_string_pair_get_value(session->string_pairs, 0), "value1");
  assert_string_equal(hs_types_array_string_pair_get_key(session->string_pairs, 1), "test2");
  assert_string_equal(hs_types_array_string_pair_get_value(session->string_pairs, 1), "value2");
  assert_string_equal(hs_types_array_string_pair_get_key(session->string_pairs, 2), "test3");
  assert_string_equal(hs_types_array_string_pair_get_value(session->string_pairs, 2), "value3");
  hs_routes_session_release_session(session);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

