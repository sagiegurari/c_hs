#ifndef __HS_OPENSSL_H__
#define __HS_OPENSSL_H__

/**
 * This is an internal header and should not be used outside the library.
 */

#ifdef HS_SSL_SUPPORTED
#include <openssl/ssl.h>

void hs_openssl_init(void);
void hs_openssl_cleanup(void);
SSL_CTX *hs_openssl_context_create(char * /* private_key_pem_file */, char * /* certificate_pem_file */);
void hs_openssl_context_release(SSL_CTX *);

#endif

#endif

