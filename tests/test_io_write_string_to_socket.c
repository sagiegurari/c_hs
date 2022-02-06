#include "fsio.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void test_impl()
{
  bool done = hs_io_write_string_to_socket(0, "test", 4);

  assert_true(!done);

  struct StringBuffer *buffer = stringbuffer_new();

  for (size_t index = 0; index < 5000; index++)
  {
    stringbuffer_append_unsigned_int(buffer, index);
  }
  char *raw = stringbuffer_to_string(buffer);
  stringbuffer_release(buffer);

  char *filename = "./test_io_write_string_to_socket.txt";

  fsio_create_empty_file(filename);
  int             socket = open(filename, O_WRONLY);

  struct HSSocket *hssocket = hs_socket_plain_new(socket);
  done = hs_io_write_string_to_socket(hssocket, raw, strlen(raw));
  hs_socket_close_and_release(hssocket);
  assert_true(done);

  char *text = fsio_read_text_file(filename);
  fsio_remove(filename);

  assert_string_equal(text, raw);
  hs_io_free(raw);
  hs_io_free(text);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

