#include "test.h"


void test_impl()
{
  struct StringBuffer *buffer = string_buffer_new();
  char                *line   = hs_io_read_line(0, buffer);

  assert_true(line == NULL);
  assert_true(string_buffer_is_empty(buffer));

  string_buffer_release(buffer);
}


int main()
{
  test_run(test_impl);
}

