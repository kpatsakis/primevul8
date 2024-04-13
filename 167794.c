_gnutls_version_get_major (gnutls_protocol_t version)
{
  int ret = -1;

  GNUTLS_VERSION_ALG_LOOP (ret = p->major);
  return ret;
}