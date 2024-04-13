_gnutls_kx_is_ok (gnutls_kx_algorithm_t algorithm)
{
  ssize_t ret = -1;
  GNUTLS_KX_ALG_LOOP (ret = p->algorithm);
  if (ret >= 0)
    ret = 0;
  else
    ret = 1;
  return ret;
}