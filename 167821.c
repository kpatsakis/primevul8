gnutls_kx_get_name (gnutls_kx_algorithm_t algorithm)
{
  const char *ret = NULL;

  /* avoid prefix */
  GNUTLS_KX_ALG_LOOP (ret = p->name);

  return ret;
}