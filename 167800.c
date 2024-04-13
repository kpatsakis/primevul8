_gnutls_mac_priority (gnutls_session_t session,
		      gnutls_mac_algorithm_t algorithm)
{				/* actually returns the priority */
  unsigned int i;
  for (i = 0; i < session->internals.mac_algorithm_priority.algorithms; i++)
    {
      if (session->internals.mac_algorithm_priority.priority[i] == algorithm)
	return i;
    }
  return -1;
}