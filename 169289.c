transit_hash_key_make (void *p)
{
  const struct transit * transit = p;

  return jhash(transit->val, transit->length, 0);
}