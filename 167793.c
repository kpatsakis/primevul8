_gnutls_kx_priority (gnutls_session_t session,
		     gnutls_kx_algorithm_t algorithm)
{
  unsigned int i;
  for (i = 0; i < session->internals.kx_algorithm_priority.algorithms; i++)
    {
      if (session->internals.kx_algorithm_priority.priority[i] == algorithm)
	return i;
    }
  return -1;
}