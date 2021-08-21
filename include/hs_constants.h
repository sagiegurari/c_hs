#ifndef __HS_CONSTANTS_H__
#define __HS_CONSTANTS_H__

enum HSHttpMethod
{
  HS_HTTP_METHOD_UNKNOWN = 0,
  HS_HTTP_METHOD_GET     = 1,
  HS_HTTP_METHOD_POST    = 2,
  HS_HTTP_METHOD_PUT     = 3,
  HS_HTTP_METHOD_DELETE  = 4,
  HS_HTTP_METHOD_HEAD    = 5,
  HS_HTTP_METHOD_CONNECT = 6,
  HS_HTTP_METHOD_OPTIONS = 7,
  HS_HTTP_METHOD_TRACE   = 8,
  HS_HTTP_METHOD_PATCH   = 9,
};

enum HSHttpProtocol
{
  HS_HTTP_PROTOCOL_UNKNOWN = 0,
  HS_HTTP_PROTOCOL_HTTP    = 1,
  HS_HTTP_PROTOCOL_HTTPS   = 2,
};

enum HSHttpResponseCode
{
  // info response
  HS_HTTP_RESPONSE_CODE_CONTINUE           = 100,
  HS_HTTP_RESPONSE_CODE_SWITCHING_PROTOCOL = 101,
  HS_HTTP_RESPONSE_CODE_PROCESSING         = 102,
  HS_HTTP_RESPONSE_CODE_EARLY_HINTS        = 103,

  // valid response
  HS_HTTP_RESPONSE_CODE_OK                              = 200,
  HS_HTTP_RESPONSE_CODE_CREATED                         = 201,
  HS_HTTP_RESPONSE_CODE_ACCEPTED                        = 202,
  HS_HTTP_RESPONSE_CODE_NON_AUTHORITATIVE_INFORMATION   = 203,
  HS_HTTP_RESPONSE_CODE_NO_CONTENT                      = 204,
  HS_HTTP_RESPONSE_CODE_RESET_CONTENT                   = 205,
  HS_HTTP_RESPONSE_CODE_PARTIAL_CONTENT                 = 206,

  // redirection
  HS_HTTP_RESPONSE_CODE_MULTIPLE_CHOICES                = 300,
  HS_HTTP_RESPONSE_CODE_MOVED_PERMANENTLY               = 301,
  HS_HTTP_RESPONSE_CODE_FOUND                           = 302,
  HS_HTTP_RESPONSE_CODE_SEE_OTHER                       = 303,
  HS_HTTP_RESPONSE_CODE_NOT_MODIFIED                    = 304,
  HS_HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT              = 307,
  HS_HTTP_RESPONSE_CODE_PERMANENT_REDIRECT              = 308,

  // client error
  HS_HTTP_RESPONSE_CODE_BAD_REQUEST                     = 400,
  HS_HTTP_RESPONSE_CODE_UNAUTHORIZED                    = 401,
  HS_HTTP_RESPONSE_CODE_PAYMENT_REQUIRED                = 402,
  HS_HTTP_RESPONSE_CODE_FORBIDDEN                       = 403,
  HS_HTTP_RESPONSE_CODE_NOT_FOUND                       = 404,
  HS_HTTP_RESPONSE_CODE_METHOD_NOT_ALLOWED              = 405,
  HS_HTTP_RESPONSE_CODE_NOT_ACCEPTABLE                  = 406,
  HS_HTTP_RESPONSE_CODE_PROXY_AUTHENTICATION_REQUIRED   = 407,
  HS_HTTP_RESPONSE_CODE_REQUEST_TIMEOUT                 = 408,
  HS_HTTP_RESPONSE_CODE_CONFLICT                        = 409,
  HS_HTTP_RESPONSE_CODE_GONE                            = 410,
  HS_HTTP_RESPONSE_CODE_LENGTH_REQUIRED                 = 411,
  HS_HTTP_RESPONSE_CODE_PRECONDITION_FAILED             = 412,
  HS_HTTP_RESPONSE_CODE_PAYLOAD_TOO_LARGE               = 413,
  HS_HTTP_RESPONSE_CODE_URI_TOO_LONG                    = 414,
  HS_HTTP_RESPONSE_CODE_UNSUPPORTED_MEDIA_TYPE          = 415,
  HS_HTTP_RESPONSE_CODE_RANGE_NOT_SATISFIABLE           = 416,
  HS_HTTP_RESPONSE_CODE_EXPECTATION_FAILED              = 417,
  HS_HTTP_RESPONSE_CODE_IM_A_TEAPOT                     = 418,
  HS_HTTP_RESPONSE_CODE_UNPROCESSABLE_ENTITY            = 422,
  HS_HTTP_RESPONSE_CODE_TOO_EARLY                       = 425,
  HS_HTTP_RESPONSE_CODE_UPGRADE_REQUIRED                = 426,
  HS_HTTP_RESPONSE_CODE_PRECONDITION_REQUIRED           = 428,
  HS_HTTP_RESPONSE_CODE_TOO_MANY_REQUESTS               = 429,
  HS_HTTP_RESPONSE_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
  HS_HTTP_RESPONSE_CODE_UNAVAILABLE_FOR_LEGAL_REASONS   = 451,

