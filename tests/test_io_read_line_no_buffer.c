#include "test.h"


void test_impl()
{
  struct HSSocket *socket = hs_socket_plain_new(1);
  char            *line   = hs_io_read_line(socket, NULL);

  hs_socket_close_and_release(socket);

  assert_true(line == NULL);
}


int main()
{
  test_run(test_impl);
}

