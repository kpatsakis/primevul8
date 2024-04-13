_gnutls_x509_sign_to_oid (gnutls_pk_algorithm_t pk,
			  gnutls_mac_algorithm_t mac)
{
  gnutls_sign_algorithm_t sign;
  const char *ret = NULL;

  sign = _gnutls_x509_pk_to_sign (pk, mac);
  if (sign == GNUTLS_SIGN_UNKNOWN)
    return NULL;

  GNUTLS_SIGN_ALG_LOOP (ret = p->oid);
  return ret;
}