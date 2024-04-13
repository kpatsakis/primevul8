_gnutls_supported_ciphersuites (gnutls_session_t session,
				cipher_suite_st ** _ciphers)
{

  unsigned int i, ret_count, j;
  unsigned int count = CIPHER_SUITES_COUNT;
  cipher_suite_st *tmp_ciphers;
  cipher_suite_st *ciphers;
  gnutls_protocol_t version;

  if (count == 0)
    {
      return 0;
    }

  tmp_ciphers = gnutls_alloca (count * sizeof (cipher_suite_st));
  if (tmp_ciphers == NULL)
    return GNUTLS_E_MEMORY_ERROR;

  ciphers = gnutls_malloc (count * sizeof (cipher_suite_st));
  if (ciphers == NULL)
    {
      gnutls_afree (tmp_ciphers);
      return GNUTLS_E_MEMORY_ERROR;
    }

  version = gnutls_protocol_get_version (session);

  for (i = 0; i < count; i++)
    {
      memcpy (&tmp_ciphers[i], &cs_algorithms[i].id,
	      sizeof (cipher_suite_st));
    }

  for (i = j = 0; i < count; i++)
    {
      /* remove private cipher suites, if requested.
       */
      if (tmp_ciphers[i].suite[0] == 0xFF &&
	  session->internals.enable_private == 0)
	continue;

      /* remove cipher suites which do not support the
       * protocol version used.
       */
      if (_gnutls_cipher_suite_get_version (&tmp_ciphers[i]) > version)
	continue;

      if (_gnutls_kx_priority
	  (session, _gnutls_cipher_suite_get_kx_algo (&tmp_ciphers[i])) < 0)
	continue;
      if (_gnutls_mac_priority
	  (session, _gnutls_cipher_suite_get_mac_algo (&tmp_ciphers[i])) < 0)
	continue;
      if (_gnutls_cipher_priority
	  (session,
	   _gnutls_cipher_suite_get_cipher_algo (&tmp_ciphers[i])) < 0)
	continue;

      memcpy (&ciphers[j], &tmp_ciphers[i], sizeof (cipher_suite_st));
      j++;
    }

  ret_count = j;

#if 0				/* expensive */
  if (ret_count > 0 && ret_count != count)
    {
      ciphers =
	gnutls_realloc_fast (ciphers, ret_count * sizeof (cipher_suite_st));
    }
  else
    {
      if (ret_count != count)
	{
	  gnutls_free (ciphers);
	  ciphers = NULL;
	}
    }
#endif

  gnutls_afree (tmp_ciphers);

  /* This function can no longer return 0 cipher suites.
   * It returns an error code instead.
   */
  if (ret_count == 0)
    {
      gnutls_assert ();
      gnutls_free (ciphers);
      return GNUTLS_E_NO_CIPHER_SUITES;
    }
  *_ciphers = ciphers;
  return ret_count;
}