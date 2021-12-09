#ifndef __HS_IO_H__
#define __HS_IO_H__

#include "hs_socket.h"
#include "string_buffer.h"
#include <stdbool.h>
#include <stdio.h>

/**
 * This is an internal header and should not be used outside the library.
 */

struct HSIOHttpRequestPayload
{
  struct StringBuffer *partial;
  struct HSSocket     *socket;
};

/**
 * Frees the provided pointer if not NULL.
 */
void hs_io_free(void *);

/**
 * Reads the next line from the socket.
 * In case more is pulled from the socket, it will remain in the provided work buffer.
 * In case no line is read, NULL will be returned.
 */
char *hs_io_read_line(struct HSSocket *, struct StringBuffer *);

/**
 * Creates an internal payload structure to be used to fetch the rest of the payload content.
 */
struct HSIOHttpRequestPayload *hs_io_new_http_request_payload(struct HSSocket *, struct StringBuffer *);

/**
 * Releases the provided payload.
 */
void hs_io_release_http_request_payload(struct HSIOHttpRequestPayload *);

/**
 * Reads the entire content into the provided buffer.
 */
bool hs_io_read_fully(struct HSSocket *, struct StringBuffer *, size_t /* length */);

/**
 * Reads the entire content and writes it to the file.
 */
bool hs_io_read_and_write_to_file(struct HSSocket *, FILE *, size_t /* length */);

/**
 * Writes the entire string to the socket.
 * In case of any error, false will be returned.
 */
bool hs_io_write_string_to_socket(struct HSSocket *, char *, size_t);

/**
 * Writes the entire file content to the socket.
 * In case of any error, false will be returned.
 */
bool hs_io_write_file_to_socket(struct HSSocket *, char *);

#endif

