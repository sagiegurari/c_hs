#ifndef __HS_IO_H__
#define __HS_IO_H__

#include "string_buffer.h"
#include <stdio.h>

struct HSIOHttpRequestPayload
{
  struct StringBuffer *partial;
  int                 socket;
};

/**
 * This is an internal header and should not be used outside the library.
 */

/**
 * Frees the provided pointer if not NULL.
 */
void hs_io_free(void *);

/**
 * Closes the socket.
 */
void hs_io_close(int);

/**
 * Reads the next line from the socket.
 * In case more is pulled from the socket, it will remain in the provided work buffer.
 * In case no line is read, NULL will be returned.
 */
char *hs_io_read_line(int /* socket */, struct StringBuffer *);

/**
 * Creates an internal payload structure to be used to fetch the rest of the payload content.
 */
struct HSIOHttpRequestPayload *hs_io_new_http_request_payload(int /* socket */, struct StringBuffer *);

/**
 * Releases the provided payload.
 */
void hs_io_release_http_request_payload(struct HSIOHttpRequestPayload *);

/**
 * Reads the entire content into the provided buffer.
 */
bool hs_io_read_fully(int /* socket */, struct StringBuffer *);

/**
 * Reads the entire content and writes it to the file.
 */
bool hs_io_read_and_write_to_file(int /* socket */, FILE *);

/**
 * Writes the entire string to the socket.
 * In case of any error, false will be returned.
 */
bool hs_io_write_string_to_socket(int /* socket */, char *);

/**
 * Writes the entire file content to the socket.
 * In case of any error, false will be returned.
 */
bool hs_io_write_file_to_socket(int /* socket */, char *);

#endif

