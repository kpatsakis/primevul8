gnutls_cipher_get_key_size (gnutls_cipher_algorithm_t algorithm)
{				/* In bytes */
  size_t ret = 0;
  GNUTLS_ALG_LOOP (ret = p->keysize);
  return ret;

}