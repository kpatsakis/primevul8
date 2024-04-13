_gnutls_version_lowest (gnutls_session_t session)
{				/* returns the lowest version supported */
  unsigned int i, min = 0xff;

  if (session->internals.protocol_priority.priority == NULL)
    {
      return GNUTLS_VERSION_UNKNOWN;
    }
  else
    for (i = 0; i < session->internals.protocol_priority.algorithms; i++)
      {
	if (session->internals.protocol_priority.priority[i] < min)
	  min = session->internals.protocol_priority.priority[i];
      }

  if (min == 0xff)
    return GNUTLS_VERSION_UNKNOWN;	/* unknown version */

  return min;
}