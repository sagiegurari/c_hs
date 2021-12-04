#include "fsio.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void test_impl()
{
  char *filename = "./test_io_write_file_to_socket.txt";

  bool done = hs_io_write_file_to_socket(0, filename);

  assert_true(!done);

  struct HSSocket *hssocket = hs_socket_plain_new(1);
  done = hs_io_write_file_to_socket(hssocket, NULL);
  hs_socket_close_and_release(hssocket);
  assert_true(!done);

  struct StringBuffer *buffer = string_buffer_new();

  for (size_t index = 0; index < 5000; index++)
  {
    string_buffer_append_unsigned_int(buffer, index);
  }
  char *raw = string_buffer_to_string(buffer);
  string_buffer_release(buffer);

  char *input_filename = "./test_io_write_file_to_socket.in.txt";
  fsio_write_text_file(input_filename, raw);

  fsio_create_empty_file(filename);
  int socket = open(filename, O_WRONLY);

  hssocket = hs_socket_plain_new(socket);
  done     = hs_io_write_file_to_socket(hssocket, input_filename);
  hs_socket_close_and_release(hssocket);
  fsio_remove(input_filename);
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

