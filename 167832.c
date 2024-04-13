_gnutls_cipher_priority (gnutls_session_t session,
			 gnutls_cipher_algorithm_t algorithm)
{
  unsigned int i;
  for (i = 0;
       i < session->internals.cipher_algorithm_priority.algorithms; i++)
    {
      if (session->internals.
	  cipher_algorithm_priority.priority[i] == algorithm)
	return i;
    }
  return -1;
}