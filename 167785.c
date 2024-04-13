_gnutls_compare_algo (gnutls_session_t session, const void *i_A1,
		      const void *i_A2)
{
  gnutls_kx_algorithm_t kA1 =
    _gnutls_cipher_suite_get_kx_algo ((const cipher_suite_st *) i_A1);
  gnutls_kx_algorithm_t kA2 =
    _gnutls_cipher_suite_get_kx_algo ((const cipher_suite_st *) i_A2);
  gnutls_cipher_algorithm_t cA1 =
    _gnutls_cipher_suite_get_cipher_algo ((const cipher_suite_st *) i_A1);
  gnutls_cipher_algorithm_t cA2 =
    _gnutls_cipher_suite_get_cipher_algo ((const cipher_suite_st *) i_A2);
  gnutls_mac_algorithm_t mA1 =
    _gnutls_cipher_suite_get_mac_algo ((const cipher_suite_st *) i_A1);
  gnutls_mac_algorithm_t mA2 =
    _gnutls_cipher_suite_get_mac_algo ((const cipher_suite_st *) i_A2);

  int p1 = (_gnutls_kx_priority (session, kA1) + 1) * 64;
  int p2 = (_gnutls_kx_priority (session, kA2) + 1) * 64;
  p1 += (_gnutls_cipher_priority (session, cA1) + 1) * 8;
  p2 += (_gnutls_cipher_priority (session, cA2) + 1) * 8;
  p1 += _gnutls_mac_priority (session, mA1);
  p2 += _gnutls_mac_priority (session, mA2);

  if (p1 > p2)
    {
      return 1;
    }
  else
    {
      if (p1 == p2)
	{
	  return 0;
	}
      return -1;
    }
}