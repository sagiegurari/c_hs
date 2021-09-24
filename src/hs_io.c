#include "hs_io.h"
#include <stdlib.h>
#include <string.h>
#include <stringfn.h>
#include <unistd.h>

#define HS_IO_READ_BUFFER_SIZE    256

char *_hs_io_read_line_from_buffer(struct StringBuffer *, bool *);


void hs_io_free(void *ptr)
{
  if (ptr == NULL)
  {
    return;
  }

  free(ptr);
}


void hs_io_close(int socket)
{
  if (!socket)
  {
    return;
  }

  close(socket);
}


char *hs_io_read_line(int socket, struct StringBuffer *work_buffer)
{
  if (!socket || work_buffer == NULL)
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
    size = read(socket, buffer, HS_IO_READ_BUFFER_SIZE);

    if (size)
    {
      buffer[size] = 0;
      string_buffer_append_string(work_buffer, buffer);

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

struct HSIOHttpRequestPayload *hs_io_new_http_request_payload(int socket, struct StringBuffer *buffer)
{
  if (!socket || buffer == NULL)
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

  string_buffer_release(payload->partial);
  hs_io_free(payload);
}


bool hs_io_read_fully(int socket, struct StringBuffer *buffer, size_t length)
{
  if (!socket || buffer == NULL)
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
    size = read(socket, io_buffer, buffer_size);

    if (size > 0)
    {
      left            = left - (size_t)size;
      io_buffer[size] = 0;
      string_buffer_append_string(buffer, io_buffer);
    }
  } while (size > 0 && left > 0);

  return(!left);
}


bool hs_io_read_and_write_to_file(int socket, FILE *fp, size_t length)
{
  if (!socket)
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
    size = read(socket, io_buffer, buffer_size);

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


bool hs_io_write_string_to_socket(int socket, char *content, size_t length)
{
  if (!socket || content == NULL)
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
    ssize_t written = write(socket, ptr, left);

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


bool hs_io_write_file_to_socket(int socket, char *filename)
{
  if (!socket || filename == NULL)
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
    if (read <= 0)
    {
      done = false;
      break;
    }

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


char *_hs_io_read_line_from_buffer(struct StringBuffer *buffer, bool *eof)
{
  size_t length = string_buffer_get_content_size(buffer);

  if (length < 2)
  {
    return(NULL);
  }

  char *content = string_buffer_to_string(buffer);
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
      string_buffer_clear(buffer);

      if (length > (index + 1))
      {
        string_buffer_append_string(buffer, content + index + 2);
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

