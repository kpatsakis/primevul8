_gnutls_version_is_supported (gnutls_session_t session,
			      const gnutls_protocol_t version)
{
  int ret = 0;

  GNUTLS_VERSION_ALG_LOOP (ret = p->supported);
  if (ret == 0)
    return 0;

  if (_gnutls_version_priority (session, version) < 0)
    return 0;			/* disabled by the user */
  else
    return 1;
}