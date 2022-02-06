#include "test.h"


void test_impl()
{
  struct StringBuffer *buffer = stringbuffer_new();
  char                *line   = hs_io_read_line(0, buffer);

  assert_true(line == NULL);
  assert_true(stringbuffer_is_empty(buffer));

  stringbuffer_release(buffer);
}


int main()
{
  test_run(test_impl);
}

