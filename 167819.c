gnutls_sign_algorithm_get_name (gnutls_sign_algorithm_t sign)
{
  const char *ret = NULL;

  /* avoid prefix */
  GNUTLS_SIGN_ALG_LOOP (ret = p->name);

  return ret;
}