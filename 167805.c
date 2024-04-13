_gnutls_supported_compression_methods (gnutls_session_t session,
				       uint8_t ** comp)
{
  unsigned int i, j;

  *comp = gnutls_malloc (sizeof (uint8_t) * SUPPORTED_COMPRESSION_METHODS);
  if (*comp == NULL)
    return GNUTLS_E_MEMORY_ERROR;

  for (i = j = 0; i < SUPPORTED_COMPRESSION_METHODS; i++)
    {
      int tmp = _gnutls_compression_get_num (session->internals.
					     compression_method_priority.
					     priority[i]);

      /* remove private compression algorithms, if requested.
       */
      if (tmp == -1 || (tmp >= MIN_PRIVATE_COMP_ALGO &&
			session->internals.enable_private == 0))
	{
	  gnutls_assert ();
	  continue;
	}

      (*comp)[j] = (uint8_t) tmp;
      j++;
    }

  if (j == 0)
    {
      gnutls_assert ();
      gnutls_free (*comp);
      *comp = NULL;
      return GNUTLS_E_NO_COMPRESSION_ALGORITHMS;
    }
  return j;
}