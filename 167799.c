_gnutls_version_get_minor (gnutls_protocol_t version)
{
  int ret = -1;

  GNUTLS_VERSION_ALG_LOOP (ret = p->minor);
  return ret;
}