_gnutls_compression_priority (gnutls_session_t session,
			      gnutls_compression_method_t algorithm)
{				/* actually returns the priority */
  unsigned int i;
  for (i = 0;
       i < session->internals.compression_method_priority.algorithms; i++)
    {
      if (session->internals.
	  compression_method_priority.priority[i] == algorithm)
	return i;
    }
  return -1;
}