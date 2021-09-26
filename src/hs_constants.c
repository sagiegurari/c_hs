#include "fsio.h"
#include "hs_constants.h"
#include "hs_io.h"
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

  case HS_MIME_TYPE_TEXT_MATHML:
    return("text/mathml");

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

  case HS_MIME_TYPE_IMAGE_X_ICON:
    return("image/x-icon");

  case HS_MIME_TYPE_IMAGE_TIFF:
    return("image/tiff");

  case HS_MIME_TYPE_IMAGE_X_MS_BMP:
    return("image/x-ms-bmp");

  case HS_MIME_TYPE_AUDIO_MIDI:
    return("audio/midi");

  case HS_MIME_TYPE_AUDIO_MPEG:
    return("audio/mpeg");

  case HS_MIME_TYPE_AUDIO_WAV:
    return("audio/wave");

  case HS_MIME_TYPE_AUDIO_WEBM:
    return("audio/webm");

  case HS_MIME_TYPE_AUDIO_OGG:
    return("audio/ogg");

  case HS_MIME_TYPE_AUDIO_X_M4A:
    return("audio/x-m4a");

  case HS_MIME_TYPE_AUDIO_X_REALAUDIO:
    return("audio/x-realaudio");

  case HS_MIME_TYPE_VIDEO_WEBM:
    return("video/webm");

  case HS_MIME_TYPE_VIDEO_3GPP:
    return("video/3gpp");

  case HS_MIME_TYPE_VIDEO_MP2T:
    return("video/mp2t");

  case HS_MIME_TYPE_VIDEO_MP4:
    return("video/mp4");

  case HS_MIME_TYPE_VIDEO_MPEG:
    return("video/mpeg");

  case HS_MIME_TYPE_VIDEO_QUICKTIME:
    return("video/quicktime");

  case HS_MIME_TYPE_VIDEO_X_FLV:
    return("video/x-flv");

  case HS_MIME_TYPE_APPLICATION_OGG:
    return("application/ogg");

  case HS_MIME_TYPE_APPLICATION_FONT_WOFF:
    return("application/font-woff");

  case HS_MIME_TYPE_APPLICATION_ATOM:
    return("application/atom+xml");

  case HS_MIME_TYPE_APPLICATION_RSS:
    return("application/rss+xml");

  case HS_MIME_TYPE_APPLICATION_JSON:
    return("application/json");

  case HS_MIME_TYPE_APPLICATION_JAVA_ARCHIVE:
    return("application/java-archive");

  case HS_MIME_TYPE_APPLICATION_MSWORD:
    return("application/msword");

  case HS_MIME_TYPE_APPLICATION_PDF:
    return("application/pdf");

  case HS_MIME_TYPE_APPLICATION_POSTSCRIPT:
    return("application/postscript");

  case HS_MIME_TYPE_APPLICATION_RTF:
    return("application/rtf");

  case HS_MIME_TYPE_APPLICATION_X_7Z_COMPRESSED:
    return("application/x-7z-compressed");

  case HS_MIME_TYPE_APPLICATION_X_JAVA_JNLP_FILE:
    return("application/x-java-jnlp-file");

  case HS_MIME_TYPE_APPLICATION_X_PERL:
    return("application/x-perl");

  case HS_MIME_TYPE_APPLICATION_X_RAR_COMPRESSED:
    return("application/x-rar-compressed");

  case HS_MIME_TYPE_APPLICATION_X_REDHAT_PACKAGE_MANAGER:
    return("application/x-redhat-package-manager");

  case HS_MIME_TYPE_APPLICATION_X_SHOCKWAVE_FLASH:
    return("application/x-shockwave-flash");

  case HS_MIME_TYPE_APPLICATION_X_TCL:
    return("application/x-tcl");

  case HS_MIME_TYPE_APPLICATION_X_X509_CA_CERT:
    return("application/x-x509-ca-cert");

  case HS_MIME_TYPE_APPLICATION_XHTML_XML:
    return("application/xhtml+xml");

  case HS_MIME_TYPE_APPLICATION_ZIP:
    return("application/zip");
  } /* switch */

  return(NULL);
} /* hs_constants_mime_type_to_string */

enum HSMimeType hs_constants_file_extension_to_mime_type(char *path)
{
  if (path == NULL)
  {
    return(HS_MIME_TYPE_APPLICATION_OCTET_STREAM);
  }

  char *extension = fsio_file_extension(path);
  if (extension == NULL)
  {
    return(HS_MIME_TYPE_APPLICATION_OCTET_STREAM);
  }

  stringfn_mut_to_lowercase(extension);

  if (  stringfn_equal(extension, ".html")
     || stringfn_equal(extension, ".htm")
     || stringfn_equal(extension, ".shtml"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_TEXT_HTML);
  }

  if (  stringfn_equal(extension, ".txt")
     || stringfn_equal(extension, ".md")
     || stringfn_equal(extension, ".c")
     || stringfn_equal(extension, ".h")
     || stringfn_equal(extension, ".cpp")
     || stringfn_equal(extension, ".java")
     || stringfn_equal(extension, ".toml")
     || stringfn_equal(extension, ".cfg")
     || stringfn_equal(extension, ".sh")
     || stringfn_equal(extension, ".bat")
     || stringfn_equal(extension, ".com")
     || stringfn_equal(extension, ".rust")
     || stringfn_equal(extension, ".editorconfig")
     || stringfn_equal(extension, ".gitignore")
     || stringfn_equal(extension, ".gitattributes")
     || stringfn_equal(extension, ".R"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_TEXT_PLAIN);
  }

