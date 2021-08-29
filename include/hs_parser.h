#ifndef __HS_PARSER_H__
#define __HS_PARSER_H__

#include "hs_types.h"

/**
 * Creates a basic http request struct representing the given resource path
 * and query string.
 * Since a lot of information is missing, only the URL based info
 * will be set in the returned request. All other information is
 * defaulted.
 */
struct HSHttpRequest *hs_parser_create_request_from_resource(char * /* resource */);

/**
 * Creates a basic http request struct representing the given URL.
 * Since a lot of information is missing, only the URL based info
 * will be set in the returned request. All other information is
 * defaulted.
 */
struct HSHttpRequest *hs_parser_create_request_from_url(char * /* url */);

/**
 * Parses the request line and populates the request struct.
 * If the line is invalid, NULL will be returned.
 */
struct HSHttpRequest *hs_parser_parse_request_line(char *);

/**
 * Parsers a http header and returns it. If invalid content, it will return NULL.
 */
struct HSKeyValue *hs_parser_parse_header(char *);

/**
 * Parsers the cookie header value (without key) and updates the cookies container.
 */
bool hs_parser_parse_cookie_header(struct HSCookies *, char *);

/**
 * Sets the specific header attribute in the given request from the header key/pair.
 * If no specific header attribute is supported, no changes will be done.
 */
void hs_parser_set_header(struct HSHttpRequest *, struct HSKeyValue *);

/**
 * Parsers the http header and creates a new http request struct.
 * The payload is not parsed.
 */
struct HSHttpRequest *hs_parser_parse_request(int /* socket */);

/**
 * Parsers the HTTP method and returns the enum value.
 * In case of unknown value or NULL, a HS_HTTP_METHOD_UNKNOWN method type will be returned.
 */
enum HSHttpMethod hs_parser_parse_method(char *);

/**
 * Parsers the query string and returns an array of key/value pairs.
 */
struct HSKeyValueArray *hs_parser_parse_query_string(char *);

/**
 * Returns the protocol from the given URL.
 * If no protocol, URL or unknown protocol, HS_HTTP_PROTOCOL_UNKNOWN will be returned.
 */
enum HSHttpProtocol hs_parser_parse_protocol_from_url(char *);

#endif

