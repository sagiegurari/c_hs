#include "hs_constants.h"
#include "stringfn.h"
#include <stddef.h>


const char *hs_constants_mime_type_to_string(enum HSMimeType mime_type)
{
  switch (mime_type)
  {
  case HS_MIME_TYPE_NONE:
    return(NULL);

  case HS_MIME_TYPE_APPLICATION_OCTET_STREAM:
    return("application/octet-stream");

  case HS_MIME_TYPE_TEXT_HTML:
    return("text/html");

  case HS_MIME_TYPE_TEXT_PLAIN:
    return("text/plain");

  case HS_MIME_TYPE_TEXT_CSS:
    return("text/css");

  case HS_MIME_TYPE_TEXT_XML:
    return("text/xml");

  case HS_MIME_TYPE_TEXT_JAVASCRIPT:
    return("text/javascript");

  case HS_MIME_TYPE_IMAGE_APNG:
    return("image/apng");

  case HS_MIME_TYPE_IMAGE_AVIF:
    return("image/avif");

  case HS_MIME_TYPE_IMAGE_GIF:
    return("image/gif");

  case HS_MIME_TYPE_IMAGE_JPEG:
    return("image/jpeg");

  case HS_MIME_TYPE_IMAGE_PNG:
    return("image/png");

  case HS_MIME_TYPE_IMAGE_SVG:
    return("image/svg+xml");

  case HS_MIME_TYPE_IMAGE_WEBP:
    return("image/webp");

  case HS_MIME_TYPE_AUDIO_WAV:
    return("audio/wave");

  case HS_MIME_TYPE_AUDIO_WEBM:
    return("audio/webm");

  case HS_MIME_TYPE_VIDEO_WEBM:
    return("video/webm");

  case HS_MIME_TYPE_AUDIO_OGG:
    return("audio/ogg");

  case HS_MIME_TYPE_VIDEO_OGG:
    return("video/ogg");

  case HS_MIME_TYPE_APPLICATION_OGG:
    return("application/ogg");

  case HS_MIME_TYPE_APPLICATION_ATOM:
    return("application/atom+xml");

  case HS_MIME_TYPE_APPLICATION_RSS:
    return("application/rss+xml");
  } /* switch */

  return(NULL);
} /* hs_constants_mime_type_to_string */

enum HSMimeType hs_constants_file_extension_to_mime_type(char *path)
{
  if (path == NULL)
  {
    return(HS_MIME_TYPE_APPLICATION_OCTET_STREAM);
  }

  if (  stringfn_ends_with(path, ".html")
     || stringfn_ends_with(path, ".htm")
     || stringfn_ends_with(path, ".shtml"))
  {
    return(HS_MIME_TYPE_TEXT_HTML);
  }

  if (stringfn_ends_with(path, ".css"))
  {
    return(HS_MIME_TYPE_TEXT_CSS);
  }

  if (  stringfn_ends_with(path, ".txt")
     || stringfn_ends_with(path, ".md")
     || stringfn_ends_with(path, ".c")
     || stringfn_ends_with(path, ".h")
     || stringfn_ends_with(path, ".cpp")
     || stringfn_ends_with(path, ".java")
     || stringfn_ends_with(path, ".toml")
     || stringfn_ends_with(path, ".cfg")
     || stringfn_ends_with(path, ".sh")
     || stringfn_ends_with(path, ".bat")
     || stringfn_ends_with(path, ".com")
     || stringfn_ends_with(path, ".rust")
     || stringfn_ends_with(path, ".editorconfig")
     || stringfn_ends_with(path, ".gitignore")
     || stringfn_ends_with(path, ".gitattributes")
     || stringfn_ends_with(path, ".R"))
  {
    return(HS_MIME_TYPE_TEXT_PLAIN);
  }

  if (stringfn_ends_with(path, ".gif"))
  {
    return(HS_MIME_TYPE_IMAGE_GIF);
  }

  if (stringfn_ends_with(path, ".xml"))
  {
    return(HS_MIME_TYPE_TEXT_XML);
  }

  if (  stringfn_ends_with(path, ".jpeg")
     || stringfn_ends_with(path, ".jpg"))
  {
    return(HS_MIME_TYPE_IMAGE_JPEG);
  }

  if (stringfn_ends_with(path, ".png"))
  {
    return(HS_MIME_TYPE_IMAGE_PNG);
  }

  if (stringfn_ends_with(path, ".js"))
  {
    return(HS_MIME_TYPE_TEXT_JAVASCRIPT);
  }

  if (stringfn_ends_with(path, ".atom"))
  {
    return(HS_MIME_TYPE_APPLICATION_ATOM);
  }

  if (stringfn_ends_with(path, ".rss"))
  {
    return(HS_MIME_TYPE_APPLICATION_RSS);
  }

  if (  stringfn_ends_with(path, ".svg")
     || stringfn_ends_with(path, ".sngz"))
  {
    return(HS_MIME_TYPE_IMAGE_SVG);
  }

  if (stringfn_ends_with(path, ".webp"))
  {
    return(HS_MIME_TYPE_IMAGE_WEBP);
  }

  if (stringfn_ends_with(path, ".webm"))
  {
    return(HS_MIME_TYPE_VIDEO_WEBM);
  }

  if (stringfn_ends_with(path, ".wav"))
  {
    return(HS_MIME_TYPE_AUDIO_WAV);
  }

  if (stringfn_ends_with(path, ".ogg"))
  {
    return(HS_MIME_TYPE_AUDIO_OGG);
  }

  return(HS_MIME_TYPE_APPLICATION_OCTET_STREAM);
} /* hs_constants_file_extension_to_mime_type */

