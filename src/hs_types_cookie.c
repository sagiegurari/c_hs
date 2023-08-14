#include "hs_io.h"
#include "hs_types.h"
#include "stringfn.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

struct HSCookies
{
  struct Vector *vector;
};

struct HSCookie *hs_types_cookie_new(void)
{
  struct HSCookie *cookie = malloc(sizeof(struct HSCookie));

  cookie->name      = NULL;
  cookie->value     = NULL;
  cookie->expires   = NULL;
  cookie->max_age   = -1;
  cookie->secure    = false;
  cookie->http_only = false;
  cookie->domain    = NULL;
  cookie->path      = NULL;
  cookie->same_site = HS_COOKIE_SAME_SITE_LAX;

  return(cookie);
}


void hs_types_cookie_release(struct HSCookie *cookie)
{
  if (cookie == NULL)
  {
    return;
  }

  hs_io_free(cookie->name);
  hs_io_free(cookie->value);
  hs_io_free(cookie->expires);
  hs_io_free(cookie->domain);
  hs_io_free(cookie->path);

  hs_io_free(cookie);
}

struct HSCookies *hs_types_cookies_new(void)
{
  struct HSCookies *cookies = malloc(sizeof(struct HSCookies));

  cookies->vector = vector_new();

  return(cookies);
}


void hs_types_cookies_release(struct HSCookies *cookies)
{
  if (cookies == NULL)
  {
    return;
  }

  size_t count = vector_size(cookies->vector);
  for (size_t index = 0; index < count; index++)
  {
    struct HSCookie *cookie = (struct HSCookie *)vector_get(cookies->vector, index);
    hs_types_cookie_release(cookie);
  }
  vector_release(cookies->vector);

  hs_io_free(cookies);
}


size_t hs_types_cookies_count(struct HSCookies *cookies)
{
  if (cookies == NULL)
  {
    return(0);
  }

  return(vector_size(cookies->vector));
}


bool hs_types_cookies_add(struct HSCookies *cookies, struct HSCookie *cookie)
{
  if (cookies == NULL || cookie == NULL || cookie->name == NULL || cookie->value == NULL)
  {
    return(false);
  }

  vector_push(cookies->vector, cookie);

  return(true);
}

struct HSCookie *hs_types_cookies_get(struct HSCookies *cookies, size_t index)
{
  if (cookies == NULL)
  {
    return(NULL);
  }

  return((struct HSCookie *)vector_get(cookies->vector, index));
}

struct HSCookie *hs_types_cookies_get_by_name(struct HSCookies *cookies, char *name)
{
  if (cookies == NULL || name == NULL)
  {
    return(NULL);
  }

  size_t count = vector_size(cookies->vector);
  for (size_t index = 0; index < count; index++)
  {
    struct HSCookie *cookie = (struct HSCookie *)vector_get(cookies->vector, index);
    if (stringfn_equal(cookie->name, name))
    {
      return(cookie);
    }
  }

  return(NULL);
}


void hs_types_cookies_remove_by_name(struct HSCookies *cookies, char *name)
{
  if (cookies == NULL || name == NULL)
  {
    return;
  }

  size_t count = vector_size(cookies->vector);
  if (!count)
  {
    return;
  }

  for (size_t index = count - 1; ; index--)
  {
    struct HSCookie *cookie = (struct HSCookie *)vector_get(cookies->vector, index);
    if (stringfn_equal(cookie->name, name))
    {
      hs_types_cookie_release(cookie);
      vector_remove(cookies->vector, index);
    }

    if (!index)
    {
      break;
    }
  }
}

