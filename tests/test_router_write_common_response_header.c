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

  headers = hs_types_new_key_value_array(50);
  cookies = hs_types_new_cookies(1);
  buffer  = hs_router_write_common_response_header(HS_HTTP_RESPONSE_CODE_OK, headers, cookies, true);
  text    = string_buffer_to_string(buffer);
  string_buffer_release(buffer);
  assert_string_equal(text, "HTTP/1.1 200 200\r\nConnection: close\r\n");
  hs_io_free(text);
  hs_types_release_key_value_array(headers);
  hs_types_release_cookies(cookies);

  headers = hs_types_new_key_value_array(1);
  cookies = hs_types_new_cookies(1);
  hs_types_key_value_array_add(headers, strdup("header1"), strdup("value1"));
  hs_types_key_value_array_add(headers, strdup("header2"), strdup("value2"));
  hs_types_key_value_array_add(headers, strdup("header3"), strdup("value3"));
  struct HSCookie *cookie = hs_types_new_cookie();
  cookie->name      = strdup("c1");
  cookie->value     = strdup("v1");
  cookie->expires   = strdup("1 1 1980");
  cookie->max_age   = 200;
  cookie->secure    = true;
  cookie->http_only = true;
  cookie->domain    = strdup("mydomain");
  cookie->path      = strdup("/somepath");
  cookie->same_site = HS_COOKIE_SAME_SITE_NONE;
  hs_types_cookies_add(cookies, cookie);
  cookie            = hs_types_new_cookie();
  cookie->name      = strdup("c2");
  cookie->value     = strdup("v2");
  cookie->same_site = HS_COOKIE_SAME_SITE_STRICT;
  hs_types_cookies_add(cookies, cookie);
  cookie        = hs_types_new_cookie();
  cookie->name  = strdup("c3");
  cookie->value = strdup("v3");
  hs_types_cookies_add(cookies, cookie);
  buffer = hs_router_write_common_response_header(HS_HTTP_RESPONSE_CODE_FORBIDDEN, headers, cookies, true);
  text   = string_buffer_to_string(buffer);
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

