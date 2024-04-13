transit_intern (struct transit *transit)
{
  struct transit *find;

  find = hash_get (transit_hash, transit, transit_hash_alloc);
  if (find != transit)
    transit_free (transit);
  find->refcnt++;

  return find;
}