_gnutls_cipher_get_iv_size (gnutls_cipher_algorithm_t algorithm)
{				/* In bytes */
  size_t ret = 0;
  GNUTLS_ALG_LOOP (ret = p->iv);
  return ret;

}