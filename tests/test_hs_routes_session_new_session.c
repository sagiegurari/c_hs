#include "hs_external_libs.h"
#include "test.h"
#include <stdlib.h>
#include <string.h>


void test_impl()
{
  struct HSSession *session = hs_routes_session_new_session();

  hs_routes_session_release_session(session);

  session = hs_routes_session_new_session();
  hashtable_insert(session->data, "key", "value", NULL);
  hs_routes_session_release_session(session);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

