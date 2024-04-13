transit_unintern (struct transit *transit)
{
  if (transit->refcnt)
    transit->refcnt--;

  if (transit->refcnt == 0)
    {
      hash_release (transit_hash, transit);
      transit_free (transit);
    }
}