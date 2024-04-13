_gnutls_cipher_is_block (gnutls_cipher_algorithm_t algorithm)
{
  size_t ret = 0;

  GNUTLS_ALG_LOOP (ret = p->block);
  return ret;

}