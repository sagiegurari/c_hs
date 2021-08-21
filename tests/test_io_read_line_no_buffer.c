#include "test.h"


void test_impl()
{
  char *line = hs_io_read_line(1, NULL);

  assert_true(line == NULL);
}


int main()
{
  test_run(test_impl);
}

