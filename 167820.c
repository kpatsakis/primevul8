gnutls_certificate_type_get_name (gnutls_certificate_type_t type)
{
  const char *ret = NULL;

  if (type == GNUTLS_CRT_X509)
    ret = "X.509";
  if (type == GNUTLS_CRT_OPENPGP)
    ret = "OPENPGP";

  return ret;
}