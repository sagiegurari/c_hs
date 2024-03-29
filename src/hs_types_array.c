#include "hs_io.h"
#include "hs_types_array.h"
#include "stringfn.h"
#include "vector.h"
#include <stdlib.h>

struct HSArrayPair
{
  char *key;
  void *value;
};

struct HSArrayStringPair
{
  struct Vector *vector;
};

static void _hs_types_array_pair_release(struct HSArrayPair *, bool);
static void _hs_types_array_release(struct Vector *, bool);
static bool _hs_types_array_pair_add(struct Vector *, char *, void *);
static char *_hs_types_array_pair_get_key(struct Vector *, size_t);
static void *_hs_types_array_pair_get_value(struct Vector *, size_t);
static void *_hs_types_array_pair_get_by_key(struct Vector *, char *);
static void _hs_types_array_pair_remove_by_key(struct Vector *, char *, bool);

struct HSArrayStringPair *hs_types_array_string_pair_new(void)
{
  struct HSArrayStringPair *array = malloc(sizeof(struct HSArrayStringPair));

  array->vector = vector_new();

  return(array);
}


void hs_types_array_string_pair_release(struct HSArrayStringPair *array)
{
  if (array == NULL)
  {
    return;
  }

  _hs_types_array_release(array->vector, true);

  hs_io_free(array);
}


size_t hs_types_array_string_pair_count(struct HSArrayStringPair *array)
{
  if (array == NULL)
  {
    return(0);
  }

  return(vector_size(array->vector));
}


bool hs_types_array_string_pair_add(struct HSArrayStringPair *array, char *key, char *value)
{
  if (array == NULL)
  {
    return(false);
  }

  return(_hs_types_array_pair_add(array->vector, key, value));
}


char *hs_types_array_string_pair_get_key(struct HSArrayStringPair *array, size_t index)
{
  if (array == NULL)
  {
    return(NULL);
  }

  return(_hs_types_array_pair_get_key(array->vector, index));
}


char *hs_types_array_string_pair_get_value(struct HSArrayStringPair *array, size_t index)
{
  if (array == NULL)
  {
    return(NULL);
  }

  return(_hs_types_array_pair_get_value(array->vector, index));
}


char *hs_types_array_string_pair_get_by_key(struct HSArrayStringPair *array, char *key)
{
  if (array == NULL)
  {
    return(NULL);
  }

  return(_hs_types_array_pair_get_by_key(array->vector, key));
}


void hs_types_array_string_pair_remove_by_key(struct HSArrayStringPair *array, char *key)
{
  if (array == NULL)
  {
    return;
  }

  _hs_types_array_pair_remove_by_key(array->vector, key, true);
}


static void _hs_types_array_pair_release(struct HSArrayPair *pair, bool release_value)
{
  if (pair == NULL)
  {
    return;
  }

  hs_io_free(pair->key);
  if (release_value)
  {
    hs_io_free(pair->value);
  }
  hs_io_free(pair);
}


static void _hs_types_array_release(struct Vector *vector, bool release_value)
{
  if (vector == NULL)
  {
    return;
  }

  size_t count = vector_size(vector);
  for (size_t index = 0; index < count; index++)
  {
    struct HSArrayPair *pair = (struct HSArrayPair *)vector_get(vector, index);
    _hs_types_array_pair_release(pair, release_value);
  }

  vector_release(vector);
}


static bool _hs_types_array_pair_add(struct Vector *vector, char *key, void *value)
{
  if (vector == NULL || key == NULL)
  {
    return(false);
  }

  struct HSArrayPair *pair = malloc(sizeof(struct HSArrayPair));
  pair->key   = key;
  pair->value = value;

  vector_push(vector, pair);

  return(true);
}


static char *_hs_types_array_pair_get_key(struct Vector *vector, size_t index)
{
  if (vector == NULL)
  {
    return(NULL);
  }

  struct HSArrayPair *pair = (struct HSArrayPair *)vector_get(vector, index);
  if (pair == NULL)
  {
    return(NULL);
  }

  return(pair->key);
}


static void *_hs_types_array_pair_get_value(struct Vector *vector, size_t index)
{
  if (vector == NULL)
  {
    return(NULL);
  }

  struct HSArrayPair *pair = (struct HSArrayPair *)vector_get(vector, index);
  if (pair == NULL)
  {
    return(NULL);
  }

  return(pair->value);
}


static void *_hs_types_array_pair_get_by_key(struct Vector *vector, char *key)
{
  if (vector == NULL || key == NULL)
  {
    return(NULL);
  }

  size_t count = vector_size(vector);
  for (size_t index = 0; index < count; index++)
  {
    struct HSArrayPair *pair = (struct HSArrayPair *)vector_get(vector, index);
    if (pair != NULL && stringfn_equal(pair->key, key))
    {
      return(pair->value);
    }
  }

  return(NULL);
}


static void _hs_types_array_pair_remove_by_key(struct Vector *vector, char *key, bool release_value)
{
  if (vector == NULL || key == NULL)
  {
    return;
  }

  size_t count = vector_size(vector);
  if (!count)
  {
    return;
  }

  for (size_t index = count - 1; ; index--)
  {
    struct HSArrayPair *pair = (struct HSArrayPair *)vector_get(vector, index);
    if (pair != NULL && stringfn_equal(pair->key, key))
    {
      _hs_types_array_pair_release(pair, release_value);
      vector_remove(vector, index);
    }

    if (!index)
    {
      break;
    }
  }
}

