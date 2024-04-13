_gnutls_version_max (gnutls_session_t session)
{				/* returns the maximum version supported */
  unsigned int i, max = 0x00;

  if (session->internals.protocol_priority.priority == NULL)
    {
      return GNUTLS_VERSION_UNKNOWN;
    }
  else
    for (i = 0; i < session->internals.protocol_priority.algorithms; i++)
      {
	if (session->internals.protocol_priority.priority[i] > max)
	  max = session->internals.protocol_priority.priority[i];
      }

  if (max == 0x00)
    return GNUTLS_VERSION_UNKNOWN;	/* unknown version */

  return max;
}