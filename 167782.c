_gnutls_x509_pk_to_oid (gnutls_pk_algorithm_t algorithm)
{
  const char *ret = NULL;

  GNUTLS_PK_ALG_LOOP (ret = p->oid);
  return ret;
}