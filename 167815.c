_gnutls_supported_ciphersuites_sorted (gnutls_session_t session,
				       cipher_suite_st ** ciphers)
{

#ifdef SORT_DEBUG
  unsigned int i;
#endif
  int count;

  count = _gnutls_supported_ciphersuites (session, ciphers);
  if (count <= 0)
    {
      gnutls_assert ();
      return count;
    }
#ifdef SORT_DEBUG
  _gnutls_debug_log ("Unsorted: \n");
  for (i = 0; i < count; i++)
    _gnutls_debug_log ("\t%d: %s\n", i,
		       _gnutls_cipher_suite_get_name ((*ciphers)[i]));
#endif

  _gnutls_qsort (session, *ciphers, count,
		 sizeof (cipher_suite_st), _gnutls_compare_algo);

#ifdef SORT_DEBUG
  _gnutls_debug_log ("Sorted: \n");
  for (i = 0; i < count; i++)
    _gnutls_debug_log ("\t%d: %s\n", i,
		       _gnutls_cipher_suite_get_name ((*ciphers)[i]));
#endif

  return count;
}