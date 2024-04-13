_gnutls_partition (gnutls_session_t session, void *_base,
		   size_t nmemb, size_t size,
		   int (*compar) (gnutls_session_t,
				  const void *, const void *))
{
  uint8_t *base = _base;
  uint8_t tmp[MAX_ELEM_SIZE];
  uint8_t ptmp[MAX_ELEM_SIZE];
  unsigned int pivot;
  unsigned int i, j;
  unsigned int full;

  i = pivot = 0;
  j = full = (nmemb - 1) * size;

  memcpy (ptmp, &base[0], size);	/* set pivot item */

  while (i < j)
    {
      while ((compar (session, &base[i], ptmp) <= 0) && (i < full))
	{
	  i += size;
	}
      while ((compar (session, &base[j], ptmp) >= 0) && (j > 0))
	j -= size;

      if (i < j)
	{
	  SWAP (&base[j], &base[i]);
	}
    }

  if (j > pivot)
    {
      SWAP (&base[pivot], &base[j]);
      pivot = j;
    }
  else if (i < pivot)
    {
      SWAP (&base[pivot], &base[i]);
      pivot = i;
    }
  return pivot / size;
}