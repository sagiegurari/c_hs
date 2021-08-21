#include "fsio.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void test_impl()
{
  bool done = hs_io_write_string_to_socket(0, "test");

  assert_true(!done);

  struct StringBuffer *buffer = string_buffer_new();

  for (size_t index = 0; index < 5000; index++)
  {
    string_buffer_append_unsigned_int(buffer, index);
  }
  char *raw = string_buffer_to_string(buffer);
  string_buffer_release(buffer);

  char *filename = "./test_io_write_string_to_socket.txt";

  fsio_create_empty_file(filename);
  int socket = open(filename, O_WRONLY);

  done = hs_io_write_string_to_socket(socket, raw);
  close(socket);
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

