#include "hs_io.h"
#include <stdlib.h>
#include <string.h>
#include <stringfn.h>
#include <unistd.h>

#define HS_IO_READ_BUFFER_SIZE    256

static char *_hs_io_read_line_from_buffer(struct StringBuffer *, bool *);


void hs_io_free(void *ptr)
{
  if (ptr == NULL)
  {
    return;
  }

  free(ptr);
}


char *hs_io_read_line(struct HSSocket *socket, struct StringBuffer *work_buffer)
{
  if (!hs_socket_is_open(socket) || work_buffer == NULL)
  {
    return(NULL);
  }

  // it is possible that from previous run, we already have a line in the buffer
  bool eof   = false;
  char *line = _hs_io_read_line_from_buffer(work_buffer, &eof);
  if (eof)
  {
    return(NULL);
  }
  if (line != NULL)
  {
    return(line);
  }

  ssize_t size                           = 0;
  char    buffer[HS_IO_READ_BUFFER_SIZE] = { 0 };
  do
  {
    // read next bulk
    size = hs_socket_read(socket, buffer, HS_IO_READ_BUFFER_SIZE);

    if (size)
    {
      buffer[size] = 0;
      stringbuffer_append_string(work_buffer, buffer);

      // attempt to get next line from buffer
      line = _hs_io_read_line_from_buffer(work_buffer, &eof);
      if (eof)
      {
        // reached end of header and start of payload
        return(NULL);
      }
      if (line != NULL)
      {
        return(line);
      }
    }
  } while (size > 0);

  return(NULL);
} /* hs_io_read_line */

struct HSIOHttpRequestPayload *hs_io_new_http_request_payload(struct HSSocket *socket, struct StringBuffer *buffer)
{
  if (!hs_socket_is_open(socket) || buffer == NULL)
  {
    return(NULL);
  }

  struct HSIOHttpRequestPayload *payload = malloc(sizeof(struct HSIOHttpRequestPayload));
  payload->socket  = socket;
  payload->partial = buffer;

  return(payload);
}


void hs_io_release_http_request_payload(struct HSIOHttpRequestPayload *payload)
{
  if (payload == NULL)
  {
    return;
  }

  stringbuffer_release(payload->partial);
  hs_io_free(payload);
}


bool hs_io_read_fully(struct HSSocket *socket, struct StringBuffer *buffer, size_t length)
{
  if (!hs_socket_is_open(socket) || buffer == NULL)
  {
    return(false);
  }

  if (!length)
  {
    return(true);
  }

  ssize_t size                              = 0;
  char    io_buffer[HS_IO_READ_BUFFER_SIZE] = { 0 };
  size_t  left                              = length;
  do
  {
    // read next bulk
    size_t buffer_size = HS_IO_READ_BUFFER_SIZE;
    if (left < HS_IO_READ_BUFFER_SIZE)
    {
      buffer_size = left;
    }
    size = hs_socket_read(socket, io_buffer, buffer_size);

    if (size > 0)
    {
      left            = left - (size_t)size;
      io_buffer[size] = 0;
      stringbuffer_append_string(buffer, io_buffer);
    }
  } while (size > 0 && left > 0);

  return(!left);
}


bool hs_io_read_and_write_to_file(struct HSSocket *socket, FILE *fp, size_t length)
{
  if (!hs_socket_is_open(socket))
  {
    return(false);
  }

  if (!length)
  {
    return(true);
  }

  ssize_t size                              = 0;
  char    io_buffer[HS_IO_READ_BUFFER_SIZE] = { 0 };
  size_t  left                              = length;
  do
  {
    // read next bulk
    size_t buffer_size = HS_IO_READ_BUFFER_SIZE;
    if (left < HS_IO_READ_BUFFER_SIZE)
    {
      buffer_size = left;
    }
    size = hs_socket_read(socket, io_buffer, buffer_size);

    if (size > 0)
    {
      left = left - (size_t)size;
      size_t written = fwrite(io_buffer, 1, (size_t)size, fp);
      if (written < (size_t)size)
      {
        return(false);
      }
    }
  } while (size > 0 && left > 0);

  return(!left);
}


bool hs_io_write_string_to_socket(struct HSSocket *socket, char *content, size_t length)
{
  if (!hs_socket_is_open(socket) || content == NULL)
  {
    return(false);
  }

  char   *ptr = content;
  size_t left = length;
  if (!left)
  {
    return(true);
  }

  do
  {
    ssize_t written = hs_socket_write(socket, ptr, left);

    // in case of error
    if (written < 0)
    {
      return(false);
    }
    else if (written > 0)
    {
      left = left - (size_t)written;
    }
  } while (left > 0);

  return(true);
}


bool hs_io_write_file_to_socket(struct HSSocket *socket, char *filename)
{
  if (!hs_socket_is_open(socket) || filename == NULL)
  {
    return(false);
  }

  FILE *fp = fopen(filename, "rb");

  if (fp == NULL)
  {
    return(false);
  }

  char buffer[HS_IO_READ_BUFFER_SIZE] = { 0 };
  bool done                           = true;
  do
  {
    if (feof(fp))
    {
      break;
    }

    size_t read = fread(buffer, 1, HS_IO_READ_BUFFER_SIZE - 1, fp);
    if (!read)
    {
      done = false;
      break;
    }
    void hs_io_release_hashtable_key(char *, void *);

    buffer[read] = '\0';

    done = hs_io_write_string_to_socket(socket, buffer, read);
  } while (done);

  if (done)
  {
    done = feof(fp);
  }

  fclose(fp);

  return(done);
} /* hs_io_write_file_to_socket */


void hs_io_release_hashtable_key(char *key, void *data)
{
  hs_io_free(key);

  if (data == NULL)
  {
    // for static check
    return;
  }
}


static char *_hs_io_read_line_from_buffer(struct StringBuffer *buffer, bool *eof)
{
  size_t length = stringbuffer_get_content_size(buffer);

  if (length < 2)
  {
    return(NULL);
  }

  char *content = stringbuffer_to_string(buffer);
  char *line    = NULL;
  for (size_t index = 0; index < length - 1; index++)
  {
    if (content[index] == '\r' && content[index + 1] == '\n')
    {
      if (index)
      {
        line = stringfn_mut_substring(content, 0, index);
      }
      else
      {
        *eof = true;
      }
      stringbuffer_clear(buffer);

      if (length > (index + 1))
      {
        stringbuffer_append_string(buffer, content + index + 2);
      }

      break;
    }
  }

  if (line == NULL)
  {
    hs_io_free(content);
  }

  return(line);
} /* _hs_io_read_line_from_buffer */

