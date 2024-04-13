_gnutls_cipher_suite_get_version (const cipher_suite_st * suite)
{
  int ret = 0;
  GNUTLS_CIPHER_SUITE_ALG_LOOP (ret = p->version);
  return ret;
}