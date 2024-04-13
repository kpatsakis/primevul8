str2solvid_check(Pool *pool, Solvable *s, const char *start, const char *end, Id evrid)
{
  if (!solvable_lookup_type(s, SOLVABLE_BUILDFLAVOR))
    {
      /* just check the evr */
      return evrid && s->evr == evrid;
    }
  else
    {
      Queue flavorq;
      int i;

      queue_init(&flavorq);
      solvable_lookup_idarray(s, SOLVABLE_BUILDFLAVOR, &flavorq);
      queue_unshift(&flavorq, s->evr);
      for (i = 0; i < flavorq.count; i++)
	{
	  const char *part = pool_id2str(pool, flavorq.elements[i]);
	  size_t partl = strlen(part);
	  if (start + partl > end || strncmp(start, part, partl) != 0)
	    break;
	  start += partl;
	  if (i + 1 < flavorq.count)
	    {
	      if (start >= end || *start != '-')
		break;
	      start++;
	    }
	}
      if (i < flavorq.count)
	{
	  queue_free(&flavorq);
	  return 0;
	}
      queue_free(&flavorq);
      return start == end;
    }
}