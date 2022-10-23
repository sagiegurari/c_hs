#ifndef HS_TYPES_ARRAY_H
#define HS_TYPES_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

struct HSArrayStringPair;

struct HSArrayDataPair;

/**
 * Creates and returns a new array struct.
 */
struct HSArrayStringPair *hs_types_array_string_pair_new();

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_array_string_pair_release(struct HSArrayStringPair *);

/**
 * Returns the current count.
 */
size_t hs_types_array_string_pair_count(struct HSArrayStringPair *);

/**
 * Adds the provided pair.
 */
bool hs_types_array_string_pair_add(struct HSArrayStringPair *, char * /* key */, char * /* value */);

/**
 * Returns the key for the given index.
 * If out of bounds, null will be returned.
 */
char *hs_types_array_string_pair_get_key(struct HSArrayStringPair *, size_t);

/**
 * Returns the value for the given index.
 * If out of bounds, null will be returned.
 */
char *hs_types_array_string_pair_get_value(struct HSArrayStringPair *, size_t);

/**
 * Searches the value by key and returns the first one.
 * If not found, null will be returned.
 */
char *hs_types_array_string_pair_get_by_key(struct HSArrayStringPair *, char *);

/**
 * Removes and frees the memory of the requested key.
 */
void hs_types_array_string_pair_remove_by_key(struct HSArrayStringPair *, char *);

/**
 * Creates and returns a new array struct.
 */
struct HSArrayDataPair *hs_types_array_data_pair_new();

/**
 * Frees all memory used by the provided struct, including
 * any internal member/struct.
 */
void hs_types_array_data_pair_release(struct HSArrayDataPair *);

/**
 * Returns the current count.
 */
size_t hs_types_array_data_pair_count(struct HSArrayDataPair *);

/**
 * Adds the provided pair.
 */
bool hs_types_array_data_pair_add(struct HSArrayDataPair *, char * /* key */, void * /* value */);

/**
 * Returns the key for the given index.
 * If out of bounds, null will be returned.
 */
char *hs_types_array_data_pair_get_key(struct HSArrayDataPair *, size_t);

/**
 * Returns the value for the given index.
 * If out of bounds, null will be returned.
 */
void *hs_types_array_data_pair_get_value(struct HSArrayDataPair *, size_t);

/**
 * Searches the value by key and returns the first one.
 * If not found, null will be returned.
 */
void *hs_types_array_data_pair_get_by_key(struct HSArrayDataPair *, char *);

/**
 * Removes and frees the memory of the requested key.
 */
void hs_types_array_data_pair_remove_by_key(struct HSArrayDataPair *, char *);

#endif

