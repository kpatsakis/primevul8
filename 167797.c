_gnutls_x509_mac_to_oid (gnutls_mac_algorithm_t algorithm)
{
  const char *ret = NULL;

  /* avoid prefix */
  GNUTLS_HASH_ALG_LOOP (ret = p->oid);

  return ret;
}