_gnutls_kx_needs_rsa_params (gnutls_kx_algorithm_t algorithm)
{
  ssize_t ret = 0;
  GNUTLS_KX_ALG_LOOP (ret = p->needs_rsa_params);
  return ret;
}