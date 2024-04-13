_gnutls_qsort (gnutls_session_t session, void *_base, size_t nmemb,
	       size_t size, int (*compar) (gnutls_session_t, const void *,
					   const void *))
{
  unsigned int pivot;
  char *base = _base;
  size_t snmemb = nmemb;

#ifdef DEBUG
  if (size > MAX_ELEM_SIZE)
    {
      gnutls_assert ();
      _gnutls_debug_log ("QSORT BUG\n");
      exit (1);
    }
#endif

  if (snmemb <= 1)
    return;
  pivot = _gnutls_partition (session, _base, nmemb, size, compar);

  _gnutls_qsort (session, base, pivot < nmemb ? pivot + 1 : pivot, size,
		 compar);
  _gnutls_qsort (session, &base[(pivot + 1) * size], nmemb - pivot - 1,
		 size, compar);
}