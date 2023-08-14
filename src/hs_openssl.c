#include "hs_openssl.h"

#ifdef HS_SSL_SUPPORTED
#include <openssl/err.h>

static int _hs_openssl_global_ssl_init = 0;


bool hs_openssl_supported(void)
{
  return(true);
}


void hs_openssl_init(void)
{
  _hs_openssl_global_ssl_init++;

  if (_hs_openssl_global_ssl_init > 1)
  {
    return;
  }

  SSL_load_error_strings();
  SSL_library_init();
  OpenSSL_add_all_algorithms();
}


void hs_openssl_cleanup(void)
{
  _hs_openssl_global_ssl_init--;

  if (_hs_openssl_global_ssl_init < 0)
  {
    _hs_openssl_global_ssl_init = 0;
  }
  else if (_hs_openssl_global_ssl_init)
  {
    return;
  }

  ERR_free_strings();
  EVP_cleanup();
}


SSL_CTX *hs_openssl_context_create(char *private_key_pem_file, char *certificate_pem_file)
{
  if (private_key_pem_file == NULL || certificate_pem_file == NULL)
  {
    return(NULL);
  }

  SSL_CTX *ssl_context = SSL_CTX_new(SSLv23_method());
  if (ssl_context == NULL)
  {
    return(NULL);
  }

  SSL_CTX_set_options(ssl_context, SSL_OP_ALL);
  SSL_CTX_set_options(ssl_context, SSL_OP_SINGLE_DH_USE);

  if ((SSL_CTX_use_certificate_file(ssl_context, certificate_pem_file, SSL_FILETYPE_PEM)) <= 0)
  {
    hs_openssl_context_release(ssl_context);
    return(NULL);
  }

  if ((SSL_CTX_use_PrivateKey_file(ssl_context, private_key_pem_file, SSL_FILETYPE_PEM)) <= 0)
  {
    hs_openssl_context_release(ssl_context);
    return(NULL);
  }

  return(ssl_context);
}


void hs_openssl_context_release(SSL_CTX *ssl_context)
{
  if (ssl_context == NULL)
  {
    return;
  }

  SSL_CTX_free(ssl_context);
}
#else


bool hs_openssl_supported(void)
{
  return(false);
}

#endif

