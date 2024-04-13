_gnutls_version_priority (gnutls_session_t session, gnutls_protocol_t version)
{				/* actually returns the priority */
  unsigned int i;

  if (session->internals.protocol_priority.priority == NULL)
    {
      gnutls_assert ();
      return -1;
    }

  for (i = 0; i < session->internals.protocol_priority.algorithms; i++)
    {
      if (session->internals.protocol_priority.priority[i] == version)
	return i;
    }
  return -1;
}