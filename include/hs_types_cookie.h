#ifndef HS_TYPES_COOKIE_H
#define HS_TYPES_COOKIE_H

#include "hs_constants.h"
#include <stdbool.h>
#include <stddef.h>

struct HSCookie
{
  char                  *name;
  char                  *value;
  char                  *expires;
  int                   max_age;
  bool                  secure;
  bool                  http_only;
  char                  *domain;
  char                  *path;
  enum HSCookieSameSite same_site;
};

struct HSCookies;

/**
 * Creates and returns a new cookie struct.
 */
struct HSCookie *hs_types_cookie_new(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_cookie_release(struct HSCookie *);

/**
 * Creates and returns a new cookies struct.
 */
struct HSCookies *hs_types_cookies_new(void);

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_cookies_release(struct HSCookies *);

/**
 * Returns the current count.
 */
size_t hs_types_cookies_count(struct HSCookies *);

/**
 * Adds the cookie to the cookies structure.
 */
bool hs_types_cookies_add(struct HSCookies *, struct HSCookie *);

/**
 * Returns the cookie for the given index.
 * If out of bounds, null will be returned.
 */
struct HSCookie *hs_types_cookies_get(struct HSCookies *, size_t);

/**
 * Searches the cookie by name and returns the first one.
 * If not found, null will be returned.
 */
struct HSCookie *hs_types_cookies_get_by_name(struct HSCookies *, char *);

/**
 * Removes and frees the memory of the requested cookie.
 */
void hs_types_cookies_remove_by_name(struct HSCookies *, char *);

#endif

