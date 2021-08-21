#include "fsio.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void test_impl()
{
  char *raw = "POST /testpost HTTP/1.0\r\n"
              "Host: MyHost\r\n"
              "Connection: close\r\n"
              "Content-Length: 11\r\n"
              "Content-Type: application/x-www-form-urlencoded\r\n"
              "\r\n"
              "12345\n67890\n\n";

  char *filename = "./test_io_read_line.txt";

  fsio_write_text_file(filename, raw);

  int                 socket = open(filename, O_RDONLY);

  struct StringBuffer *buffer = string_buffer_new();

  char                *line = hs_io_read_line(socket, buffer);
  assert_string_equal(line, "POST /testpost HTTP/1.0");
  hs_io_free(line);

  line = hs_io_read_line(socket, buffer);
  assert_string_equal(line, "Host: MyHost");
  hs_io_free(line);

  line = hs_io_read_line(socket, buffer);
  assert_string_equal(line, "Connection: close");
  hs_io_free(line);

  line = hs_io_read_line(socket, buffer);
  assert_string_equal(line, "Content-Length: 11");
  hs_io_free(line);

  line = hs_io_read_line(socket, buffer);
  assert_string_equal(line, "Content-Type: application/x-www-form-urlencoded");
  hs_io_free(line);

  line = hs_io_read_line(socket, buffer);
  assert_true(line == NULL);

  close(socket);
  fsio_remove(filename);
  string_buffer_release(buffer);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

