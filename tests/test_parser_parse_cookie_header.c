#include "test.h"


void test_impl()
{
  struct HSCookies *cookies = hs_parser_parse_cookie_header("name=value");

  assert_true(cookies != NULL);
  assert_num_equal(cookies->count, 1);
  assert_string_equal(cookies->cookies[0]->name, "name");
  assert_string_equal(cookies->cookies[0]->value, "value");
  hs_types_release_cookies(cookies);

  cookies = hs_parser_parse_cookie_header("name=value;");

  assert_true(cookies != NULL);
  assert_num_equal(cookies->count, 1);
  assert_string_equal(cookies->cookies[0]->name, "name");
  assert_string_equal(cookies->cookies[0]->value, "value");
  hs_types_release_cookies(cookies);

  cookies = hs_parser_parse_cookie_header("name=value; ");

  assert_true(cookies != NULL);
  assert_num_equal(cookies->count, 1);
  assert_string_equal(cookies->cookies[0]->name, "name");
  assert_string_equal(cookies->cookies[0]->value, "value");
  hs_types_release_cookies(cookies);

  cookies = hs_parser_parse_cookie_header("name=value; name2=value2; name3=value3");

  assert_true(cookies != NULL);
  assert_num_equal(cookies->count, 3);
  assert_string_equal(cookies->cookies[0]->name, "name");
  assert_string_equal(cookies->cookies[0]->value, "value");
  assert_string_equal(cookies->cookies[1]->name, "name2");
  assert_string_equal(cookies->cookies[1]->value, "value2");
  assert_string_equal(cookies->cookies[2]->name, "name3");
  assert_string_equal(cookies->cookies[2]->value, "value3");
  hs_types_release_cookies(cookies);

  cookies = hs_parser_parse_cookie_header("name=value; name2=value2; name3=");

  assert_true(cookies != NULL);
  assert_num_equal(cookies->count, 2);
  assert_string_equal(cookies->cookies[0]->name, "name");
  assert_string_equal(cookies->cookies[0]->value, "value");
  assert_string_equal(cookies->cookies[1]->name, "name2");
  assert_string_equal(cookies->cookies[1]->value, "value2");
  hs_types_release_cookies(cookies);

  cookies = hs_parser_parse_cookie_header(NULL);
  assert_true(cookies == NULL);

  cookies = hs_parser_parse_cookie_header("name");
  assert_true(cookies == NULL);

  cookies = hs_parser_parse_cookie_header("name=");
  assert_true(cookies == NULL);
} /* test_impl */


int main()
{
  test_run(test_impl);
}

