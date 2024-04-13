gnutls_cipher_get_name (gnutls_cipher_algorithm_t algorithm)
{
  const char *ret = NULL;

  /* avoid prefix */
  GNUTLS_ALG_LOOP (ret = p->name);

  return ret;
}