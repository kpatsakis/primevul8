_gnutls_cipher_suite_is_ok (cipher_suite_st * suite)
{
  size_t ret;
  const char *name = NULL;

  GNUTLS_CIPHER_SUITE_ALG_LOOP (name = p->name);
  if (name != NULL)
    ret = 0;
  else
    ret = 1;
  return ret;

}