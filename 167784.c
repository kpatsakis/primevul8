_gnutls_cipher_suite_get_mac_algo (const cipher_suite_st * suite)
{				/* In bytes */
  int ret = 0;
  GNUTLS_CIPHER_SUITE_ALG_LOOP (ret = p->mac_algorithm);
  return ret;

}