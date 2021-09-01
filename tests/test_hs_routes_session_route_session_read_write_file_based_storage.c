#include "fsio.h"
#include "test.h"


void test_impl()
{
  char *filename = "./sessions/hs_routes_session_route_session_read_from_file_based_storage.txt";

  char *session_string = hs_routes_session_route_session_read_from_file_based_storage(NULL, NULL);

  assert_true(session_string == NULL);

  fsio_write_text_file(filename, "test 123\nsecond line");
  session_string = hs_routes_session_route_session_read_from_file_based_storage("hs_routes_session_route_session_read_from_file_based_storage.txt", NULL);
  assert_string_equal(session_string, "test 123\nsecond line");
  hs_io_free(session_string);

  bool written = hs_routes_session_route_session_write_to_file_based_storage("mysession", "test content", NULL);
  assert_true(written);
  session_string = hs_routes_session_route_session_read_from_file_based_storage("mysession", NULL);
  assert_string_equal(session_string, "test content");
  hs_io_free(session_string);

  fsio_remove("./sessions");
} /* test_impl */


int main()
{
  test_run(test_impl);
}

