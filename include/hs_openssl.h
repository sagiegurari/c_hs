#ifndef HS_OPENSSL_H
#define HS_OPENSSL_H

#include <stdbool.h>

/**
 * This is an internal header and should not be used outside the library.
 */

bool hs_openssl_supported(void);

#ifdef HS_SSL_SUPPORTED
#include <openssl/ssl.h>

void hs_openssl_init(void);
void hs_openssl_cleanup(void);
SSL_CTX *hs_openssl_context_create(char * /* private_key_pem_file */, char * /* certificate_pem_file */);
void hs_openssl_context_release(SSL_CTX *);

#endif

#endif

