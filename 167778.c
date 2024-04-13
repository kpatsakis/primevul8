_gnutls_cipher_is_ok (gnutls_cipher_algorithm_t algorithm)
{
  ssize_t ret = -1;
  GNUTLS_ALG_LOOP (ret = p->id);
  if (ret >= 0)
    ret = 0;
  else
    ret = 1;
  return ret;
}