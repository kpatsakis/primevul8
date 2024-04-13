_gnutls_cipher_get_block_size (gnutls_cipher_algorithm_t algorithm)
{
  size_t ret = 0;
  GNUTLS_ALG_LOOP (ret = p->blocksize);
  return ret;

}