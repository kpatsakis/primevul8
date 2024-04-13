_gnutls_cipher_suite_get_name (cipher_suite_st * suite)
{
  const char *ret = NULL;

  /* avoid prefix */
  GNUTLS_CIPHER_SUITE_ALG_LOOP (ret = p->name + sizeof ("GNUTLS_") - 1);

  return ret;
}