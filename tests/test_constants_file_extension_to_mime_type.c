#include "fsio.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void test_impl()
{
  enum HSMimeType mime_type = hs_constants_file_extension_to_mime_type(NULL);

  assert_num_equal(mime_type, HS_MIME_TYPE_APPLICATION_OCTET_STREAM);

  mime_type = hs_constants_file_extension_to_mime_type("test");
  assert_num_equal(mime_type, HS_MIME_TYPE_APPLICATION_OCTET_STREAM);

  mime_type = hs_constants_file_extension_to_mime_type("test.html");
  assert_num_equal(mime_type, HS_MIME_TYPE_TEXT_HTML);

  mime_type = hs_constants_file_extension_to_mime_type("test.HTML");
  assert_num_equal(mime_type, HS_MIME_TYPE_TEXT_HTML);
}


int main()
{
  test_run(test_impl);
}