  // server error
  HS_HTTP_RESPONSE_CODE_INTERNAL_SERVER_ERROR           = 500,
  HS_HTTP_RESPONSE_CODE_NOT_IMPLEMENTED                 = 501,
  HS_HTTP_RESPONSE_CODE_BAD_GATEWAY                     = 502,
  HS_HTTP_RESPONSE_CODE_SERVICE_UNAVAILABLE             = 503,
  HS_HTTP_RESPONSE_CODE_GATEWAY_TIMEOUT                 = 504,
  HS_HTTP_RESPONSE_CODE_HTTP_VERSION_NOT_SUPPORTED      = 505,
  HS_HTTP_RESPONSE_CODE_VARIANT_ALSO_NEGOTIATES         = 506,
  HS_HTTP_RESPONSE_CODE_INSUFFICIENT_STORAGE            = 507,
  HS_HTTP_RESPONSE_CODE_LOOP_DETECTED                   = 508,
  HS_HTTP_RESPONSE_CODE_NOT_EXTENDED                    = 510,
  HS_HTTP_RESPONSE_CODE_NETWORK_AUTHENTICATION_REQUIRED = 511,
};

enum HSCookieSameSite
{
  HS_COOKIE_SAME_SITE_NONE   = 0,
  HS_COOKIE_SAME_SITE_STRICT = 1,
  HS_COOKIE_SAME_SITE_LAX    = 2,
};

enum HSMimeType
{
  // use none to enable setting custom mime type header manually in route response
  HS_MIME_TYPE_NONE                     = 0,
  HS_MIME_TYPE_APPLICATION_OCTET_STREAM = 1,
  HS_MIME_TYPE_TEXT_HTML                = 2,
  HS_MIME_TYPE_TEXT_PLAIN               = 3,
  HS_MIME_TYPE_TEXT_CSS                 = 4,
  HS_MIME_TYPE_TEXT_XML                 = 5,
  HS_MIME_TYPE_TEXT_JAVASCRIPT          = 6,
  HS_MIME_TYPE_IMAGE_APNG               = 7,
  HS_MIME_TYPE_IMAGE_AVIF               = 8,
  HS_MIME_TYPE_IMAGE_GIF                = 9,
  HS_MIME_TYPE_IMAGE_JPEG               = 10,
  HS_MIME_TYPE_IMAGE_PNG                = 11,
  HS_MIME_TYPE_IMAGE_SVG                = 12,
  HS_MIME_TYPE_IMAGE_WEBP               = 13,
  HS_MIME_TYPE_AUDIO_WAV                = 14,
  HS_MIME_TYPE_AUDIO_WEBM               = 15,
  HS_MIME_TYPE_VIDEO_WEBM               = 16,
  HS_MIME_TYPE_AUDIO_OGG                = 17,
  HS_MIME_TYPE_VIDEO_OGG                = 18,
  HS_MIME_TYPE_APPLICATION_OGG          = 19,
  HS_MIME_TYPE_APPLICATION_ATOM         = 20,
  HS_MIME_TYPE_APPLICATION_RSS          = 21,
};

enum HSConnectionType
{
  HS_CONNECTION_TYPE_UNKNOWN    = 0,
  HS_CONNECTION_TYPE_CLOSE      = 1,
  HS_CONNECTION_TYPE_KEEP_ALIVE = 2,
};

/**
 * Return the string value of the mime type.
 */
const char *hs_constants_mime_type_to_string(enum HSMimeType);

/**
 * Returns the mime type for the file extension or binary if unknown.
 */
enum HSMimeType hs_constants_file_extension_to_mime_type(char *);

#endif

