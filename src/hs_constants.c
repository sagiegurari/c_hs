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

  if (  stringfn_ends_with(path, ".html")
     || stringfn_ends_with(path, ".htm")
     || stringfn_ends_with(path, ".shtml"))
  {
    return(HS_MIME_TYPE_TEXT_HTML);
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

  if (stringfn_ends_with(path, ".css"))
  {
    return(HS_MIME_TYPE_TEXT_CSS);
  }

  if (stringfn_ends_with(path, ".xml"))
  {
    return(HS_MIME_TYPE_TEXT_XML);
  }

  if (stringfn_ends_with(path, ".js"))
  {
    return(HS_MIME_TYPE_TEXT_JAVASCRIPT);
  }

  if (stringfn_ends_with(path, ".mml"))
  {
    return(HS_MIME_TYPE_TEXT_MATHML);
  }

  if (stringfn_ends_with(path, ".gif"))
  {
    return(HS_MIME_TYPE_IMAGE_GIF);
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

  if (  stringfn_ends_with(path, ".svg")
     || stringfn_ends_with(path, ".sngz"))
  {
    return(HS_MIME_TYPE_IMAGE_SVG);
  }

  if (stringfn_ends_with(path, ".webp"))
  {
    return(HS_MIME_TYPE_IMAGE_WEBP);
  }

  if (stringfn_ends_with(path, ".ico"))
  {
    return(HS_MIME_TYPE_IMAGE_X_ICON);
  }

  if (  stringfn_ends_with(path, ".tif")
     || stringfn_ends_with(path, ".tiff"))
  {
    return(HS_MIME_TYPE_IMAGE_TIFF);
  }

  if (stringfn_ends_with(path, ".bmp"))
  {
    return(HS_MIME_TYPE_IMAGE_X_MS_BMP);
  }

  if (  stringfn_ends_with(path, ".mid")
     || stringfn_ends_with(path, ".midi")
     || stringfn_ends_with(path, ".kar"))
  {
    return(HS_MIME_TYPE_AUDIO_MIDI);
  }

  if (stringfn_ends_with(path, ".mp3"))
  {
    return(HS_MIME_TYPE_AUDIO_MPEG);
  }

  if (stringfn_ends_with(path, ".wav"))
  {
    return(HS_MIME_TYPE_AUDIO_WAV);
  }

  if (stringfn_ends_with(path, ".ogg"))
  {
    return(HS_MIME_TYPE_AUDIO_OGG);
  }

  if (stringfn_ends_with(path, ".m4a"))
  {
    return(HS_MIME_TYPE_AUDIO_X_M4A);
  }

  if (stringfn_ends_with(path, ".ra"))
  {
    return(HS_MIME_TYPE_AUDIO_X_REALAUDIO);
  }

  if (stringfn_ends_with(path, ".webm"))
  {
    return(HS_MIME_TYPE_VIDEO_WEBM);
  }

  if (  stringfn_ends_with(path, ".3gpp")
     || stringfn_ends_with(path, ".3gp"))
  {
    return(HS_MIME_TYPE_VIDEO_3GPP);
  }

  if (stringfn_ends_with(path, ".ts"))
  {
    return(HS_MIME_TYPE_VIDEO_MP2T);
  }

  if (stringfn_ends_with(path, ".mp4"))
  {
    return(HS_MIME_TYPE_VIDEO_MP4);
  }

  if (  stringfn_ends_with(path, ".mpeg")
     || stringfn_ends_with(path, ".mpg"))

  {
    return(HS_MIME_TYPE_VIDEO_MPEG);
  }

  if (  stringfn_ends_with(path, ".mpeg")
     || stringfn_ends_with(path, ".mpg"))

  {
    return(HS_MIME_TYPE_VIDEO_QUICKTIME);
  }

  if (stringfn_ends_with(path, ".mov"))

  {
    return(HS_MIME_TYPE_VIDEO_X_FLV);
  }

  if (stringfn_ends_with(path, ".flv"))
  {
    return(HS_MIME_TYPE_VIDEO_X_FLV);
  }

  if (stringfn_ends_with(path, ".woff"))
  {
    return(HS_MIME_TYPE_APPLICATION_FONT_WOFF);
  }

  if (stringfn_ends_with(path, ".atom"))
  {
    return(HS_MIME_TYPE_APPLICATION_ATOM);
  }

  if (stringfn_ends_with(path, ".rss"))
  {
    return(HS_MIME_TYPE_APPLICATION_RSS);
  }

  if (stringfn_ends_with(path, ".json"))
  {
    return(HS_MIME_TYPE_APPLICATION_JSON);
  }

  if (  stringfn_ends_with(path, ".jar")
     || stringfn_ends_with(path, ".war")
     || stringfn_ends_with(path, ".ear"))
  {
    return(HS_MIME_TYPE_APPLICATION_JAVA_ARCHIVE);
  }

  if (stringfn_ends_with(path, ".doc"))
  {
    return(HS_MIME_TYPE_APPLICATION_MSWORD);
  }

  if (stringfn_ends_with(path, ".pdf"))
  {
    return(HS_MIME_TYPE_APPLICATION_PDF);
  }

  if (  stringfn_ends_with(path, ".ps")
     || stringfn_ends_with(path, ".eps")
     || stringfn_ends_with(path, ".ai"))
  {
    return(HS_MIME_TYPE_APPLICATION_POSTSCRIPT);
  }

  if (stringfn_ends_with(path, ".rtf"))
  {
    return(HS_MIME_TYPE_APPLICATION_RTF);
  }

  if (stringfn_ends_with(path, ".7z"))
  {
    return(HS_MIME_TYPE_APPLICATION_X_7Z_COMPRESSED);
  }

  if (stringfn_ends_with(path, ".jnlp"))
  {
    return(HS_MIME_TYPE_APPLICATION_X_JAVA_JNLP_FILE);
  }

  if (  stringfn_ends_with(path, ".pl")
     || stringfn_ends_with(path, ".pm"))
  {
    return(HS_MIME_TYPE_APPLICATION_X_PERL);
  }

  if (stringfn_ends_with(path, ".rar"))
  {
    return(HS_MIME_TYPE_APPLICATION_X_RAR_COMPRESSED);
  }

  if (stringfn_ends_with(path, ".rpm"))
  {
    return(HS_MIME_TYPE_APPLICATION_X_REDHAT_PACKAGE_MANAGER);
  }

  if (stringfn_ends_with(path, ".swf"))
  {
    return(HS_MIME_TYPE_APPLICATION_X_SHOCKWAVE_FLASH);
  }

  if (  stringfn_ends_with(path, ".tcl")
     || stringfn_ends_with(path, ".tk"))
  {
    return(HS_MIME_TYPE_APPLICATION_X_TCL);
  }

  if (  stringfn_ends_with(path, ".cert")
     || stringfn_ends_with(path, ".der")
     || stringfn_ends_with(path, ".pem"))
  {
    return(HS_MIME_TYPE_APPLICATION_X_X509_CA_CERT);
  }

  if (stringfn_ends_with(path, ".xhtml"))
  {
    return(HS_MIME_TYPE_APPLICATION_XHTML_XML);
  }

  if (stringfn_ends_with(path, ".zip"))
  {
    return(HS_MIME_TYPE_APPLICATION_ZIP);
  }

  return(HS_MIME_TYPE_APPLICATION_OCTET_STREAM);
}   /* hs_constants_file_extension_to_mime_type */

