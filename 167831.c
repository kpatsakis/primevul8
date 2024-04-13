_gnutls_bsort (gnutls_session_t session, void *_base, size_t nmemb,
	       size_t size, int (*compar) (gnutls_session_t, const void *,
					   const void *))
{
  unsigned int i, j;
  int full = nmemb * size;
  char *base = _base;
  char tmp[MAX_ELEM_SIZE];

  for (i = 0; i < full; i += size)
    {
      for (j = 0; j < full; j += size)
	{
	  if (compar (session, &base[i], &base[j]) < 0)
	    {
	      SWAP (&base[j], &base[i]);
	    }
	}
    }

}