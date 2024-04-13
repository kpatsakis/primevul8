_gnutls_cipher_suite_get_kx_algo (const cipher_suite_st * suite)
{
  int ret = 0;

  GNUTLS_CIPHER_SUITE_ALG_LOOP (ret = p->kx_algorithm);
  return ret;

}