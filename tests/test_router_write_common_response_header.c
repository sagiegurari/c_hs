#include "fsio.h"
#include "stringfn.h"
#include "test.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void test_impl()
{
  struct HSKeyValueArray *headers = NULL;
  struct HSCookies       *cookies = NULL;
  struct StringBuffer    *buffer  = hs_router_write_common_response_header(HS_HTTP_RESPONSE_CODE_NOT_FOUND, headers, cookies, false);
  char                   *text    = string_buffer_to_string(buffer);

  string_buffer_release(buffer);
  assert_string_equal(text, "HTTP/1.1 404 404\r\nConnection: keep-alive\r\n");
  hs_io_free(text);
  hs_types_release_key_value_array(headers);
  hs_types_release_cookies(cookies);

  headers = hs_types_new_key_value_array();
  cookies = hs_types_new_cookies();
  buffer  = hs_router_write_common_response_header(HS_HTTP_RESPONSE_CODE_OK, headers, cookies, true);
  text    = string_buffer_to_string(buffer);
  string_buffer_release(buffer);
  assert_string_equal(text, "HTTP/1.1 200 200\r\nConnection: close\r\n");
  hs_io_free(text);
  hs_types_release_key_value_array(headers);
  hs_types_release_cookies(cookies);

  headers           = hs_types_new_key_value_array();
  cookies           = hs_types_new_cookies();
  headers->count    = 3;
  headers->pairs    = malloc(sizeof(struct HSKeyValue) * headers->count);
  headers->pairs[0] = hs_types_new_key_value(strdup("header1"), strdup("value1"));
  headers->pairs[1] = hs_types_new_key_value(strdup("header2"), strdup("value2"));
  headers->pairs[2] = hs_types_new_key_value(strdup("header3"), strdup("value3"));
  cookies->count    = 3;
  cookies->cookies  = malloc(sizeof(struct HSCookie) * cookies->count);
  for (size_t index = 0; index < cookies->count; index++)
  {
    cookies->cookies[index] = hs_types_new_cookie();
  }
  cookies->cookies[0]->name      = strdup("c1");
  cookies->cookies[0]->value     = strdup("v1");
  cookies->cookies[0]->expires   = strdup("1 1 1980");
  cookies->cookies[0]->max_age   = 200;
  cookies->cookies[0]->secure    = true;
  cookies->cookies[0]->http_only = true;
  cookies->cookies[0]->domain    = strdup("mydomain");
  cookies->cookies[0]->path      = strdup("/somepath");
  cookies->cookies[0]->same_site = HS_COOKIE_SAME_SITE_NONE;
  cookies->cookies[1]->name      = strdup("c2");
  cookies->cookies[1]->value     = strdup("v2");
  cookies->cookies[1]->same_site = HS_COOKIE_SAME_SITE_STRICT;
  cookies->cookies[2]->name      = strdup("c3");
  cookies->cookies[2]->value     = strdup("v3");
  buffer                         = hs_router_write_common_response_header(HS_HTTP_RESPONSE_CODE_FORBIDDEN, headers, cookies, true);
  text                           = string_buffer_to_string(buffer);
  string_buffer_release(buffer);
  assert_string_equal(text, "HTTP/1.1 403 403\r\n"
                      "header1: value1\r\n"
                      "header2: value2\r\n"
                      "header3: value3\r\n"
                      "Set-Cookie: c1=v1; Expires=1 1 1980; Max-Age=200; Secure; HttpOnly; Domain=mydomain; Path=/somepath; SameSite=None\r\n"
                      "Set-Cookie: c2=v2; SameSite=Strict\r\n"
                      "Set-Cookie: c3=v3; SameSite=Lax\r\n"
                      "Connection: close\r\n");
  hs_io_free(text);
  hs_types_release_key_value_array(headers);
  hs_types_release_cookies(cookies);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

