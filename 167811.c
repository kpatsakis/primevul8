gnutls_pk_algorithm_get_name (gnutls_pk_algorithm_t algorithm)
{
  const char *ret = NULL;

  /* avoid prefix */
  GNUTLS_PK_ALG_LOOP (ret = p->name);

  return ret;
}