  if (stringfn_equal(extension, ".css"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_TEXT_CSS);
  }

  if (stringfn_equal(extension, ".xml"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_TEXT_XML);
  }

  if (stringfn_equal(extension, ".js"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_TEXT_JAVASCRIPT);
  }

  if (stringfn_equal(extension, ".mml"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_TEXT_MATHML);
  }

  if (stringfn_equal(extension, ".gif"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_IMAGE_GIF);
  }

  if (  stringfn_equal(extension, ".jpeg")
     || stringfn_equal(extension, ".jpg"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_IMAGE_JPEG);
  }

  if (stringfn_equal(extension, ".png"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_IMAGE_PNG);
  }

  if (  stringfn_equal(extension, ".svg")
     || stringfn_equal(extension, ".sngz"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_IMAGE_SVG);
  }

  if (stringfn_equal(extension, ".webp"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_IMAGE_WEBP);
  }

  if (stringfn_equal(extension, ".ico"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_IMAGE_X_ICON);
  }

  if (  stringfn_equal(extension, ".tif")
     || stringfn_equal(extension, ".tiff"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_IMAGE_TIFF);
  }

  if (stringfn_equal(extension, ".bmp"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_IMAGE_X_MS_BMP);
  }

  if (  stringfn_equal(extension, ".mid")
     || stringfn_equal(extension, ".midi")
     || stringfn_equal(extension, ".kar"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_AUDIO_MIDI);
  }

  if (stringfn_equal(extension, ".mp3"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_AUDIO_MPEG);
  }

  if (stringfn_equal(extension, ".wav"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_AUDIO_WAV);
  }

  if (stringfn_equal(extension, ".ogg"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_AUDIO_OGG);
  }

  if (stringfn_equal(extension, ".m4a"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_AUDIO_X_M4A);
  }

  if (stringfn_equal(extension, ".ra"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_AUDIO_X_REALAUDIO);
  }

  if (stringfn_equal(extension, ".webm"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_VIDEO_WEBM);
  }

  if (  stringfn_equal(extension, ".3gpp")
     || stringfn_equal(extension, ".3gp"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_VIDEO_3GPP);
  }

  if (stringfn_equal(extension, ".ts"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_VIDEO_MP2T);
  }

  if (stringfn_equal(extension, ".mp4"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_VIDEO_MP4);
  }

  if (  stringfn_equal(extension, ".mpeg")
     || stringfn_equal(extension, ".mpg"))

  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_VIDEO_MPEG);
  }

  if (  stringfn_equal(extension, ".mpeg")
     || stringfn_equal(extension, ".mpg"))

  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_VIDEO_QUICKTIME);
  }

  if (stringfn_equal(extension, ".mov"))

  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_VIDEO_X_FLV);
  }

  if (stringfn_equal(extension, ".flv"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_VIDEO_X_FLV);
  }

  if (stringfn_equal(extension, ".woff"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_FONT_WOFF);
  }

  if (stringfn_equal(extension, ".atom"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_ATOM);
  }

  if (stringfn_equal(extension, ".rss"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_RSS);
  }

  if (stringfn_equal(extension, ".json"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_JSON);
  }

  if (  stringfn_equal(extension, ".jar")
     || stringfn_equal(extension, ".war")
     || stringfn_equal(extension, ".ear"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_JAVA_ARCHIVE);
  }

  if (stringfn_equal(extension, ".doc"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_MSWORD);
  }

  if (stringfn_equal(extension, ".pdf"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_PDF);
  }

  if (  stringfn_equal(extension, ".ps")
     || stringfn_equal(extension, ".eps")
     || stringfn_equal(extension, ".ai"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_POSTSCRIPT);
  }

  if (stringfn_equal(extension, ".rtf"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_RTF);
  }

  if (stringfn_equal(extension, ".7z"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_X_7Z_COMPRESSED);
  }

  if (stringfn_equal(extension, ".jnlp"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_X_JAVA_JNLP_FILE);
  }

  if (  stringfn_equal(extension, ".pl")
     || stringfn_equal(extension, ".pm"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_X_PERL);
  }

  if (stringfn_equal(extension, ".rar"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_X_RAR_COMPRESSED);
  }

  if (stringfn_equal(extension, ".rpm"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_X_REDHAT_PACKAGE_MANAGER);
  }

  if (stringfn_equal(extension, ".swf"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_X_SHOCKWAVE_FLASH);
  }

  if (  stringfn_equal(extension, ".tcl")
     || stringfn_equal(extension, ".tk"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_X_TCL);
  }

  if (  stringfn_equal(extension, ".cert")
     || stringfn_equal(extension, ".der")
     || stringfn_equal(extension, ".pem"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_X_X509_CA_CERT);
  }

  if (stringfn_equal(extension, ".xhtml"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_XHTML_XML);
  }

  if (stringfn_equal(extension, ".zip"))
  {
    hs_io_free(extension);
    return(HS_MIME_TYPE_APPLICATION_ZIP);
  }

  hs_io_free(extension);
  return(HS_MIME_TYPE_APPLICATION_OCTET_STREAM);
}   /* hs_constants_file_extension_to_mime_type */

