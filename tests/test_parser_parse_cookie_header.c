#include "test.h"


void test_impl()
{
  struct HSCookies *cookies = hs_types_cookies_new();
  bool             done     = hs_parser_parse_cookie_header(cookies, "name=value");

  assert_true(done);
  assert_num_equal(hs_types_cookies_count(cookies), 1);
  struct HSCookie *cookie = hs_types_cookies_get(cookies, 0);
  assert_string_equal(cookie->name, "name");
  assert_string_equal(cookie->value, "value");
  hs_types_cookies_release(cookies);

  cookies = hs_types_cookies_new();
  done    = hs_parser_parse_cookie_header(cookies, "name=value;");

  assert_true(done);
  assert_num_equal(hs_types_cookies_count(cookies), 1);
  cookie = hs_types_cookies_get(cookies, 0);
  assert_string_equal(cookie->name, "name");
  assert_string_equal(cookie->value, "value");
  hs_types_cookies_release(cookies);

  cookies = hs_types_cookies_new();
  done    = hs_parser_parse_cookie_header(cookies, "name=value; ");

  assert_true(done);
  assert_num_equal(hs_types_cookies_count(cookies), 1);
  cookie = hs_types_cookies_get(cookies, 0);
  assert_string_equal(cookie->name, "name");
  assert_string_equal(cookie->value, "value");
  hs_types_cookies_release(cookies);

  cookies = hs_types_cookies_new();
  done    = hs_parser_parse_cookie_header(cookies, "name=value; name2=value2; name3=value3");

  assert_true(done);
  assert_num_equal(hs_types_cookies_count(cookies), 3);
  cookie = hs_types_cookies_get(cookies, 0);
  assert_string_equal(cookie->name, "name");
  assert_string_equal(cookie->value, "value");
  cookie = hs_types_cookies_get(cookies, 1);
  assert_string_equal(cookie->name, "name2");
  assert_string_equal(cookie->value, "value2");
  cookie = hs_types_cookies_get(cookies, 2);
  assert_string_equal(cookie->name, "name3");
  assert_string_equal(cookie->value, "value3");
  hs_types_cookies_release(cookies);

  cookies = hs_types_cookies_new();
  done    = hs_parser_parse_cookie_header(cookies, "name=value; name2=value2; name3=");

  assert_true(done);
  assert_num_equal(hs_types_cookies_count(cookies), 2);
  cookie = hs_types_cookies_get(cookies, 0);
  assert_string_equal(cookie->name, "name");
  assert_string_equal(cookie->value, "value");
  cookie = hs_types_cookies_get(cookies, 1);
  assert_string_equal(cookie->name, "name2");
  assert_string_equal(cookie->value, "value2");
  hs_types_cookies_release(cookies);

  cookies = hs_types_cookies_new();
  done    = hs_parser_parse_cookie_header(cookies, NULL);
  assert_true(!done);
  hs_types_cookies_release(cookies);

  cookies = hs_types_cookies_new();
  done    = hs_parser_parse_cookie_header(cookies, "name");
  assert_true(!done);
  hs_types_cookies_release(cookies);

  cookies = hs_types_cookies_new();
  done    = hs_parser_parse_cookie_header(cookies, "name=");
  assert_true(!done);
  hs_types_cookies_release(cookies);

  done = hs_parser_parse_cookie_header(NULL, "name=value; name2=value2; name3=");
  assert_true(!done);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

